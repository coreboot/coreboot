/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
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

#ifndef DRIVERS_GIC_H
#define DRIVERS_GIC_H

#include <stdint.h>

#define NR(start, end) (((end) - (start)) / sizeof(uint32_t))

struct gicd_mmio {
	uint32_t ctlr;				/* 0x000 - 0x003 */
	uint32_t typer;				/* 0x004 - 0x007 */
	uint32_t iidr;				/* 0x008 - 0x00b */
	uint32_t reserved_1[NR(0xc, 0x80)];	/* 0x00c - 0x07f */
	uint32_t igroupr[NR(0x80, 0x100)];	/* 0x080 - 0x0ff */
	uint32_t isenabler[NR(0x100, 0x180)];	/* 0x100 - 0x17f */
	uint32_t icenabler[NR(0x180, 0x200)];	/* 0x180 - 0x1ff */
	uint32_t ispendr[NR(0x200, 0x280)];	/* 0x200 - 0x27f */
	uint32_t icpendr[NR(0x280, 0x300)];	/* 0x280 - 0x2ff */
	uint32_t isactiver[NR(0x300, 0x380)];	/* 0x300 - 0x37f */
	uint32_t icactiver[NR(0x380, 0x400)];	/* 0x380 - 0x3ff */
	uint32_t ipriorityr[NR(0x400, 0x7fc)];	/* 0x400 - 0x7fb */
	uint32_t reserved_2[NR(0x7fc, 0x800)];	/* 0x7fc - 0x7ff */
	uint32_t itargetsr[NR(0x800, 0xbfc)];	/* 0x800 - 0xbfb */
	uint32_t reserved_3[NR(0xbfc, 0xc00)];	/* 0xbfc - 0x2ff */
	uint32_t icfgr[NR(0xc00, 0xd00)];	/* 0xc00 - 0xcff */
	uint32_t reserved_4[NR(0xd00, 0xe00)];	/* 0xd00 - 0xdff */
	uint32_t nsacr[NR(0xe00, 0xf00)];	/* 0xe00 - 0xeff */
	uint32_t sgir;				/* 0xf00 - 0xf03 */
	uint32_t reserved_5[NR(0xf04, 0xf10)];	/* 0xf04 - 0xf0f */
	uint32_t cpendsgir[NR(0xf10, 0xf20)];	/* 0xf10 - 0xf1f */
	uint32_t spendsgir[NR(0xf20, 0xf30)];	/* 0xf20 - 0xf2f */
	uint32_t reserved_6[NR(0xf30, 0xfe8)];	/* 0xf30 - 0xfe7 */
	uint32_t icpidr2;			/* 0xfe8 - 0xfeb */
	uint32_t reserved_7[NR(0xfec, 0x1000)];	/* 0xfec - 0xfff */
};

struct gicc_mmio {
	uint32_t ctlr;				/* 0x000 - 0x003 */
	uint32_t pmr;				/* 0x004 - 0x007 */
	uint32_t bpr;				/* 0x008 - 0x00b */
	uint32_t iar;				/* 0x00c - 0x00f */
	uint32_t eoir;				/* 0x010 - 0x013 */
	uint32_t rpr;				/* 0x014 - 0x017 */
	uint32_t hppir;				/* 0x018 - 0x01b */
	uint32_t apbr;				/* 0x01c - 0x01f */
	uint32_t aiar;				/* 0x020 - 0x023 */
	uint32_t aeoir;				/* 0x024 - 0x027 */
	uint32_t ahppir;			/* 0x028 - 0x02b */
	uint32_t resered_1[NR(0x2c, 0xd0)];	/* 0x02c - 0x0cf */
	uint32_t apr[NR(0xd0, 0xe0)];		/* 0x0d0 - 0x0df */
	uint32_t nsapr[NR(0xe0, 0xf0)];		/* 0x0e0 - 0x0ef */
	uint32_t resered_2[NR(0xf0, 0xfc)];	/* 0x0f0 - 0x0fb */
	uint32_t iidr;				/* 0x0fc - 0x0ff */
	uint32_t reserved_3[NR(0x100, 0x1000)];	/* 0x100 - 0xfff */
	uint32_t dir;				/* 0x1000 - 0x1003 */
};

#undef NR

#endif /* DRIVERS_GIC_H */
