/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
