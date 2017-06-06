/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#include <console/console.h>
#include <intelblocks/cse.h>
#include <fsp/util.h>
#include <reset.h>
#include <timer.h>

/* Reset Request  */
#define MKHI_GLOBAL_RESET			0x0b

#define GR_ORIGIN_BIOS_MEM_INIT			0x01
#define GR_ORIGIN_BIOS_POST			0x02
#define GR_ORIGIN_MEBX				0x03

#define GLOBAL_RST_TYPE				0x01

#define BIOS_HOST_ADD				0x00
#define HECI_MKHI_ADD				0x07

static int send_heci_reset_message(void)
{
	int status;
	struct reset_reply {
		u8 group_id;
		u8 command;
		u8 reserved;
		u8 result;
	} __attribute__ ((packed)) reply;
	struct reset_message {
		u8 group_id;
		u8 cmd;
		u8 reserved;
		u8 result;
		u8 req_origin;
		u8 reset_type;
	} __attribute__ ((packed));
	struct reset_message msg = {
		.cmd = MKHI_GLOBAL_RESET,
		.group_id = 0,
		.reserved = 0,
		.result = 0,
		.req_origin = GR_ORIGIN_BIOS_POST,
		.reset_type = GLOBAL_RST_TYPE
	};
	size_t reply_size;

	heci_reset();

	status = heci_send(&msg, sizeof(msg), BIOS_HOST_ADD, HECI_MKHI_ADD);
	if (status != 1)
		return -1;

	reply_size = sizeof(reply);
	heci_receive(&reply, &reply_size);
	/* get reply result from HECI MSG  */
	if (reply.result != 0) {
		printk(BIOS_DEBUG, "%s: Exit with Failure\n", __func__);
		return -1;
	}
	printk(BIOS_DEBUG, "%s: Exit with Success\n",  __func__);
	return 0;
}

void do_global_reset(void)
{
	/* Ask CSE to do the global reset */
	send_heci_reset_message();
	/*
	 * TODO: Presumbily we shouldn't return. But if we did, fallback to
	 * alternative way of triggered global reset provided by pmclib.
	 */
}

void chipset_handle_reset(uint32_t status)
{
	switch (status) {
	case FSP_STATUS_RESET_REQUIRED_3: /* Global Reset */
		printk(BIOS_DEBUG, "GLOBAL RESET!!\n");
		do_global_reset();
		break;
	default:
		printk(BIOS_ERR, "unhandled reset type %x\n", status);
		die("unknown reset type");
		break;
	}
}
