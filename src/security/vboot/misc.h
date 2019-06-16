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
 */

#ifndef __VBOOT_MISC_H__
#define __VBOOT_MISC_H__

#include <assert.h>
#include <arch/early_variables.h>
#include <security/vboot/vboot_common.h>

struct vb2_context;
struct vb2_shared_data;

struct selected_region {
	uint32_t offset;
	uint32_t size;
};

/*
 * this is placed at the start of the vboot work buffer. selected_region is used
 * for the verstage to return the location of the selected slot. buffer is used
 * by the vboot2 core. Keep the struct CPU architecture agnostic as it crosses
 * stage boundaries.
 */
struct vboot_working_data {
	struct selected_region selected_region;
	uint32_t flags;
	/* offset of the buffer from the start of this struct */
	uint16_t buffer_offset;
	uint16_t buffer_size;
};

/*
 * Definitions for vboot_working_data.flags values.
 */
/* vboot requests display initialization from coreboot. */
#define VBOOT_WD_FLAG_DISPLAY_INIT (1 << 0)
/* vboot has selected developer mode. */
#define VBOOT_WD_FLAG_DEVELOPER_MODE (1 << 1)

/*
 * Source: security/vboot/common.c
 */
struct vboot_working_data *vboot_get_working_data(void);
void vboot_init_work_context(struct vb2_context *ctx);
void vboot_finalize_work_context(struct vb2_context *ctx);
struct vb2_shared_data *vboot_get_shared_data(void);

/* Returns 0 on success. < 0 on failure. */
int vboot_get_selected_region(struct region *region);

void vboot_set_selected_region(const struct region *region);
int vboot_is_slot_selected(void);

/*
 * Source: security/vboot/vboot_handoff.c
 */
void vboot_fill_handoff(void);

/*
 * Source: security/vboot/bootmode.c
 */
void vboot_save_recovery_reason_vbnv(void);

/*
 * The stage loading code is compiled and entered from multiple stages. The
 * helper functions below attempt to provide more clarity on when certain
 * code should be called. They are implemented inline for better compile-time
 * code elimination.
 */

static inline int verification_should_run(void)
{
	if (CONFIG(VBOOT_SEPARATE_VERSTAGE))
		return ENV_VERSTAGE;
	else if (CONFIG(VBOOT_STARTS_IN_ROMSTAGE))
		return ENV_ROMSTAGE;
	else if (CONFIG(VBOOT_STARTS_IN_BOOTBLOCK))
		return ENV_BOOTBLOCK;
	else
		dead_code();
}

static inline int verstage_should_load(void)
{
	if (CONFIG(VBOOT_SEPARATE_VERSTAGE))
		return ENV_BOOTBLOCK;
	else
		return 0;
}

static inline int vboot_logic_executed(void)
{
	extern int vboot_executed;	/* should not be globally accessible */

	/* If we are in the stage that runs verification, or in the stage that
	   both loads the verstage and is returned to from it afterwards, we
	   need to check a global to see if verfication has run. */
	if (verification_should_run() ||
	    (verstage_should_load() && CONFIG(VBOOT_RETURN_FROM_VERSTAGE)))
		return car_get_var(vboot_executed);

	if (CONFIG(VBOOT_STARTS_IN_BOOTBLOCK)) {
		/* All other stages are "after the bootblock" */
		return !ENV_BOOTBLOCK;
	} else if (CONFIG(VBOOT_STARTS_IN_ROMSTAGE)) {
		/* Post-RAM stages are "after the romstage" */
#ifdef __PRE_RAM__
		return 0;
#else
		return 1;
#endif
	} else {
		dead_code();
	}
}


#endif /* __VBOOT_MISC_H__ */
