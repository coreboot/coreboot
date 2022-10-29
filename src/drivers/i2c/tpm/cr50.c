/* SPDX-License-Identifier: GPL-2.0-only */

/* Based on Linux Kernel TPM driver */

/*
 * cr50 is a TPM 2.0 capable device that requires special
 * handling for the I2C interface.
 *
 * - Use an interrupt for transaction status instead of hardcoded delays
 * - Must use write+wait+read read protocol
 * - All 4 bytes of status register must be read/written at once
 * - Burst count max is 63 bytes, and burst count behaves
 *   slightly differently than other I2C TPMs
 * - When reading from FIFO the full burstcnt must be read
 *   instead of just reading header and determining the remainder
 */

#include <commonlib/endian.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <drivers/tpm/cr50.h>
#include <endian.h>
#include <security/tpm/tis.h>
#include <string.h>
#include <types.h>
#include <timer.h>

#include "tpm.h"

#define CR50_MAX_BUFSIZE	63
#define CR50_TIMEOUT_INIT_MS	30000	/* Very long timeout for TPM init */
#define CR50_TIMEOUT_LONG_MS	2000	/* Long timeout while waiting for TPM */
#define CR50_TIMEOUT_SHORT_MS	2	/* Short timeout during transactions */
#define CR50_TIMEOUT_NOIRQ_MS	20	/* Timeout for TPM ready without IRQ */
#define CR50_DID_VID		0x00281ae0L
#define TI50_DID_VID		0x504a6666L

struct tpm_inf_dev {
	int bus;
	int locality;
	unsigned int addr;
	uint8_t buf[CR50_MAX_BUFSIZE + sizeof(uint8_t)];
};

static struct tpm_inf_dev tpm_dev;

__weak int tis_plat_irq_status(void)
{
	static int warning_displayed;

	if (!warning_displayed) {
		printk(BIOS_WARNING, "%s() not implemented, wasting 20ms to wait on"
		       " Cr50!\n", __func__);
		warning_displayed = 1;
	}
	mdelay(CR50_TIMEOUT_NOIRQ_MS);

	return 1;
}

/*
 * cr50_i2c_read() - read from TPM register
 *
 * @addr: register address to read from
 * @buffer: provided by caller
 * @len: number of bytes to read
 *
 * 1) send register address byte 'addr' to the TPM
 * 2) wait for TPM to indicate it is ready
 * 3) read 'len' bytes of TPM response into the provided 'buffer'
 *
 * Return -1 on error, 0 on success.
 */
static int cr50_i2c_read(uint8_t addr, uint8_t *buffer, size_t len)
{
	if (tpm_dev.addr == 0)
		return -1;

	/* Clear interrupt before starting transaction */
	tis_plat_irq_status();

	/* Send the register address byte to the TPM */
	if (i2c_write_raw(tpm_dev.bus, tpm_dev.addr, &addr, 1)) {
		printk(BIOS_ERR, "%s: Address write failed\n", __func__);
		return -1;
	}

	/* Wait for TPM to be ready with response data */
	if (cr50_wait_tpm_ready() != CB_SUCCESS)
		return -1;

	/* Read response data from the TPM */
	if (i2c_read_raw(tpm_dev.bus, tpm_dev.addr, buffer, len)) {
		printk(BIOS_ERR, "%s: Read response failed\n", __func__);
		return -1;
	}

	return 0;
}

/*
 * cr50_i2c_write() - write to TPM register
 *
 * @addr: register address to write to
 * @buffer: data to write
 * @len: number of bytes to write
 *
 * 1) prepend the provided address to the provided data
 * 2) send the address+data to the TPM
 * 3) wait for TPM to indicate it is done writing
 *
 * Returns -1 on error, 0 on success.
 */
static int cr50_i2c_write(uint8_t addr, const uint8_t *buffer, size_t len)
{
	if (tpm_dev.addr == 0)
		return -1;
	if (len > CR50_MAX_BUFSIZE)
		return -1;

	/* Prepend the 'register address' to the buffer */
	tpm_dev.buf[0] = addr;
	memcpy(tpm_dev.buf + 1, buffer, len);

	/* Clear interrupt before starting transaction */
	tis_plat_irq_status();

	/* Send write request buffer with address */
	if (i2c_write_raw(tpm_dev.bus, tpm_dev.addr, tpm_dev.buf, len + 1)) {
		printk(BIOS_ERR, "%s: Error writing to TPM\n", __func__);
		return -1;
	}

	/* Wait for TPM to be ready */
	return cr50_wait_tpm_ready() == CB_SUCCESS ? 0 : -1;
}

/*
 * Cr50 processes reset requests asynchronously and consceivably could be busy
 * executing a long command and not reacting to the reset pulse for a while.
 *
 * This function will make sure that the AP does not proceed with boot until
 * TPM finished reset processing.
 */
static int process_reset(void)
{
	struct stopwatch sw;
	int rv = 0;
	uint8_t access;

	/*
	 * Locality is released by TPM reset.
	 *
	 * If locality is taken at this point, this could be due to the fact
	 * that the TPM is performing a long operation and has not processed
	 * reset request yet. We'll wait up to CR50_TIMEOUT_INIT_MS and see if
	 * it releases locality when reset is processed.
	 */
	stopwatch_init_msecs_expire(&sw, CR50_TIMEOUT_INIT_MS);
	do {
		const uint8_t mask =
			TPM_ACCESS_VALID | TPM_ACCESS_ACTIVE_LOCALITY;

		rv = cr50_i2c_read(TPM_ACCESS(0),
				   &access, sizeof(access));
		if (rv || ((access & mask) == mask)) {
			/*
			 * Don't bombard the chip with traffic, let it keep
			 * processing the command.
			 */
			mdelay(2);
			continue;
		}

		printk(BIOS_INFO, "TPM ready after %lld ms\n",
		       stopwatch_duration_msecs(&sw));

		return 0;
	} while (!stopwatch_expired(&sw));

	if (rv)
		printk(BIOS_ERR, "Failed to read TPM\n");
	else
		printk(BIOS_ERR,
			"TPM failed to reset after %lld ms, status: %#x\n",
			stopwatch_duration_msecs(&sw), access);

	return -1;
}

/*
 * Locality could be already claimed (if this is a later coreboot stage and
 * the RO did not release it), or not yet claimed, if this is verstage or the
 * older RO did release it.
 */
static int claim_locality(void)
{
	uint8_t access;
	const uint8_t mask = TPM_ACCESS_VALID | TPM_ACCESS_ACTIVE_LOCALITY;

	if (cr50_i2c_read(TPM_ACCESS(0), &access, sizeof(access)))
		return -1;

	if ((access & mask) == mask) {
		printk(BIOS_INFO, "Locality already claimed\n");
		return 0;
	}

	access = TPM_ACCESS_REQUEST_USE;
	if (cr50_i2c_write(TPM_ACCESS(0),
			   &access, sizeof(access)))
		return -1;

	if (cr50_i2c_read(TPM_ACCESS(0), &access, sizeof(access)))
		return -1;

	if ((access & mask) != mask) {
		printk(BIOS_INFO, "Failed to claim locality.\n");
		return -1;
	}

	return 0;
}

/* cr50 requires all 4 bytes of status register to be read */
static uint8_t cr50_i2c_tis_status(void)
{
	uint8_t buf[4];
	if (cr50_i2c_read(TPM_STS(tpm_dev.locality), buf, sizeof(buf)) < 0) {
		printk(BIOS_ERR, "%s: Failed to read status\n", __func__);
		return 0;
	}
	return buf[0];
}

/* cr50 requires all 4 bytes of status register to be written */
static void cr50_i2c_tis_ready(void)
{
	uint8_t buf[4] = { TPM_STS_COMMAND_READY };
	cr50_i2c_write(TPM_STS(tpm_dev.locality), buf, sizeof(buf));
	mdelay(CR50_TIMEOUT_SHORT_MS);
}

/* cr50 uses bytes 3:2 of status register for burst count and
 * all 4 bytes must be read */
static int cr50_i2c_wait_burststs(uint8_t mask, size_t *burst, int *status)
{
	uint8_t buf[4];
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, CR50_TIMEOUT_LONG_MS);

	while (!stopwatch_expired(&sw)) {
		if (cr50_i2c_read(TPM_STS(tpm_dev.locality), buf, sizeof(buf)) != 0) {
			mdelay(CR50_TIMEOUT_SHORT_MS);
			continue;
		}

		*status = buf[0];
		*burst = read_le16(&buf[1]);

		/* Check if mask matches and burst is valid */
		if ((*status & mask) == mask &&
		    *burst > 0 && *burst <= CR50_MAX_BUFSIZE)
			return 0;

		mdelay(CR50_TIMEOUT_SHORT_MS);
	}

	printk(BIOS_ERR, "%s: Timeout reading burst and status\n", __func__);
	return -1;
}

static int cr50_i2c_tis_recv(uint8_t *buf, size_t buf_len)
{
	size_t burstcnt, current, len, expected;
	uint8_t addr = TPM_DATA_FIFO(tpm_dev.locality);
	uint8_t mask = TPM_STS_VALID | TPM_STS_DATA_AVAIL;
	int status;

	if (buf_len < TPM_HEADER_SIZE)
		goto out_err;

	if (cr50_i2c_wait_burststs(mask, &burstcnt, &status) < 0) {
		printk(BIOS_ERR, "%s: First chunk not available\n", __func__);
		goto out_err;
	}

	/* Read first chunk of burstcnt bytes */
	if (cr50_i2c_read(addr, buf, burstcnt) != 0) {
		printk(BIOS_ERR, "%s: Read failed\n", __func__);
		goto out_err;
	}

	/* Determine expected data in the return buffer */
	expected = read_be32(buf + TPM_RSP_SIZE_BYTE);
	if (expected > buf_len) {
		printk(BIOS_ERR, "%s: Too much data: %zu > %zu\n",
		       __func__, expected, buf_len);
		goto out_err;
	}

	/* Now read the rest of the data */
	current = burstcnt;
	while (current < expected) {
		/* Read updated burst count and check status */
		if (cr50_i2c_wait_burststs(mask, &burstcnt, &status) < 0)
			goto out_err;

		len = MIN(burstcnt, expected - current);
		if (cr50_i2c_read(addr, buf + current, len) != 0) {
			printk(BIOS_ERR, "%s: Read failed\n", __func__);
			goto out_err;
		}

		current += len;
	}

	/* Ensure TPM is done reading data */
	if (cr50_i2c_wait_burststs(TPM_STS_VALID, &burstcnt, &status) < 0)
		goto out_err;
	if (status & TPM_STS_DATA_AVAIL) {
		printk(BIOS_ERR, "%s: Data still available\n", __func__);
		goto out_err;
	}

	return current;

out_err:
	/* Abort current transaction if still pending */
	if (cr50_i2c_tis_status() & TPM_STS_COMMAND_READY)
		cr50_i2c_tis_ready();
	return -1;
}

static int cr50_i2c_tis_send(uint8_t *buf, size_t len)
{
	int status;
	size_t burstcnt, limit, sent = 0;
	uint8_t tpm_go[4] = { TPM_STS_GO };
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, CR50_TIMEOUT_LONG_MS);

	/* Wait until TPM is ready for a command */
	while (!(cr50_i2c_tis_status() & TPM_STS_COMMAND_READY)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "%s: Command ready timeout\n",
			       __func__);
			return -1;
		}

		cr50_i2c_tis_ready();
	}

	while (len > 0) {
		uint8_t mask = TPM_STS_VALID;

		/* Wait for data if this is not the first chunk */
		if (sent > 0)
			mask |= TPM_STS_DATA_EXPECT;

		/* Read burst count and check status */
		if (cr50_i2c_wait_burststs(mask, &burstcnt, &status) < 0)
			goto out_err;

		/* Use burstcnt - 1 to account for the address byte
		 * that is inserted by cr50_i2c_write() */
		limit = MIN(burstcnt - 1, len);
		if (cr50_i2c_write(TPM_DATA_FIFO(tpm_dev.locality), &buf[sent], limit) != 0) {
			printk(BIOS_ERR, "%s: Write failed\n", __func__);
			goto out_err;
		}

		sent += limit;
		len -= limit;
	}

	/* Ensure TPM is not expecting more data */
	if (cr50_i2c_wait_burststs(TPM_STS_VALID, &burstcnt, &status) < 0)
		goto out_err;
	if (status & TPM_STS_DATA_EXPECT) {
		printk(BIOS_ERR, "%s: Data still expected\n", __func__);
		goto out_err;
	}

	/* Start the TPM command */
	if (cr50_i2c_write(TPM_STS(tpm_dev.locality), tpm_go, sizeof(tpm_go)) < 0) {
		printk(BIOS_ERR, "%s: Start command failed\n", __func__);
		goto out_err;
	}
	return sent;

out_err:
	/* Abort current transaction if still pending */
	if (cr50_i2c_tis_status() & TPM_STS_COMMAND_READY)
		cr50_i2c_tis_ready();
	return -1;
}

static void cr50_vendor_init(struct tpm_chip *chip)
{
	chip->req_complete_mask = TPM_STS_DATA_AVAIL | TPM_STS_VALID;
	chip->req_complete_val = TPM_STS_DATA_AVAIL | TPM_STS_VALID;
	chip->req_canceled = TPM_STS_COMMAND_READY;
	chip->status = &cr50_i2c_tis_status;
	chip->recv = &cr50_i2c_tis_recv;
	chip->send = &cr50_i2c_tis_send;
	chip->cancel = &cr50_i2c_tis_ready;
}

int tpm_vendor_probe(unsigned int bus, uint32_t addr)
{
	return 0;
}

static int cr50_i2c_probe(uint32_t *did_vid)
{
	int retries;

	/*
	 * 1s should be enough to synchronize with the TPM even under the
	 * worst nested reset request conditions. In vast majority of cases
	 * there would be no wait at all. If this probe fails, boot likely
	 * cannot proceed, so an extra long timeout is appropriate.
	 */
	printk(BIOS_INFO, "Probing TPM I2C: ");

	for (retries = 100; retries > 0; retries--) {
		int rc;

		rc = cr50_i2c_read(TPM_DID_VID(0), (uint8_t *)did_vid, 4);

		/* Exit once DID and VID verified */
		if (!rc && (*did_vid == CR50_DID_VID || *did_vid == TI50_DID_VID)) {
			printk(BIOS_INFO, "done! DID_VID 0x%08x\n", *did_vid);
			return 0;
		}

		/* TPM might be resetting, let's retry in a bit. */
		mdelay(10);
		printk(BIOS_INFO, ".");
	}

	/*
	 * I2C reads failed, or the DID and VID didn't match
	 */
	printk(BIOS_ERR, "DID_VID 0x%08x not recognized\n", *did_vid);
	return -1;
}

int tpm_vendor_init(struct tpm_chip *chip, unsigned int bus, uint32_t dev_addr)
{
	uint32_t did_vid = 0;

	if (dev_addr == 0) {
		printk(BIOS_ERR, "%s: missing device address\n", __func__);
		return -1;
	}

	tpm_dev.bus = bus;
	tpm_dev.addr = dev_addr;

	cr50_vendor_init(chip);

	if (cr50_i2c_probe(&did_vid))
		return -1;

	if (ENV_SEPARATE_VERSTAGE || ENV_BOOTBLOCK)
		if (process_reset())
			return -1;

	if (claim_locality())
		return -1;

	printk(BIOS_DEBUG, "cr50 TPM 2.0 (i2c %u:0x%02x id 0x%x)\n",
	       bus, dev_addr, did_vid >> 16);

	if (tpm_first_access_this_boot()) {
		/* This is called for the side-effect of printing the version string. */
		cr50_get_firmware_version(NULL);
                cr50_set_board_cfg();
	}

	chip->is_open = 1;
	return 0;
}

enum cb_err tis_vendor_write(unsigned int addr, const void *buffer, size_t bytes)
{
	return cr50_i2c_write(addr & 0xff, buffer, bytes) ? CB_ERR : CB_SUCCESS;
}

enum cb_err tis_vendor_read(unsigned int addr, void *buffer, size_t bytes)
{
	return cr50_i2c_read(addr & 0xff, buffer, bytes) ? CB_ERR : CB_SUCCESS;
}
