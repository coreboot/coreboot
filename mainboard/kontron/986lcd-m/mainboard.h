/*
 * This file is part of the coreboot project.
 * Constants that are mainboard-defined and do not belong in Kconfig. 
 * We really do not want this stuff to be visible -- it will make it appear that they can be 
 * changed, and they can not. 
 * 
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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

#define CHANNEL_XOR_RANDOMIZATION 0
#define ENABLE_ACPI_MODE_IN_COREBOOT 1

/* never defined in v2 */
#define MAINBOARD_PCI_SUBSYSTEM_VENDOR_ID 0
#define MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID 0

/* nowhere else to go yet */
#define TEST_SMM_FLASH_LOCKDOWN 0

#define TTYS0_BASE 0x3f8

