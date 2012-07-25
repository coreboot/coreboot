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

void vx900_enable_pci_config_space(void)
{
	/* Enable multifunction bit for northbridge.
	 * This enables the PCI configuration spaces of D0F1 to D0F7 to be
	 * accessed */
	pci_write_config8(PCI_DEV(0, 0, 0), 0x4f, 0x01);
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
