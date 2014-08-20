/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#ifndef __CHROMEOS_SYMBOLS_H
#define __CHROMEOS_SYMBOLS_H

extern u8 _vboot2_work[];
extern u8 _evboot2_work[];
#define _vboot2_work_size (_evboot2_work - _vboot2_work)

/* Careful: _e<stage> and _<stage>_size only defined for the current stage! */
extern u8 _verstage[];
extern u8 _everstage[];
#define _verstage_size (_everstage - _verstage)

#endif /* __CHROMEOS_SYMBOLS_H */
