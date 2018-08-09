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

#include <compiler.h>
#include <console/console.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <fsp/util.h>
#include <reset.h>
#include <string.h>
#include <timer.h>
#include <soc/pci_devs.h>

/* Reset Request  */
#define MKHI_GLOBAL_RESET			0x0b
#define MKHI_STATUS_SUCCESS			0

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
	} __packed reply;
	struct reset_message {
		u8 group_id;
		u8 cmd;
		u8 reserved;
		u8 result;
		u8 req_origin;
		u8 reset_type;
	} __packed;
	struct reset_message msg = {
		.cmd = MKHI_GLOBAL_RESET,
		.req_origin = GR_ORIGIN_BIOS_POST,
		.reset_type = GLOBAL_RST_TYPE
	};
	size_t reply_size;

	heci_reset();

	status = heci_send(&msg, sizeof(msg), BIOS_HOST_ADD, HECI_MKHI_ADD);
	if (status != 1)
		return -1;

	reply_size = sizeof(reply);
	memset(&reply, 0, reply_size);
	if (!heci_receive(&reply, &reply_size))
		return -1;
	if (reply.result != MKHI_STATUS_SUCCESS) {
		printk(BIOS_DEBUG, "Returned Mkhi Status is not success!\n");
		return -1;
	}
	printk(BIOS_DEBUG, "Heci receive success!\n");
	return 0;
}

void do_global_reset(void)
{
	/* Ask CSE to do the global reset */
	if (!send_heci_reset_message())
		return;

	/* global reset if CSE fail to reset */
	pmc_global_reset_enable(1);
	hard_reset();
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
