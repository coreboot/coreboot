/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <timer.h>
#include "ipmi_if.h"

#define IPMI_KCS_STATE(_x)	((_x) >> 6)

#define IPMI_KCS_GET_STATUS_ABORT 0x60
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

#define IPMI_CMD(_x) ((_x) + CONFIG_IPMI_KCS_REGISTER_SPACING)
#define IPMI_DATA(_x) ((_x))
#define IPMI_STAT(_x) ((_x) + CONFIG_IPMI_KCS_REGISTER_SPACING)

static unsigned char ipmi_kcs_status(int port)
{
	unsigned char status = inb(IPMI_STAT(port));
	if (CONFIG(DEBUG_IPMI))
		printk(BIOS_SPEW, "%s: 0x%02x\n", __func__, status);
	return status;
}

static int wait_ibf_timeout(int port)
{
	if (!wait_ms(CONFIG_IPMI_KCS_TIMEOUT_MS, !(ipmi_kcs_status(port) & IPMI_KCS_IBF))) {
		printk(BIOS_ERR, "wait_ibf timeout!\n");
		return 1;
	} else {
		return 0;
	}
}

static int wait_obf_timeout(int port)
{
	if (!wait_ms(CONFIG_IPMI_KCS_TIMEOUT_MS, (ipmi_kcs_status(port) & IPMI_KCS_OBF))) {
		printk(BIOS_ERR, "wait_obf timeout!\n");
		return 1;
	} else {
		return 0;
	}
}

static int ipmi_kcs_send_data_byte(int port, const unsigned char byte)
{
	unsigned char status;

	if (CONFIG(DEBUG_IPMI))
		printk(BIOS_SPEW, "%s: 0x%02x\n", __func__, byte);

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

	if (CONFIG(DEBUG_IPMI))
		printk(BIOS_SPEW, "%s: 0x%02x\n", __func__, byte);

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
	if (CONFIG(DEBUG_IPMI))
		printk(BIOS_SPEW, "%s: 0x%02x\n", __func__, byte);

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

	ret = ipmi_kcs_send_cmd_byte(port, IPMI_KCS_START_WRITE);
	if (ret) {
		printk(BIOS_ERR, "IPMI START WRITE failed\n");
		return ret;
	}

	ret = ipmi_kcs_send_data_byte(port, (netfn << 2) | (lun & 3));
	if (ret) {
		printk(BIOS_ERR, "IPMI NETFN failed\n");
		return ret;
	}

	if (!len) {
		ret = ipmi_kcs_send_cmd_byte(port, IPMI_KCS_END_WRITE);
		if (ret) {
			printk(BIOS_ERR, "IPMI END WRITE failed\n");
			return ret;
		}

		ret = ipmi_kcs_send_last_data_byte(port, cmd);
		if (ret) {
			printk(BIOS_ERR, "IPMI BYTE WRITE failed\n");
			return ret;
		}
	} else {
		ret = ipmi_kcs_send_data_byte(port, cmd);
		if (ret) {
			printk(BIOS_ERR, "IPMI CMD failed\n");
			return ret;
		}

		while (len > 1) {
			ret = ipmi_kcs_send_data_byte(port, *msg++);
			if (ret) {
				printk(BIOS_ERR, "IPMI BYTE WRITE failed\n");
				return ret;
			}
			len--;
		}

		ret = ipmi_kcs_send_cmd_byte(port, IPMI_KCS_END_WRITE);
		if (ret) {
			printk(BIOS_ERR, "IPMI END WRITE failed\n");
			return ret;
		}

		ret = ipmi_kcs_send_last_data_byte(port, *msg);
		if (ret) {
			printk(BIOS_ERR, "IPMI BYTE WRITE failed\n");
			return ret;
		}
	}

	return 0;
}

static int ipmi_kcs_read_message(int port, unsigned char *msg, int len)
{
	int status, ret = 0;

	if (wait_ibf_timeout(port))
		return 1;

	for (;;) {
		status = ipmi_kcs_status(port);

		if (IPMI_KCS_STATE(status) == IPMI_KCS_STATE_IDLE)
			return ret;

		if (IPMI_KCS_STATE(status) != IPMI_KCS_STATE_READ) {
			printk(BIOS_ERR, "%s: wrong state: 0x%02x\n", __func__,
			       status);
			return -1;
		}

		if (wait_obf_timeout(port))
			return -1;

		if (msg && (ret < len)) {
			*msg++ = inb(IPMI_DATA(port));
			ret++;
		}

		if (wait_ibf_timeout(port))
			return -1;

		outb(IPMI_KCS_READ_BYTE, IPMI_DATA(port));
	}
	return ret;
}

int ipmi_message(int port, int netfn, int lun, int cmd,
		 const unsigned char *inmsg, int inlen,
		 unsigned char *outmsg, int outlen)
{
	if (ipmi_kcs_send_message(port, netfn, lun, cmd, inmsg, inlen)) {
		printk(BIOS_ERR, "ipmi_kcs_send_message failed\n");
		return -1;
	}

	return ipmi_kcs_read_message(port, outmsg, outlen);
}
