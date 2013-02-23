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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "msrtool.h"

int geodelx_probe(const struct targetdef *target, const struct cpuid_t *id) {
	return 5 == id->family && 10 == id->model;
}

const struct msrdef geodelx_msrs[] = {
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
			{ MSR1(8), "1 GB" },
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
			{ MSR1(5), "32 KB" },
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
			{ MSR1(8), "1 GB" },
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
			{ MSR1(5), "32 KB" },
			{ MSR1(6), "Reserved" },
			{ MSR1(7), "DIMM0 Not Installed" },
			{ BITVAL_EOT }
		}},
		{ 31, 2, RESERVED },
		{ 29, 2, "MSR_BA", "Mode Register Set Bank Address", PRESENT_BIN, {
			{ MSR1(0), "Program the DIMM Mode Register" },
			{ MSR1(1), "Program the DIMM Extended Mode Register" },
			{ MSR1(2), "Reserved" },
			{ MSR1(3), "Reserved" },
			{ BITVAL_EOT }
		}},
		{ 27, 1, "RST_DLL", "Mode Register Reset DLL", PRESENT_BIN, {
			{ MSR1(0), "Do not reset DLL" },
			{ MSR1(1), "Reset DLL" },
			{ BITVAL_EOT }
		}},
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
		{ 7, 4, "REF_STAG", "Refresh Staggering", PRESENT_DEC, NOBITS },
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
		{ 47, 6, RESERVED },
		{ 41, 1, "TRUNC_DIS", "Burst Truncate Disable", PRESENT_BIN, {
			{ MSR1(0), "Bursts Enabled" },
			{ MSR1(1), "Bursts Disabled" },
			{ BITVAL_EOT }
		}},
		{ 40, 1, "REORDER_DIS", "Reorder Disable", PRESENT_BIN, {
			{ MSR1(0), "Reordering Enabled" },
			{ MSR1(1), "Reordering Disabled" },
			{ BITVAL_EOT }
		}},
		{ 39, 6, RESERVED },
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
			{ MSR1(2), "2" },
			{ MSR1(3), "3" },
			{ MSR1(4), "4" },
			{ MSR1(5), "1.5" },
			{ MSR1(6), "2.5" },
			{ MSR1(7), "3.5" },
			{ BITVAL_EOT }
		}},
		{ 27, 4, "ACT2ACTREF", "ACT to ACT/REF Period. tRC", PRESENT_DEC, NOBITS },
		{ 23, 4, "ACT2PRE", "ACT to PRE Period. tRAS", PRESENT_DEC, NOBITS },
		{ 19, 1, RESERVED },
		{ 18, 3, "PRE2ACT", "PRE to ACT Period. tRP", PRESENT_DEC, NOBITS },
		{ 15, 1, RESERVED },
		{ 14, 3, "ACT2CMD", "Delay Time from ACT to Read/Write. tRCD", PRESENT_DEC, NOBITS },
		{ 11, 4, "ACT2ACT", "ACT(0) to ACT(1) Period. tRRD", PRESENT_DEC, NOBITS },
		{ 7, 2, "DPLWR", "Data-in to PRE Period. tDPLW", PRESENT_DEC, {
			{ MSR1(0), "Invalid value" },
			{ MSR1(1), "1" },
			{ MSR1(2), "2" },
			{ MSR1(3), "3" },
			{ BITVAL_EOT }
		}},
		{ 5, 2, "DPLRD", "Data-in to PRE Period. tDPLR", PRESENT_DEC, {
			{ MSR1(0), "Invalid value" },
			{ MSR1(1), "1" },
			{ MSR1(2), "2" },
			{ MSR1(3), "3" },
			{ BITVAL_EOT }
		}},
		{ 3, 4, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x2000001a, MSRTYPE_RDWR, MSR2(0, 0x11080001), "MC_CF1017_DATA", "Feature Enables", {
		{ 63, 34, RESERVED },
		{ 29, 2, "WR_TO_RD", "Write to Read Delay. tWTR", PRESENT_DEC, NOBITS },
		{ 27, 1, RESERVED },
		{ 26, 3, "RD_TMG_CTL", "Read Timing Control", PRESENT_DEC, NOBITS },
		{ 23, 3, RESERVED },
		{ 20, 5, "REF2ACT", "Refresh to Activate Delay. tRFC", PRESENT_DEC, NOBITS },
		{ 15, 8, "PM1_UP_DLY", "PMode1 Up Delay", PRESENT_DEC, NOBITS },
		{ 7, 5, RESERVED },
		{ 2, 3, "WR2DAT", "Write Command to Data Latency", PRESENT_DEC, {
			{ MSR1(0), "No delay" },
			{ MSR1(1), "1-clock delay for unbuffered DIMMs" },
			{ MSR1(2), "2-clock delay" },
			{ MSR1(3), "Invalid value" },
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
		{ 35, 1, "STOP_CNT1", "Stop Counter 1", PRESENT_DEC, NOBITS },
		{ 34, 1, "RST_CNT1", "Reset Counter 1", PRESENT_DEC, NOBITS },
		{ 33, 1, "STOP_CNT0", "Stop Counter 0", PRESENT_DEC, NOBITS },
		{ 32, 1, "RST_CNT0", "Reset Counter 0", PRESENT_DEC, NOBITS },
		{ 31, 32, RESERVED },
		{ BITS_EOT }
	}},
	{ 0x2000001d, MSRTYPE_RDWR, MSR2(0, 0x1300), "MC_CFCLK_DBUG", "Clocking and Debug", {
		{ 63, 29, RESERVED },
		{ 34, 1, "B2B_DIS", "Back-to-Back Command Disable", PRESENT_BIN, {
			{ MSR1(0), "Allow back-to-back commands" },
			{ MSR1(1), "Disable back-to-back commands" },
			{ BITVAL_EOT }
		}},
		{ 33, 1, "MTEST_RBEX_EN", "MTEST RBEX Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Enable" },
			{ BITVAL_EOT }
		}},
		{ 32, 1, "MTEST_EN", "MTEST Enable", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Enable" },
			{ BITVAL_EOT }
		}},
		{ 31, 15, RESERVED },
		{ 16, 1, "FORCE_PRE", "Force Precharge-all", PRESENT_BIN, {
			{ MSR1(0), "Disable" },
			{ MSR1(1), "Enable" },
			{ BITVAL_EOT }
		}},
		{ 15, 3, RESERVED },
		{ 12, 1, "TRISTATE_DIS", "TRI-STATE Disable", PRESENT_BIN, {
			{ MSR1(0), "Tri-stating enabled" },
			{ MSR1(1), "Tri-stating disabled" },
			{ BITVAL_EOT }
		}},
		{ 11, 2, RESERVED },
		{ 9, 1, "MASK_CKE1", "CKE1 Mask", PRESENT_BIN, {
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
	{ 0x4c000014, MSRTYPE_RDWR, MSR2(0, 0), "GLCP_SYS_RSTPLL", "GLCP System Reset and PLL Control", {
		{ 63, 20, RESERVED },
		{ 43, 5, "GLIUMULT", "GLIU Multiplier", PRESENT_DEC, NOBITS },
		{ 38, 1, "GLIUDIV", "GLIU Divide", PRESENT_DEC, {
			{ MSR1(0), "Do not predivide input" },
			{ MSR1(1), "Divide by 2" },
			{ BITVAL_EOT }
		}},
		{ 37, 5, "COREMULT", "CPU Core Multiplier", PRESENT_DEC, NOBITS },
		{ 32, 1, "COREDIV", "CPU Core Divide", PRESENT_DEC, {
			{ MSR1(0), "Do not predivide input" },
			{ MSR1(1), "Divide by 2" },
			{ BITVAL_EOT }
		}},
		{ 31, 6, "SWFLAGS", "Flags", PRESENT_BIN, NOBITS },
		{ 25, 1, "GLIULOCK", "GLIU PLL Lock", PRESENT_DEC, {
			{ MSR1(1), "PLL locked" },
			{ MSR1(0), "PLL is not locked" },
			{ BITVAL_EOT }
		}},
		{ 24, 1, "CORELOCK", "CPU Core PLL Lock", PRESENT_DEC, {
			{ MSR1(1), "PLL locked" },
			{ MSR1(0), "PLL is not locked" },
			{ BITVAL_EOT }
		}},
		{ 23, 8, "HOLD_COUNT", "Hold Count, divided by 16", PRESENT_DEC, NOBITS },
		{ 15, 1, RESERVED },
		{ 14, 1, "GLIUPD", "GLIU PLL Power Down mode", PRESENT_DEC, NOBITS },
		{ 13, 1, "COREPD", "CPU Core PLL Power Down mode", PRESENT_DEC, NOBITS },
		{ 12, 1, "GLIUBYPASS", "GLIU PLL Bypass", PRESENT_DEC, {
			{ MSR1(1), "DOTREF input directly drives the GLIU clock spines" },
			{ MSR1(0), "DOTPLL drives the GLIU clock" },
			{ BITVAL_EOT }
		}},
		{ 11, 1, "COREBYPASS", "CPU Core PLL Bypass", PRESENT_DEC, {
			{ MSR1(1), "DOTREF input directly drives the CPU Core clock" },
			{ MSR1(0), "DOTPLL drives the CPU Core clock" },
			{ BITVAL_EOT }
		}},
		{ 10, 1, "LPFEN", "Loop Filter", PRESENT_DEC, {
			{ MSR1(1), "Enabled" },
			{ MSR1(0), "Disabled" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "VA_SEMI_SYNC_MODE", "CPU-GLIU Sync Mode", PRESENT_DEC, {
			{ MSR1(1), "CPU does not use GLIU FIFO" },
			{ MSR1(0), "The GLIU FIFO is used by the CPU" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "PCI_SEMI_SYNC_MODE", "PCI-GLIU Sync Mode", PRESENT_DEC, {
			{ MSR1(1), "PCI does not use mb_func_clk and pci_func_clk falling edges" },
			{ MSR1(0), "Falling edges on mb_func_clk and pci_func_clk are used by PCI" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "BOOTSTRAP_PW1", "PW1 bootstrap", PRESENT_DEC, {
			{ MSR1(1), "66MHz PCI clock" },
			{ MSR1(0), "33MHz PCI clock" },
			{ BITVAL_EOT }
		}},
		{ 6, 1, "BOOTSTRAP_IRQ13", "IRQ13 bootstrap", PRESENT_DEC, {
			{ MSR1(1), "Stall-on-reset debug feature enabled" },
			{ MSR1(0), "No stall" },
			{ BITVAL_EOT }
		}},
		{ 5, 5, "BOOTSTRAPS", "CPU/GLIU frequency select", PRESENT_BIN, NOBITS },
		{ 0, 1, "CHIP_RESET", "Chip Reset", PRESENT_DEC, NOBITS },
		{ BITS_EOT }
	}},
	{ MSR_EOT }
};
