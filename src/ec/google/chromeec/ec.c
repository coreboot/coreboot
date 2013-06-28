/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include <arch/hlt.h>
#include <reset.h>
#ifndef __PRE_RAM__
#include <elog.h>
#include <stdlib.h>
#include <string.h>
#include "chip.h"
#endif
#include "ec.h"
#include "ec_commands.h"
#include <vendorcode/google/chromeos/chromeos.h>

uint8_t google_chromeec_calc_checksum(const uint8_t *data, int size)
{
	int csum;

	for (csum = 0; size > 0; data++, size--)
		csum += *data;
	return (uint8_t)(csum & 0xff);
}

int google_chromeec_kbbacklight(int percent)
{
	struct chromeec_command cec_cmd;
	struct ec_params_pwm_set_keyboard_backlight cmd_backlight;
	struct ec_response_pwm_get_keyboard_backlight rsp_backlight;
	/* if they were dumb, help them out */
	percent = percent % 101;
	cec_cmd.cmd_code = EC_CMD_PWM_SET_KEYBOARD_BACKLIGHT;
	cec_cmd.cmd_version = 0;
	cmd_backlight.percent = percent;
	cec_cmd.cmd_data_in = &cmd_backlight;
	cec_cmd.cmd_data_out = &rsp_backlight;
	cec_cmd.cmd_size_in = sizeof(cmd_backlight);
	cec_cmd.cmd_size_out = sizeof(rsp_backlight);
	google_chromeec_command(&cec_cmd);
	printk(BIOS_DEBUG, "Google Chrome set keyboard backlight: %x status (%x)\n",
	       rsp_backlight.percent, cec_cmd.cmd_code);
	return cec_cmd.cmd_code;

}

void google_chromeec_post(u8 postcode)
{
	/* backlight is a percent. postcode is a u8.
	 * Convert the u8 to %.
	 */
	postcode = (postcode/4) + (postcode/8);
	google_chromeec_kbbacklight(postcode);
}

/*
 * Query the EC for specified mask indicating enabled events.
 * The EC maintains separate event masks for SMI, SCI and WAKE.
 */
static u32 google_chromeec_get_mask(u8 type)
{
	struct ec_params_host_event_mask req;
	struct ec_response_host_event_mask rsp;
	struct chromeec_command cmd;

	cmd.cmd_code = type;
	cmd.cmd_version = 0;
	cmd.cmd_data_in = &req;
	cmd.cmd_size_in = sizeof(req);
	cmd.cmd_data_out = &rsp;
	cmd.cmd_size_out = sizeof(rsp);

	if (google_chromeec_command(&cmd) == 0)
		return rsp.mask;
	return 0;
}

u32 google_chromeec_get_events_b(void)
{
	return google_chromeec_get_mask(EC_CMD_HOST_EVENT_GET_B);
}

#ifndef __SMM__
/* Check for recovery mode and ensure EC is in RO */
void google_chromeec_early_init(void)
{
	struct chromeec_command cec_cmd;
	struct ec_response_get_version cec_resp = {{0}};

	cec_cmd.cmd_code = EC_CMD_GET_VERSION;
	cec_cmd.cmd_version = 0;
	cec_cmd.cmd_data_out = &cec_resp;
	cec_cmd.cmd_size_in = 0;
	cec_cmd.cmd_size_out = sizeof(cec_resp);
	google_chromeec_command(&cec_cmd);

	if (cec_cmd.cmd_code ||
	    (recovery_mode_enabled() &&
	     (cec_resp.current_image != EC_IMAGE_RO))) {
		struct ec_params_reboot_ec reboot_ec;
		/* Reboot the EC and make it come back in RO mode */
		reboot_ec.cmd = EC_REBOOT_COLD;
		reboot_ec.flags = 0;
		cec_cmd.cmd_code = EC_CMD_REBOOT_EC;
		cec_cmd.cmd_version = 0;
		cec_cmd.cmd_data_in = &reboot_ec;
		cec_cmd.cmd_size_in = sizeof(reboot_ec);
		cec_cmd.cmd_size_out = 0; /* ignore response, if any */
		printk(BIOS_DEBUG, "Rebooting with EC in RO mode:\n");
		google_chromeec_command(&cec_cmd);
		udelay(1000);
		hard_reset();
		hlt();
	}
}
#endif /* ! __SMM__ */

#ifndef __PRE_RAM__

int google_chromeec_i2c_xfer(uint8_t chip, uint8_t addr, int alen,
			     uint8_t *buffer, int len, int is_read)
{
	union {
		struct ec_params_i2c_passthru p;
		uint8_t outbuf[EC_HOST_PARAM_SIZE];
	} params;
	union {
		struct ec_response_i2c_passthru r;
		uint8_t inbuf[EC_HOST_PARAM_SIZE];
	} response;
	struct ec_params_i2c_passthru *p = &params.p;
	struct ec_response_i2c_passthru *r = &response.r;
	struct ec_params_i2c_passthru_msg *msg = p->msg;
	struct chromeec_command cmd;
	uint8_t *pdata;
	int read_len, write_len;
	int size;
	int rv;

	p->port = 0;

	if (alen != 1) {
		printk(BIOS_ERR, "Unsupported address length %d\n", alen);
		return -1;
	}
	if (is_read) {
		read_len = len;
		write_len = alen;
		p->num_msgs = 2;
	} else {
		read_len = 0;
		write_len = alen + len;
		p->num_msgs = 1;
	}

	size = sizeof(*p) + p->num_msgs * sizeof(*msg);
	if (size + write_len > sizeof(params)) {
		printk(BIOS_ERR, "Params too large for buffer\n");
		return -1;
	}
	if (sizeof(*r) + read_len > sizeof(response)) {
		printk(BIOS_ERR, "Read length too big for buffer\n");
		return -1;
	}

	/* Create a message to write the register address and optional data */
	pdata = (uint8_t *)p + size;
	msg->addr_flags = chip;
	msg->len = write_len;
	pdata[0] = addr;
	if (!is_read)
		memcpy(pdata + 1, buffer, len);
	msg++;

	if (read_len) {
		msg->addr_flags = chip | EC_I2C_FLAG_READ;
		msg->len = read_len;
	}

	cmd.cmd_code = EC_CMD_I2C_PASSTHRU;
	cmd.cmd_version = 0;
	cmd.cmd_data_in = p;
	cmd.cmd_size_in = size + write_len;
	cmd.cmd_data_out = r;
	cmd.cmd_size_out = sizeof(*r) + read_len;
	rv = google_chromeec_command(&cmd);
	if (rv != 0)
		return rv;

	/* Parse response */
	if (r->i2c_status & EC_I2C_STATUS_ERROR) {
		printk(BIOS_ERR, "Transfer failed with status=0x%x\n",
		       r->i2c_status);
		return -1;
	}

	if (cmd.cmd_size_out < sizeof(*r) + read_len) {
		printk(BIOS_ERR, "Truncated read response\n");
		return -1;
	}

	if (read_len)
		memcpy(buffer, r->data, read_len);

	return 0;
}

static int google_chromeec_set_mask(u8 type, u32 mask)
{
	struct ec_params_host_event_mask req;
	struct ec_response_host_event_mask rsp;
	struct chromeec_command cmd;

	req.mask = mask;
	cmd.cmd_code = type;
	cmd.cmd_version = 0;
	cmd.cmd_data_in = &req;
	cmd.cmd_size_in = sizeof(req);
	cmd.cmd_data_out = &rsp;
	cmd.cmd_size_out = sizeof(rsp);

	return google_chromeec_command(&cmd);
}

int google_chromeec_set_sci_mask(u32 mask)
{
	printk(BIOS_DEBUG, "Chrome EC: Set SCI mask to 0x%08x\n", mask);
	return google_chromeec_set_mask(
		EC_CMD_HOST_EVENT_SET_SCI_MASK, mask);
}

int google_chromeec_set_smi_mask(u32 mask)
{
	printk(BIOS_DEBUG, "Chrome EC: Set SMI mask to 0x%08x\n", mask);
	return google_chromeec_set_mask(
		EC_CMD_HOST_EVENT_SET_SMI_MASK, mask);
}

int google_chromeec_set_wake_mask(u32 mask)
{
	printk(BIOS_DEBUG, "Chrome EC: Set WAKE mask to 0x%08x\n", mask);
	return google_chromeec_set_mask(
		EC_CMD_HOST_EVENT_SET_WAKE_MASK, mask);
}

u32 google_chromeec_get_wake_mask(void)
{
	return google_chromeec_get_mask(
		EC_CMD_HOST_EVENT_GET_WAKE_MASK);
}

#if CONFIG_ELOG
/* Find the last port80 code from the previous boot */
static u16 google_chromeec_get_port80_last_boot(void)
{
	struct ec_response_port80_last_boot rsp;
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_PORT80_LAST_BOOT,
		.cmd_data_out = &rsp,
		.cmd_size_out = sizeof(rsp),
	};

	/* Get last port80 code */
	if (google_chromeec_command(&cmd) == 0)
		return rsp.code;

	return 0;
}
#endif

void google_chromeec_log_events(u32 mask)
{
#if CONFIG_ELOG
	u8 event;
	u16 code;

	/* Find the last port80 code */
	code = google_chromeec_get_port80_last_boot();

	/* Log the last post code only if it is abornmal */
	if (code > 0 && code != POST_OS_BOOT && code != POST_OS_RESUME)
		printk(BIOS_DEBUG, "Chrome EC: Last POST code was 0x%02x\n",
		       code);

	while ((event = google_chromeec_get_event()) != 0) {
		if (EC_HOST_EVENT_MASK(event) & mask)
			elog_add_event_byte(ELOG_TYPE_EC_EVENT, event);
	}
#endif
}

u16 google_chromeec_get_board_version(void)
{
	struct chromeec_command cmd;
	struct ec_response_board_version board_v;

	cmd.cmd_code = EC_CMD_GET_BOARD_VERSION;
	cmd.cmd_version = 0;
	cmd.cmd_size_in = 0;
	cmd.cmd_size_out = sizeof(board_v);
	cmd.cmd_data_out = &board_v;

	if (google_chromeec_command(&cmd) != 0)
		return 0;

	return board_v.board_version;
}

int google_chromeec_set_usb_charge_mode(u8 port_id, enum usb_charge_mode mode)
{
	struct chromeec_command cmd;
	struct ec_params_usb_charge_set_mode set_mode = {
		.usb_port_id = port_id,
		.mode = mode,
	};

	cmd.cmd_code = EC_CMD_USB_CHARGE_SET_MODE;
	cmd.cmd_version = 0;
	cmd.cmd_size_in = sizeof(set_mode);
	cmd.cmd_data_in = &set_mode;
	cmd.cmd_size_out = 0;
	cmd.cmd_data_out = NULL;

	return google_chromeec_command(&cmd);
}

#ifndef __SMM__

static
int google_chromeec_hello(void)
{
	struct chromeec_command cec_cmd;
	struct ec_params_hello cmd_hello;
	struct ec_response_hello rsp_hello;
	cmd_hello.in_data = 0x10203040;
	cec_cmd.cmd_code = EC_CMD_HELLO;
	cec_cmd.cmd_version = 0;
	cec_cmd.cmd_data_in = &cmd_hello.in_data;
	cec_cmd.cmd_data_out = &rsp_hello.out_data;
	cec_cmd.cmd_size_in = sizeof(cmd_hello.in_data);
	cec_cmd.cmd_size_out = sizeof(rsp_hello.out_data);
	google_chromeec_command(&cec_cmd);
	printk(BIOS_DEBUG, "Google Chrome EC: Hello got back %x status (%x)\n",
	       rsp_hello.out_data, cec_cmd.cmd_code);
	return cec_cmd.cmd_code;
}

static int ec_image_type; /* Cached EC image type (ro or rw). */

void google_chromeec_init(void)
{
	struct chromeec_command cec_cmd;
	struct ec_response_get_version cec_resp = {{0}};

	printk(BIOS_DEBUG, "Google Chrome EC: Initializing keyboard.\n");

	google_chromeec_hello();

	cec_cmd.cmd_code = EC_CMD_GET_VERSION;
	cec_cmd.cmd_version = 0;
	cec_cmd.cmd_data_out = &cec_resp;
	cec_cmd.cmd_size_in = 0;
	cec_cmd.cmd_size_out = sizeof(cec_resp);
	google_chromeec_command(&cec_cmd);

	if (cec_cmd.cmd_code) {
		printk(BIOS_DEBUG,
		       "Google Chrome EC: version command failed!\n");
	} else {
		printk(BIOS_DEBUG, "Google Chrome EC: version:\n");
		printk(BIOS_DEBUG, "    ro: %s\n", cec_resp.version_string_ro);
		printk(BIOS_DEBUG, "    rw: %s\n", cec_resp.version_string_rw);
		printk(BIOS_DEBUG, "  running image: %d\n",
		       cec_resp.current_image);
		ec_image_type = cec_resp.current_image;
	}

	if (cec_cmd.cmd_code ||
	    (recovery_mode_enabled() &&
	     (cec_resp.current_image != EC_IMAGE_RO))) {
		struct ec_params_reboot_ec reboot_ec;
		/* Reboot the EC and make it come back in RO mode */
		reboot_ec.cmd = EC_REBOOT_COLD;
		reboot_ec.flags = 0;
		cec_cmd.cmd_code = EC_CMD_REBOOT_EC;
		cec_cmd.cmd_version = 0;
		cec_cmd.cmd_data_in = &reboot_ec;
		cec_cmd.cmd_size_in = sizeof(reboot_ec);
		cec_cmd.cmd_size_out = 0; /* ignore response, if any */
		printk(BIOS_DEBUG, "Rebooting with EC in RO mode:\n");
		google_chromeec_command(&cec_cmd);
		udelay(1000);
		hard_reset();
		hlt();
	}

}

int google_ec_running_ro(void)
{
	return (ec_image_type == EC_IMAGE_RO);
}
#endif /* ! __SMM__ */

#endif /* ! __PRE_RAM__ */
