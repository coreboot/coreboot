/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __SOC_MARVELL_MVMAP2315_PMIC_H__
#define __SOC_MARVELL_MVMAP2315_PMIC_H__

#include <stdint.h>

#include <soc/addressmap.h>
#include <types.h>

#define MVMAP2315_LOWPWR_FLAG				BIT(4)

#define MVMAP2315_APGENCFG_SYSBARDISABLE		BIT(1)
#define MVMAP2315_APGENCFG_BROADCASTINNER		BIT(2)
#define MVMAP2315_APGENCFG_BROADCASTOUTER		BIT(3)
#define MVMAP2315_APGENCFG_BROADCASTCACHEMAINT		BIT(4)
#define MVMAP2315_APGENCFG_BROADCASTCACHEMAINTPOU	BIT(5)
#define MVMAP2315_APCORECFG0_AA64NAA32			BIT(4)
struct mvmap2315_cpu_regs {
	u32 fuse00;
	u32 fuse01;
	u32 fuse02;
	u32 fuse03;
	u32 fuse04;
	u32 fuse05;
	u32 fuse06;
	u32 fuse07;
	u32 fuse10;
	u32 fuse11;
	u32 fuse12;
	u32 fuse13;
	u32 fuse14;
	u32 fuse15;
	u32 fuse16;
	u32 fuse17;
	u8 _reserved0[0x20];
	u32 fuse20;
	u32 fuse21;
	u32 fuse22;
	u32 fuse23;
	u32 fuse24;
	u32 fuse25;
	u32 fuse26;
	u32 fuse27;
	u32 fuse30;
	u32 fuse31;
	u32 fuse32;
	u32 fuse33;
	u32 fuse34;
	u32 fuse35;
	u32 fuse36;
	u32 fuse37;
	u32 fuse40;
	u32 fuse41;
	u32 fuse42;
	u32 fuse43;
	u32 fuse44;
	u32 fuse45;
	u32 fuse46;
	u32 fuse47;
	u32 fuse50;
	u32 fuse51;
	u32 fuse52;
	u32 fuse53;
	u32 fuse54;
	u32 fuse55;
	u32 fuse56;
	u32 fuse57;
	u32 fuse60;
	u32 fuse61;
	u32 fuse62;
	u32 fuse63;
	u32 fuse64;
	u32 fuse65;
	u32 fuse66;
	u32 fuse67;
	u32 fuse70;
	u32 fuse71;
	u32 fuse72;
	u32 fuse73;
	u32 fuse74;
	u32 fuse75;
	u32 fuse76;
	u32 fuse77;
	u32 fuse80;
	u32 fuse81;
	u32 fuse82;
	u32 fuse83;
	u32 fuse84;
	u32 fuse85;
	u32 fuse86;
	u32 fuse87;
	u32 fuse90;
	u32 fuse91;
	u32 fuse92;
	u32 fuse93;
	u32 fuse94;
	u32 fuse95;
	u32 fuse96;
	u32 fuse97;
	u32 fuse100;
	u32 fuse101;
	u32 fuse102;
	u32 fuse103;
	u32 fuse104;
	u32 fuse105;
	u32 fuse106;
	u32 fuse107;
	u32 fuse110;
	u32 fuse111;
	u32 fuse112;
	u32 fuse113;
	u32 fuse114;
	u32 fuse115;
	u32 fuse116;
	u32 fuse117;
	u32 fuse120;
	u32 fuse121;
	u32 fuse122;
	u32 fuse123;
	u32 fuse124;
	u32 fuse125;
	u32 fuse126;
	u32 fuse127;
	u32 fuse130;
	u32 fuse131;
	u32 fuse132;
	u32 fuse133;
	u32 fuse134;
	u32 fuse135;
	u32 fuse136;
	u32 fuse137;
	u32 fuse140;
	u32 fuse141;
	u32 fuse142;
	u32 fuse143;
	u32 fuse144;
	u32 fuse145;
	u32 fuse146;
	u32 fuse147;
	u32 fuse150;
	u32 fuse151;
	u32 fuse152;
	u32 fuse153;
	u32 fuse154;
	u32 fuse155;
	u32 fuse156;
	u32 fuse157;
	u32 apgencfg;
	u32 apcorecfg0;
	u32 apcorecfg1;
	u32 apcorecfg2;
	u32 apcorecfg3;
	u32 rvbaraddr_low0;
	u32 rvbaraddr_low1;
	u32 rvbaraddr_low2;
	u32 rvbaraddr_low3;
	u8 _reserved1[0x10];
	u32 rvbaraddr_high0;
	u32 rvbaraddr_high1;
	u32 rvbaraddr_high2;
	u32 rvbaraddr_high3;
	u32 highvecremap;
};

check_member(mvmap2315_cpu_regs, highvecremap, 0x264);
static struct mvmap2315_cpu_regs * const mvmap2315_cpu
					= (void *)MVMAP2315_CPU_BASE;

enum boot_options {
	NO_BOOT = 0,
	CHARGING_SCREEN = 1,
	FULL_BOOT = 2
};

void no_boot(void);
void full_boot(void);
void charging_screen(void);
void mcu_start(void);
void ap_start(void *entry);
u32 get_boot_path(void);

#endif /* __SOC_MARVELL_MVMAP2315_PMIC_H__ */
