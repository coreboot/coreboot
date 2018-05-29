/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#ifndef __VBOOT_SYMBOLS_H__
#define __VBOOT_SYMBOLS_H__

extern u8 _vboot2_work[];
extern u8 _evboot2_work[];
#define _vboot2_work_size (_evboot2_work - _vboot2_work)

#endif /* __VBOOT_SYMBOLS_H__ */
