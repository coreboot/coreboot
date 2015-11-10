/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Marvell Inc.
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

#ifndef __SOC_MARVELL_ARMADA38X_COMMON_H_
#define __SOC_MARVELL_ARMADA38X_COMMON_H_

#include <types.h>
#include <arch/io.h>

#define INTER_REGS_BASE 0xF1000000

#define MV_TRUE (1)
#define MV_FALSE (0)

/* The following is a list of Marvell status    */
#define MV_ERROR (-1)
#define MV_OK (0)
#define MV_FAIL (1)
#define MV_BAD_VALUE (2)
#define MV_OUT_OF_RANGE (3)
#define MV_BAD_PARAM (4)
#define MV_BAD_PTR (5)
#define MV_BAD_SIZE (6)
#define MV_BAD_STATE (7)
#define MV_SET_ERROR (8)
#define MV_GET_ERROR (9)
#define MV_CREATE_ERROR (10)
#define MV_NOT_FOUND (11)
#define MV_NO_MORE (12)
#define MV_NO_SUCH (13)
#define MV_TIMEOUT (14)
#define MV_NO_CHANGE (15)
#define MV_NOT_SUPPORTED (16)
#define MV_NOT_IMPLEMENTED (17)
#define MV_NOT_INITIALIZED (18)
#define MV_NO_RESOURCE (19)
#define MV_FULL (20)
#define MV_EMPTY (21)
#define MV_INIT_ERROR (22)
#define MV_HW_ERROR (23)
#define MV_TX_ERROR (24)
#define MV_RX_ERROR (25)
#define MV_NOT_READY (26)
#define MV_ALREADY_EXIST (27)
#define MV_OUT_OF_CPU_MEM (28)
#define MV_NOT_STARTED (29)
#define MV_BUSY (30)
#define MV_TERMINATE (31)
#define MV_NOT_ALIGNED (32)
#define MV_NOT_ALLOWED (33)
#define MV_WRITE_PROTECT (34)
#define MV_DROPPED (35)
#define MV_STOLEN (36)
#define MV_CONTINUE (37)
#define MV_RETRY (38)

#define MV_INVALID (int)(-1)

#define MV_BOARD_TCLK_200MHZ 200000000
#define MV_BOARD_TCLK_250MHZ 250000000

#define MPP_SAMPLE_AT_RESET (0x18600)

#define MV_6810_DEV_ID 0x6810

#define BIT0        0x00000001
#define BIT1        0x00000002
#define BIT2        0x00000004
#define BIT3        0x00000008
#define BIT4        0x00000010
#define BIT5        0x00000020
#define BIT6        0x00000040
#define BIT7        0x00000080
#define BIT8        0x00000100
#define BIT9        0x00000200
#define BIT10       0x00000400
#define BIT11       0x00000800
#define BIT12       0x00001000
#define BIT13       0x00002000
#define BIT14       0x00004000
#define BIT15       0x00008000
#define BIT16       0x00010000
#define BIT17       0x00020000
#define BIT18       0x00040000
#define BIT19       0x00080000
#define BIT20       0x00100000
#define BIT21       0x00200000
#define BIT22       0x00400000
#define BIT23       0x00800000
#define BIT24       0x01000000
#define BIT25       0x02000000
#define BIT26       0x04000000
#define BIT27       0x08000000
#define BIT28       0x10000000
#define BIT29       0x20000000
#define BIT30       0x40000000
#define BIT31       0x80000000

static inline uint32_t mrvl_reg_read(uint32_t offset)
{
	return read32((void *)(INTER_REGS_BASE + offset));
}
static inline void mrvl_reg_write(uint32_t offset, uint32_t val)
{
	write32((void *)(INTER_REGS_BASE + offset), val);
}
static inline void mrvl_reg_bit_set(uint32_t offset, uint32_t bit_mask)
{
	mrvl_reg_write(offset, (mrvl_reg_read(offset) | bit_mask));
}
static inline void mrvl_reg_bit_reset(uint32_t offset, uint32_t bit_mask)
{
	mrvl_reg_write(offset, (mrvl_reg_read(offset) & (~bit_mask)));
}

#endif  // __SOC_MARVELL_ARMADA38X_COMMON_H__
