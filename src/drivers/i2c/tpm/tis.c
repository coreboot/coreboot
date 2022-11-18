/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <commonlib/endian.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <endian.h>
#include <lib.h>
#include <security/tpm/tis.h>

#include "tpm.h"

/* global structure for tpm chip data */
static struct tpm_chip chip;

#define TPM_CMD_COUNT_BYTE 2
#define TPM_CMD_ORDINAL_BYTE 6

int tis_open(void)
{
	int rc;

	if (chip.is_open) {
		printk(BIOS_DEBUG, "%s() called twice.\n", __func__);
		return -1;
	}

	rc = tpm_vendor_init(&chip, CONFIG_DRIVER_TPM_I2C_BUS,
			     CONFIG_DRIVER_TPM_I2C_ADDR);
	if (rc < 0)
		chip.is_open = 0;

	if (rc)
		return -1;

	return 0;
}

int tis_close(void)
{
	if (chip.is_open) {
		tpm_vendor_cleanup(&chip);
		chip.is_open = 0;
	}

	return 0;
}

int tis_init(void)
{
	return tpm_vendor_probe(CONFIG_DRIVER_TPM_I2C_BUS,
				CONFIG_DRIVER_TPM_I2C_ADDR);
}

static ssize_t tpm_transmit(const uint8_t *sbuf, size_t sbufsiz, void *rbuf,
			size_t rbufsiz)
{
	int rc;
	uint32_t count;

	memcpy(&count, sbuf + TPM_CMD_COUNT_BYTE, sizeof(count));
	count = be32_to_cpu(count);

	if (!chip.vendor.send || !chip.vendor.status || !chip.vendor.cancel)
		return -1;

	if (count == 0) {
		printk(BIOS_DEBUG, "%s: no data\n", __func__);
		return -1;
	}
	if (count > sbufsiz) {
		printk(BIOS_DEBUG, "%s: invalid count value %x %zx\n", __func__,
			count, sbufsiz);
		return -1;
	}

	ASSERT(chip.vendor.send);
	rc = chip.vendor.send(&chip, (uint8_t *)sbuf, count);
	if (rc < 0) {
		printk(BIOS_DEBUG, "%s: tpm_send error\n", __func__);
		goto out;
	}

	int timeout = 2 * 60 * 1000; /* two minutes timeout */
	while (timeout) {
		ASSERT(chip.vendor.status);
		uint8_t status = chip.vendor.status(&chip);
		if ((status & chip.vendor.req_complete_mask) ==
		    chip.vendor.req_complete_val) {
			goto out_recv;
		}

		if (status == chip.vendor.req_canceled) {
			printk(BIOS_DEBUG,
				"%s: Operation Canceled\n", __func__);
			rc = -1;
			goto out;
		}
		mdelay(TPM_TIMEOUT);
		timeout--;
	}

	ASSERT(chip.vendor.cancel);
	chip.vendor.cancel(&chip);
	printk(BIOS_DEBUG, "%s: Operation Timed out\n", __func__);
	rc = -1; //ETIME;
	goto out;

out_recv:

	rc = chip.vendor.recv(&chip, (uint8_t *)rbuf, rbufsiz);
	if (rc < 0)
		printk(BIOS_DEBUG, "%s: tpm_recv: error %d\n", __func__, rc);
out:
	return rc;
}

int tis_sendrecv(const uint8_t *sendbuf, size_t sbuf_size,
		uint8_t *recvbuf, size_t *rbuf_len)
{
	ASSERT(sbuf_size >= 10);

	/* Display the TPM command */
	if (CONFIG(DRIVER_TPM_DISPLAY_TIS_BYTES)) {
		printk(BIOS_DEBUG, "TPM Command: 0x%08x\n",
			read_at_be32(sendbuf, sizeof(uint16_t)
				+ sizeof(uint32_t)));
		hexdump(sendbuf, sbuf_size);
	}

	int len = tpm_transmit(sendbuf, sbuf_size, recvbuf, *rbuf_len);

	if (len < 10) {
		*rbuf_len = 0;
		return -1;
	}

	if (len > *rbuf_len) {
		*rbuf_len = len;
		return -1;
	}

	*rbuf_len = len;

	/* Display the TPM response */
	if (CONFIG(DRIVER_TPM_DISPLAY_TIS_BYTES)) {
		printk(BIOS_DEBUG, "TPM Response: 0x%08x\n",
			read_at_be32(recvbuf, sizeof(uint16_t)
				+ sizeof(uint32_t)));
		hexdump(recvbuf, *rbuf_len);
	}

	return 0;
}
