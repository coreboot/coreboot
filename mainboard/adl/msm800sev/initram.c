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
#include <io.h>
#include <device/pnp.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd_geodelx.h>
#include <southbridge/amd/cs5536/cs5536.h>
#include <northbridge/amd/geodelx/raminit.h>
#include <superio/winbond/w83627hf/w83627hf.h>

#define SERIAL_DEV 0x30

#define MANUALCONF 0		/* Do automatic strapped PLL config */
#define PLLMSRHI 0x00001490	/* manual settings for the PLL */
#define PLLMSRLO 0x02000030
#define DIMM0 ((u8) 0xA0)
#define DIMM1 ((u8) 0xA2)

int main(void)
{
	void done_cache_as_ram_main(void);
	void w83627hf_enable_serial(u8 dev, u8 serial, u16 iobase);
	post_code(POST_START_OF_MAIN);

	system_preinit();
	geodelx_msr_init();

	cs5536_early_setup();

	/* NOTE: must do this AFTER the early_setup!
	 * it is counting on some early MSR setup
	 * for cs5536
	 */
	cs5536_disable_internal_uart();
	w83627hf_enable_serial(0x2e, 0x30, 0x3f8);
	console_init();

	pll_reset(MANUALCONF, PLLMSRHI, PLLMSRLO);

	cpu_reg_init(0, DIMM0, DIMM1);
	sdram_set_registers();
	sdram_set_spd_registers(DIMM0, DIMM1);
	sdram_enable(DIMM0, DIMM1);

	/* Check low memory */
	//ram_check(0x00000000, 640*1024);

	/* Switch from Cache as RAM to real RAM */
	printk(BIOS_SPEW, "Before wbinvd\n");
	__asm__("wbinvd\n");
	printk(BIOS_SPEW, "After wbinvd\n");
}
