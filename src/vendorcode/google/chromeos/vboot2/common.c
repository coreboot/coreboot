/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc.
 */

#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <reset.h>
#include "../chromeos.h"
#include "../symbols.h"
#include "../vboot_handoff.h"
#include "misc.h"

static const size_t vb_work_buf_size = 16 * KiB;

struct vb2_working_data * const vboot_get_working_data(void)
{
	if (IS_ENABLED(CONFIG_VBOOT_DYNAMIC_WORK_BUFFER))
		/* cbmem_add() does a cbmem_find() first. */
		return cbmem_add(CBMEM_ID_VBOOT_WORKBUF, vb_work_buf_size);
	else
		return (struct vb2_working_data *)_vboot2_work;
}

size_t vb2_working_data_size(void)
{
	if (IS_ENABLED(CONFIG_VBOOT_DYNAMIC_WORK_BUFFER))
		return vb_work_buf_size;
	else
		return _vboot2_work_size;
}

void *vboot_get_work_buffer(struct vb2_working_data *wd)
{
	return (void *)((uintptr_t)wd + wd->buffer_offset);
}
