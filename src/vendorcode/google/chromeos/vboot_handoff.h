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
#ifndef VBOOT_HANDOFF_H
#define VBOOT_HANDOFF_H

#include <vboot_api.h>

/*
 * The vboot handoff structure keeps track of a maximum number of firmware
 * components in the verfieid RW area of flash.  This is not a restriction on
 * the number of components packed in a firmware block. It's only the maximum
 * number of parsed firmware components (address and size) included in the
 * handoff structure.
 */

#define MAX_PARSED_FW_COMPONENTS 5

struct firmware_component {
	uint32_t address;
	uint32_t size;
} __attribute__((packed));

/*
 * The vboot_handoff structure contains the data to be consumed by downstream
 * firmware after firmware selection has been completed. Namely it provides
 * vboot shared data as well as the flags from VbInit. As noted above a finite
 * number of components are parsed from the verfieid firmare region.
 */
struct vboot_handoff {
	VbInitParams init_params;
	uint32_t selected_firmware;
	struct firmware_component components[MAX_PARSED_FW_COMPONENTS];
	char shared_data[VB_SHARED_DATA_MIN_SIZE];
} __attribute__((packed));


#endif /* VBOOT_HANDOFF_H */
