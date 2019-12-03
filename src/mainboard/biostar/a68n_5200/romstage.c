/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2016 Edward O'Callaghan <funfunctor@folklore1984.net>
 * Copyright (C) 2017 Damien Zammit <damien@zamaudio.com>
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

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define SB_MMIO 0xFED80000
#define SB_MMIO_MISC32(x) *(volatile u32 *)(SB_MMIO + 0xE00 + (x))

#define SERIAL_DEV PNP_DEV(0x2e, IT8728F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8728F_GPIO)
#define CLKIN_DEV PNP_DEV(0x2e, IT8728F_GPIO)

static void sbxxx_enable_48mhzout(void)
{
	/* most likely programming to 48MHz out signal */
	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 48MHz */
	u32 reg32;
	reg32 = SB_MMIO_MISC32(0x28);
	reg32 &= 0xfff8ffff;
	SB_MMIO_MISC32(0x28) = reg32;

	/* Enable Auxiliary Clock1, disable FCH 14 MHz OscClk */
	reg32 = SB_MMIO_MISC32(0x40);
	reg32 &= 0xffffbffb;
	SB_MMIO_MISC32(0x40) = reg32;
}

void board_BeforeAgesa(struct sysinfo *cb)
{
	u8 byte;

	/* Enable the AcpiMmio space */
	outb(0x24, 0xcd6);
	outb(0x1, 0xcd7);

	/* Set LPC decode enables. */
	pci_devfn_t dev = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev, 0x44, 0xff03ffd5);

	/* enable SIO LPC decode */
	byte = pci_read_config8(dev, 0x48);
	byte |= 3;	/* 2e, 2f */
	pci_write_config8(dev, 0x48, byte);

	/* enable serial decode */
	byte = pci_read_config8(dev, 0x44);
	byte |= (1 << 6);  /* 0x3f8 */
	pci_write_config8(dev, 0x44, byte);

	/* run ite */
	sbxxx_enable_48mhzout();
	ite_conf_clkin(CLKIN_DEV, ITE_UART_CLK_PREDIVIDE_48);
	ite_kill_watchdog(GPIO_DEV);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
