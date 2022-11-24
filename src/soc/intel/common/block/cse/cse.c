/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <assert.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/post_codes.h>
#include <option.h>
#include <security/vboot/misc.h>
#include <security/vboot/vboot_common.h>
#include <soc/intel/common/reset.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/me.h>
#include <string.h>
#include <timer.h>
#include <types.h>

#define HECI_BASE_SIZE (4 * KiB)

#define MAX_HECI_MESSAGE_RETRY_COUNT 5

/* Wait up to 15 sec for HECI to get ready */
#define HECI_DELAY_READY_MS	(15 * 1000)
/* Wait up to 100 usec between circular buffer polls */
#define HECI_DELAY_US		100
/* Wait up to 5 sec for CSE to chew something we sent */
#define HECI_SEND_TIMEOUT_MS	(5 * 1000)
/* Wait up to 5 sec for CSE to blurp a reply */
#define HECI_READ_TIMEOUT_MS	(5 * 1000)
/* Wait up to 1 ms for CSE CIP */
#define HECI_CIP_TIMEOUT_US	1000
/* Wait up to 5 seconds for CSE to boot from RO(BP1) */
#define CSE_DELAY_BOOT_TO_RO_MS	(5 * 1000)

#define SLOT_SIZE		sizeof(uint32_t)

#define MMIO_CSE_CB_WW		0x00
#define MMIO_HOST_CSR		0x04
#define MMIO_CSE_CB_RW		0x08
#define MMIO_CSE_CSR		0x0c
#define MMIO_CSE_DEVIDLE	0x800
#define  CSE_DEV_IDLE		(1 << 2)
#define  CSE_DEV_CIP		(1 << 0)

#define CSR_IE			(1 << 0)
#define CSR_IS			(1 << 1)
#define CSR_IG			(1 << 2)
#define CSR_READY		(1 << 3)
#define CSR_RESET		(1 << 4)
#define CSR_RP_START		8
#define CSR_RP			(((1 << 8) - 1) << CSR_RP_START)
#define CSR_WP_START		16
#define CSR_WP			(((1 << 8) - 1) << CSR_WP_START)
#define CSR_CBD_START		24
#define CSR_CBD			(((1 << 8) - 1) << CSR_CBD_START)

#define MEI_HDR_IS_COMPLETE	(1 << 31)
#define MEI_HDR_LENGTH_START	16
#define MEI_HDR_LENGTH_SIZE	9
#define MEI_HDR_LENGTH		(((1 << MEI_HDR_LENGTH_SIZE) - 1) \
					<< MEI_HDR_LENGTH_START)
#define MEI_HDR_HOST_ADDR_START	8
#define MEI_HDR_HOST_ADDR	(((1 << 8) - 1) << MEI_HDR_HOST_ADDR_START)
#define MEI_HDR_CSE_ADDR_START	0
#define MEI_HDR_CSE_ADDR	(((1 << 8) - 1) << MEI_HDR_CSE_ADDR_START)

/* Get HECI BAR 0 from PCI configuration space */
static uintptr_t get_cse_bar(pci_devfn_t dev)
{
	uintptr_t bar;

	bar = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	assert(bar != 0);
	/*
	 * Bits 31-12 are the base address as per EDS for SPI,
	 * Don't care about 0-11 bit
	 */
	return bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
}

static void heci_assign_resource(pci_devfn_t dev, uintptr_t tempbar)
{
	u16 pcireg;

	/* Assign Resources */
	/* Clear BIT 1-2 of Command Register */
	pcireg = pci_read_config16(dev, PCI_COMMAND);
	pcireg &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pci_write_config16(dev, PCI_COMMAND, pcireg);

	/* Program Temporary BAR for HECI device */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, tempbar);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, 0x0);

	/* Enable Bus Master and MMIO Space */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
}

/*
 * Initialize the CSE device with provided temporary BAR. If BAR is 0 use a
 * default. This is intended for pre-mem usage only where BARs haven't been
 * assigned yet and devices are not enabled.
 */
void cse_init(uintptr_t tempbar)
{
	pci_devfn_t dev = PCH_DEV_CSE;

	/* Check if device enabled */
	if (!is_cse_enabled())
		return;

	/* Assume it is already initialized, nothing else to do */
	if (get_cse_bar(dev))
		return;

	/* Use default pre-ram bar */
	if (!tempbar)
		tempbar = HECI1_BASE_ADDRESS;

	/* Assign HECI resource and enable the resource */
	heci_assign_resource(dev, tempbar);

	/* Trigger HECI Reset and make Host ready for communication with CSE */
	heci_reset();
}

static uint32_t read_bar(pci_devfn_t dev, uint32_t offset)
{
	return read32p(get_cse_bar(dev) + offset);
}

static void write_bar(pci_devfn_t dev, uint32_t offset, uint32_t val)
{
	return write32p(get_cse_bar(dev) + offset, val);
}

static uint32_t read_cse_csr(void)
{
	return read_bar(PCH_DEV_CSE, MMIO_CSE_CSR);
}

static uint32_t read_host_csr(void)
{
	return read_bar(PCH_DEV_CSE, MMIO_HOST_CSR);
}

static void write_host_csr(uint32_t data)
{
	write_bar(PCH_DEV_CSE, MMIO_HOST_CSR, data);
}

static size_t filled_slots(uint32_t data)
{
	uint8_t wp, rp;
	rp = data >> CSR_RP_START;
	wp = data >> CSR_WP_START;
	return (uint8_t)(wp - rp);
}

static size_t cse_filled_slots(void)
{
	return filled_slots(read_cse_csr());
}

static size_t host_empty_slots(void)
{
	uint32_t csr;
	csr = read_host_csr();

	return ((csr & CSR_CBD) >> CSR_CBD_START) - filled_slots(csr);
}

static void clear_int(void)
{
	uint32_t csr;
	csr = read_host_csr();
	csr |= CSR_IS;
	write_host_csr(csr);
}

static uint32_t read_slot(void)
{
	return read_bar(PCH_DEV_CSE, MMIO_CSE_CB_RW);
}

static void write_slot(uint32_t val)
{
	write_bar(PCH_DEV_CSE, MMIO_CSE_CB_WW, val);
}

static int wait_write_slots(size_t cnt)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, HECI_SEND_TIMEOUT_MS);
	while (host_empty_slots() < cnt) {
		udelay(HECI_DELAY_US);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "HECI: timeout, buffer not drained\n");
			return 0;
		}
	}
	return 1;
}

static int wait_read_slots(size_t cnt)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, HECI_READ_TIMEOUT_MS);
	while (cse_filled_slots() < cnt) {
		udelay(HECI_DELAY_US);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "HECI: timed out reading answer!\n");
			return 0;
		}
	}
	return 1;
}

/* get number of full 4-byte slots */
static size_t bytes_to_slots(size_t bytes)
{
	return ALIGN_UP(bytes, SLOT_SIZE) / SLOT_SIZE;
}

static int cse_ready(void)
{
	uint32_t csr;
	csr = read_cse_csr();
	return csr & CSR_READY;
}

static bool cse_check_hfs1_com(int mode)
{
	union me_hfsts1 hfs1;
	hfs1.data = me_read_config32(PCI_ME_HFSTS1);
	return hfs1.fields.operation_mode == mode;
}

bool cse_is_hfs1_cws_normal(void)
{
	union me_hfsts1 hfs1;
	hfs1.data = me_read_config32(PCI_ME_HFSTS1);
	if (hfs1.fields.working_state == ME_HFS1_CWS_NORMAL)
		return true;
	return false;
}

bool cse_is_hfs1_com_normal(void)
{
	return cse_check_hfs1_com(ME_HFS1_COM_NORMAL);
}

bool cse_is_hfs1_com_secover_mei_msg(void)
{
	return cse_check_hfs1_com(ME_HFS1_COM_SECOVER_MEI_MSG);
}

bool cse_is_hfs1_com_soft_temp_disable(void)
{
	return cse_check_hfs1_com(ME_HFS1_COM_SOFT_TEMP_DISABLE);
}

/*
 * Starting from TGL platform, HFSTS1.spi_protection_mode replaces mfg_mode to indicate
 * SPI protection status as well as end-of-manufacturing(EOM) status where EOM flow is
 * triggered in single staged operation (either through first boot with required MFIT
 * configuratin or FPT /CLOSEMANUF).
 * In staged manufacturing flow, spi_protection_mode alone doesn't indicate the EOM status.
 *
 * HFSTS1.spi_protection_mode description:
 * mfg_mode = 0 means SPI protection is on.
 * mfg_mode = 1 means SPI is unprotected.
 */
bool cse_is_hfs1_spi_protected(void)
{
	union me_hfsts1 hfs1;
	hfs1.data = me_read_config32(PCI_ME_HFSTS1);
	return !hfs1.fields.mfg_mode;
}

bool cse_is_hfs3_fw_sku_lite(void)
{
	union me_hfsts3 hfs3;
	hfs3.data = me_read_config32(PCI_ME_HFSTS3);
	return hfs3.fields.fw_sku == ME_HFS3_FW_SKU_LITE;
}

/* Makes the host ready to communicate with CSE */
void cse_set_host_ready(void)
{
	uint32_t csr;
	csr = read_host_csr();
	csr &= ~CSR_RESET;
	csr |= (CSR_IG | CSR_READY);
	write_host_csr(csr);
}

/* Polls for ME mode ME_HFS1_COM_SECOVER_MEI_MSG for 15 seconds */
uint8_t cse_wait_sec_override_mode(void)
{
	struct stopwatch sw;
	stopwatch_init_msecs_expire(&sw, HECI_DELAY_READY_MS);
	while (!cse_is_hfs1_com_secover_mei_msg()) {
		udelay(HECI_DELAY_US);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "HECI: Timed out waiting for SEC_OVERRIDE mode!\n");
			return 0;
		}
	}
	printk(BIOS_DEBUG, "HECI: CSE took %lld ms to enter security override mode\n",
			stopwatch_duration_msecs(&sw));
	return 1;
}

/*
 * Polls for CSE's current operation mode 'Soft Temporary Disable'.
 * The CSE enters the current operation mode when it boots from RO(BP1).
 */
uint8_t cse_wait_com_soft_temp_disable(void)
{
	struct stopwatch sw;
	stopwatch_init_msecs_expire(&sw, CSE_DELAY_BOOT_TO_RO_MS);
	while (!cse_is_hfs1_com_soft_temp_disable()) {
		udelay(HECI_DELAY_US);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "HECI: Timed out waiting for CSE to boot from RO!\n");
			return 0;
		}
	}
	printk(BIOS_SPEW, "HECI: CSE took %lld ms to boot from RO\n",
			stopwatch_duration_msecs(&sw));
	return 1;
}

static int wait_heci_ready(void)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, HECI_DELAY_READY_MS);
	while (!cse_ready()) {
		udelay(HECI_DELAY_US);
		if (stopwatch_expired(&sw))
			return 0;
	}

	return 1;
}

static void host_gen_interrupt(void)
{
	uint32_t csr;
	csr = read_host_csr();
	csr |= CSR_IG;
	write_host_csr(csr);
}

static size_t hdr_get_length(uint32_t hdr)
{
	return (hdr & MEI_HDR_LENGTH) >> MEI_HDR_LENGTH_START;
}

static int
send_one_message(uint32_t hdr, const void *buff)
{
	size_t pend_len, pend_slots, remainder, i;
	uint32_t tmp;
	const uint32_t *p = buff;

	/* Get space for the header */
	if (!wait_write_slots(1))
		return 0;

	/* First, write header */
	write_slot(hdr);

	pend_len = hdr_get_length(hdr);
	pend_slots = bytes_to_slots(pend_len);

	if (!wait_write_slots(pend_slots))
		return 0;

	/* Write the body in whole slots */
	i = 0;
	while (i < ALIGN_DOWN(pend_len, SLOT_SIZE)) {
		write_slot(*p++);
		i += SLOT_SIZE;
	}

	remainder = pend_len % SLOT_SIZE;
	/* Pad to 4 bytes not touching caller's buffer */
	if (remainder) {
		memcpy(&tmp, p, remainder);
		write_slot(tmp);
	}

	host_gen_interrupt();

	/* Make sure nothing bad happened during transmission */
	if (!cse_ready())
		return 0;

	return pend_len;
}

/*
 * Send message msg of size len to host from host_addr to cse_addr.
 * Returns CSE_TX_RX_SUCCESS on success and other enum values on failure scenarios.
 * Also, in case of errors, heci_reset() is triggered.
 */
static enum cse_tx_rx_status
heci_send(const void *msg, size_t len, uint8_t host_addr, uint8_t client_addr)
{
	uint8_t retry;
	uint32_t csr, hdr;
	size_t sent, remaining, cb_size, max_length;
	const uint8_t *p;

	if (!msg || !len)
		return CSE_TX_ERR_INPUT;

	clear_int();

	for (retry = 0; retry < MAX_HECI_MESSAGE_RETRY_COUNT; retry++) {
		p = msg;

		if (!wait_heci_ready()) {
			printk(BIOS_ERR, "HECI: not ready\n");
			continue;
		}

		csr = read_host_csr();
		cb_size = ((csr & CSR_CBD) >> CSR_CBD_START) * SLOT_SIZE;
		/*
		 * Reserve one slot for the header. Limit max message
		 * length by 9 bits that are available in the header.
		 */
		max_length = MIN(cb_size, (1 << MEI_HDR_LENGTH_SIZE) - 1)
				- SLOT_SIZE;
		remaining = len;

		/*
		 * Fragment the message into smaller messages not exceeding
		 * useful circular buffer length. Mark last message complete.
		 */
		do {
			hdr = MIN(max_length, remaining)
				<< MEI_HDR_LENGTH_START;
			hdr |= client_addr << MEI_HDR_CSE_ADDR_START;
			hdr |= host_addr << MEI_HDR_HOST_ADDR_START;
			hdr |= (MIN(max_length, remaining) == remaining) ?
						MEI_HDR_IS_COMPLETE : 0;
			sent = send_one_message(hdr, p);
			p += sent;
			remaining -= sent;
		} while (remaining > 0 && sent != 0);

		if (!remaining)
			return CSE_TX_RX_SUCCESS;
	}

	printk(BIOS_DEBUG, "HECI: Trigger HECI reset\n");
	heci_reset();
	return CSE_TX_ERR_CSE_NOT_READY;
}

static enum cse_tx_rx_status
recv_one_message(uint32_t *hdr, void *buff, size_t maxlen, size_t *recv_len)
{
	uint32_t reg, *p = buff;
	size_t recv_slots, remainder, i;

	/* first get the header */
	if (!wait_read_slots(1))
		return CSE_RX_ERR_TIMEOUT;

	*hdr = read_slot();
	*recv_len = hdr_get_length(*hdr);

	if (!*recv_len)
		printk(BIOS_WARNING, "HECI: message is zero-sized\n");

	recv_slots = bytes_to_slots(*recv_len);

	i = 0;
	if (*recv_len > maxlen) {
		printk(BIOS_ERR, "HECI: response is too big\n");
		return CSE_RX_ERR_RESP_LEN_MISMATCH;
	}

	/* wait for the rest of messages to arrive */
	wait_read_slots(recv_slots);

	/* fetch whole slots first */
	while (i < ALIGN_DOWN(*recv_len, SLOT_SIZE)) {
		*p++ = read_slot();
		i += SLOT_SIZE;
	}

	/*
	 * If ME is not ready, something went wrong and
	 * we received junk
	 */
	if (!cse_ready())
		return CSE_RX_ERR_CSE_NOT_READY;

	remainder = *recv_len % SLOT_SIZE;

	if (remainder) {
		reg = read_slot();
		memcpy(p, &reg, remainder);
	}
	return CSE_TX_RX_SUCCESS;
}

/*
 * Receive message into buff not exceeding maxlen. Message is considered
 * successfully received if a 'complete' indication is read from ME side
 * and there was enough space in the buffer to fit that message. maxlen
 * is updated with size of message that was received.
 * Returns CSE_TX_RX_SUCCESS on success and other enum values on failure scenarios.
 * Also, in case of errors, heci_reset() is triggered.
 */
static enum cse_tx_rx_status heci_receive(void *buff, size_t *maxlen)
{
	uint8_t retry;
	size_t left, received;
	uint32_t hdr = 0;
	uint8_t *p;
	enum cse_tx_rx_status ret = CSE_RX_ERR_TIMEOUT;

	if (!buff || !maxlen || !*maxlen)
		return CSE_RX_ERR_INPUT;

	clear_int();

	for (retry = 0; retry < MAX_HECI_MESSAGE_RETRY_COUNT; retry++) {
		p = buff;
		left = *maxlen;

		if (!wait_heci_ready()) {
			printk(BIOS_ERR, "HECI: not ready\n");
			continue;
		}

		/*
		 * Receive multiple packets until we meet one marked
		 * complete or we run out of space in caller-provided buffer.
		 */
		do {
			ret = recv_one_message(&hdr, p, left, &received);
			if (ret) {
				printk(BIOS_ERR, "HECI: Failed to receive!\n");
				goto CSE_RX_ERR_HANDLE;
			}
			left -= received;
			p += received;
			/* If we read out everything ping to send more */
			if (!(hdr & MEI_HDR_IS_COMPLETE) && !cse_filled_slots())
				host_gen_interrupt();
		} while (received && !(hdr & MEI_HDR_IS_COMPLETE) && left > 0);

		if ((hdr & MEI_HDR_IS_COMPLETE) && received) {
			*maxlen = p - (uint8_t *)buff;
			return CSE_TX_RX_SUCCESS;
		}
	}

CSE_RX_ERR_HANDLE:
	printk(BIOS_DEBUG, "HECI: Trigger HECI Reset\n");
	heci_reset();
	return CSE_RX_ERR_CSE_NOT_READY;
}

enum cse_tx_rx_status heci_send_receive(const void *snd_msg, size_t snd_sz, void *rcv_msg,
					size_t *rcv_sz, uint8_t cse_addr)
{
	enum cse_tx_rx_status ret;

	ret = heci_send(snd_msg, snd_sz, BIOS_HOST_ADDR, cse_addr);
	if (ret) {
		printk(BIOS_ERR, "HECI: send Failed\n");
		return ret;
	}

	if (rcv_msg != NULL) {
		ret = heci_receive(rcv_msg, rcv_sz);
		if (ret) {
			printk(BIOS_ERR, "HECI: receive Failed\n");
			return ret;
		}
	}
	return ret;
}

/*
 * Attempt to reset the device. This is useful when host and ME are out
 * of sync during transmission or ME didn't understand the message.
 */
int heci_reset(void)
{
	uint32_t csr;

	/* Clear post code to prevent eventlog entry from unknown code. */
	post_code(POST_CODE_ZERO);

	/* Send reset request */
	csr = read_host_csr();
	csr |= (CSR_RESET | CSR_IG);
	write_host_csr(csr);

	if (wait_heci_ready()) {
		/* Device is back on its imaginary feet, clear reset */
		cse_set_host_ready();
		return 1;
	}

	printk(BIOS_CRIT, "HECI: reset failed\n");

	return 0;
}

bool is_cse_devfn_visible(unsigned int devfn)
{
	int slot = PCI_SLOT(devfn);
	int func = PCI_FUNC(devfn);

	if (!is_devfn_enabled(devfn)) {
		printk(BIOS_WARNING, "HECI: CSE device %02x.%01x is disabled\n", slot, func);
		return false;
	}

	if (pci_read_config16(PCI_DEV(0, slot, func), PCI_VENDOR_ID) == 0xFFFF) {
		printk(BIOS_WARNING, "HECI: CSE device %02x.%01x is hidden\n", slot, func);
		return false;
	}

	return true;
}

bool is_cse_enabled(void)
{
	return is_cse_devfn_visible(PCH_DEVFN_CSE);
}

uint32_t me_read_config32(int offset)
{
	return pci_read_config32(PCH_DEV_CSE, offset);
}

static bool cse_is_global_reset_allowed(void)
{
	/*
	 * Allow sending GLOBAL_RESET command only if:
	 *  - CSE's current working state is Normal and current operation mode is Normal.
	 *  - (or) CSE's current working state is normal and current operation mode can
	 *    be Soft Temp Disable or Security Override Mode if CSE's Firmware SKU is
	 *    Lite.
	 */
	if (!cse_is_hfs1_cws_normal())
		return false;

	if (cse_is_hfs1_com_normal())
		return true;

	if (cse_is_hfs3_fw_sku_lite()) {
		if (cse_is_hfs1_com_soft_temp_disable() || cse_is_hfs1_com_secover_mei_msg())
			return true;
	}
	return false;
}

/*
 * Sends GLOBAL_RESET_REQ cmd to CSE with reset type GLOBAL_RESET.
 * Returns 0 on failure and 1 on success.
 */
static int cse_request_reset(enum rst_req_type rst_type)
{
	int status;
	struct mkhi_hdr reply;
	struct reset_message {
		struct mkhi_hdr hdr;
		uint8_t req_origin;
		uint8_t reset_type;
	} __packed;
	struct reset_message msg = {
		.hdr = {
			.group_id = MKHI_GROUP_ID_CBM,
			.command = MKHI_CBM_GLOBAL_RESET_REQ,
		},
		.req_origin = GR_ORIGIN_BIOS_POST,
		.reset_type = rst_type
	};
	size_t reply_size;

	printk(BIOS_DEBUG, "HECI: Global Reset(Type:%d) Command\n", rst_type);

	if (!(rst_type == GLOBAL_RESET || rst_type == CSE_RESET_ONLY)) {
		printk(BIOS_ERR, "HECI: Unsupported reset type is requested\n");
		return 0;
	}

	if (!cse_is_global_reset_allowed() || !is_cse_enabled()) {
		printk(BIOS_ERR, "HECI: CSE does not meet required prerequisites\n");
		return 0;
	}

	heci_reset();

	reply_size = sizeof(reply);
	memset(&reply, 0, reply_size);

	if (rst_type == CSE_RESET_ONLY)
		status = heci_send(&msg, sizeof(msg), BIOS_HOST_ADDR, HECI_MKHI_ADDR);
	else
		status = heci_send_receive(&msg, sizeof(msg), &reply, &reply_size,
									HECI_MKHI_ADDR);

	printk(BIOS_DEBUG, "HECI: Global Reset %s!\n", !status ? "success" : "failure");
	return status;
}

int cse_request_global_reset(void)
{
	return cse_request_reset(GLOBAL_RESET);
}

static bool cse_is_hmrfpo_enable_allowed(void)
{
	/*
	 * Allow sending HMRFPO ENABLE command only if:
	 *  - CSE's current working state is Normal and current operation mode is Normal
	 *  - (or) cse's current working state is normal and current operation mode is
	 *    Soft Temp Disable if CSE's Firmware SKU is Lite
	 */
	if (!cse_is_hfs1_cws_normal())
		return false;

	if (cse_is_hfs1_com_normal())
		return true;

	if (cse_is_hfs3_fw_sku_lite() && cse_is_hfs1_com_soft_temp_disable())
		return true;

	return false;
}

/* Sends HMRFPO Enable command to CSE */
int cse_hmrfpo_enable(void)
{
	struct hmrfpo_enable_msg {
		struct mkhi_hdr hdr;
		uint32_t nonce[2];
	} __packed;

	/* HMRFPO Enable message */
	struct hmrfpo_enable_msg msg = {
		.hdr = {
			.group_id = MKHI_GROUP_ID_HMRFPO,
			.command = MKHI_HMRFPO_ENABLE,
		},
		.nonce = {0},
	};

	/* HMRFPO Enable response */
	struct hmrfpo_enable_resp {
		struct mkhi_hdr hdr;
		/* Base addr for factory data area, not relevant for client SKUs */
		uint32_t fct_base;
		/* Length of factory data area, not relevant for client SKUs */
		uint32_t fct_limit;
		uint8_t status;
		uint8_t reserved[3];
	} __packed;

	struct hmrfpo_enable_resp resp;
	size_t resp_size = sizeof(struct hmrfpo_enable_resp);

	if (cse_is_hfs1_com_secover_mei_msg()) {
		printk(BIOS_DEBUG, "HECI: CSE is already in security override mode, "
			       "skip sending HMRFPO_ENABLE command to CSE\n");
		return 1;
	}

	printk(BIOS_DEBUG, "HECI: Send HMRFPO Enable Command\n");

	if (!cse_is_hmrfpo_enable_allowed()) {
		printk(BIOS_ERR, "HECI: CSE does not meet required prerequisites\n");
		return 0;
	}

	if (heci_send_receive(&msg, sizeof(struct hmrfpo_enable_msg),
				&resp, &resp_size, HECI_MKHI_ADDR))
		return 0;

	if (resp.hdr.result) {
		printk(BIOS_ERR, "HECI: Resp Failed:%d\n", resp.hdr.result);
		return 0;
	}

	if (resp.status) {
		printk(BIOS_ERR, "HECI: HMRFPO_Enable Failed (resp status: %d)\n", resp.status);
		return 0;
	}

	return 1;
}

/*
 * Sends HMRFPO Get Status command to CSE to get the HMRFPO status.
 * The status can be DISABLED/LOCKED/ENABLED
 */
int cse_hmrfpo_get_status(void)
{
	struct hmrfpo_get_status_msg {
		struct mkhi_hdr hdr;
	} __packed;

	struct hmrfpo_get_status_resp {
		struct mkhi_hdr hdr;
		uint8_t status;
		uint8_t reserved[3];
	} __packed;

	struct hmrfpo_get_status_msg msg = {
		.hdr = {
			.group_id = MKHI_GROUP_ID_HMRFPO,
			.command = MKHI_HMRFPO_GET_STATUS,
		},
	};
	struct hmrfpo_get_status_resp resp;
	size_t resp_size = sizeof(struct hmrfpo_get_status_resp);

	printk(BIOS_INFO, "HECI: Sending Get HMRFPO Status Command\n");

	if (!cse_is_hfs1_cws_normal()) {
		printk(BIOS_ERR, "HECI: CSE's current working state is not Normal\n");
		return -1;
	}

	if (heci_send_receive(&msg, sizeof(struct hmrfpo_get_status_msg),
				&resp, &resp_size, HECI_MKHI_ADDR)) {
		printk(BIOS_ERR, "HECI: HMRFPO send/receive fail\n");
		return -1;
	}

	if (resp.hdr.result) {
		printk(BIOS_ERR, "HECI: HMRFPO Resp Failed:%d\n",
				resp.hdr.result);
		return -1;
	}

	return resp.status;
}

void print_me_fw_version(void *unused)
{
	struct me_fw_ver_resp resp = {0};

	/* Ignore if UART debugging is disabled */
	if (!CONFIG(CONSOLE_SERIAL))
		return;

	if (get_me_fw_version(&resp) == CB_SUCCESS) {
		printk(BIOS_DEBUG, "ME: Version: %d.%d.%d.%d\n", resp.code.major,
			resp.code.minor, resp.code.hotfix, resp.code.build);
		return;
	}
	printk(BIOS_DEBUG, "ME: Version: Unavailable\n");
}

enum cb_err get_me_fw_version(struct me_fw_ver_resp *resp)
{
	const struct mkhi_hdr fw_ver_msg = {
		.group_id = MKHI_GROUP_ID_GEN,
		.command = MKHI_GEN_GET_FW_VERSION,
	};

	if (resp == NULL) {
		printk(BIOS_ERR, "%s failed, null pointer parameter\n", __func__);
		return CB_ERR;
	}
	size_t resp_size = sizeof(*resp);

	/* Ignore if CSE is disabled */
	if (!is_cse_enabled())
		return CB_ERR;

	/*
	 * Ignore if ME Firmware SKU type is Lite since
	 * print_boot_partition_info() logs RO(BP1) and RW(BP2) versions.
	 */
	if (cse_is_hfs3_fw_sku_lite())
		return CB_ERR;

	/*
	 * Prerequisites:
	 * 1) HFSTS1 Current Working State is Normal
	 * 2) HFSTS1 Current Operation Mode is Normal
	 * 3) It's after DRAM INIT DONE message (taken care of by calling it
	 *    during ramstage
	 */
	if (!cse_is_hfs1_cws_normal() || !cse_is_hfs1_com_normal())
		return CB_ERR;

	heci_reset();

	if (heci_send_receive(&fw_ver_msg, sizeof(fw_ver_msg), resp, &resp_size,
									HECI_MKHI_ADDR))
		return CB_ERR;

	if (resp->hdr.result)
		return CB_ERR;


	return CB_SUCCESS;
}

void cse_trigger_vboot_recovery(enum csme_failure_reason reason)
{
	printk(BIOS_DEBUG, "cse: CSE status registers: HFSTS1: 0x%x, HFSTS2: 0x%x "
	       "HFSTS3: 0x%x\n", me_read_config32(PCI_ME_HFSTS1),
	       me_read_config32(PCI_ME_HFSTS2), me_read_config32(PCI_ME_HFSTS3));

	if (CONFIG(VBOOT))
		vboot_fail_and_reboot(vboot_get_context(), VB2_RECOVERY_INTEL_CSE_LITE_SKU,
				      reason);

	die("cse: Failed to trigger recovery mode(recovery subcode:%d)\n", reason);
}

static bool disable_cse_idle(pci_devfn_t dev)
{
	struct stopwatch sw;
	uint32_t dev_idle_ctrl = read_bar(dev, MMIO_CSE_DEVIDLE);
	dev_idle_ctrl &= ~CSE_DEV_IDLE;
	write_bar(dev, MMIO_CSE_DEVIDLE, dev_idle_ctrl);

	stopwatch_init_usecs_expire(&sw, HECI_CIP_TIMEOUT_US);
	do {
		dev_idle_ctrl = read_bar(dev, MMIO_CSE_DEVIDLE);
		if ((dev_idle_ctrl & CSE_DEV_CIP) == CSE_DEV_CIP)
			return true;
		udelay(HECI_DELAY_US);
	} while (!stopwatch_expired(&sw));

	return false;
}

static void enable_cse_idle(pci_devfn_t dev)
{
	uint32_t dev_idle_ctrl = read_bar(dev, MMIO_CSE_DEVIDLE);
	dev_idle_ctrl |= CSE_DEV_IDLE;
	write_bar(dev, MMIO_CSE_DEVIDLE, dev_idle_ctrl);
}

enum cse_device_state get_cse_device_state(unsigned int devfn)
{
	pci_devfn_t dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));
	uint32_t dev_idle_ctrl = read_bar(dev, MMIO_CSE_DEVIDLE);
	if ((dev_idle_ctrl & CSE_DEV_IDLE) == CSE_DEV_IDLE)
		return DEV_IDLE;

	return DEV_ACTIVE;
}

static enum cse_device_state ensure_cse_active(pci_devfn_t dev)
{
	if (!disable_cse_idle(dev))
		return DEV_IDLE;
	pci_or_config32(dev, PCI_COMMAND, PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	return DEV_ACTIVE;
}

static void ensure_cse_idle(pci_devfn_t dev)
{
	enable_cse_idle(dev);

	pci_and_config32(dev, PCI_COMMAND, ~(PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));
}

bool set_cse_device_state(unsigned int devfn, enum cse_device_state requested_state)
{
	enum cse_device_state current_state = get_cse_device_state(devfn);
	pci_devfn_t dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

	if (current_state == requested_state)
		return true;

	if (requested_state == DEV_ACTIVE)
		return ensure_cse_active(dev) == requested_state;
	else
		ensure_cse_idle(dev);

	return true;
}

void cse_set_to_d0i3(void)
{
	if (!is_cse_devfn_visible(PCH_DEVFN_CSE))
		return;

	set_cse_device_state(PCH_DEVFN_CSE, DEV_IDLE);
}

/* Function to set D0I3 for all HECI devices */
void heci_set_to_d0i3(void)
{
	for (int i = 0; i < CONFIG_MAX_HECI_DEVICES; i++) {
		unsigned int devfn = PCI_DEVFN(PCH_DEV_SLOT_CSE, i);
		if (!is_cse_devfn_visible(devfn))
			continue;

		set_cse_device_state(devfn, DEV_IDLE);
	}
}

/* Initialize the HECI devices. */
void heci_init(void)
{
	for (int i = 0; i < CONFIG_MAX_HECI_DEVICES; i++) {
		unsigned int devfn = PCI_DEVFN(PCH_DEV_SLOT_CSE, i);
		pci_devfn_t dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

		if (!is_cse_devfn_visible(devfn))
			continue;

		/* Assume it is already initialized, nothing else to do */
		if (get_cse_bar(dev))
			return;

		heci_assign_resource(dev, HECI1_BASE_ADDRESS + (i * HECI_BASE_SIZE));

		ensure_cse_active(dev);
	}
	/* Trigger HECI Reset and make Host ready for communication with CSE */
	heci_reset();
}

void cse_control_global_reset_lock(void)
{
	/*
	 * As per ME BWG recommendation the BIOS should not lock down CF9GR bit during
	 * manufacturing and re-manufacturing environment if HFSTS1 [4] is set. Note:
	 * this recommendation is not applicable for CSE-Lite SKUs where BIOS should set
	 * CF9LOCK bit irrespectively.
	 *
	 * Other than that, make sure payload/OS can't trigger global reset.
	 *
	 * BIOS must also ensure that CF9GR is cleared and locked (Bit31 of ETR3)
	 * prior to transferring control to the OS.
	 */
	if (CONFIG(SOC_INTEL_CSE_LITE_SKU) || cse_is_hfs1_spi_protected())
		pmc_global_reset_disable_and_lock();
	else
		pmc_global_reset_enable(false);
}

#if ENV_RAMSTAGE

/*
 * Disable the Intel (CS)Management Engine via HECI based on a cmos value
 * of `me_state`. A value of `0` will result in a (CS)ME state of `0` (working)
 * and value of `1` will result in a (CS)ME state of `3` (disabled).
 *
 * It isn't advised to use this in combination with me_cleaner.
 *
 * It is advisable to have a second cmos option called `me_state_counter`.
 * Whilst not essential, it avoid reboots loops if the (CS)ME fails to
 * change states after 3 attempts. Some versions of the (CS)ME need to be
 * reset 3 times.
 *
 * Ideal cmos values would be:
 *
 * # coreboot config options: cpu
 * 432     1       e       5       me_state
 * 440     4       h       0       me_state_counter
 *
 * #ID     value   text
 * 5       0       Enable
 * 5       1       Disable
 */

static void me_reset_with_count(void)
{
	unsigned int cmos_me_state_counter = get_uint_option("me_state_counter", UINT_MAX);

	if (cmos_me_state_counter != UINT_MAX) {
		printk(BIOS_DEBUG, "CMOS: me_state_counter = %u\n", cmos_me_state_counter);
		/* Avoid boot loops by only trying a state change 3 times */
		if (cmos_me_state_counter < ME_DISABLE_ATTEMPTS) {
			cmos_me_state_counter++;
			set_uint_option("me_state_counter", cmos_me_state_counter);
			printk(BIOS_DEBUG, "ME: Reset attempt %u/%u.\n", cmos_me_state_counter,
									 ME_DISABLE_ATTEMPTS);
			do_global_reset();
		} else {
			/*
			 * If the (CS)ME fails to change states after 3 attempts, it will
			 * likely need a cold boot, or recovering.
			 */
			printk(BIOS_ERR, "Failed to change ME state in %u attempts!\n",
									 ME_DISABLE_ATTEMPTS);

		}
	} else {
		printk(BIOS_DEBUG, "ME: Resetting");
		do_global_reset();
	}
}

static void cse_set_state(struct device *dev)
{

	/* (CS)ME Disable Command */
	struct me_disable_command {
		struct mkhi_hdr hdr;
		uint32_t rule_id;
		uint8_t rule_len;
		uint32_t rule_data;
	} __packed me_disable = {
		.hdr = {
			.group_id = MKHI_GROUP_ID_FWCAPS,
			.command = MKHI_SET_ME_DISABLE,
		},
		.rule_id = ME_DISABLE_RULE_ID,
		.rule_len = ME_DISABLE_RULE_LENGTH,
		.rule_data = ME_DISABLE_COMMAND,
	};

	struct me_disable_reply {
		struct mkhi_hdr hdr;
		uint32_t rule_id;
	} __packed;

	struct me_disable_reply disable_reply;

	size_t disable_reply_size;

	/* (CS)ME Enable Command */
	struct me_enable_command {
		struct mkhi_hdr hdr;
	} me_enable = {
		.hdr = {
			.group_id = MKHI_GROUP_ID_BUP_COMMON,
			.command = MKHI_SET_ME_ENABLE,
		},
	};

	struct me_enable_reply {
		struct mkhi_hdr hdr;
	} __packed;

	struct me_enable_reply enable_reply;

	size_t enable_reply_size;

	/* Function Start */

	int send;
	int result;
	/*
	 * Check if the CMOS value "me_state" exists, if it doesn't, then
	 * don't do anything.
	 */
	const unsigned int cmos_me_state = get_uint_option("me_state", UINT_MAX);

	if (cmos_me_state == UINT_MAX)
		return;

	printk(BIOS_DEBUG, "CMOS: me_state = %u\n", cmos_me_state);

	/*
	 * We only take action if the me_state doesn't match the CS(ME) working state
	 */

	const unsigned int soft_temp_disable = cse_is_hfs1_com_soft_temp_disable();

	if (cmos_me_state && !soft_temp_disable) {
		/* me_state should be disabled, but it's enabled */
		printk(BIOS_DEBUG, "ME needs to be disabled.\n");
		send = heci_send_receive(&me_disable, sizeof(me_disable),
			&disable_reply, &disable_reply_size, HECI_MKHI_ADDR);
		result = disable_reply.hdr.result;
	} else if (!cmos_me_state && soft_temp_disable) {
		/* me_state should be enabled, but it's disabled */
		printk(BIOS_DEBUG, "ME needs to be enabled.\n");
		send = heci_send_receive(&me_enable, sizeof(me_enable),
			&enable_reply, &enable_reply_size, HECI_MKHI_ADDR);
		result = enable_reply.hdr.result;
	} else {
		printk(BIOS_DEBUG, "ME is %s.\n", cmos_me_state ? "disabled" : "enabled");
		unsigned int cmos_me_state_counter = get_uint_option("me_state_counter",
								 UINT_MAX);
		/* set me_state_counter to 0 */
		if ((cmos_me_state_counter != UINT_MAX && cmos_me_state_counter != 0))
			set_uint_option("me_state_counter", 0);
		return;
	}

	printk(BIOS_DEBUG, "HECI: ME state change send %s!\n",
							!send ? "success" : "failure");
	printk(BIOS_DEBUG, "HECI: ME state change result %s!\n",
							result ? "success" : "failure");

	/*
	 * Reset if the result was successful, or if the send failed as some older
	 * version of the Intel (CS)ME won't successfully receive the message unless reset
	 * twice.
	 */
	if (send || !result)
		me_reset_with_count();
}

/*
 * `cse_final_ready_to_boot` function is native implementation of equivalent events
 * performed by FSP NotifyPhase(Ready To Boot) API invocations.
 *
 * Operations are:
 * 1. Perform global reset lock.
 * 2. Put HECI1 to D0i3 and disable the HECI1 if the user selects
 *      DISABLE_HECI1_AT_PRE_BOOT config or CSE HFSTS1 Operation Mode is
 *      `Software Temporary Disable`.
 */
static void cse_final_ready_to_boot(void)
{
	cse_control_global_reset_lock();

	if (CONFIG(DISABLE_HECI1_AT_PRE_BOOT) || cse_is_hfs1_com_soft_temp_disable()) {
		cse_set_to_d0i3();
		heci1_disable();
	}
}

/*
 * `cse_final_end_of_firmware` function is native implementation of equivalent events
 * performed by FSP NotifyPhase(End of Firmware) API invocations.
 *
 * Operations are:
 * 1. Set D0I3 for all HECI devices.
 */
static void cse_final_end_of_firmware(void)
{
	heci_set_to_d0i3();
}

/*
 * `cse_final` function is native implementation of equivalent events performed by
 * each FSP NotifyPhase() API invocations.
 */
static void cse_final(struct device *dev)
{
	/* SoC user decided to send EOP late */
	if (CONFIG(SOC_INTEL_CSE_SEND_EOP_LATE))
		return;

	/* 1. Send EOP to CSE if not done.*/
	if (CONFIG(SOC_INTEL_CSE_SET_EOP))
		cse_send_end_of_post();

	if (!CONFIG(USE_FSP_NOTIFY_PHASE_READY_TO_BOOT))
		cse_final_ready_to_boot();

	if (!CONFIG(USE_FSP_NOTIFY_PHASE_END_OF_FIRMWARE))
		cse_final_end_of_firmware();
}

struct device_operations cse_ops = {
	.set_resources		= pci_dev_set_resources,
	.read_resources		= pci_dev_read_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
	.enable			= cse_set_state,
	.final			= cse_final,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_CSE0,
	PCI_DID_INTEL_APL_CSE0,
	PCI_DID_INTEL_GLK_CSE0,
	PCI_DID_INTEL_CNL_CSE0,
	PCI_DID_INTEL_LWB_CSE0,
	PCI_DID_INTEL_LWB_CSE0_SUPER,
	PCI_DID_INTEL_CNP_H_CSE0,
	PCI_DID_INTEL_ICL_CSE0,
	PCI_DID_INTEL_CMP_CSE0,
	PCI_DID_INTEL_CMP_H_CSE0,
	PCI_DID_INTEL_TGL_CSE0,
	PCI_DID_INTEL_TGL_H_CSE0,
	PCI_DID_INTEL_MCC_CSE0,
	PCI_DID_INTEL_MCC_CSE1,
	PCI_DID_INTEL_MCC_CSE2,
	PCI_DID_INTEL_MCC_CSE3,
	PCI_DID_INTEL_JSP_CSE0,
	PCI_DID_INTEL_JSP_CSE1,
	PCI_DID_INTEL_JSP_CSE2,
	PCI_DID_INTEL_JSP_CSE3,
	PCI_DID_INTEL_ADP_P_CSE0,
	PCI_DID_INTEL_ADP_P_CSE1,
	PCI_DID_INTEL_ADP_P_CSE2,
	PCI_DID_INTEL_ADP_P_CSE3,
	PCI_DID_INTEL_ADP_S_CSE0,
	PCI_DID_INTEL_ADP_S_CSE1,
	PCI_DID_INTEL_ADP_S_CSE2,
	PCI_DID_INTEL_ADP_S_CSE3,
	PCI_DID_INTEL_ADP_M_CSE0,
	PCI_DID_INTEL_ADP_M_CSE1,
	PCI_DID_INTEL_ADP_M_CSE2,
	PCI_DID_INTEL_ADP_M_CSE3,
	0,
};

static const struct pci_driver cse_driver __pci_driver = {
	.ops			= &cse_ops,
	.vendor			= PCI_VID_INTEL,
	/* SoC/chipset needs to provide PCI device ID */
	.devices		= pci_device_ids
};

#endif
