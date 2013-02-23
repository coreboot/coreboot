/*
 * This file is part of the coreboot project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _SOUTHBRIDGE_AMD_CS5536
#define _SOUTHBRIDGE_AMD_CS5536

#define MAX_UNWANTED_VPCI 8	/* increase if needed */

struct southbridge_amd_cs5536_config {
	unsigned int lpc_serirq_enable;	/* interrupt enables for LPC bus; each bit is an irq 0-15 */
	unsigned int lpc_serirq_polarity;	/* LPC IRQ polarity; each bit is an irq 0-15 */
	unsigned char lpc_serirq_mode;	/* 0:Continuous 1:Quiet */
	unsigned int enable_gpio_int_route;	/* GPIO(0-0x20) for INT D:C:B:A, 0xFF=none. See virtual pci spec... */
	unsigned char enable_ide_nand_flash;	/* 0:IDE 1:FLASH, if you are using nand flash instead of IDE drive */
	unsigned char enable_USBP4_device;	/* Enable USB Port 4 0:host 1:device */
	unsigned int enable_USBP4_overcurrent;	/* 0:off, xxxx:overcurrent setting, e.g. 0x3FEA CS5536 - Data Book (pages 380-381) */
	unsigned char com1_enable;	/* enable COM1 */
	unsigned int com1_address;	/* e.g. 0x3F8 */
	unsigned int com1_irq;	/* e.g. 4 */
	unsigned char com2_enable;	/* enable COM2 */
	unsigned int com2_address;	/* e.g. 0x2F8 */
	unsigned int com2_irq;	/* e.g. 3 */
	unsigned int unwanted_vpci[MAX_UNWANTED_VPCI];	/* the following allow you to disable unwanted virtualized PCI devices */
};

#endif				/* _SOUTHBRIDGE_AMD_CS5536 */
