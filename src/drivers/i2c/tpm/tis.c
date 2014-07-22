/*
 * Copyright (C) 2011 Infineon Technologies
 * Copyright 2013 Google Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <delay.h>
#include <arch/byteorder.h>
#include <device/i2c.h>
#include <tpm.h>
#include "tpm.h"

#include <console/console.h>

/* global structure for tpm chip data */
struct tpm_chip g_chip;

#define TPM_CMD_COUNT_BYTE 2
#define TPM_CMD_ORDINAL_BYTE 6

int tis_open(void)
{
	int rc;

	if (g_chip.is_open) {
		printk(BIOS_DEBUG, "tis_open() called twice.\n");
		return -1;
	}

	rc = tpm_vendor_init(CONFIG_DRIVER_TPM_I2C_BUS,
				CONFIG_DRIVER_TPM_I2C_ADDR);

	if (rc < 0)
		g_chip.is_open = 0;

	if (rc) {
		return -1;
	}

	return 0;
}

int tis_close(void)
{
	if (g_chip.is_open) {
		tpm_vendor_cleanup(&g_chip);
		g_chip.is_open = 0;
	}

	return 0;
}

int tis_init(void)
{
	int bus = CONFIG_DRIVER_TPM_I2C_BUS;
	int chip = CONFIG_DRIVER_TPM_I2C_ADDR;

	/*
	 * Probe TPM twice; the first probing might fail because TPM is asleep,
	 * and the probing can wake up TPM.
	 */
	if (i2c_writeb(bus, chip, 0, 0) && i2c_writeb(bus, chip, 0, 0))
		return -1;

	return 0;
}

static ssize_t tpm_transmit(const uint8_t *buf, size_t bufsiz)
{
	int rc;
	uint32_t count, ordinal;

	struct tpm_chip *chip = &g_chip;

	memcpy(&count, buf + TPM_CMD_COUNT_BYTE, sizeof(count));
	count = be32_to_cpu(count);
	memcpy(&ordinal, buf + TPM_CMD_ORDINAL_BYTE, sizeof(ordinal));
	ordinal = be32_to_cpu(ordinal);

	if (count == 0) {
		printk(BIOS_DEBUG, "tpm_transmit: no data\n");
		return -1;
	}
	if (count > bufsiz) {
		printk(BIOS_DEBUG, "tpm_transmit: invalid count value %x %zx\n",
			count, bufsiz);
		return -1;
	}

	ASSERT(chip->vendor.send);
	rc = chip->vendor.send(chip, (uint8_t *) buf, count);
	if (rc < 0) {
		printk(BIOS_DEBUG, "tpm_transmit: tpm_send error\n");
		goto out;
	}

	if (chip->vendor.irq)
		goto out_recv;

	int timeout = 2 * 60 * 1000; /* two minutes timeout */
	while (timeout) {
		ASSERT(chip->vendor.status);
		uint8_t status = chip->vendor.status(chip);
		if ((status & chip->vendor.req_complete_mask) ==
		    chip->vendor.req_complete_val) {
			goto out_recv;
		}

		if ((status == chip->vendor.req_canceled)) {
			printk(BIOS_DEBUG, "tpm_transmit: Operation Canceled\n");
			rc = -1;
			goto out;
		}
		mdelay(TPM_TIMEOUT);
		timeout--;
	}

	ASSERT(chip->vendor.cancel);
	chip->vendor.cancel(chip);
	printk(BIOS_DEBUG, "tpm_transmit: Operation Timed out\n");
	rc = -1; //ETIME;
	goto out;

out_recv:

	rc = chip->vendor.recv(chip, (uint8_t *) buf, TPM_BUFSIZE);
	if (rc < 0)
		printk(BIOS_DEBUG, "tpm_transmit: tpm_recv: error %d\n", rc);
out:
	return rc;
}

int tis_sendrecv(const uint8_t *sendbuf, size_t sbuf_size,
		uint8_t *recvbuf, size_t *rbuf_len)
{
	uint8_t buf[TPM_BUFSIZE];

	if (sizeof(buf) < sbuf_size)
		return -1;

	memcpy(buf, sendbuf, sbuf_size);

	int len = tpm_transmit(buf, sbuf_size);

	if (len < 10) {
		*rbuf_len = 0;
		return -1;
	}

	if (len > *rbuf_len) {
		*rbuf_len = len;
		return -1;
	}

	memcpy(recvbuf, buf, len);
	*rbuf_len = len;

	return 0;
}
