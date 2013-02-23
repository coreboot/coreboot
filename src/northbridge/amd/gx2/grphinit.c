/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2012 Nils Jacobs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <arch/io.h>
#include <stdint.h>
#include <cpu/amd/vr.h>
#include <console/console.h>
#include <cpu/amd/gx2def.h>
#include <cpu/x86/msr.h>
#include <stdlib.h>

void geodegx2_vga_msr_init(void);
void graphics_init(void);

struct msrinit {
	u32 msrnum;
	msr_t msr;
};

static const struct msrinit geodegx2_vga_msr[] = {
	/* Enable the GLIU Memory routing to the memory A0000-BFFFF
	* PDID1 : Port 4, GLIU0
	* PBASE : 0x000A0
	* PMASK : 0xFFFE0
	*/
	{.msrnum = GLIU0_P2D_BM_4, {.lo = 0x0a0fffe0, .hi = 0x80000000}},
};

void geodegx2_vga_msr_init(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(geodegx2_vga_msr); i++)
		wrmsr(geodegx2_vga_msr[i].msrnum, geodegx2_vga_msr[i].msr);
}


/* This function mirrors the Graphics_Init routine in GeodeROM. */
void graphics_init(void)
{
	uint16_t wClassIndex, wData, res;

	/* SoftVG initialization */
	printk(BIOS_DEBUG, "Graphics init...\n");

	geodegx2_vga_msr_init();

	/* Call SoftVG with the main configuration parameters. */
	/* NOTE: SoftVG expects the memory size to be given in 512 KB blocks */

	wClassIndex = (VRC_VG << 8) + VG_CONFIG;

	/*
	 * Graphics Driver Enabled (13)				0, NO (lets BIOS controls the GP)
	 * External Monochrome Card Support(12)			0, NO
	 * Controller Priority Select(11)			1, Primary
	 * Display Select(10:8)					0x0, CRT
	 * Graphics Memory Size(7:1)				CONFIG_VIDEO_MB >> 1,
	 *							defined in devicetree.cb
	 * PLL Reference Clock Bypass(0)			0, Default
	 */

	/* Video RAM has to be given in 512KB chunks
	 *   the value is read @ 7:1 (value in 7:0 looks like /2)
	 *   so we can add the real value in megabytes
	 */

	wData = VG_CFG_PRIORITY | VG_CFG_DSCRT | ((CONFIG_VIDEO_MB * 2) & VG_MEM_MASK);
	vrWrite(wClassIndex, wData);

	res = vrRead(wClassIndex);
	printk(BIOS_DEBUG, "VRC_VG value: 0x%04x\n", res);
}


