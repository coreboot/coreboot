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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "early_vx900.h"
#include <arch/romcc_io.h>
#include <console/console.h>

void vx900_enable_pci_config_space(void)
{
	/* MMCONF is not yet enabled, so we'll need to specify we want to do
	 * pci_io. We don't want to do pci_mmio until we enable it */
	/* Enable multifunction bit for northbridge.
	 * This enables the PCI configuration spaces of D0F1 to D0F7 to be
	 * accessed */
	pci_io_write_config8(HOST_CTR, 0x4f, 0x01);

#if CONFIG_MMCONF_SUPPORT
	/* COOL, now enable MMCONF */
	u8 reg8 = pci_io_read_config8(TRAF_CTR, 0x60);
	reg8 |= 3;
	pci_io_write_config8(PCI_DEV(TRAF_CTR, 0x60, reg8);
	reg8 = CONFIG_MMCONF_BASE_ADDRESS >> 28;
	pci_io_write_config8(TRAF_CTR, 0x61, reg8);
#endif
}

void vx900_disable_legacy_rom_shadow(void)
{
	/* Disable shitty 8086 legacy shadows
	 * This frees the entire 640k-1M range for DRAM
	 * VGA may still use 640k-768k if enabled later */
	pci_write_config8(MCU, 0x80, 0xff); /* ROM 768k - 832k */
	pci_write_config8(MCU, 0x81, 0xff); /* ROM 832k - 896k */
	pci_write_config8(MCU, 0x82, 0xff); /* ROM 896k - 960k */
	/* ROM 960k - 1M * SMRAM: 640k - 768k */
	pci_write_config8(MCU, 0x83, 0x31);
}

void vx900_disable_gfx(void)
{
	/* Disable GFX */
	pci_mod_config8(MCU, 0xa1, 1<<7, 0);
}
