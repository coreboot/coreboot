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

#include <arch/io.h>
#include <console/console.h>

#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/common/amd_defs.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <southbridge/amd/agesa/hudson/smbus.h>
#include <stdint.h>
#include <string.h>

#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#define MMIO_NON_POSTED_START 0xfed00000
#define MMIO_NON_POSTED_END   0xfedfffff
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

void board_BeforeAgesa(struct sysinfo *cb)
{
	u8 byte;
	pci_devfn_t dev;

	if (IS_ENABLED(CONFIG_POST_DEVICE_PCI_PCIE))
		hudson_pci_port80();
	else if (IS_ENABLED(CONFIG_POST_DEVICE_LPC))
		hudson_lpc_port80();

	/* enable SIO LPC decode */
	dev = PCI_DEV(0, 0x14, 3);
	byte = pci_read_config8(dev, 0x48);
	byte |= 3;		/* 2e, 2f */
	pci_write_config8(dev, 0x48, byte);

	/* enable serial decode */
	byte = pci_read_config8(dev, 0x44);
	byte |= (1 << 6);  /* 0x3f8 */
	pci_write_config8(dev, 0x44, byte);

	post_code(0x30);

	/* enable SB MMIO space */
	outb(0x24, 0xcd6);
	outb(0x1, 0xcd7);

	/* enable SIO clock */
	sbxxx_enable_48mhzout();

	if (IS_ENABLED(CONFIG_BOARD_ASUS_F2A85_M_PRO))
		superio_init_m_pro();
	else
		superio_init_m();

	/* turn on secondary smbus at b20 */
	outb(0x28, 0xcd6);
	byte = inb(0xcd7);
	byte |= 1;
	outb(byte, 0xcd7);

	/* set DDR3 voltage */
	byte = CONFIG_BOARD_ASUS_F2A85_M_DDR3_VOLT_VAL;

	/* default is byte = 0x0, so no need to set it in this case */
	if (byte)
		do_smbus_write_byte(0xb20, 0x15, 0x3, byte);
}
