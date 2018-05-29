/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "early_vx900.h"
#include <arch/io.h>
#include <console/console.h>

/**
 * \brief Enable accessing of PCI configuration space for all devices.
 *
 * Enable accessing of D0F1 through D0F7, which would otherwise not be
 * accessible. If MMCONF is enabled, configure it here. This is the first
 * function that should be called in romstage.
 */
void vx900_enable_pci_config_space(void)
{
	/* MMCONF is not yet enabled, so we'll need to specify we want to do
	 * pci_io. We don't want to do pci_mmio until we enable it */
	/* Enable multifunction bit for northbridge.
	 * This enables the PCI configuration spaces of D0F1 to D0F7 to be
	 * accessed */
	pci_io_write_config8(HOST_CTR, 0x4f, 0x01);

	/* COOL, now enable MMCONF */
	u8 reg8 = pci_io_read_config8(TRAF_CTR, 0x60);
	reg8 |= 3;
	pci_io_write_config8(TRAF_CTR, 0x60, reg8);

	reg8 = CONFIG_MMCONF_BASE_ADDRESS >> 28;
	pci_io_write_config8(TRAF_CTR, 0x61, reg8);
}

/**
 *\brief Prints information regarding the hardware strapping on VX900
 *
 * Certain features on the VX900 are controlled by strapping pins which are
 * hardwired on the mainboard. These values determine whether the ROM is on the
 * SPI or LPC bus, or whether auto-reset is enabled.
 * \n
 * Having a feel for these values is important when trying to fix obscure
 * problems found when porting a mainboard based on the VX900.
 * \n
 * These values are decoded and printed to the terminal.
 */
void vx900_print_strapping_info(void)
{
	u8 strap = pci_read_config8(SNMIC, 0x56);

	printk(BIOS_DEBUG, "VX900 strapping pins indicate that:\n");
	printk(BIOS_DEBUG, " ROM is on %s bus\n",
	       (strap & (1 << 0)) ? "SPI" : "LPC");
	printk(BIOS_DEBUG, " Auto reset is %s\n",
	       (strap & (1 << 1)) ? "disabled" : "enabled");
	printk(BIOS_DEBUG, " LPC FWH command is %s\n",
	       (strap & (1 << 2)) ? "enabled" : "disabled");
	printk(BIOS_DEBUG, " Debug link is is %s\n",
	       (strap & (1 << 4)) ? "enabled" : "disabled");
	printk(BIOS_DEBUG, " PCI master mode is %s\n",
	       (strap & (1 << 5)) ? "enabled" : "disabled");
}

/**
 *\brief Disables the auto-reboot mechanism on VX900
 *
 * The VX900 has an auto-reboot mechanism that can be enabled by a hardware
 * strap. This mechanism can make development annoying, since we don't know if
 * the reset was caused by a bug in coreboot, or by this mechanism.
 */
void vx900_disable_auto_reboot(void)
{
	if (pci_read_config8(SNMIC, 0x56) & (1 << 1)) {
		printk(BIOS_DEBUG, "Auto-reboot is disabled in hardware\n");
		return;
	}
	/* Disable the GP3 timer, which is the root of all evil */
	pci_write_config8(LPC, 0x98, 0);
	/* Yep, that's all it takes */
	printk(BIOS_DEBUG, "GP3 timer disabled."
		    " Auto-reboot should not give you any more trouble.\n");
}

/**
 * \brief Disables 'shadowing' of system ROM
 *
 * Disable unnecessary shadowing of the ROM in the first 1MB of address space.
 * coreboot runs in 32-bit mode from the start. Shadowing only gets in the way.
 * This function frees the entire 640k-1M range for DRAM. VGA may still use
 * the 640k-768k range, if enabled later.
 */
void vx900_disable_legacy_rom_shadow(void)
{
	pci_write_config8(MCU, 0x80, 0xff);	/* LPC ROM 768k - 832k */
	pci_write_config8(MCU, 0x81, 0xff);	/* LPC ROM 832k - 896k */
	pci_write_config8(MCU, 0x82, 0xff);	/* LPC ROM 896k - 960k */
	/* LPC ROM 960k - 1M * SMRAM: 640k - 768k */
	pci_write_config8(MCU, 0x83, 0x31);

	/* Bits 6:0 are the ROM shadow on top of 4G, so leave those untouched */
	pci_mod_config8(LPC, 0x41, 1 << 7, 0);	/* LPC ROM 896k - 960k */

	pci_write_config8(SNMIC, 0x61, 0);	/* 768k - 832k */
	pci_write_config8(SNMIC, 0x62, 0);	/* 832k - 896k */
	pci_write_config8(SNMIC, 0x63, 0);	/* 896k - 1M   */
	pci_write_config8(SNMIC, 0x64, 0);	/* 896k - 960k */
}

/**
 * \brief Disables the VX900 integrated graphics controller
 *
 * Disable the graphics controller entirely. It will no longer be visible as a
 * PCI device.
 */
void vx900_disable_gfx(void)
{
	/* Disable GFX */
	pci_mod_config8(MCU, 0xa1, 1 << 7, 0);
}
