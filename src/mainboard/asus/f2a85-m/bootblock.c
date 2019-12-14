/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2012 Rudolf Marek <r.marek@assembler.cz>
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

#include <bootblock_common.h>
#include <device/pnp_type.h>
#include <southbridge/amd/common/amd_defs.h>
#include <stdint.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#define SB_MMIO_MISC32(x) *(volatile u32 *)(AMD_SB_ACPI_MMIO_ADDR + 0xE00 + (x))

static void sbxxx_enable_48mhzout(void)
{
	/* most likely programming to 48MHz out signal */
	u32 reg32;
	reg32 = SB_MMIO_MISC32(0x28);
	reg32 &= 0xffc7ffff;
	reg32 |= 0x00100000;
	SB_MMIO_MISC32(0x28) = reg32;

	reg32 = SB_MMIO_MISC32(0x40);
	reg32 &= ~0x80u;
	SB_MMIO_MISC32(0x40) = reg32;
}

static void superio_init_m(void)
{
	pnp_devfn_t uart = PNP_DEV(0x2e, IT8728F_SP1);
	pnp_devfn_t gpio = PNP_DEV(0x2e, IT8728F_GPIO);

	ite_kill_watchdog(gpio);
	ite_enable_serial(uart, CONFIG_TTYS0_BASE);
	ite_enable_3vsbsw(gpio);
}

static void superio_init_m_pro(void)
{
	pnp_devfn_t uart = PNP_DEV(0x2e, NCT6779D_SP1);

	nuvoton_enable_serial(uart, CONFIG_TTYS0_BASE);
}

void bootblock_mainboard_early_init(void)
{
	/* enable SIO clock */
	sbxxx_enable_48mhzout();

	if (CONFIG(BOARD_ASUS_F2A85_M_PRO))
		superio_init_m_pro();
	else
		superio_init_m();
}
