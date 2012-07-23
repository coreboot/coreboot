/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <elog.h>

#define GSMI_RET_SUCCESS		0x00
#define GSMI_RET_INVALID_PARAMETER	0x82
#define GSMI_RET_UNSUPPORTED		0x83

#define GSMI_CMD_SET_EVENT_LOG		0x08
#define GSMI_CMD_CLEAR_EVENT_LOG	0x09
#define GSMI_CMD_HANDSHAKE_TYPE		0xc1

#define GSMI_HANDSHAKE_NONE		0x7f
#define GSMI_LOG_ENTRY_TYPE_KERNEL	0xDEAD

struct gsmi_set_eventlog_param {
	u32 data_ptr;
	u32 data_len;
	u32 type;
} __attribute__ ((packed));

struct gsmi_set_eventlog_type1 {
	u16 type;
	u32 instance;
} __attribute__ ((packed));

struct gsmi_clear_eventlog_param {
	u32 percentage;
	u32 data_type;
} __attribute__ ((packed));

/* Param is usually EBX, ret in EAX */
u32 gsmi_exec(u8 command, u32 *param)
{
	struct gsmi_set_eventlog_param *sel;
	struct gsmi_set_eventlog_type1 *type1;
	struct gsmi_clear_eventlog_param *cel;
	u32 ret = GSMI_RET_UNSUPPORTED;

	switch (command) {
	case GSMI_CMD_HANDSHAKE_TYPE:
		/* Used by kernel to verify basic SMI functionality */
		printk(BIOS_DEBUG, "GSMI Handshake\n");
		ret = GSMI_HANDSHAKE_NONE;
		break;

	case GSMI_CMD_SET_EVENT_LOG:
		/* Look for a type1 event */
		sel = (struct gsmi_set_eventlog_param *)(*param);
		if (!sel)
			break;

		/* Make sure the input is usable */
		if (sel->type != 1 && sel->data_ptr != 0 &&
		    sel->data_len != sizeof(struct gsmi_set_eventlog_type1))
			break;

		/* Event structure within the data buffer */
		type1 = (struct gsmi_set_eventlog_type1 *)(sel->data_ptr);
		if (!type1)
			break;

		printk(BIOS_DEBUG, "GSMI Set Event Log "
		       "(type=0x%x instance=0x%x)\n",
		       type1->type, type1->instance);

		if (type1->type == GSMI_LOG_ENTRY_TYPE_KERNEL) {
			/* Special case for linux kernel shutdown reason */
			elog_add_event_dword(ELOG_TYPE_OS_EVENT,
					     type1->instance);
		} else {
			/* Add other events that may be used for testing */
			elog_add_event_dword(type1->type, type1->instance);
		}
		ret = GSMI_RET_SUCCESS;
		break;

	case GSMI_CMD_CLEAR_EVENT_LOG:
		/* Get paramter buffer even though we don't use it */
		cel = (struct gsmi_clear_eventlog_param *)(*param);
		if (!cel)
			break;

		printk(BIOS_DEBUG, "GSMI Clear Event Log (%u%% type=%u)\n",
		       cel->percentage, cel->data_type);

		if (elog_clear() == 0)
			ret = GSMI_RET_SUCCESS;
		break;

	default:
		printk(BIOS_DEBUG, "GSMI Unknown: 0x%02x\n", command);
		break;
	}

	return ret;
}
