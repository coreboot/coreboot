/*
 * This file is part of the LinuxBIOS project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <post_code.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd_geodelx.h>
#include <southbridge/amd/cs5536/cs5536.h>
#include <northbridge/amd/geodelx/raminit.h>

#define MANUALCONF 0		/* Do automatic strapped PLL config */
#define PLLMSRHI 0x00001490	/* manual settings for the PLL */
#define PLLMSRLO 0x02000030
#define DIVIL_LBAR_GPIO		0x5140000c
#define DIMM0 ((u8) 0xA0)
#define DIMM1 ((u8) 0xA2)

/* this is an incredibly mainboard-specific number that has no appropriate place
  * outside this file. 
  */
#define GPIO_BASE            0x6100

/* empty function to always fail smbus reads */
int smbus_read_byte(unsigned device, unsigned address)
{
	return -1;
}

static void init_gpio(void)
{
	struct msr msr;
	printk(BIOS_DEBUG, "Initializing GPIO module...\n");

	// initialize the GPIO LBAR
	msr.lo = GPIO_BASE;
	msr.hi = 0x0000f001;
	wrmsr(DIVIL_LBAR_GPIO, msr);
	msr = rdmsr(DIVIL_LBAR_GPIO);
	printk(BIOS_DEBUG, "DIVIL_LBAR_GPIO set to 0x%08x 0x%08x\n", msr.hi, msr.lo);
}

static void sdram_hardwire(void)
{
	/* Total size of DIMM = 2^row address (byte 3) * 2^col address (byte 4) *
	 *                      component Banks (byte 17) * module banks, side (byte 5) *
	 *                      width in bits (byte 6,7)
	 *                    = Density per side (byte 31) * number of sides (byte 5) */
	/* 1. Initialize GLMC registers base on SPD values, do one DIMM for now */
	struct msr  msr;

	msr.hi = 0x10075012;
	msr.lo = 0x00000040;
	
	wrmsr(MC_CF07_DATA, msr);		//GX3

	/* timing and mode ... */

	//msr = rdmsr(0x20000019);
	
	/* per standard bios settings */	
/*
	msr.hi = 0x18000108;
	msr.lo = 
			(6<<28) |		// cas_lat
			(10<<24)|		// ref2act
			(7<<20)|		// act2pre
			(3<<16)|		// pre2act
			(3<<12)|		// act2cmd
			(2<<8)|			// act2act
			(2<<6)|			// dplwr
			(2<<4)|			// dplrd
			(3);			// dal
	* the msr value reported by quanta is very, very different. 
	 * we will go with that value for now. 
	 *
	//msr.lo = 0x286332a3;
*/
	//wrmsr(0x20000019, msr);	//GX3

}

/* CPU and GLIU mult/div */
#define PLLMSRhi 0x0000039C
/* Hold Count - how long we will sit in reset */
#define PLLMSRlo 0x00DE0000

struct wmsr {
	u32 reg;
	struct msr  msr;
} dbe61_msr[] = {
	{.reg = 0x10000020, {.lo = 0xfff80, .hi = 0x20000000}},
	{.reg = 0x10000021, {.lo = 0x80fffe0, .hi = 0x20000000}},
	{.reg = 0x40000020, {.lo = 0xfff80, .hi = 0x20000000}},
	{.reg = 0x40000021, {.lo = 0x80fffe0, .hi = 0x20000000}},
};

static void dbe61_msr_init(void)
{
	int i;
	for(i = 0; i < sizeof(dbe61_msr)/sizeof(dbe61_msr[0]); i++)
		wrmsr(dbe61_msr[i].reg, dbe61_msr[i].msr);
}

int main(void)
{
	post_code(POST_START_OF_MAIN);

	system_preinit();
	dbe61_msr_init();

	cs5536_early_setup();

	/* NOTE: must do this AFTER the early_setup!
	 * it is counting on some early MSR setup
	 * for cs5536.
	 */
	cs5536_setup_onchipuart();

	pll_reset(MANUALCONF, PLLMSRHI, PLLMSRLO);

	cpu_reg_init(0, DIMM0, DIMM1);

	sdram_hardwire();
	/* Check low memory */
	/*ram_check(0x00000000, 640*1024); */
	init_gpio();
	return 0;
}
