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

#include <stdint.h>

/* functions implemented per mainboard: */
int get_developer_mode_switch(void);
int get_recovery_mode_switch(void);
int get_write_protect_state(void);
#ifdef __PRE_RAM__
void save_chromeos_gpios(void);
#endif

/* functions implemented in vbnv.c: */
int get_recovery_mode_from_vbnv(void);
int vboot_wants_oprom(void);
extern int oprom_is_loaded;

void read_vbnv(uint8_t *vbnv_copy);
void save_vbnv(const uint8_t *vbnv_copy);

/* functions implemented in chromeos.c: */
int developer_mode_enabled(void);
int recovery_mode_enabled(void);

/* functions implemented in vboot.c */
void init_chromeos(int bootmode);

#if CONFIG_VBOOT_VERIFY_FIRMWARE
struct romstage_handoff;
void vboot_verify_firmware(struct romstage_handoff *handoff);
void *vboot_get_payload(int *len);
/* Returns 0 on success < 0 on error. */
int vboot_get_handoff_info(void **addr, uint32_t *size);
#endif

#endif
