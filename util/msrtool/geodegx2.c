/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2008 Peter Stuge <peter@stuge.se>
 * Copyright (c) 2009 Nils Jacobs <njacobs8@hetnet.nl>
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

int geodegx2_probe(const struct targetdef *target, const struct cpuid_t *id) {
	return 5 == id->family && 5 == id->model;
}

const struct msrdef geodegx2_msrs[] = {
	{ 0x10000020, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU0_P2D_BM0", "GLIU0 P2D Base Mask Descriptor 0", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x10000021, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU0_P2D_BM1", "GLIU0 P2D Base Mask Descriptor 1", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x10000022, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU0_P2D_BM2", "GLIU0 P2D Base Mask Descriptor 2", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x10000023, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU0_P2D_BM3", "GLIU0 P2D Base Mask Descriptor 3", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x10000024, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU0_P2D_BM4", "GLIU0 P2D Base Mask Descriptor 4", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x10000025, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU0_P2D_BM5", "GLIU0 P2D Base Mask Descriptor 5", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x10000026, MSRTYPE_RDWR, MSR2(0x00000FF0, 0xFFF00000), "GLIU0_P2D_BMO0", "GLIU0 P2D Base Mask Offset Descriptor 0", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, "POFFSET", "Physical Memory Address 2s Comp Offset", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x10000027, MSRTYPE_RDWR, MSR2(0x00000FF0, 0xFFF00000), "GLIU0_P2D_BMO1", "GLIU0 P2D Base Mask Offset Descriptor 1", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, "POFFSET", "Physical Memory Address 2s Comp Offset", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x10000028, MSRTYPE_RDWR, MSR2(0x00000000, 0x000FFFFF), "GLIU0_P2D_R0", "GLIU0 P2D Range Descriptor 0", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PMAX", "Physical Memory Address Max.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMIN", "Physical Memory Address Min.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x10000029, MSRTYPE_RDWR, MSR2(0x00000000, 0x000FFFFF), "GLIU0_P2D_RO0", "GLIU0 P2D Range Offset Descriptor 0", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, "POFFSET", "Physical Memory Address 2s Comp Offset", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 39, 20, "PMAX", "Physical Memory Address Max.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMIN", "Physical Memory Address Min.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x1000002A, MSRTYPE_RDWR, MSR2(0x00000000, 0x000FFFFF), "GLIU0_P2D_RO1", "GLIU0 P2D Range Offset Descriptor 1", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, "POFFSET", "Physical Memory Address 2s Comp Offset", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 39, 20, "PMAX", "Physical Memory Address Max.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMIN", "Physical Memory Address Min.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x1000002B, MSRTYPE_RDWR, MSR2(0x00000000, 0x000FFFFF), "GLIU0_P2D_RO2", "GLIU0 P2D Range Offset Descriptor 2", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, "POFFSET", "Physical Memory Address 2s Comp Offset", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 39, 20, "PMAX", "Physical Memory Address Max.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMIN", "Physical Memory Address Min.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x1000002C, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU0_P2D_SC0", "GLIU0 P2D Swiss Cheese Descriptor 0", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 12, RESERVED },
		{ 47, 16, "WEN", "Enable hits to the base for the ith 16K page for writes", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 16, "REN", "Enable hits to the base for the ith 16K page for ", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 15, 2, RESERVED },
		{ 13, 14, "PSCBASE", "Physical Memory Address Base for hit", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x100000E0, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU0_IOD_BM0", "GLIU0 IOD Base Mask Descriptor 0", {
		{ 63, 3, "IDID", "IO Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "IBASE", "Physical IO Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "IMASK", "Physical IO Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x100000E1, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU0_IOD_BM1", "GLIU0 IOD Base Mask Descriptor 1", {
		{ 63, 3, "IDID", "IO Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "IBASE", "Physical IO Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "IMASK", "Physical IO Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x100000E2, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU0_IOD_BM2", "GLIU0 IOD Base Mask Descriptor 2", {
		{ 63, 3, "IDID", "IO Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "IBASE", "Physical IO Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "IMASK", "Physical IO Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x100000E3, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU0_IOD_SC0", "GLIU0 IOD Swiss Cheese Descriptor 0", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x100000E4, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU0_IOD_SC1", "GLIU0 IOD Swiss Cheese Descriptor 1", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x100000E5, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU0_IOD_SC2", "GLIU0 IOD Swiss Cheese Descriptor 2", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x100000E6, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU0_IOD_SC3", "GLIU0 IOD Swiss Cheese Descriptor 3", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x100000E7, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU0_IOD_SC4", "GLIU0 IOD Swiss Cheese Descriptor 4", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x100000E8, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU0_IOD_SC5", "GLIU0 IOD Swiss Cheese Descriptor 5", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x20000018, MSRTYPE_RDWR, MSR2(0x10071007, 0x40), "MC_CF07_DATA", "Refresh and SDRAM Program", {
		{ 63, 4, "D1_SZ", "DIMM1 Size", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "8 MB" },
			{ MSR1(2), "16 MB" },
			{ MSR1(3), "32 MB" },
			{ MSR1(4), "64 MB" },
			{ MSR1(5), "128 MB" },
			{ MSR1(6), "256 MB" },
			{ MSR1(7), "512 MB" },
			{ MSR1(8), "Reserved" },
			{ MSR1(9), "Reserved" },
			{ MSR1(10), "Reserved" },
			{ MSR1(11), "Reserved" },
			{ MSR1(12), "Reserved" },
			{ MSR1(13), "Reserved" },
			{ MSR1(14), "Reserved" },
			{ MSR1(15), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ 59, 3, RESERVED },
		{ 56, 1, "D1_MB", "DIMM1 Module Banks", PRESENT_BIN, {
			{ MSR1(0), "1 Module bank" },
			{ MSR1(1), "2 Module banks" },
			{ BITVAL_EOT }
		}},
		{ 55, 3, RESERVED },
		{ 52, 1, "D1_CB", "DIMM1 Component Banks", PRESENT_BIN, {
			{ MSR1(0), "2 Component banks" },
			{ MSR1(1), "4 Component banks" },
			{ BITVAL_EOT }
		}},
		{ 51, 1, RESERVED },
		{ 50, 3, "D1_PSZ", "DIMM1 Page Size", PRESENT_BIN, {
			{ MSR1(0), "1 KB" },
			{ MSR1(1), "2 KB" },
			{ MSR1(2), "4 KB" },
			{ MSR1(3), "8 KB" },
			{ MSR1(4), "16 KB" },
			{ MSR1(5), "Reserved" },
			{ MSR1(6), "Reserved" },
			{ MSR1(7), "DIMM1 Not Installed" },
			{ BITVAL_EOT }
		}},
		{ 47, 4, "D0_SZ", "DIMM0 Size", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "8 MB" },
			{ MSR1(2), "16 MB" },
			{ MSR1(3), "32 MB" },
			{ MSR1(4), "64 MB" },
			{ MSR1(5), "128 MB" },
			{ MSR1(6), "256 MB" },
			{ MSR1(7), "512 MB" },
			{ MSR1(8), "Reserved" },
			{ MSR1(9), "Reserved" },
			{ MSR1(10), "Reserved" },
			{ MSR1(11), "Reserved" },
			{ MSR1(12), "Reserved" },
			{ MSR1(13), "Reserved" },
			{ MSR1(14), "Reserved" },
			{ MSR1(15), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ 43, 3, RESERVED },
		{ 40, 1, "D0_MB", "DIMM0 Module Banks", PRESENT_BIN, {
			{ MSR1(0), "1 Module bank" },
			{ MSR1(1), "2 Module banks" },
			{ BITVAL_EOT }
		}},
		{ 39, 3, RESERVED },
		{ 36, 1, "D0_CB", "DIMM0 Component Banks", PRESENT_BIN, {
			{ MSR1(0), "2 Component banks" },
			{ MSR1(1), "4 Component banks" },
			{ BITVAL_EOT }
		}},
		{ 35, 1, RESERVED },
		{ 34, 3, "D0_PSZ", "DIMM0 Page Size", PRESENT_BIN, {
			{ MSR1(0), "1 KB" },
			{ MSR1(1), "2 KB" },
			{ MSR1(2), "4 KB" },
			{ MSR1(3), "8 KB" },
			{ MSR1(4), "16 KB" },
			{ MSR1(5), "Reserved" },
			{ MSR1(6), "Reserved" },
			{ MSR1(7), "DIMM0 Not Installed" },
			{ BITVAL_EOT }
		}},
		{ 31, 2, RESERVED },
		{ 29, 2, "EMR_BA", "Mode Register Set Bank Address", PRESENT_BIN, {
			{ MSR1(0), "Program the DIMM Mode Register" },
			{ MSR1(1), "Program the DIMM Extended Mode Register" },
			{ MSR1(2), "Reserved" },
			{ MSR1(3), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ 27, 1, RESERVED },
		{ 26, 1, "EMR_QFC", "Extended Mode Register FET Control", PRESENT_BIN, {
			{ MSR1(0), "Enable" },
			{ MSR1(1), "Disable" },
			{ BITVAL_EOT }
		}},
		{ 25, 1, "EMR_DRV", "Extended Mode Register Drive Strength Control", PRESENT_BIN, {
			{ MSR1(0), "Normal" },
			{ MSR1(1), "Reduced" },
			{ BITVAL_EOT }
		}},
		{ 24, 1, "EMR_DLL", "Extended Mode Register DLL", PRESENT_BIN, {
			{ MSR1(0), "Enable" },
			{ MSR1(1), "Disable" },
			{ BITVAL_EOT }
		}},
		{ 23, 16, "REF_INT", "Refresh Interval", PRESENT_DEC, NOBITS },
		{ 7, 2, "REF_STAG", "Refresh Staggering", PRESENT_DEC, {
			{ MSR1(0), "4 SDRAM Clks" },
			{ MSR1(1), "1 SDRAM Clks" },
			{ MSR1(2), "2 SDRAM Clks" },
			{ MSR1(3), "3 SDRAM Clks" },
			{ BITVAL_EOT }
		}},
		{ 5, 2, RESERVED },
		{ 3, 1, "REF_TST", "Test Refresh", PRESENT_BIN, NOBITS },
		{ 2, 1, RESERVED },
		{ 1, 1, "SOFT_RST", "Software Reset", PRESENT_BIN, NOBITS },
		{ 0, 1, "PROG_DRAM", "Program Mode Register in SDRAM", PRESENT_BIN, NOBITS },
		{ BITS_EOT }
	}},
	{ 0x20000019, MSRTYPE_RDWR, MSR2(0x18000008, 0x287337a3), "MC_CF8F_DATA", "Timing and Mode Program", {
		{ 63, 8, "STALE_REQ", "GLIU Max Stale Request Count", PRESENT_DEC, NOBITS },
		{ 55, 3, RESERVED },
		{ 52, 2, "XOR_BIT_SEL", "XOR Bit Select", PRESENT_BIN, {
			{ MSR1(0), "ADDR[18]" },
			{ MSR1(1), "ADDR[19]" },
			{ MSR1(2), "ADDR[20]" },
			{ MSR1(3), "ADDR[21]" },
			{ BITVAL_EOT }
		}},
		{ 50, 1, "XOR_MB0", "XOR MB0 Enable", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 49, 1, "XOR_BA1", "XOR BA1 Enable", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 48, 1, "XOR_BA0", "XOR BA0 Enable", PRESENT_BIN, {
			{ MSR1(0), "Disabled" },
			{ MSR1(1), "Enabled" },
			{ BITVAL_EOT }
		}},
		{ 47, 8, RESERVED },
		{ 39, 1, "AP_B2B", "Autoprecharge Back-to-Back Command", PRESENT_BIN, {
			{ MSR1(0), "Enable" },
			{ MSR1(1), "Disable" },
			{ BITVAL_EOT }
		}},
		{ 38, 1, "AP_EN", "Autoprecharge", PRESENT_BIN, {
			{ MSR1(0), "Enable" },
			{ MSR1(1), "Disable" },
			{ BITVAL_EOT }
		}},
		{ 37, 4, RESERVED },
		{ 33, 1, "HOI_LOI", "High / Low Order Interleave Select", PRESENT_BIN, {
			{ MSR1(0), "Low Order Interleave" },
			{ MSR1(1), "High Order Interleave" },
			{ BITVAL_EOT }
		}},
		{ 32, 1, RESERVED },
		{ 31, 1, "THZ_DLY", "tHZ Delay", PRESENT_BIN, NOBITS },
		{ 30, 3, "CAS_LAT", "Read CAS Latency", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "Reserved" },
			{ MSR1(2), "2 Clks" },
			{ MSR1(3), "Reserved" },
			{ MSR1(4), "Reserved" },
			{ MSR1(5), "1.5 Clks" },
			{ MSR1(6), "2.5 Clks" },
			{ MSR1(7), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ 27, 4, "REF2ACT", "ACT to ACT/REF Period. tRC", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "1 Clks" },
			{ MSR1(2), "2 Clks" },
			{ MSR1(3), "3 Clks" },
			{ MSR1(4), "4 Clks" },
			{ MSR1(5), "5 Clks" },
			{ MSR1(6), "7 Clks" },
			{ MSR1(7), "8 Clks" },
			{ MSR1(8), "9 Clks" },
			{ MSR1(9), "10 Clks" },
			{ MSR1(10), "11 Clks" },
			{ MSR1(11), "12 Clks" },
			{ MSR1(12), "13 Clks" },
			{ MSR1(13), "14 Clks" },
			{ MSR1(14), "15 Clks" },
			{ MSR1(15), "16 Clks" },
			{ BITVAL_EOT }
		}},
		{ 23, 4, "ACT2PRE", "ACT to PRE Period. tRAS", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "1 Clks" },
			{ MSR1(2), "2 Clks" },
			{ MSR1(3), "3 Clks" },
			{ MSR1(4), "4 Clks" },
			{ MSR1(5), "5 Clks" },
			{ MSR1(6), "7 Clks" },
			{ MSR1(7), "8 Clks" },
			{ MSR1(8), "9 Clks" },
			{ MSR1(9), "10 Clks" },
			{ MSR1(10), "11 Clks" },
			{ MSR1(11), "12 Clks" },
			{ MSR1(12), "13 Clks" },
			{ MSR1(13), "14 Clks" },
			{ MSR1(14), "15 Clks" },
			{ MSR1(15), "16 Clks" },
			{ BITVAL_EOT }
		}},
		{ 19, 1, RESERVED },
		{ 18, 3, "PRE2ACT", "PRE to ACT Period. tRP", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "1 Clks" },
			{ MSR1(2), "2 Clks" },
			{ MSR1(3), "3 Clks" },
			{ MSR1(4), "4 Clks" },
			{ MSR1(5), "5 Clks" },
			{ MSR1(6), "6 Clks" },
			{ MSR1(7), "7 Clks" },
			{ BITVAL_EOT }
		}},
		{ 15, 1, RESERVED },
		{ 14, 3, "ACT2CMD", "Delay Time from ACT to Read/Write. tRCD", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "1 Clks" },
			{ MSR1(2), "2 Clks" },
			{ MSR1(3), "3 Clks" },
			{ MSR1(4), "4 Clks" },
			{ MSR1(5), "5 Clks" },
			{ MSR1(6), "6 Clks" },
			{ MSR1(7), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ 11, 4, "ACT2ACT", "ACT(0) to ACT(1) Period. tRRD", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "1 Clks" },
			{ MSR1(2), "2 Clks" },
			{ MSR1(3), "3 Clks" },
			{ MSR1(4), "4 Clks" },
			{ MSR1(5), "5 Clks" },
			{ MSR1(6), "6 Clks" },
			{ MSR1(7), "7 Clks" },
			{ MSR1(8), "Reserved" },
			{ MSR1(9), "Reserved" },
			{ MSR1(10), "Reserved" },
			{ MSR1(11), "Reserved" },
			{ MSR1(12), "Reserved" },
			{ MSR1(13), "Reserved" },
			{ MSR1(14), "Reserved" },
			{ MSR1(15), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ 7, 2, "DPLWR", "Data-in to PRE Period. tDPLW", PRESENT_DEC, {
			{ MSR1(0), "Invalid value" },
			{ MSR1(1), "1 Clks" },
			{ MSR1(2), "2 Clks" },
			{ MSR1(3), "3 Clks" },
			{ BITVAL_EOT }
		}},
		{ 5, 2, "DPLRD", "Data-in to PRE Period. tDPLR", PRESENT_DEC, {
			{ MSR1(0), "Invalid value" },
			{ MSR1(1), "1 Clks" },
			{ MSR1(2), "2 Clks" },
			{ MSR1(3), "3 Clks" },
			{ BITVAL_EOT }
		}},
		{ 3, 1, RESERVED },
		{ 2, 3, "DAL", "Data-in to ACT (REF) Period. tDAL", PRESENT_BIN, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "1 clks" },
			{ MSR1(2), "2 Clks" },
			{ MSR1(3), "3 Clks" },
			{ MSR1(4), "4 Clks" },
			{ MSR1(5), "5 Clks" },
			{ MSR1(6), "6 Clks" },
			{ MSR1(7), "7 Clks" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x2000001a, MSRTYPE_RDWR, MSR2(0, 0), "MC_CF1017_DATA", "Feature Enables", {
		{ 63, 55, RESERVED },
		{ 8, 1, "PM1_UP_DLY", "PMode1 Up Delay", PRESENT_DEC, {
			{ MSR1(0), "No delay" },
			{ MSR1(1), "Enable delay" },
			{ BITVAL_EOT }
		}},
			{ 7, 5, RESERVED },
		{ 2, 3, "WR2DAT", "Write Command to Data Latency", PRESENT_DEC, {
			{ MSR1(0), "Reserved" },
			{ MSR1(1), "Value when unbuffered DDR SDRAMs are used" },
			{ MSR1(2), "Value when registered DDR SDRAMs are used" },
			{ MSR1(3), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x2000001b, MSRTYPE_RDONLY, MSR2(0, 0), "MC_CFPERF_CNT1", "Performance Counters", {
		{ 63, 32, "CNT0", "Counter 0", PRESENT_DEC, NOBITS },
		{ 31, 32, "CNT1", "Counter 1", PRESENT_DEC, NOBITS },
		{ BITS_EOT }
	}},
	{ 0x2000001c, MSRTYPE_RDWR, MSR2(0, 0x00ff00ff), "MC_PERFCNT2", "Counter and CAS Control", {
		{ 63, 28, RESERVED },
		{ 35, 1, "STOP_CNT1", "Stop Counter 1", PRESENT_DEC, {
			{ MSR1(0), "Counter 1 counts" },
			{ MSR1(1), "Stop Counter" },
			{ BITVAL_EOT }
		}},
			{ 34, 1, "RST_CNT1", "Reset Counter 1", PRESENT_DEC, {
			{ MSR1(0), "Do nothing" },
			{ MSR1(1), "Reset counter 1" },
			{ BITVAL_EOT }
		}},
			{ 33, 1, "STOP_CNT0", "Stop Counter 0", PRESENT_DEC, {
			{ MSR1(0), "Counter 0 counts" },
			{ MSR1(1), "Stop counter 0" },
			{ BITVAL_EOT }
		}},
			{ 32, 1, "RST_CNT0", "Reset Counter 0", PRESENT_DEC, {
			{ MSR1(0), "Do nothing" },
			{ MSR1(1), "Reset counter 0" },
			{ BITVAL_EOT }
		}},
		{ 31, 8, "CNT1_MASK", "Counter 1 Mask", PRESENT_BIN, NOBITS },
		{ 23, 8, "CNT1_DATA", "Counter 1 Data", PRESENT_BIN, NOBITS },
		{ 15, 8, "CNT0_MASK", "Counter 0 Mask", PRESENT_BIN, NOBITS },
		{ 7, 8, "CNT0_DATA", "Counter 0 Data", PRESENT_BIN, NOBITS },
		{ BITS_EOT }
	}},
	{ 0x2000001d, MSRTYPE_RDWR, MSR2(0, 0x300), "MC_CFCLK_DBUG", "Clocking and Debug", {
		{ 63, 29, RESERVED },
		{ 34, 1, "B2B_EN", "Back-to-Back Command Enable", PRESENT_BIN, {
			{ MSR1(0), "Allow back-to-back commands" },
			{ MSR1(1), "Disable back-to-back commands" },
			{ BITVAL_EOT }
		}},
		{ 33, 1, RESERVED },
		{ 32, 1, "MTEST_EN", "MTEST Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Enable" },
			{ BITVAL_EOT }
		}},
		{ 31, 22, RESERVED },
		{ 9, 1, "MASK_CKE[1:0]", "CKE Mask", PRESENT_BIN, {
			{ MSR1(0), "CKE1 output enable unmasked" },
			{ MSR1(1), "CKE1 output enable masked" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "MASK_CKE0", "CKE0 Mask", PRESENT_BIN, {
			{ MSR1(0), "CKE0 output enable unmasked" },
			{ MSR1(1), "CKE0 output enable masked" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "CNTL_MSK1", "Control Mask 1", PRESENT_BIN, {
			{ MSR1(0), "DIMM1 CAS1# RAS1# WE1# CS[3:2]# output enable unmasked" },
			{ MSR1(1), "DIMM1 CAS1# RAS1# WE1# CS[3:2]# output enable masked" },
			{ BITVAL_EOT }
		}},
		{ 6, 1, "CNTL_MSK0", "Control Mask 0", PRESENT_BIN, {
			{ MSR1(0), "DIMM0 CAS0# RAS0# WE0# CS[1:0]# output enable unmasked" },
			{ MSR1(1), "DIMM0 CAS0# RAS0# WE0# CS[1:0]# output enable masked" },
			{ BITVAL_EOT }
		}},
		{ 5, 1, "ADRS_MSK", "Address Mask", PRESENT_BIN, {
			{ MSR1(0), "MA and BA output enable unmasked" },
			{ MSR1(1), "MA and BA output enable masked" },
			{ BITVAL_EOT }
		}},
		{ 4, 5, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x40000020, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_P2D_BM0", "GLIU1 P2D Base Mask Descriptor 0", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x40000021, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_P2D_BM1", "GLIU1 P2D Base Mask Descriptor 1", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x40000022, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_P2D_BM2", "GLIU1 P2D Base Mask Descriptor 2", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x40000023, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_P2D_BM3", "GLIU1 P2D Base Mask Descriptor 3", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x40000024, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_P2D_BM4", "GLIU1 P2D Base Mask Descriptor 4", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x40000025, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_P2D_BM5", "GLIU1 P2D Base Mask Descriptor 5", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x40000026, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_P2D_BM6", "GLIU1 P2D Base Mask Descriptor 6", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x40000027, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_P2D_BM7", "GLIU1 P2D Base Mask Descriptor 7", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x40000028, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_P2D_BM8", "GLIU1 P2D Base Mask Descriptor 8", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PBASE", "Physical Memory Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMASK", "Physical Memory Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x40000029, MSRTYPE_RDWR, MSR2(0x00000000, 0x000FFFFF), "GLIU1_P2D_R0", "GLIU0 P2D Range Descriptor 0", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PMAX", "Physical Memory Address Max.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMIN", "Physical Memory Address Min.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x4000002A, MSRTYPE_RDWR, MSR2(0x00000000, 0x000FFFFF), "GLIU1_P2D_R1", "GLIU0 P2D Range Descriptor 1", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PMAX", "Physical Memory Address Max.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMIN", "Physical Memory Address Min.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x4000002B, MSRTYPE_RDWR, MSR2(0x00000000, 0x000FFFFF), "GLIU0_P2D_R2", "GLIU0 P2D Range Descriptor 2", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PMAX", "Physical Memory Address Max.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMIN", "Physical Memory Address Min.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x4000002C, MSRTYPE_RDWR, MSR2(0x00000000, 0x000FFFFF), "GLIU0_P2D_R3", "GLIU0 P2D Range Descriptor 3", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "PMAX", "Physical Memory Address Max.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "PMIN", "Physical Memory Address Min.", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x4000002D, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU1_P2D_SC0", "GLIU1 P2D Swiss Cheese Descriptor 0", {
		{ 63, 3, "PDID1", "Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 12, RESERVED },
		{ 47, 16, "WEN", "Enable hits to the base for the ith 16K page for writes", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 16, "REN", "Enable hits to the base for the ith 16K page for ", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 15, 2, RESERVED },
		{ 13, 14, "PSCBASE", "Physical Memory Address Base for hit", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x400000E0, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_IOD_BM0", "GLIU1 IOD Base Mask Descriptor 0", {
		{ 63, 3, "IDID", "IO Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "IBASE", "Physical IO Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "IMASK", "Physical IO Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x400000E1, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_IOD_BM1", "GLIU1 IOD Base Mask Descriptor 1", {
		{ 63, 3, "IDID", "IO Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "IBASE", "Physical IO Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "IMASK", "Physical IO Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x400000E2, MSRTYPE_RDWR, MSR2(0x000000FF, 0xFFF00000), "GLIU1_IOD_BM2", "GLIU1 IOD Base Mask Descriptor 2", {
		{ 63, 3, "IDID", "IO Descriptor Destination ID", PRESENT_BIN, {
			{ MSR1(0), "Port 0 = GLIU0:GLIU GLIU1:GLIU" },
			{ MSR1(1), "Port 1 = GLIU0:GLMC GLIU1:Interface to GLIU0" },
			{ MSR1(2), "Port 2 = GLIU0:Interface to GLIU1 GLIU1:Not Used" },
			{ MSR1(3), "Port 3 = GLIU0:CPU Core GLIU1:GLCP" },
			{ MSR1(4), "Port 4 = GLIU0:DC GLIU1:GLPCI" },
			{ MSR1(5), "Port 5 = GLIU0:GP GLIU1:GIO" },
			{ MSR1(6), "Port 6 = GLIU0:VP GLIU1:Not Used" },
			{ MSR1(7), "Port 7 = GLIU0:Not Used GLIU1:Not Used" },
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 20, RESERVED },
		{ 39, 20, "IBASE", "Physical IO Address Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 19, 20, "IMASK", "Physical IO Address Mask", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},
	{ 0x400000E3, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU1_IOD_SC0", "GLIU1 IOD Swiss Cheese Descriptor 0", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x400000E4, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU1_IOD_SC1", "GLIU1 IOD Swiss Cheese Descriptor 1", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x400000E5, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU1_IOD_SC2", "GLIU1 IOD Swiss Cheese Descriptor 2", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x400000E6, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU1_IOD_SC3", "GLIU1 IOD Swiss Cheese Descriptor 3", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x400000E7, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU1_IOD_SC4", "GLIU1 IOD Swiss Cheese Descriptor 4", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x400000E8, MSRTYPE_RDWR, MSR2(0x00000000, 0x00000000), "GLIU1_IOD_SC5", "GLIU1 IOD Swiss Cheese Descriptor 5", {
		{ 63, 3, "IDID1", "Descriptor Destination ID 1", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 36, 1, "PCMP_BIZ", "Compare Bizarro Flag", PRESENT_BIN, {
			{ MSR1(0), "Only act if Bizarro Flag = 0 (Memory or I/O)" },
			{ MSR1(1), "Only act if Bizarro Flag = 1 (PCI, Shutdown or Halt)" },
			{ BITVAL_EOT }
		}},
		{ 59, 28, RESERVED },
		{ 31, 8, "EN", "Enable for hits to IDID1 or else SUBP", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 2, RESERVED },
		{ 21, 1, "WEN", "Descriptor hits  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 20, 1, "WEN", "Descriptor hit  IDID1 on write request Types else SUBP", PRESENT_BIN, {
			{ BITVAL_EOT }
		}},
		{ 19, 17, "IBASE", "IO Memory Base", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 2, 3, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x4c00000f, MSRTYPE_RDWR, MSR2(0, 0), "GLCP_DELAY_CONTROLS", "GLCP I/O Delay Controls", {
		{ 63, 1, "EN", "Delay Settings Enable", PRESENT_DEC, {
			{ MSR1(0), "Use default values" },
			{ MSR1(1), "Use value in bits [62:0]" },
			{ BITVAL_EOT }
		}},
		{ 62, 2, RESERVED },
		{ 60, 5, "GIO", "Delay Geode Companion Device", PRESENT_DEC, NOBITS },
		{ 55, 5, "PCI_IN", "Delay PCI Inputs", PRESENT_DEC, NOBITS },
		{ 50, 5, "PCI_OUT", "Delay PCI Outputs", PRESENT_DEC, NOBITS },
		{ 45, 5, RESERVED},
		{ 40, 5, "DOTCLK", "Delay Dot Clock", PRESENT_DEC, NOBITS },
		{ 35, 5, "DRGB", "Delay Digital RGBs", PRESENT_DEC, NOBITS },
		{ 30, 5, "SDCLK_IN", "Delay SDRAM Clock Input", PRESENT_DEC, NOBITS },
		{ 25, 5, "SDCLK_OUT", "Delay SDRAM Clock Output", PRESENT_DEC, NOBITS },
		{ 20, 5, "MEM_CTL", "Delay Memory Controls", PRESENT_DEC, NOBITS },
		{ 15, 9, RESERVED},
		{ 6, 1, "MEM_ODDOUT", "Delay Odd Memory Data Output Bits", PRESENT_DEC, {
			{ MSR1(0), "No Delay" },
			{ MSR1(1), "Delay" },
			{ BITVAL_EOT }
		}},
		{ 5, 2, RESERVED },
		{ 3, 2, "DQS_CLK_IN", "Delay DQS Before Clocking Input", PRESENT_DEC, NOBITS },
		{ 1, 2, "DQS_CLK_OUT", "Delay DQS Before Clocking Output", PRESENT_DEC, NOBITS },
		{ BITS_EOT }
	}},
	{ 0x4c000014, MSRTYPE_RDWR, MSR2(0, 0), "GLCP_SYS_RSTPLL", "GLCP System Reset and PLL Control", {
		{ 63, 19, RESERVED },
		{ 44, 4, "MDIV", "GLIU1 Divisor", PRESENT_BIN, {
			{ MSR1(0), "Divide by 2" },
			{ MSR1(1), "Divide by 3" },
			{ MSR1(2), "Divide by 4" },
			{ MSR1(3), "Divide by 5" },
			{ MSR1(4), "Divide by 6" },
			{ MSR1(5), "Divide by 7" },
			{ MSR1(6), "Divide by 8" },
			{ MSR1(7), "Divide by 9" },
			{ MSR1(8), "Divide by 10" },
			{ MSR1(9), "Divide by 11" },
			{ MSR1(10), "Divide by 12" },
			{ MSR1(11), "Divide by 13" },
			{ MSR1(12), "Divide by 14" },
			{ MSR1(13), "Divide by 15" },
			{ MSR1(14), "Divide by 16" },
			{ MSR1(15), "Divide by 17" },
			{ BITVAL_EOT }
		}},
		{ 40, 3, "VDIV", "CPU Core Divisor", PRESENT_BIN, {
			{ MSR1(0), "Divide by 2" },
			{ MSR1(1), "Divide by 3" },
			{ MSR1(2), "Divide by 4" },
			{ MSR1(3), "Divide by 5" },
			{ MSR1(4), "Divide by 6" },
			{ MSR1(5), "Divide by 7" },
			{ MSR1(6), "Divide by 8" },
			{ MSR1(7), "Divide by 9" },
			{ BITVAL_EOT }
		}},
		{ 37, 6, "FBDIV", "Feedback Devisor", PRESENT_DEC, NOBITS },
		{ 31, 6, "SWFLAGS", "Software Flags", PRESENT_BIN, NOBITS },
		{ 25, 1, "LOCK", "PLL Lock", PRESENT_DEC, {
			{ MSR1(1), "PLL locked" },
			{ MSR1(0), "PLL is not locked" },
			{ BITVAL_EOT }
		}},
		{ 24, 1, "LOCKWAIT", "Lock Wait", PRESENT_DEC, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Enable" },
			{ BITVAL_EOT }
		}},
		{ 23, 8, "HOLD_COUNT", "Hold Count, divided by 16", PRESENT_DEC, NOBITS },
		{ 15, 1, "BYPASS", "PLL Bypass", PRESENT_DEC, {
			{ MSR1(0), "Use PLL as Clocksource" },
			{ MSR1(1), "Use DOTREF as Clocksource" },
			{ BITVAL_EOT }
		}},
		{ 14, 1, "PD", "Power Down", PRESENT_DEC, {
			{ MSR1(0), "PLL active" },
			{ MSR1(1), "PLL in power down mode" },
			{ BITVAL_EOT }
		}},
			{ 13, 1, "RESETPLL", "PLL Reset", PRESENT_DEC, NOBITS },
		{ 12, 2, RESERVED },
		{ 10, 1, "DDRMODE", "DDR Mode", PRESENT_DEC, {
			{ MSR1(0), "DDR communication enabled" },
			{ MSR1(1), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "VA_SEMI_SYNC_MODE", "Synchronous CPU Core and GLIU1", PRESENT_DEC, {
			{ MSR1(1), "CPU does not use GLIU1 FIFO" },
			{ MSR1(0), "The GLIU1 FIFO is used by the CPU" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "PCI_SEMI_SYNC_MODE", "Synchronous CPU Core and GLIU1", PRESENT_DEC, {
			{ MSR1(1), "PCI does not use mb_func_clk and pci_func_clk falling edges" },
			{ MSR1(0), "Falling edges on mb_func_clk and pci_func_clk are used by PCI" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "DSTALL", "Debug Stall", PRESENT_DEC, NOBITS },
		{ 6, 3, "BOOTSTRAP_STAT", "Bootstrap Status", PRESENT_BIN, NOBITS },
		{ 3, 1, "DOTPOSTDIV3", "DOTPLL Post-Divide by 3", PRESENT_DEC, NOBITS },
		{ 2, 1, "DOTPREMULT2", "DOTPLL Pre-Multiply by 2", PRESENT_DEC, NOBITS },
		{ 1, 1, "DOTPREDIV2", "DOTPLL Pre-Divide by 2", PRESENT_DEC, NOBITS },
		{ 0, 1, "CHIP_RESET", "Chip Reset", PRESENT_DEC, NOBITS },
		{ BITS_EOT }
	}},
	{ MSR_EOT }
};
