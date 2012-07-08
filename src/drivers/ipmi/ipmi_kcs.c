/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <string.h>
#include <delay.h>
#include "ipmi_kcs.h"

#define IPMI_KCS_STATE(_x)	((_x) >> 6)

#define IPMI_KCS_GET_STATUS_ABORT
#define IPMI_KCS_START_WRITE 0x61
#define IPMI_KCS_END_WRITE 0x62
#define IPMI_KCS_READ_BYTE 0x68

#define IPMI_KCS_OBF 0x01
#define IPMI_KCS_IBF 0x02
#define IPMI_KCS_ATN 0x04

#define IPMI_KCS_STATE_IDLE 0x00
#define IPMI_KCS_STATE_READ 0x01
#define IPMI_KCS_STATE_WRITE 0x02
#define IPMI_KCS_STATE_ERROR 0x03

#define IPMI_CMD(_x) ((_x) + 1)
#define IPMI_DATA(_x) ((_x))
#define IPMI_STAT(_x) ((_x) + 1)

static unsigned char ipmi_kcs_status(int port)
{
	unsigned char status = inb(IPMI_STAT(port));
	printk(BIOS_DEBUG, "%s: 0x%02x\n", __func__, status);
	return status;
}

static int wait_ibf_timeout(int port)
{
	int timeout = 1000;
	do {
		if (!(ipmi_kcs_status(port) & IPMI_KCS_IBF))
			return 0;
		udelay(100);
	} while(timeout--);
	printk(BIOS_ERR, "wait_ibf timeout!\n");
	return timeout;
}

static int wait_obf_timeout(int port)
{
	int timeout = 1000;
	do {
		if ((ipmi_kcs_status(port) & IPMI_KCS_OBF))
			return 0;
		udelay(100);
	} while(timeout--);

	printk(BIOS_ERR, "wait_obf timeout!\n");
	return timeout;
}


static int ipmi_kcs_send_data_byte(int port, const unsigned char byte)
{
	unsigned char status;

	printk(BIOS_DEBUG, "%s: %02x\n", __func__, byte);

	outb(byte, IPMI_DATA(port));

	if (wait_ibf_timeout(port))
		return 1;

        status = ipmi_kcs_status(port);
	if ((status & IPMI_KCS_OBF) &&
	    IPMI_KCS_STATE(status) != IPMI_KCS_STATE_WRITE) {
		printk(BIOS_ERR, "%s: status %02x\n", __func__, status);
		return 1;
	}

	if (ipmi_kcs_status(port) & IPMI_KCS_OBF)
		inb(IPMI_DATA(port));
	return 0;
}

static int ipmi_kcs_send_last_data_byte(int port, const unsigned char byte)
{
	unsigned char status;

	printk(BIOS_DEBUG, "%s: %02x\n", __func__, byte);

	if (wait_ibf_timeout(port))
		return 1;

        status = ipmi_kcs_status(port);
	if ((status & IPMI_KCS_OBF) &&
	    IPMI_KCS_STATE(status) != IPMI_KCS_STATE_WRITE) {
		printk(BIOS_ERR, "%s: status %02x\n", __func__, status);
		return 1;
	}

	if (ipmi_kcs_status(port) & IPMI_KCS_OBF)
		inb(IPMI_DATA(port));

	outb(byte, IPMI_DATA(port));
	return 0;
}

static int ipmi_kcs_send_cmd_byte(int port, const unsigned char byte)
{
	printk(BIOS_DEBUG, "%s: 0x%02x\n", __func__, byte);

	if (wait_ibf_timeout(port))
		return 1;

	if (ipmi_kcs_status(port) & IPMI_KCS_OBF)
		inb(IPMI_DATA(port));
	outb(byte, IPMI_CMD(port));

	if (wait_ibf_timeout(port))
		return 1;

	if (ipmi_kcs_status(port) & IPMI_KCS_OBF)
		inb(IPMI_DATA(port));

	return 0;
}

static int ipmi_kcs_send_message(int port, int netfn, int lun, int cmd,
				const unsigned char *msg, int len)
{
	int ret;

	if ((ret = ipmi_kcs_send_cmd_byte(port, IPMI_KCS_START_WRITE))) {
		printk(BIOS_ERR, "IPMI START WRITE failed\n");
		return ret;
	}

	if ((ret = ipmi_kcs_send_data_byte(port, (netfn << 2) | (lun & 3)))) {
		printk(BIOS_ERR, "IPMI NETFN failed\n");
		return ret;
	}

	if ((ret = ipmi_kcs_send_data_byte(port, cmd))) {
		printk(BIOS_ERR, "IPMI CMD failed\n");
		return ret;
	}

	while(len-- > 1) {
		if ((ret = ipmi_kcs_send_data_byte(port, *msg++))) {
			printk(BIOS_ERR, "IPMI BYTE WRITE failed\n");
			return ret;
		}
	}

	if ((ret = ipmi_kcs_send_cmd_byte(port, IPMI_KCS_END_WRITE))) {
		printk(BIOS_ERR, "IPMI END WRITE failed\n");
		return ret;
	}

	if ((ret = ipmi_kcs_send_last_data_byte(port, *msg++))) {
		printk(BIOS_ERR, "IPMI BYTE WRITE failed\n");
		return ret;
	}
	return 0;
}

static int ipmi_kcs_read_message(int port, unsigned char *msg, int len)
{
	int status, ret = 0;

	if (!msg)
		return 0;

	if (wait_ibf_timeout(port))
		return 1;

	for(;;) {
		status = ipmi_kcs_status(port);

		if (IPMI_KCS_STATE(status) == IPMI_KCS_STATE_IDLE)
			return ret;

		if (IPMI_KCS_STATE(status) != IPMI_KCS_STATE_READ) {
			printk(BIOS_ERR, "%s: wrong state: 0x%02x\n", __func__, status);
			return -1;
		}

		if (wait_obf_timeout(port))
			return -1;

		*msg++ = inb(IPMI_DATA(port));
		ret++;

		if (wait_ibf_timeout(port))
			return -1;

		outb(IPMI_KCS_READ_BYTE, IPMI_DATA(port));
	}
	return ret;
}

int ipmi_kcs_message(int port, int netfn, int lun, int cmd,
			const unsigned char *inmsg, int inlen,
			unsigned char *outmsg, int outlen)
{
	if (ipmi_kcs_send_message(port, netfn, lun, cmd, inmsg, inlen)) {
		printk(BIOS_ERR, "ipmi_kcs_send_message failed\n");
		return -1;
	}

	return ipmi_kcs_read_message(port, outmsg, outlen);
}
