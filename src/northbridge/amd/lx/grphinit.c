/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <cpu/amd/lxdef.h>
#include <cpu/x86/msr.h>
#include <stdlib.h>

void geodelx_vga_msr_init(void);
void graphics_init(void);

struct msrinit {
	u32 msrnum;
	msr_t msr;
};

static const struct msrinit geodelx_vga_msr[] = {
	/* Enable the GLIU Memory routing to the hardware
	* PDID1 : Port 4, GLIU0
	* PBASE : 0x000A0
	* PMASK : 0xFFFE0
	*/
	{.msrnum = MSR_GLIU0_BASE4, {.lo = 0x0a0fffe0, .hi = 0x80000000}},
	/* Enable the GLIU IO Routing
	* IDID  : Port 4, GLIU0
	* IBASE : 0x003c0
	* IMASK : 0xffff0
	*/
	{.msrnum = GLIU0_IOD_BM_0,  {.lo = 0x3c0ffff0, .hi = 0x80000000}},
	/* Enable the GLIU IO Routing
	* IDID  : Port 4, GLIU0
	* IBASE : 0x003d0
	* IMASK : 0xffff0
	*/
	{.msrnum = GLIU0_IOD_BM_1,  {.lo = 0x3d0ffff0, .hi = 0x80000000}},
};

void geodelx_vga_msr_init(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(geodelx_vga_msr); i++)
		wrmsr(geodelx_vga_msr[i].msrnum, geodelx_vga_msr[i].msr);
}

 /*
  * This function mirrors the Graphics_Init routine in GeodeROM.
  */
void graphics_init(void)
{
	uint16_t wClassIndex, wData, res;

	/* SoftVG initialization */
	printk(BIOS_DEBUG, "Graphics init...\n");

	geodelx_vga_msr_init();

	/* Call SoftVG with the main configuration parameters. */
	/* NOTE: SoftVG expects the memory size to be given in 2MB blocks */

	wClassIndex = (VRC_VG << 8) + VG_CONFIG;

	/*
	 * Graphics Driver Enabled (13)                         0, NO (lets BIOS controls the GP)
	 * External Monochrome Card Support(12)         0, NO
	 * Controller Priority Select(11)                       1, Primary
	 * Display Select(10:8)                                         0x0, CRT
	 * Graphics Memory Size(7:1)                            CONFIG_VIDEO_MB >> 1,
	 *                                                                                      defined in devicetree.cb
	 * PLL Reference Clock Bypass(0)                        0, Default
	 */

	/* Video RAM has to be given in 2MB chunks
	 *   the value is read @ 7:1 (value in 7:0 looks like /2)
	 *   so we can add the real value in megabytes
	 */

	wData = VG_CFG_DRIVER | VG_CFG_PRIORITY |
			VG_CFG_DSCRT | (CONFIG_VIDEO_MB & VG_MEM_MASK);
	vrWrite(wClassIndex, wData);

	res = vrRead(wClassIndex);
	printk(BIOS_DEBUG, "VRC_VG value: 0x%04x\n", res);
}
