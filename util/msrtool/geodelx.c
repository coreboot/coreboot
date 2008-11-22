/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2008 Peter Stuge <peter@stuge.se>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "msrtool.h"

int geodelx_probe(const struct targetdef *target) {
	struct cpuid_t *id = cpuid();
	return 10 == id->family && 5 == id->model;
}

const struct msrdef geodelx_msrs[] = {
	{ 0x4c00000f, MSRTYPE_RDWR, MSR2(0, 0), "GLCP_DELAY_CONTROLS", "GLCP I/O Delay Controls", {
		{ 63, 1, "EN", "Enable", PRESENT_DEC, {
			{ MSR1(0), "Use default values" },
			{ MSR1(1), "Use value in bits [62:0]" },
			{ BITVAL_EOT }
		}},
		{ 62, 1, "B_DQ", "Buffer Control for DQ DQS DQM TLA drive", PRESENT_DEC, {
			{ MSR1(1), "Half power" },
			{ MSR1(0), "Quarter power" },
			{ BITVAL_EOT }
		}},
		{ 61, 1, "B_CMD", "Buffer Control for RAS CAS CKE CS WE drive", PRESENT_DEC, {
			{ MSR1(1), "Half power" },
			{ MSR1(0), "Quarter power" },
			{ BITVAL_EOT }
		}},
		{ 60, 1, "B_MA", "Buffer Control for MA BA drive", PRESENT_DEC, {
			{ MSR1(0), "Half power" },
			{ MSR1(1), "Full power" },
			{ BITVAL_EOT }
		}},
		{ 59, 1, "SDCLK_SET", "SDCLK Setup", PRESENT_DEC, {
			{ MSR1(0), "Full SDCLK setup" },
			{ MSR1(1), "Half SDCLK setup for control signals" },
			{ BITVAL_EOT }
		}},
		{ 58, 3, "DDR_RLE", "DDR read latch enable position", PRESENT_DEC, NOBITS },
		{ 55, 1, "SDCLK_DIS", "SDCLK disable [1,3,5]", PRESENT_DEC, {
			{ MSR1(0), "All SDCLK output" },
			{ MSR1(1), "SDCLK[0,2,4] output only" },
			{ BITVAL_EOT }
		}},
		{ 54, 3, "TLA1_OA", "TLA hint pin output adjust", PRESENT_DEC, NOBITS },
		{ 51, 2, "D_TLA1", "Output delay for TLA1", PRESENT_DEC, NOBITS },
		{ 49, 2, "D_TLA0", "Output delay for TLA0", PRESENT_DEC, NOBITS },
		{ 47, 2, "D_DQ_E", "Output delay for DQ DQM - even byte lanes", PRESENT_DEC, NOBITS },
		{ 45, 2, "D_DQ_O", "Output delay for DQ DQM - odd byte lanes", PRESENT_DEC, NOBITS },
		{ 43, 2, RESERVED},
		{ 41, 2, "D_SDCLK", "Output delay for SDCLK", PRESENT_DEC, NOBITS },
		{ 39, 2, "D_CMD_O", "Output delay for CKE CS RAS CAS WE - odd bits", PRESENT_DEC, NOBITS },
		{ 37, 2, "D_CMD_E", "Output delay for CKE CS RAS CAS WE - even bits", PRESENT_DEC, NOBITS },
		{ 35, 2, "D_MA_O", "Output delay for BA MA - odd bits", PRESENT_DEC, NOBITS },
		{ 33, 2, "D_MA_E", "Output delay for BA MA - even bits", PRESENT_DEC, NOBITS },
		{ 31, 2, "D_PCI_O", "Output delay for pci_ad IRQ13 SUSPA# INTA# - odd bits", PRESENT_DEC, NOBITS },
		{ 29, 2, "D_PCI_E", "Output delay for pci_ad IRQ13 SUSPA# INTA# - even bits", PRESENT_DEC, NOBITS },
		{ 27, 2, "D_DOTCLK", "Output delay for DOTCLK", PRESENT_DEC, NOBITS },
		{ 25, 2, "D_DRGB_O", "Output delay for DRGB[31:0] - odd bits", PRESENT_DEC, NOBITS },
		{ 23, 2, "D_DRGB_E", "Output delay for DRGB[31:0] HSYNC VSYNC DISPEN VDDEN LDE_MOD - even bits", PRESENT_DEC, NOBITS },
		{ 21, 2, "D_PCI_IN", "Input delay for pci_ad CBE# PAR STOP# FRAME# IRDY# TRDY# DEVSEL# REQ# GNT# CIS", PRESENT_DEC, NOBITS },
		{ 19, 2, "D_TDBGI", "Input delay for TDBGI", PRESENT_DEC, NOBITS },
		{ 17, 2, "D_VIP", "Input delay for VID[15:0] VIP_HSYNC VIP_VSYNC", PRESENT_DEC, NOBITS },
		{ 15, 2, "D_VIPCLK", "Input delay for VIPCLK", PRESENT_DEC, NOBITS },
		{ 13, 1, "H_SDCLK", "Half SDCLK hold select (for cmd addr)", PRESENT_DEC, {
			{ MSR1(1), "Half SDCLK setup for MA and BA" },
			{ MSR1(0), "Full SDCLK setup" },
			{ BITVAL_EOT }
		}},
		{ 12, 2, "PLL_FD_DEL", "PLL Feedback Delay", PRESENT_BIN, {
			{ MSR1(0), "No feedback delay" },
			{ MSR1(1), "~350 ps" },
			{ MSR1(2), "~700 ps" },
			{ MSR1(3), "~1100 ps (Max feedback delay)" },
			{ BITVAL_EOT }
		}},
		{ 10, 5, RESERVED },
		{ 5, 1, "DLL_OV", "DLL Override (to DLL)", PRESENT_DEC, NOBITS },
		{ 4, 5, "DLL_OVS/RSDA", "DLL Override Setting or Read Strobe Delay Adjust", PRESENT_DEC, NOBITS },
		{ BITS_EOT }
	}},
/*
	{ 0, MSRTYPE_RDONLY, MSR2(0, 0), "TEMPLATE", "Template MSR", {
		{ 63, 64, RESERVED },
		{ BITS_EOT }
	}},
*/
	{ MSR_EOT }
};
