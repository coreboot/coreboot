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
 */

#ifndef __BOOTMODE_H__
#define __BOOTMODE_H__

/* functions implemented per mainboard: */
void init_bootmode_straps(void);
int get_write_protect_state(void);
int get_recovery_mode_switch(void);
int get_recovery_mode_retrain_switch(void);
int clear_recovery_mode_switch(void);
int get_wipeout_mode_switch(void);
int get_lid_switch(void);

/* Return 1 if display initialization is required. 0 if not. */
int display_init_required(void);
int gfx_get_init_done(void);
void gfx_set_init_done(int done);

#endif /* __BOOTMODE_H__ */
