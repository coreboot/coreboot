/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef VBOOT_CONTEXT_H
#define VBOOT_CONTEXT_H

#include <stdint.h>
#include <vboot_api.h>

/* The vboot context structure provides all the necessary data for invoking
 * vboot. The vboot loader sets everything up for vboot module to use. */

struct vboot_context {
	struct vboot_handoff *handoff;
	VbCommonParams *cparams;
	VbSelectFirmwareParams *fparams;
	uint8_t *fw_a;
	uint32_t fw_a_size;
	uint8_t *fw_b;
	uint32_t fw_b_size;
	/* Callback implementations living in romstage. */
	void (*read_vbnv)(uint8_t *vbnv_copy);
	void (*save_vbnv)(const uint8_t *vbnv_copy);
	int (*tis_init)(void);
	int (*tis_open)(void);
	int (*tis_close)(void);
	int (*tis_sendrecv)(const u8 *sendbuf, size_t send_size, u8 *recvbuf,
	                    size_t *recv_len);
	void (*log_msg)(const char *fmt, va_list args);
	void (*fatal_error)(void);
};

#endif /* VBOOT_CONTEXT_H */
