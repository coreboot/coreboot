/*
 * Copyright 2013 Google Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __BOOT_FIT_H__
#define __BOOT_FIT_H__

#include <stddef.h>
#include <stdint.h>

#include "base/device_tree.h"
#include "base/list.h"

typedef enum CompressionType
{
	CompressionInvalid,
	CompressionNone,
	CompressionLzma,
	CompressionLz4,
} CompressionType;

typedef struct FitImageNode
{
	const char *name;
	void *data;
	uint32_t size;
	CompressionType compression;

	ListNode list_node;
} FitImageNode;

typedef struct FitConfigNode
{
	const char *name;
	const char *kernel;
	FitImageNode *kernel_node;
	const char *fdt;
	FitImageNode *fdt_node;
	const char *ramdisk;
	FitImageNode *ramdisk_node;
	FdtProperty compat;
	int compat_rank;
	int compat_pos;

	ListNode list_node;
} FitConfigNode;

/*
 * Unpack a FIT image into memory, choosing the right configuration through the
 * compatible string set by fit_add_compat() and unflattening the corresponding
 * kernel device tree.
 */
FitImageNode *fit_load(void *fit, char *cmd_line, DeviceTree **dt);

/*
 * Add a compatible string for the preferred kernel DT to the list for this
 * platform. This should be called before the first fit_load() so it will be
 * ranked as a better match than the default compatible strings. |compat| must
 * stay accessible throughout depthcharge's runtime (i.e. not stack-allocated)!
 */
void fit_add_compat(const char *compat);

void fit_add_ramdisk(DeviceTree *tree, void *ramdisk_addr, size_t ramdisk_size);

#endif /* __BOOT_FIT_H__ */
