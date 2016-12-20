/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC
 * Copyright (C) 2014 Kyösti Mälkki <kyosti.malkki@gmail.com>
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

#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/cimx/cimx_util.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5104d/nct5104d.h>
#include "gpio_ftns.h"
#include "SB800.h"

#define SIO_PORT 0x2e
#define SERIAL_DEV PNP_DEV(SIO_PORT, NCT5104D_SP1)

static void early_lpc_init(void)
{
	u32 mmio_base;

	/* PC Engines requires system boot when power is applied. This feature is
	 * controlled in PM_REG 5Bh register. "Always Power On" works by writing a
	 * value of 05h.
	 */
	u8 bdata = pm_ioread(SB_PMIOA_REG5B);
	bdata &= 0xf8; //clear bits 0-2
	bdata |= 0x05; //set bits 0,2
	pm_iowrite(SB_PMIOA_REG5B, bdata);

	/* Multi-function pins switch to GPIO0-35, these pins are shared with PCI pins */
	bdata = pm_ioread(SB_PMIOA_REGEA);
	bdata &= 0xfe; //clear bit 0
	bdata |= 0x01; //set bit 0
	pm_iowrite(SB_PMIOA_REGEA, bdata);

	//configure required GPIOs
	mmio_base = find_gpio_base();
	configure_gpio(mmio_base, GPIO_10,  GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_HIGH);
	configure_gpio(mmio_base, GPIO_11,  GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_HIGH);
	configure_gpio(mmio_base, GPIO_15,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_16,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_17,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_18,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_187, GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_189, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
	configure_gpio(mmio_base, GPIO_190, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
	configure_gpio(mmio_base, GPIO_191, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
}

void board_BeforeAgesa(struct sysinfo *cb)
{
	early_lpc_init();
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
