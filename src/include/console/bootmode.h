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

#ifndef __BOOTMODE_H__
#define __BOOTMODE_H__

#if CONFIG_BOOTMODE_STRAPS
int get_developer_mode_switch(void);
int get_recovery_mode_switch(void);
int developer_mode_enabled(void);
int recovery_mode_enabled(void);
#else
#define recovery_mode_enabled() 0
#define developer_mode_enabled() 0
#endif

#endif /* __BOOTMODE_H__ */
