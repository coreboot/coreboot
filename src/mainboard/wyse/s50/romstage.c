/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Nils Jacobs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <console/console.h>
#include <lib.h>
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/gx2def.h>
#include <cpu/amd/geode_post_code.h>

#include "southbridge/amd/cs5536/cs5536_early_smbus.c"
#include "southbridge/amd/cs5536/cs5536_early_setup.c"

static inline int spd_read_byte(unsigned device, unsigned address)
{
        return smbus_read_byte(device, address);
}

#include "northbridge/amd/gx2/raminit.h"

	/* This is needed because ROMCC doesn`t now the ctz bitop */
static inline unsigned int ctz(unsigned int n)
{
	int zeros;

	n = (n ^ (n - 1)) >> 1;
	for (zeros = 0; n; zeros++)
	{
	  n >>= 1;
	}
	return zeros;
}

static void sdram_set_spd_registers(const struct mem_controller *ctrl) 
{
	/* Total size of DIMM = 2^row address (byte 3) * 2^col address (byte 4) *
	 *                      component Banks (byte 17) * module banks, side (byte 5) *
	 *                      width in bits (byte 6,7)
	 *                    = Density per side (byte 31) * number of sides (byte 5) */
	/* 1. Initialize GLMC registers base on SPD values, do one DIMM for now */
	msr_t msr;
	unsigned char module_banks, val;
	uint16_t dimm_size;

	msr = rdmsr(MC_CF07_DATA);

	/* get module banks (sides) per dimm, SPD byte 5 */
	module_banks = spd_read_byte(0xA0, 5);
	if (module_banks < 1 || module_banks > 2)
		print_err("Module banks per dimm\n");
	module_banks >>= 1;
	msr.hi &= ~(1 << CF07_UPPER_D0_MB_SHIFT);
	msr.hi |= (module_banks << CF07_UPPER_D0_MB_SHIFT);

	/* get component banks per module bank, SPD byte 17 */
	val = spd_read_byte(0xA0, 17);
	if (val < 2 || val > 4)
		print_err("Component banks per module bank\n");
	val >>= 2;
	msr.hi &= ~(0x1 << CF07_UPPER_D0_CB_SHIFT);
	msr.hi |=  (val << CF07_UPPER_D0_CB_SHIFT);

	dimm_size = spd_read_byte(0xA0, 31);
	dimm_size |= (dimm_size << 8);	/* align so 1GB(bit0) is bit 8, this is a little weird to get gcc to not optimize this out */
	dimm_size &= 0x01FC;	/* and off 2GB DIMM size : not supported and the 1GB size we just moved up to bit 8 as well as all the extra on top */
	/*       Module Density * Module Banks */
	dimm_size <<= (0 >> CF07_UPPER_D0_MB_SHIFT) & 1;	/* shift to multiply by # DIMM banks */
	if (dimm_size != 0) {
	dimm_size = ctz(dimm_size);
	}
	if (dimm_size > 7) {	/* 7 is 512MB only support 512MB per DIMM */
		print_err("Only support up to 512MB \n");
		hlt();
	}
	msr.hi |= dimm_size << CF07_UPPER_D0_SZ_SHIFT;

	/* page size = 2^col address */
	val = spd_read_byte(0xA0, 4);
	val -= 7;
	msr.hi &= ~(0x7 << CF07_UPPER_D0_PSZ_SHIFT);
	msr.hi |=  (val << CF07_UPPER_D0_PSZ_SHIFT);

	print_debug("computed msr.hi ");
	print_debug_hex32(msr.hi);
	print_debug("\n");

	msr.lo = 0x00003400;
	wrmsr(MC_CF07_DATA, msr);

	msr = rdmsr(MC_CF8F_DATA);
	msr.hi = 0x18000008;
	msr.lo = 0x296332a3;
	wrmsr(MC_CF8F_DATA, msr);		

}

#include "northbridge/amd/gx2/raminit.c"
#include "lib/generic_sdram.c"
#include "northbridge/amd/gx2/pll_reset.c"
#include "cpu/amd/model_gx2/cpureginit.c"
#include "cpu/amd/model_gx2/syspreinit.c"
#include "cpu/amd/model_lx/msrinit.c"

void main(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {(0xa<<3)|0, (0xa<<3)|1}}
	};

	SystemPreInit();

	cs5536_early_setup();
	
	/* cs5536_disable_internal_uart	 disable them. Set them up now... */
	cs5536_setup_onchipuart(1);

	uart_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pll_reset();

	cpuRegInit();
	print_err("done cpuRegInit\n");

	sdram_initialize(1, memctrl);
	print_err("ram setup done\n");

	msr_init();

	/* Check all of memory */
	/*ram_check(0x00000000, 640*1024);*/
	print_err("ram check done\n");
}

