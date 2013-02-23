/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

/* CX700 has 48 bytes of scratch registers in D0F4 starting at Reg. 0xd0 */
#define	SCRATCH_REG_BASE		0xd0
#define	SCRATCH_RANK_0			0xd0
#define	SCRATCH_RANK_1			0xd1
#define	SCRATCH_RANK_2			0xd2
#define	SCRATCH_RANK_3			0xd3
#define	SCRATCH_DIMM_NUM		0xd4
#define	SCRATCH_RANK_NUM		0xd5
#define	SCRATCH_RANK_MAP		0xd6
#define	SCRATCH_DRAM_FREQ		0xd7
#define	SCRATCH_DRAM_NB_ODT		0xd8
#define	SCRATCH_RANK0_SIZE_REG		0xe0	/* RxE0~RxE3 */
#define	SCRATCH_RANK0_MA_REG		0xe4	/* RxE4~RxE7 */
#define	SCRATCH_CHA_DQSI_LOW_REG	0xe8
#define	SCRATCH_CHA_DQSI_HIGH_REG	0xe9
#define	SCRATCH_ChA_DQSI_REG		0xea
#define	SCRATCH_DRAM_256M_BIT		0xee
#define	SCRATCH_FLAGS			0xef

#define DDRII_666	0x5
#define DDRII_533	0x4
#define DDRII_400	0x3
#define DDRII_333	0x2
#define DDRII_266	0x1
#define DDRII_200	0x0


