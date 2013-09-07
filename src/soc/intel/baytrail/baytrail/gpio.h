/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#ifndef _BAYTRAIL_GPIO_H_
#define _BAYTRAIL_GPIO_H_

#include <stdint.h>
#include <arch/io.h>
#include <baytrail/iomap.h>

/* Registers sitting behind the IO_BASE_ADDRESS */

#define SCORE_PCONF_OFFSET	0x0000
#define SSUS_PCONF_OFFSET	0x2000

static inline unsigned int score_pconf0(int pad_num)
{
	return IO_BASE_ADDRESS + SCORE_PCONF_OFFSET + pad_num * 16;
}


static inline unsigned int ssus_pconf0(int pad_num)
{
	return IO_BASE_ADDRESS + SSUS_PCONF_OFFSET + pad_num * 16;
}

static inline void score_select_func(int pad, int func)
{
	uint32_t reg;
	uint32_t pconf0_addr = score_pconf0(pad);

	reg = read32(pconf0_addr);
	reg &= ~0x7;
	reg |= func & 0x7;
	write32(pconf0_addr, reg);
}

static inline void ssus_select_func(int pad, int func)
{
	uint32_t reg;
	uint32_t pconf0_addr = ssus_pconf0(pad);

	reg = read32(pconf0_addr);
	reg &= ~0x7;
	reg |= func & 0x7;
	write32(pconf0_addr, reg);
}

/* SCORE Pad definitions. */
#define PCU_SMB_CLK_PAD			88
#define PCU_SMB_DATA_PAD		90
#define PCU_SMB_ALERT_PAD		92

#endif /* _BAYTRAIL_GPIO_H_ */
