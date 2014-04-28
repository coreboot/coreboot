/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __CHROMEOS_H__
#define __CHROMEOS_H__

#include <stddef.h>
#include <stdint.h>
#include <bootmode.h>

/* functions implemented in vbnv.c: */
int get_recovery_mode_from_vbnv(void);
int vboot_wants_oprom(void);

void read_vbnv(uint8_t *vbnv_copy);
void save_vbnv(const uint8_t *vbnv_copy);

/* functions implemented in vboot.c */
void init_chromeos(int bootmode);

#if CONFIG_VBOOT_VERIFY_FIRMWARE
/* Returns 0 on success < 0 on error. */
int vboot_get_handoff_info(void **addr, uint32_t *size);
int vboot_enable_developer(void);
int vboot_enable_recovery(void);
#endif

#include "gnvs.h"
struct device;

#if CONFIG_CHROMEOS_RAMOOPS
void chromeos_ram_oops_init(chromeos_acpi_t *chromeos);
#if CONFIG_CHROMEOS_RAMOOPS_DYNAMIC
static inline void chromeos_reserve_ram_oops(struct device *dev, int idx) {}
#else /* CONFIG_CHROMEOS_RAMOOPS_DYNAMIC */
void chromeos_reserve_ram_oops(struct device *dev, int idx);
#endif /* CONFIG_CHROMEOS_RAMOOPS_DYNAMIC */
#else  /* !CONFIG_CHROMEOS_RAMOOPS */
static inline void chromeos_ram_oops_init(chromeos_acpi_t *chromeos) {}
static inline void chromeos_reserve_ram_oops(struct device *dev, int idx) {}
#endif /* CONFIG_CHROMEOS_RAMOOPS */

#endif
