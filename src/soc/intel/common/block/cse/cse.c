/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/early_variables.h>
#include <assert.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/cse.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <string.h>
#include <timer.h>

#define MAX_HECI_MESSAGE_RETRY_COUNT 5

/* Wait up to 15 sec for HECI to get ready */
#define HECI_DELAY_READY	(15 * 1000)
/* Wait up to 100 usec between circular buffer polls */
#define HECI_DELAY		100
/* Wait up to 5 sec for CSE to chew something we sent */
#define HECI_SEND_TIMEOUT	(5 * 1000)
/* Wait up to 5 sec for CSE to blurp a reply */
#define HECI_READ_TIMEOUT	(5 * 1000)

#define SLOT_SIZE		sizeof(uint32_t)

#define MMIO_CSE_CB_WW		0x00
#define MMIO_HOST_CSR		0x04
#define MMIO_CSE_CB_RW		0x08
#define MMIO_CSE_CSR		0x0c

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


struct cse_device {
	uintptr_t sec_bar;
} g_cse CAR_GLOBAL;

/*
 * Initialize the device with provided temporary BAR. If BAR is 0 use a
 * default. This is intended for pre-mem usage only where BARs haven't been
 * assigned yet and devices are not enabled.
 */
void heci_init(uintptr_t tempbar)
{
	struct cse_device *cse = car_get_var_ptr(&g_cse);
	device_t dev = PCH_DEV_CSE;
	u8 pcireg;

	/* Assume it is already initialized, nothing else to do */
	if (cse->sec_bar)
		return;

	/* Use default pre-ram bar */
	if (!tempbar)
		tempbar = HECI1_BASE_ADDRESS;

	/* Assign Resources to HECI1 */
	/* Clear BIT 1-2 of Command Register */
	pcireg = pci_read_config8(dev, PCI_COMMAND);
	pcireg &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pci_write_config8(dev, PCI_COMMAND, pcireg);

	/* Program Temporary BAR for HECI1 */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, tempbar);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, 0x0);

	/* Enable Bus Master and MMIO Space */
	pcireg = pci_read_config8(dev, PCI_COMMAND);
	pcireg |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config8(dev, PCI_COMMAND, pcireg);

	cse->sec_bar = tempbar;
}

/* Get HECI BAR 0 from PCI configuration space */
static uint32_t get_cse_bar(void)
{
	uintptr_t bar;

	bar = pci_read_config32(PCH_DEV_CSE, PCI_BASE_ADDRESS_0);
	assert(bar != 0);
	/*
	 * Bits 31-12 are the base address as per EDS for SPI,
	 * Don't care about 0-11 bit
	 */
	return bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
}

static uint32_t read_bar(uint32_t offset)
{
	struct cse_device *cse = car_get_var_ptr(&g_cse);
	/* Reach PCI config space to get BAR in case CAR global not available */
	if (!cse->sec_bar)
		cse->sec_bar = get_cse_bar();
	return read32((void *)(cse->sec_bar + offset));
}

static void write_bar(uint32_t offset, uint32_t val)
{
	struct cse_device *cse = car_get_var_ptr(&g_cse);
	/* Reach PCI config space to get BAR in case CAR global not available */
	if (!cse->sec_bar)
		cse->sec_bar = get_cse_bar();
	return write32((void *)(cse->sec_bar + offset), val);
}

static uint32_t read_cse_csr(void)
{
	return read_bar(MMIO_CSE_CSR);
}

static uint32_t read_host_csr(void)
{
	return read_bar(MMIO_HOST_CSR);
}

static void write_host_csr(uint32_t data)
{
	write_bar(MMIO_HOST_CSR, data);
}

static size_t filled_slots(uint32_t data)
{
	uint8_t wp, rp;
	rp = data >> CSR_RP_START;
	wp = data >> CSR_WP_START;
	return (uint8_t) (wp - rp);
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
	return read_bar(MMIO_CSE_CB_RW);
}

static void write_slot(uint32_t val)
{
	write_bar(MMIO_CSE_CB_WW, val);
}

static int wait_write_slots(size_t cnt)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, HECI_SEND_TIMEOUT);
	while (host_empty_slots() < cnt) {
		udelay(HECI_DELAY);
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

	stopwatch_init_msecs_expire(&sw, HECI_READ_TIMEOUT);
	while (cse_filled_slots() < cnt) {
		udelay(HECI_DELAY);
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

static int wait_heci_ready(void)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, HECI_DELAY_READY);
	while (!cse_ready()) {
		udelay(HECI_DELAY);
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

int
heci_send(const void *msg, size_t len, uint8_t host_addr, uint8_t client_addr)
{
	uint8_t retry;
	uint32_t csr, hdr;
	size_t sent, remaining, cb_size, max_length;
	const uint8_t *p;

	if (!msg || !len)
		return 0;

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
			return 1;
	}
	return 0;
}

static size_t
recv_one_message(uint32_t *hdr, void *buff, size_t maxlen)
{
	uint32_t reg, *p = buff;
	size_t recv_slots, recv_len, remainder, i;

	/* first get the header */
	if (!wait_read_slots(1))
		return 0;

	*hdr = read_slot();
	recv_len = hdr_get_length(*hdr);

	if (!recv_len)
		printk(BIOS_WARNING, "HECI: message is zero-sized\n");

	recv_slots = bytes_to_slots(recv_len);

	i = 0;
	if (recv_len > maxlen) {
		printk(BIOS_ERR, "HECI: response is too big\n");
		return 0;
	}

	/* wait for the rest of messages to arrive */
	wait_read_slots(recv_slots);

	/* fetch whole slots first */
	while (i < ALIGN_DOWN(recv_len, SLOT_SIZE)) {
		*p++ = read_slot();
		i += SLOT_SIZE;
	}

	/*
	 * If ME is not ready, something went wrong and
	 * we received junk
	 */
	if (!cse_ready())
		return 0;

	remainder = recv_len % SLOT_SIZE;

	if (remainder) {
		reg = read_slot();
		memcpy(p, &reg, remainder);
	}

	return recv_len;
}

int heci_receive(void *buff, size_t *maxlen)
{
	uint8_t retry;
	size_t left, received;
	uint32_t hdr = 0;
	uint8_t *p;

	if (!buff || !maxlen || !*maxlen)
		return 0;

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
			received = recv_one_message(&hdr, p, left);
			if (!received) {
				printk(BIOS_ERR, "HECI: Failed to recieve!\n");
				return 0;
			}
			left -= received;
			p += received;
			/* If we read out everything ping to send more */
			if (!(hdr & MEI_HDR_IS_COMPLETE) && !cse_filled_slots())
				host_gen_interrupt();
		} while (received && !(hdr & MEI_HDR_IS_COMPLETE) && left > 0);

		if ((hdr & MEI_HDR_IS_COMPLETE) && received) {
			*maxlen = p - (uint8_t *) buff;
			return 1;
		}
	}
	return 0;
}

/*
 * Attempt to reset the device. This is useful when host and ME are out
 * of sync during transmission or ME didn't understand the message.
 */
int heci_reset(void)
{
	uint32_t csr;

	/* Send reset request */
	csr = read_host_csr();
	csr |= CSR_RESET;
	csr |= CSR_IG;
	write_host_csr(csr);

	if (wait_heci_ready()) {
		/* Device is back on its imaginary feet, clear reset */
		csr = read_host_csr();
		csr &= ~CSR_RESET;
		csr |= CSR_IG;
		csr |= CSR_READY;
		write_host_csr(csr);
		return 1;
	}

	printk(BIOS_CRIT, "HECI: reset failed\n");

	return 0;
}

#if ENV_RAMSTAGE

static void update_sec_bar(struct device *dev)
{
	g_cse.sec_bar = find_resource(dev, PCI_BASE_ADDRESS_0)->base;
}

static void cse_set_resources(struct device *dev)
{
	if (dev->path.pci.devfn == PCH_DEVFN_CSE)
		update_sec_bar(dev);

	pci_dev_set_resources(dev);
}

static struct device_operations cse_ops = {
	.set_resources		= cse_set_resources,
	.read_resources		= pci_dev_read_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_CSE0,
	PCI_DEVICE_ID_INTEL_GLK_CSE0,
	PCI_DEVICE_ID_INTEL_CNL_CSE0,
	PCI_DEVICE_ID_INTEL_SKL_CSE0,
	0,
};

static const struct pci_driver cse_driver __pci_driver = {
	.ops			= &cse_ops,
	.vendor			= PCI_VENDOR_ID_INTEL,
	/* SoC/chipset needs to provide PCI device ID */
	.devices		= pci_device_ids
};

#endif
