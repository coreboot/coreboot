/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google, Inc.
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
#ifndef VBOOT_COMMON_H
#define VBOOT_COMMON_H

#include <stdint.h>

struct vboot_region {
	uintptr_t offset_addr;
	int32_t size;
};

/* The FW areas consist of multiple components. At the beginning of
 * each area is the number of total compoments as well as the size and
 * offset for each component. One needs to caculate the total size of the
 * signed firmware region based off of the embedded metadata. */
struct vboot_component_entry {
	uint32_t offset;
	uint32_t size;
} __attribute__((packed));

struct vboot_components {
	uint32_t num_components;
	struct vboot_component_entry entries[0];
} __attribute__((packed));

void vboot_locate_region(const char *name, struct vboot_region *region);

struct vboot_components *vboot_locate_components(struct vboot_region *region);

/*
 * This is a dual purpose routine. If dest is non-NULL the region at
 * offset_addr will be read into the area pointed to by dest.  If dest
 * is NULL,the region will be mapped to a memory location. NULL is
 * returned on error else the location of the requested region.
 */
void *vboot_get_region(uintptr_t offset_addr, size_t size, void *dest);

#endif /* VBOOT_COMMON_H */
