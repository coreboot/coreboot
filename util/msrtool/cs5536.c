/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2009 Peter Stuge <peter@stuge.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include "msrtool.h"

int cs5536_probe(const struct targetdef *target, const struct cpuid_t *id) {
	return (NULL != pci_dev_find(0x1022, 0x2090));
}

/**
 * Documentation referenced:
 *
 * 33238G: AMD Geode(tm) CS5536 Companion Device Data Book
 * http://www.amd.com/files/connectivitysolutions/geode/geode_lx/33238G_cs5536_db.pdf
 *
 */

const struct msrdef cs5536_msrs[] = {
	/* 0x51400008-0x5140000f per 33238G pages 356-361 */
	/* 0x51400015 per 33238G pages 365-366 */
	/* 0x51400020-0x51400027 per 33238G pages 379-385 */
	{ 0x51400008, MSRTYPE_RDWR, MSR2(0, 0), "DIVIL_LBAR_IRQ", "Local BAR - IRQ Mapper", {
		{ 63, 15, RESERVED },
		{ 48, 1, RESERVED },
		{ 47, 4, "IO_MASK", "I/O Address Mask Value", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 43, 11, RESERVED },
		{ 32, 1, "LBAR_EN", "LBAR Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable LBAR" },
			{ MSR1(1), "Enable LBAR" },
			{ BITVAL_EOT }
		}},
		{ 31, 15, RESERVED },
		{ 16, 1, RESERVED },
		{ 15, 11, "BASE_ADDR", "Base Address in I/O Space", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 4, 5, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x51400009, MSRTYPE_RDWR, MSR2(0, 0), "DIVIL_LBAR_KEL", "Local BAR - Keyboard Emulation Logic from USB", {
		{ 63, 20, "MEM_MASK", "Memory Address Mask Value", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 43, 11, RESERVED },
		{ 32, 1, "LBAR_EN", "LBAR Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable LBAR" },
			{ MSR1(1), "Enable LBAR" },
			{ BITVAL_EOT }
		}},
		{ 31, 20, "BASE_ADDR", "Base Address in Memory Space", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 11, 12, RESERVED },
		{ BITS_EOT }
	}},
	/* 0x5140000a is not mentioned in the databook */
	{ 0x5140000b, MSRTYPE_RDWR, MSR2(0, 0), "DIVIL_LBAR_SMB", "Local BAR - System Management Bus", {
		{ 63, 15, RESERVED },
		{ 48, 1, RESERVED },
		{ 47, 4, "IO_MASK", "I/O Address Mask Value", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 43, 11, RESERVED },
		{ 32, 1, "LBAR_EN", "LBAR Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable LBAR" },
			{ MSR1(1), "Enable LBAR" },
			{ BITVAL_EOT }
		}},
		{ 31, 15, RESERVED },
		{ 16, 1, RESERVED },
		{ 15, 8, "BASE_ADDR", "Base Address in I/O Space", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x5140000c, MSRTYPE_RDWR, MSR2(0, 0), "DIVIL_LBAR_GPIO", "Local BAR - GPIO and Input Conditioning Functions", {
		{ 63, 15, RESERVED },
		{ 48, 1, RESERVED },
		{ 47, 4, "IO_MASK", "I/O Address Mask Value", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 43, 11, RESERVED },
		{ 32, 1, "LBAR_EN", "LBAR Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable LBAR" },
			{ MSR1(1), "Enable LBAR" },
			{ BITVAL_EOT }
		}},
		{ 31, 15, RESERVED },
		{ 16, 1, RESERVED },
		{ 15, 8, "BASE_ADDR", "Base Address in I/O Space", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x5140000d, MSRTYPE_RDWR, MSR2(0, 0), "DIVIL_LBAR_MFGPT", "Local BAR - MFGPTs", {
		{ 63, 15, RESERVED },
		{ 48, 1, RESERVED },
		{ 47, 4, "IO_MASK", "I/O Address Mask Value", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 43, 11, RESERVED },
		{ 32, 1, "LBAR_EN", "LBAR Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable LBAR" },
			{ MSR1(1), "Enable LBAR" },
			{ BITVAL_EOT }
		}},
		{ 31, 15, RESERVED },
		{ 16, 1, RESERVED },
		{ 15, 8, "BASE_ADDR", "Base Address in I/O Space", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x5140000e, MSRTYPE_RDWR, MSR2(0, 0), "DIVIL_LBAR_ACPI", "Local BAR - ACPI", {
		{ 63, 15, RESERVED },
		{ 48, 1, RESERVED },
		{ 47, 4, "IO_MASK", "I/O Address Mask Value", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 43, 11, RESERVED },
		{ 32, 1, "LBAR_EN", "LBAR Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable LBAR" },
			{ MSR1(1), "Enable LBAR" },
			{ BITVAL_EOT }
		}},
		{ 31, 15, RESERVED },
		{ 16, 1, RESERVED },
		{ 15, 8, "BASE_ADDR", "Base Address in I/O Space", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 7, 8, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x5140000f, MSRTYPE_RDWR, MSR2(0, 0), "DIVIL_LBAR_PMS", "Local BAR - Power Management Support", {
		{ 63, 15, RESERVED },
		{ 48, 1, RESERVED },
		{ 47, 4, "IO_MASK", "I/O Address Mask Value", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 43, 11, RESERVED },
		{ 32, 1, "LBAR_EN", "LBAR Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable LBAR" },
			{ MSR1(1), "Enable LBAR" },
			{ BITVAL_EOT }
		}},
		{ 31, 15, RESERVED },
		{ 16, 1, RESERVED },
		{ 15, 9, "BASE_ADDR", "Base Address in I/O Space", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 6, 7, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x51400015, MSRTYPE_RDWR, MSR2(0, 0x70), "DIVIL_BALL_OPTS", "Ball Options Control", {
		{ 63, 32, RESERVED },
		{ 31, 20, RESERVED },
		{ 11, 2, "SEC_BOOT_LOC", "Secondary Boot Location", PRESENT_BIN, {
			{ MSR1(0), "LPC ROM" },
			{ MSR1(2), "NOR Flash on IDE" },
			{ MSR1(3), "Firmware Hub" },
			{ BITVAL_EOT }
		}},
		{ 9, 2, "BOOT_OP_LATCHED", "Latched Value of Boot Option", PRESENT_BIN, {
			{ MSR1(0), "LPC ROM" },
			{ MSR1(2), "NOR Flash on IDE" },
			{ MSR1(3), "Firmware Hub" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, RESERVED },
		{ 6, 1, "PIN_OPT_LALL", "All LPC Pin Option Selection", PRESENT_BIN, {
			{ MSR1(0), "All LPC pins become GPIOs including LPC_DRQ# and LPC_SERIRQ" },
			{ MSR1(1), "All LPC pins are controlled by the LPC controller except LPC_DRQ# and LPC_SERIRQ (bits [5:4])" },
			{ BITVAL_EOT }
		}},
		{ 5, 1, "PIN_OPT_LIRQ", "LPC_SERIRQ or GPIO21 Pin Option Selection", PRESENT_BIN, {
			{ MSR1(0), "Ball G2 is GPIO21" },
			{ MSR1(1), "Ball G2 functions as LPC_SERIRQ" },
			{ BITVAL_EOT }
		}},
		{ 4, 1, "PIN_OPT_LDRQ", "LPC_DRQ# or GPIO20 Pin Option Selection", PRESENT_BIN, {
			{ MSR1(0), "Ball G1 is GPIO20" },
			{ MSR1(1), "Ball G1 functions as LPC_DRQ#" },
			{ BITVAL_EOT }
		}},
		{ 3, 2, "PRI_BOOT_LOC", "Primary Boot Location", PRESENT_BIN, {
			{ MSR1(0), "LPC ROM" },
			{ MSR1(2), "NOR Flash on IDE" },
			{ MSR1(3), "Firmware Hub" },
			{ BITVAL_EOT }
		}},
		{ 1, 1, RESERVED },
		{ 0, 1, "PIN_OPT_IDE", "IDE or Flash Controller Pin Function Selection", PRESENT_BIN, {
			{ MSR1(0), "All IDE pins associated with Flash Controller" },
			{ MSR1(1), "All IDE pins associated with IDE Controller" },
			{ BITVAL_EOT }
		}},
	}},
	{ 0x51400020, MSRTYPE_RDWR, MSR2(0, 0), "PIC_YSEL_LOW", "IRQ Mapper Unrestricted Y Select Low", {
		{ 63, 32, RESERVED },
		{ 31, 4, "MAP_Y7", "Map Unrestricted Y Input 7", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 27, 4, "MAP_Y6", "Map Unrestricted Y Input 6", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 23, 4, "MAP_Y5", "Map Unrestricted Y Input 5", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 19, 4, "MAP_Y4", "Map Unrestricted Y Input 4", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 15, 4, "MAP_Y3", "Map Unrestricted Y Input 3", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 11, 4, "MAP_Y2", "Map Unrestricted Y Input 2", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 7, 4, "MAP_Y1", "Map Unrestricted Y Input 1", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 3, 4, "MAP_Y0", "Map Unrestricted Y Input 0", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x51400021, MSRTYPE_RDWR, MSR2(0, 0), "PIC_YSEL_HIGH", "IRQ Mapper Unrestricted Y Select High", {
		{ 63, 32, RESERVED },
		{ 31, 4, "MAP_Y15", "Map Unrestricted Y Input 15", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 27, 4, "MAP_Y14", "Map Unrestricted Y Input 14", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 23, 4, "MAP_Y13", "Map Unrestricted Y Input 13", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 19, 4, "MAP_Y12", "Map Unrestricted Y Input 12", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 15, 4, "MAP_Y11", "Map Unrestricted Y Input 11", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 11, 4, "MAP_Y10", "Map Unrestricted Y Input 10", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 7, 4, "MAP_Y9", "Map Unrestricted Y Input 9", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 3, 4, "MAP_Y8", "Map Unrestricted Y Input 8", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x51400022, MSRTYPE_RDWR, MSR2(0, 0), "PIC_ZSEL_LOW", "IRQ Mapper Unrestricted Z Select Low", {
		{ 63, 32, RESERVED },
		{ 31, 4, "MAP_Z7", "Map Unrestricted Z Input 7", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 27, 4, "MAP_Z6", "Map Unrestricted Z Input 6", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 23, 4, "MAP_Z5", "Map Unrestricted Z Input 5", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 19, 4, "MAP_Z4", "Map Unrestricted Z Input 4", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 15, 4, "MAP_Z3", "Map Unrestricted Z Input 3", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 11, 4, "MAP_Z2", "Map Unrestricted Z Input 2", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 7, 4, "MAP_Z1", "Map Unrestricted Z Input 1", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 3, 4, "MAP_Z0", "Map Unrestricted Z Input 0", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x51400023, MSRTYPE_RDWR, MSR2(0, 0), "PIC_ZSEL_HIGH", "IRQ Mapper Unrestricted Z Select High", {
		{ 63, 32, RESERVED },
		{ 31, 4, "MAP_Z15", "Map Unrestricted Z Input 15", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 27, 4, "MAP_Z14", "Map Unrestricted Z Input 14", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 23, 4, "MAP_Z13", "Map Unrestricted Z Input 13", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 19, 4, "MAP_Z12", "Map Unrestricted Z Input 12", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 15, 4, "MAP_Z11", "Map Unrestricted Z Input 11", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 11, 4, "MAP_Z10", "Map Unrestricted Z Input 10", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 7, 4, "MAP_Z9", "Map Unrestricted Z Input 9", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ 3, 4, "MAP_Z8", "Map Unrestricted Z Input 8", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Interrupt Group 1" },
			{ MSR1(2), "Interrupt Group 2" },
			{ MSR1(3), "Interrupt Group 3" },
			{ MSR1(4), "Interrupt Group 4" },
			{ MSR1(5), "Interrupt Group 5" },
			{ MSR1(6), "Interrupt Group 6" },
			{ MSR1(7), "Interrupt Group 7" },
			{ MSR1(8), "Interrupt Group 8" },
			{ MSR1(9), "Interrupt Group 9" },
			{ MSR1(10), "Interrupt Group 10" },
			{ MSR1(11), "Interrupt Group 11" },
			{ MSR1(12), "Interrupt Group 12" },
			{ MSR1(13), "Interrupt Group 13" },
			{ MSR1(14), "Interrupt Group 14" },
			{ MSR1(15), "Interrupt Group 15" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x51400024, MSRTYPE_RDWR, MSR2(0, 0xffff), "PIC_IRQM_PRIM", "IRQ Mapper Primary Mask", {
		{ 63, 48, RESERVED },
		{ 15, 1, "PRIM15_MSK", "Primary Input 15 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 14, 1, "PRIM14_MSK", "Primary Input 14 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 13, 1, "PRIM13_MSK", "Primary Input 13 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 12, 1, "PRIM12_MSK", "Primary Input 12 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 11, 1, "PRIM11_MSK", "Primary Input 11 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 10, 1, "PRIM10_MSK", "Primary Input 10 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "PRIM9_MSK", "Primary Input 9 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "PRIM8_MSK", "Primary Input 8 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "PRIM7_MSK", "Primary Input 7 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 6, 1, "PRIM6_MSK", "Primary Input 6 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 5, 1, "PRIM5_MSK", "Primary Input 5 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 4, 1, "PRIM4_MSK", "Primary Input 4 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 3, 1, "PRIM3_MSK", "Primary Input 3 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 2, 1, RESERVED },
		{ 1, 1, "PRIM1_MSK", "Primary Input 1 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "PRIM0_MSK", "Primary Input 0 Mask", PRESENT_DEC, {
			{ MSR1(0), "Mask the interrupt source" },
			{ MSR1(1), "Do not mask the interrupt source" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x51400025, MSRTYPE_RDWR, MSR2(0, 0), "PIC_IRQM_LPC", "IRQ Mapper LPC Mask", {
		{ 63, 48, RESERVED },
		{ 15, 1, "LPC15_EN", "LPC Input 15 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 14, 1, "LPC14_EN", "LPC Input 14 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 13, 1, "LPC13_EN", "LPC Input 13 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 12, 1, "LPC12_EN", "LPC Input 12 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 11, 1, "LPC11_EN", "LPC Input 11 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 10, 1, "LPC10_EN", "LPC Input 10 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "LPC9_EN", "LPC Input 9 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "LPC8_EN", "LPC Input 8 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "LPC7_EN", "LPC Input 7 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 6, 1, "LPC6_EN", "LPC Input 6 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 5, 1, "LPC5_EN", "LPC Input 5 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 4, 1, "LPC4_EN", "LPC Input 4 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 3, 1, "LPC3_EN", "LPC Input 3 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 2, 1, RESERVED },
		{ 1, 1, "LPC1_EN", "LPC Input 1 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "LPC0_EN", "LPC Input 0 Enable", PRESENT_DEC, {
			{ MSR1(0), "Disable interrupt source" },
			{ MSR1(1), "Enable interrupt source" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x51400026, MSRTYPE_RDONLY, MSR2(0, 0), "PIC_XIRR_STS_LOW", "IRQ Mapper Extended Interrupt Request Status Low", {
		{ 63, 32, RESERVED },
		{ 31, 1, "IG7_STS_Z", "Unrestricted Source Z Input 7", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 30, 1, "IG7_STS_Y", "Unrestricted Source Y Input 7", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 29, 1, "IG7_STS_LPC", "LPC Input 7", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 28, 1, "IG7_STS_PRIM", "Primary Input 7", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 27, 1, "IG6_STS_Z", "Unrestricted Source Z Input 6", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 26, 1, "IG6_STS_Y", "Unrestricted Source Y Input 6", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 25, 1, "IG6_STS_LPC", "LPC Input 6", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 24, 1, "IG6_STS_PRIM", "Primary Input 6", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 23, 1, "IG5_STS_Z", "Unrestricted Source Z Input 5", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 22, 1, "IG5_STS_Y", "Unrestricted Source Y Input 5", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 21, 1, "IG5_STS_LPC", "LPC Input 5", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 20, 1, "IG5_STS_PRIM", "Primary Input 5", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 19, 1, "IG4_STS_Z", "Unrestricted Source Z Input 4", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 18, 1, "IG4_STS_Y", "Unrestricted Source Y Input 4", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 17, 1, "IG4_STS_LPC", "LPC Input 4", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 16, 1, "IG4_STS_PRIM", "Primary Input 4", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 15, 1, "IG3_STS_Z", "Unrestricted Source Z Input 3", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 14, 1, "IG3_STS_Y", "Unrestricted Source Y Input 3", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 13, 1, "IG3_STS_LPC", "LPC Input 3", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 12, 1, "IG3_STS_PRIM", "Primary Input 3", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 11, 1, "IG2_STS_Z", "Unrestricted Source Z Input 2", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 10, 1, "IG2_STS_Y", "Unrestricted Source Y Input 2", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 9, 2, RESERVED },
		{ 7, 1, "IG1_STS_Z", "Unrestricted Source Z Input 1", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 6, 1, "IG1_STS_Y", "Unrestricted Source Y Input 1", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 5, 1, "IG1_STS_LPC", "LPC Input 1", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 4, 1, "IG1_STS_PRIM", "Primary Input 1", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 3, 2, RESERVED },
		{ 1, 1, "IG0_STS_LPC", "LPC Input 0", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "IG0_STS_PRIM", "Primary Input 0", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x51400027, MSRTYPE_RDONLY, MSR2(0, 0), "PIC_XIRR_STS_HIGH", "IRQ Mapper Extended Interrupt Request Status High", {
		{ 63, 32, RESERVED },
		{ 31, 1, "IG15_STS_Z", "Unrestricted Source Z Input 15", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 30, 1, "IG15_STS_Y", "Unrestricted Source Y Input 15", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 29, 1, "IG15_STS_LPC", "LPC Input 15", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 28, 1, "IG15_STS_PRIM", "Primary Input 15", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 27, 1, "IG14_STS_Z", "Unrestricted Source Z Input 14", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 26, 1, "IG14_STS_Y", "Unrestricted Source Y Input 14", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 25, 1, "IG14_STS_LPC", "LPC Input 14", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 24, 1, "IG14_STS_PRIM", "Primary Input 14", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 23, 1, "IG13_STS_Z", "Unrestricted Source Z Input 13", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 22, 1, "IG13_STS_Y", "Unrestricted Source Y Input 13", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 21, 1, "IG13_STS_LPC", "LPC Input 13", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 20, 1, "IG13_STS_PRIM", "Primary Input 13", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 19, 1, "IG12_STS_Z", "Unrestricted Source Z Input 12", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 18, 1, "IG12_STS_Y", "Unrestricted Source Y Input 12", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 17, 1, "IG12_STS_LPC", "LPC Input 12", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 16, 1, "IG12_STS_PRIM", "Primary Input 12", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 15, 1, "IG11_STS_Z", "Unrestricted Source Z Input 11", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 14, 1, "IG11_STS_Y", "Unrestricted Source Y Input 11", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 13, 1, "IG11_STS_LPC", "LPC Input 11", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 12, 1, "IG11_STS_PRIM", "Primary Input 11", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 11, 1, "IG10_STS_Z", "Unrestricted Source Z Input 10", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 10, 1, "IG10_STS_Y", "Unrestricted Source Y Input 10", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "IG10_STS_LPC", "LPC Input 10", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "IG10_STS_PRIM", "Primary Input 10", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "IG9_STS_Z", "Unrestricted Source Z Input 9", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 6, 1, "IG9_STS_Y", "Unrestricted Source Y Input 9", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 5, 1, "IG9_STS_LPC", "LPC Input 9", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 4, 1, "IG9_STS_PRIM", "Primary Input 9", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 3, 1, "IG8_STS_Z", "Unrestricted Source Z Input 8", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 2, 1, "IG8_STS_Y", "Unrestricted Source Y Input 8", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 1, 1, "IG8_STS_LPC", "LPC Input 8", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "IG8_STS_PRIM", "Primary Input 8", PRESENT_BIN, {
			{ MSR1(0), "No interrupt" },
			{ MSR1(1), "INTERRUPT" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x5140004e, MSRTYPE_RDWR, MSR2(0, 0), "LPC_SERIRQ", "LPC Serial IRQ Control", {
		{ 31, 16, "INVERT", "IRQ[x] input is active low", PRESENT_HEX },
		{ 15, 8, RESERVED },
		{ 7, 1, "SIRQ_EN", "Serial IRQ Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Enable" },
			{ BITVAL_EOT }
		}},
		{ 6, 1, "SIRQ_MODE", "Serial IRQ Interface Mode", PRESENT_BIN, {
			{ MSR1(0), "Continuous (Idle)" },
			{ MSR1(1), "Quiet (Active)" },
			{ BITVAL_EOT }
		}},
		{ 5, 4, "IRQ_FRAME", "IRQ Data Frames", PRESENT_BIN, {
			{ MSR1(0), "17" },
			{ MSR1(1), "18" },
			{ MSR1(2), "19" },
			{ MSR1(3), "20" },
			{ MSR1(4), "21" },
			{ MSR1(5), "22" },
			{ MSR1(6), "23" },
			{ MSR1(7), "24" },
			{ MSR1(8), "25" },
			{ MSR1(9), "26" },
			{ MSR1(10), "27" },
			{ MSR1(11), "28" },
			{ MSR1(12), "29" },
			{ MSR1(13), "30" },
			{ MSR1(14), "31" },
			{ MSR1(15), "32" },
			{ BITVAL_EOT }
		}},
		{ 1, 2, "START_FPW", "Start Frame Pulse Width", PRESENT_BIN, {
			{ MSR1(0), "4 clocks" },
			{ MSR1(1), "6 clocks" },
			{ MSR1(2), "8 clocks" },
			{ MSR1(3), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ MSR_EOT }
};
