/*
 * This file is part of the coreboot project.
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

#ifndef ROM_SEGS_H
#define ROM_SEGS_H

#define ROM_CODE_SEG 0x08
#define ROM_DATA_SEG 0x10
#define ROM_CODE_SEG64 0x18

/*
 * This define is placed here to make sure future romstage programmers
 * know about it.
 * It is used for STM setup code.
 */
#define SMM_TASK_STATE_SEG 0x20

#endif /* ROM_SEGS_H */
