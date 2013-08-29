/*
 * Copyright (C) 2011 Infineon Technologies
 *
 * Authors:
 * Peter Huewe <huewe.external@infineon.com>
 *
 * Description:
 * Device driver for TCG/TCPA TPM (trusted platform module).
 * Specifications at www.trustedcomputinggroup.org
 *
 * It is based on the Linux kernel driver tpm.c from Leendert van
 * Dorn, Dave Safford, Reiner Sailer, and Kyleen Hall.
 *
 * Version: 2.1.1
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
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
#include <console/console.h>
#include <arch/byteorder.h>
#include "tis.h"
#include "tpm.h"

/* global structure for tpm chip data */
struct tpm_chip g_chip;

#define TPM_CMD_COUNT_BYTE 2
#define TPM_CMD_ORDINAL_BYTE 6

ssize_t tpm_transmit(const uint8_t *buf, size_t bufsiz)
{
	ssize_t rc;
	uint32_t count, ordinal;

	struct tpm_chip *chip = &g_chip;

	memcpy(&count, buf + TPM_CMD_COUNT_BYTE, sizeof(count));
	count = be32_to_cpu(count);
	memcpy(&ordinal, buf + TPM_CMD_ORDINAL_BYTE, sizeof(ordinal));
	ordinal = be32_to_cpu(ordinal);

	if (count == 0) {
		printk(BIOS_DEBUG, "tpm_transmit: no data\n");
		return -1; //ENODATA;
	}
	if (count > bufsiz) {
		printk(BIOS_DEBUG, "tpm_transmit: invalid count value %x %zx\n",
			count, bufsiz);
		return -1; //E2BIG;
	}

	ASSERT(chip->vendor.send);
	rc = chip->vendor.send(chip, (uint8_t *) buf, count);
	if (rc < 0) {
		printk(BIOS_DEBUG, "tpm_transmit: tpm_send: error %zd\n", rc);
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
		printk(BIOS_DEBUG, "tpm_transmit: tpm_recv: error %zd\n", rc);
out:
	return rc;
}

#define TPM_ERROR_SIZE 10

struct tpm_chip *tpm_register_hardware(const struct tpm_vendor_specific *entry)
{
	struct tpm_chip *chip;

	/* Driver specific per-device data */
	chip = &g_chip;
	memcpy(&chip->vendor, entry, sizeof(struct tpm_vendor_specific));
	chip->is_open = 1;

	return chip;
}

int tpm_open(unsigned bus, uint32_t dev_addr)
{
	int rc;
	if (g_chip.is_open)
		return -1; //EBUSY;
	rc = tpm_vendor_init(bus, dev_addr);
	if (rc < 0)
		g_chip.is_open = 0;
	return rc;
}

void tpm_close(void)
{
	if (g_chip.is_open) {
		tpm_vendor_cleanup(&g_chip);
		g_chip.is_open = 0;
	}
}
