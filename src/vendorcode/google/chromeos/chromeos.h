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

/* functions implemented per mainboard: */
int get_developer_mode_switch(void);
int get_recovery_mode_switch(void);
#ifdef __PRE_RAM__
void save_chromeos_gpios(void);
#endif

/* functions implemented in vbnv.c: */
int get_recovery_mode_from_vbnv(void);
int vboot_wants_oprom(void);
extern int oprom_is_loaded;

/* functions implemented in chromeos.c: */
int developer_mode_enabled(void);
int recovery_mode_enabled(void);

/* functions implemented in vboot.c */
void init_chromeos(int bootmode);

#endif
