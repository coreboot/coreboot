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
 */
#ifndef VBOOT_COMMON_H
#define VBOOT_COMMON_H

#include <stdint.h>
#include <commonlib/region.h>

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

/* The following functions return 0 on success, < 0 on error. */
int vboot_named_region_device(const char *name, struct region_device *rdev);
int vboot_region_device(const struct region *reg, struct region_device *rdev);
int vboot_get_handoff_info(void **addr, uint32_t *size);

/* The following functions return 1 for true and 0 for false. */
int vboot_skip_display_init(void);
int vboot_enable_recovery(void);
int vboot_enable_developer(void);

int vboot_recovery_reason(void);

void vboot_reboot(void);

/*
 * Save the provided hash digest to a secure location to check against in
 * the resume path. Returns 0 on success, < 0 on error.
 */
int vboot_save_hash(void *digest, size_t digest_size);

/*
 * Retrieve the previously saved hash digest.  Returns 0 on success,
 * < 0 on error.
 */
int vboot_retrieve_hash(void *digest, size_t digest_size);

/*
 * Determine if the platform is resuming from suspend. Returns 0 when
 * not resuming, > 0 if resuming, and < 0 on error.
 */
int vboot_platform_is_resuming(void);

/* Allow the platform to do any clean up work when vboot requests a reboot. */
void vboot_platform_prepare_reboot(void);

/* Main logic for verified boot. verstage() is the stage entry point
 * while the verstage_main() is just the core logic. */
void verstage_main(void);
void verstage_mainboard_init(void);
void verstage(void);

#endif /* VBOOT_COMMON_H */
