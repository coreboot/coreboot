/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2010 Anders Jenbo <anders@jenbo.dk>
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
 */

#include <stdint.h>
#include <stdlib.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <southbridge/intel/i82801ax/i82801ax.h>
#include <northbridge/intel/i82810/raminit.h>
#include "drivers/pc80/udelay_io.c"
#include <cpu/x86/bist.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8712f/it8712f.h>
#include <lib.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8712F_SP1)
#define CLKIN_DEV PNP_DEV(0x2e, IT8712F_GPIO)

#include <cpu/intel/romstage.h>
void main(unsigned long bist)
{
	ite_conf_clkin(CLKIN_DEV, ITE_UART_CLK_PREDIVIDE_24);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	report_bist_failure(bist);
	enable_smbus();
	dump_spd_registers();
	sdram_set_registers();
	sdram_set_spd_registers();
	sdram_enable();
	dump_spd_registers();
}
