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
 */
#ifndef VBOOT_HANDOFF_H
#define VBOOT_HANDOFF_H


#include <vboot_api.h>
#include <vboot_struct.h>
#include "chromeos.h"
#include "vboot_common.h"

/*
 * The vboot_handoff structure contains the data to be consumed by downstream
 * firmware after firmware selection has been completed. Namely it provides
 * vboot shared data as well as the flags from VbInit.
 */
struct vboot_handoff {
	VbInitParams init_params;
	uint32_t selected_firmware;
	char shared_data[VB_SHARED_DATA_MIN_SIZE];
} __attribute__((packed));


#endif /* VBOOT_HANDOFF_H */
