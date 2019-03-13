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

#include <security/vboot/vboot_common.h>

struct vb2_context;
struct vb2_shared_data;

/*
 * Source: security/vboot/common.c
 */
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
 * Source: security/vboot/vboot_loader.c
 */
int vboot_logic_executed(void);

/*
 * Source: security/vboot/bootmode.c
 */
void vboot_save_recovery_reason_vbnv(void);

#endif /* __VBOOT_MISC_H__ */
