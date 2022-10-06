/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "inteltool.h"
#include "pcr.h"

#ifdef __NetBSD__
#include <machine/sysarch.h>
#endif

#define MAX_PCR_PORTS 8 /* how often may `--pcr` be specified */

enum long_only_opts {
	LONG_OPT_PCR = 0x100,
	LONG_OPT_RANGE = 0x101,
};

/*
 * http://pci-ids.ucw.cz/read/PC/8086
 * http://en.wikipedia.org/wiki/Intel_Tick-Tock
 * http://en.wikipedia.org/wiki/List_of_Intel_chipsets
 * http://en.wikipedia.org/wiki/Intel_Xeon_chipsets
 */
static const struct {
	uint16_t vendor_id, device_id;
	char *name;
} supported_chips_list[] = {
	/* Host bridges/DRAM controllers (Northbridges) */
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443LX, "443LX" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443BX, "443BX" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443BX_NO_AGP, "443BX without AGP" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810, "810" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810_DC, "810-DC100" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810E_DC, "810E DC-133" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82830M, "830M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82845, "845" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82865, "865" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82915, "915G/P/GV/GL/PL/910GL" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945P, "945P" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945GM, "945GM" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945GSE, "945GSE" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82946, "946GZ/PL" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82965PM, "965PM" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q965, "Q963/82Q965" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82975X, "975X" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82G33, "P35/G33/G31/P31" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q33, "Q33" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q35, "Q35" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82X38, "X38/X48" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_32X0, "3200/3210" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82XX4X,
	  "GL40/GS40/GM45/GS45/PM45" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q45, "Q45/Q43" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82G45, "G45/G43/P45/P43" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82G41, "G41" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82B43, "B43 (Base)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82B43_2, "B43 (Soft)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82X58, "X58" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I5000P,
	  "Intel i5000P Memory Controller Hub" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I5000X,
	  "Intel i5000X Memory Controller Hub" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I5000Z,
	  "Intel i5000Z Memory Controller Hub" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I5000V,
	  "Intel i5000V Memory Controller Hub" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SCH_POULSBO, "SCH Poulsbo" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ATOM_DXXX,
	  "Atom D400/500 Series" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ATOM_NXXX, "Atom N400 Series" },
	/* Host bridges /DRAM controllers integrated in CPUs */
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_0TH_GEN,
	  "0th generation (Nehalem family) Core Processor" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_1ST_GEN,
	  "1st generation (Westmere family) Core Processor" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D,
	  "2nd generation (Sandy Bridge family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M,
	  "2nd generation (Sandy Bridge family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3,
	  "2nd generation (Sandy Bridge family) Core Processor (Xeon E3)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D,
	  "3rd generation (Ivy Bridge family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M,
	  "3rd generation (Ivy Bridge family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3,
	  "3rd generation (Ivy Bridge family) Core Processor (Xeon E3 v2)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c,
	  "3rd generation (Ivy Bridge family) Core Processor" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_D,
	  "4th generation (Haswell family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_M,
	  "4th generation (Haswell family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_E3,
	  "4th generation (Haswell family) Core Processor (Xeon E3 v3)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_U,
	  "4th generation (Haswell family) Core Processor ULT" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_U,
	  "5th generation (Broadwell family) Core Processor ULT" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_M,
	  "6th generation (Skylake-H family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_WST,
	  "6th generation (Skylake-S/H family) Core Processor (Workstation)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D,
	  "6th generation (Skylake-S family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D2,
	  "6th generation (Skylake-S family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_E,
	  "6th generation (Skylake family) Core Processor Xeon E (Server)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_U,
	  "6th generation (Skylake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_Y,
	  "6th generation (Skylake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_BAYTRAIL, "Bay Trail" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U,
	  "7th generation (Kaby Lake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_Y,
	  "7th generation (Kaby Lake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U_Q,
	  "7th generation (Kaby Lake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_E3,
	  "7th generation (Kaby Lake family) Core Processor Xeon E3-1200" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_1,
	  "8th generation (Coffee Lake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_2,
	  "8th generation (Whiskey Lake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_10TH_GEN_U,
	  "10th generation (Icelake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_CML_U1,
	  "10th generation (Comet Lake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_CML_U2,
	  "10th generation (Comet Lake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_CML_U3,
	  "10th generation (Comet Lake family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HEWITTLAKE,
	  "Xeon E7 v4/Xeon E5 v4/Xeon E3 v4/Xeon D (Hewitt Lake)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SAPPHIRERAPIDS_SP,
	  "Xeon Scalable Processor 4th generation (Sapphire Rapids SP)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_TGL_ID_U_2,
	 "11th generation (Tiger Lake UP3 family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_TGL_ID_U_4,
	 "11th generation (Tiger Lake UP3 family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_TGL_ID_Y_2,
	 "11th generation (Tiger Lake UP4 family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_TGL_ID_Y_4,
	 "11th generation (Tiger Lake UP4 family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_TGL_ID_H_4,
	 "11th generation (Tiger Lake H family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_TGL_ID_H_6,
	 "11th generation (Tiger Lake H family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_TGL_ID_H_8,
	 "11th generation (Tiger Lake H family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_ADL_ID_S_8_8,
	 "12th generation (Alder Lake S family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_ADL_ID_S_8_4,
	 "12th generation (Alder Lake S family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_ADL_ID_S_6_4,
	 "12th generation (Alder Lake S family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_ADL_ID_S_8_0,
	 "12th generation (Alder Lake S family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_ADL_ID_S_6_0,
	 "12th generation (Alder Lake S family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_ADL_ID_P_6_8,
	  "12th generation (Alder Lake P family) Core Processor"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_ADL_ID_P_4_8,
	  "12th generation (Alder Lake P family) Core Processor"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_ADL_ID_P_2_8,
	  "12th generation (Alder Lake P family) Core Processor"},

	/* Southbridges (LPC controllers) */
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82371XX, "371AB/EB/MB" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH10, "ICH10" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH10D, "ICH10D" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH10DO, "ICH10DO" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH10R, "ICH10R" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9DH, "ICH9DH" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9DO, "ICH9DO" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9R, "ICH9R" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9, "ICH9" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9M, "ICH9M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9ME, "ICH9M-E" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH8M, "ICH8-M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH8ME, "ICH8M-E" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH8, "ICH8" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_NM10, "NM10" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7MDH, "ICH7-M DH" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7M, "ICH7-M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7, "ICH7" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7DH, "ICH7DH" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH6, "ICH6" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH5, "ICH5" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH4M, "ICH4-M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH4, "ICH4" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH2, "ICH2" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH0, "ICH0" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH, "ICH" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I63XX, "631xESB/632xESB/3100" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SCH_POULSBO_LPC, "SCH Poulsbo" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3400_DESKTOP, "3400 Desktop" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3400_MOBILE, "3400 Mobile" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_P55, "P55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PM55, "PM55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H55, "H55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM57, "QM57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H57, "H57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM55, "HM55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q57, "Q57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM57, "HM57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF,
	  "3400 Mobile SFF" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B55_A, "B55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QS57, "QS57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3400, "3400" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3420, "3420" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3450, "3450" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B55_B, "B55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z68, "Z68" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_P67, "P67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UM67, "UM67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM65, "HM65" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H67, "H67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM67, "HM67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q65, "Q65" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QS67, "QS67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q67, "Q67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM67, "QM67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B65, "B65" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C202, "C202" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C204, "C204" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C206, "C206" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H61, "H61" },
	{ PCI_VENDOR_ID_INTEL, 0x1d40, "X79" },
	{ PCI_VENDOR_ID_INTEL, 0x1d41, "X79" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z77, "Z77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z75, "Z75" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q77, "Q77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q75, "Q75" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B75, "B75" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H77, "H77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C216, "C216" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM77, "QM77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QS77, "QS77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM77, "HM77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UM77, "UM77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM76, "HM76" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM75, "HM75" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM70, "HM70" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_NM70, "NM70" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICELAKE_LP_U,
	  "Icelake U LPC" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL,
	  "Lynx Point Low Power Full Featured Engineering Sample" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM,
	  "Lynx Point Low Power Premium SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE,
	  "Lynx Point Low Power Base SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP_PREM,
	  "Wildcat Point Low Power Premium SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP,
	  "Wildcat Point Low Power Base SKU" },
	{ PCI_VENDOR_ID_INTEL, 0x2310, "DH89xxCC" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_BAYTRAIL_LPC, "Bay Trail" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_PRE,
	  "Sunrise Point Desktop Engineering Sample" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_PRE,
	  "Sunrise Point-LP Engineering Sample" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_SKL,
	  "Sunrise Point-LP U Base/Skylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_SKL,
	  "Sunrise Point-LP Y Premium/Skylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_SKL,
	  "Sunrise Point-LP U Premium/Skylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_KBL,
	  "Sunrise Point-LP U Base/Kabylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_KBL,
	  "Sunrise Point-LP Y Premium/Kabylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_KBL,
	  "Sunrise Point-LP U Premium/Kabylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_BASE,
	  "Sunrise Point-LP U iHDCP 2.2 Base/Kabylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_PREM,
	  "Sunrise Point-LP U iHDCP 2.2 Premium/Kabylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_IHDCP_PREM,
	  "Sunrise Point-LP Y iHDCP 2.2 Premium/Kabylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CANNONPOINT_LP_U_PREM,
	  "Cannon Point-LP U Premium/CoffeeLake/Whiskeylake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_PREM,
	  "Comet Point-LP U Premium/Cometlake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_BASE,
	  "Comet Point-LP U Base/Cometlake" },
	{ PCI_VENDOR_ID_INTEL,  PCI_DEVICE_ID_INTEL_TIGERPOINT_U_SUPER,
	  "Tiger Point U Engineering Sample" },
	{ PCI_VENDOR_ID_INTEL,  PCI_DEVICE_ID_INTEL_TIGERPOINT_U_PREM,
	  "Tiger Point U Premium/Tigerlake" },
	{ PCI_VENDOR_ID_INTEL,  PCI_DEVICE_ID_INTEL_TIGERPOINT_U_BASE,
	  "Tiger Point U Base/Tigerlake" },
	{ PCI_VENDOR_ID_INTEL,  PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_SUPER,
	  "Tiger Point Y Engineering Sample" },
	{ PCI_VENDOR_ID_INTEL,  PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_PREM,
	  "Tiger Point Y Premium/Tigerlake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H110, "H110" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H170, "H170" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z170, "Z170" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q170, "Q170" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q150, "Q150" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B150, "B150" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C236, "C236" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C232, "C232" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM170, "QM170" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM170, "HM170" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CM236, "CM236" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM175, "HM175" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM175, "QM175" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CM238, "CM238" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C621, "C621" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C621A, "C621A" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C622, "C622" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C624, "C624" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C625, "C625" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C626, "C626" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C627, "C627" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C628, "C628" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C629, "C629" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C624_SUPER, "C624 Super SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C627_SUPER_1, "C627 Super SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C621_SUPER, "C621 Super SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C627_SUPER_2, "C627 Super SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C628_SUPER, "C628 Super SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_EBG, "Emmits Burg" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H270, "H270" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z270, "Z270"	},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q270, "Q270"	},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q250, "Q250"	},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B250, "B250"	},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z370, "Z370" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H310C, "H310C" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_X299, "X299" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H310, "H310" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H370, "H370" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z390, "Z390" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q370, "Q370" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B360, "B360" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C246, "C246" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C242, "C242" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM370, "QM370" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM370, "HM370" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CM246, "CM246" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C8_MOBILE, "C8 Mobile"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C8_DESKTOP, "C8 Desktop"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z87, "Z87"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z85, "Z85"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM86, "HM86"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H87, "H87"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM87, "HM87"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q85, "Q85"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q87, "Q87"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM87, "QM87"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B85, "B85"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C222, "C222"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C224, "C224"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C226, "C226"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H81, "H81"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C9_MOBILE, "C9 Mobile"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C9_DESKTOP, "C9 Desktop"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM97, "HM97"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z97, "Z97"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H97, "H97"},
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_APL_LPC, "Apollo Lake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_DNV_LPC, "Denverton" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_GLK_LPC, "Gemini Lake" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H510, "H510" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H570, "H570" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z590, "Z590" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q570, "Q570" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B560, "B560" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_W580, "W580" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C256, "C256" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C252, "C252" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM570, "HM570" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM580, "QM580" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WM590, "WM590" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H610, "H610" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B660, "B660" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H670, "H670" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q670, "Q670" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z690, "Z690" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_W680, "W680" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_W685, "W685" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WM690, "WM690" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM670, "HM670" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WM790, "WM790" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM770, "HM770" },
	/* Intel GPUs */
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_G35_EXPRESS,
	  "Intel(R) G35 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_G35_EXPRESS_1,
	  "Intel(R) G35 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_965_EXPRESS,
	  "Mobile Intel(R) 965 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_965_EXPRESS_1,
	  "Mobile Intel(R) 965 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_965_EXPRESS_2,
	  "Mobile Intel(R) 965 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_965_EXPRESS_3,
	  "Mobile Intel(R) 965 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_4_SERIES,
	  "Mobile Intel(R) 4 Series Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_4_SERIES_1,
	  "Mobile Intel(R) 4 Series Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_G45,
	  "Intel(R) G45/G43 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_G45_1,
	  "Intel(R) G45/G43 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q45,
	  "Intel(R) Q45/Q43 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q45_1,
	  "Intel(R) Q45/Q43 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_G41,
	  "Intel(R) G41 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_G41_1,
	  "Intel(R) G41 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B43,
	  "Intel(R) B43 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B43_1,
	  "Intel(R) B43 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B43_2,
	  "Intel(R) B43 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B43_3,
	  "Intel(R) B43 Express Chipset Family" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_GRAPHICS,
	  "Intel(R) HD Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_GRAPHICS_1,
	  "Intel(R) HD Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_GRAPHICS_2,
	  "Intel(R) HD Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_2000,
	  "Intel(R) HD 2000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_2000_1,
	  "Intel(R) HD 2000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_3000,
	  "Intel(R) HD 3000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_3000_1,
	  "Intel(R) HD 3000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_3000_2,
	  "Intel(R) HD 3000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_3000_3,
	  "Intel(R) HD 3000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_3000_4,
	  "Intel(R) HD 3000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_3000_5,
	  "Intel(R) HD 3000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_2500,
	  "Intel(R) HD 2500 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_2500_1,
	  "Intel(R) HD 2500 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_2500_2,
	  "Intel(R) HD 2500 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_4000,
	  "Intel(R) HD 4000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_4000_1,
	  "Intel(R) HD 4000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_4000_2,
	  "Intel(R) HD 4000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_4400,
	  "Intel(R) HD 4400 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_4400_1,
	  "Intel(R) HD 4400 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_4600,
	  "Intel(R) HD 4600 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_4600_1,
	  "Intel(R) HD 4600 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_5000,
	  "Intel(R) HD 5000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_5000_1,
	  "Intel(R) HD 5000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_5000_2,
	  "Intel(R) HD 5000 Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_510,
	  "Intel(R) HD Graphics 510" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_515,
	  "Intel(R) HD Graphics 515" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_520,
	  "Intel(R) HD Graphics 520" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_530_1,
	  "Intel(R) HD Graphics 530" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HD_530_2,
	  "Intel(R) HD Graphics 530" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_615_1,
	  "Intel(R) UHD Graphics 615" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_615_2,
	  "Intel(R) UHD Graphics 615" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_617,
	  "Intel(R) UHD Graphics 617" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_620_1,
	  "Intel(R) UHD Graphics 620" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_620_2,
	  "Intel(R) UHD Graphics 620" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_620_3,
	  "Intel(R) UHD Graphics 620" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_630_1,
	  "Intel(R) UHD Graphics 630" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_630_2,
	  "Intel(R) UHD Graphics 630" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_630_3,
	  "Intel(R) UHD Graphics 630" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_630_4,
	  "Intel(R) UHD Graphics 630" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_630_5,
	  "Intel(R) UHD Graphics 630" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_630_6,
	  "Intel(R) UHD Graphics 630" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_630_7,
	  "Intel(R) UHD Graphics 630" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_640,
	  "Intel(R) UHD Graphics 640" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_IRIS_540,
	  "Intel(R) Iris Graphics 540" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_IRIS_550,
	  "Intel(R) Iris Graphics 550" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_IRIS_PRO_580,
	  "Intel(R) Iris Pro Graphics 580" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_IRIS_PLUS_650,
	  "Intel(R) Iris Plus Graphics 650" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_IRIS_PLUS_655,
	  "Intel(R) Iris Plus Graphics 655" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_IRIS_PLUS_G7,
	  "Intel(R) Iris Plus Graphics G7" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_TGL_GT2_UY,
	  "Intel(R) Iris Xe Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_TGL_GT2_Y,
	  "Intel(R) Iris Xe Graphics" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_TGL_GT1,
	  "Intel(R) UHD Graphics for 11th Gen Intel(R) Processors" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_TGL_GT1_2,
	  "Intel(R) UHD Graphics for 11th Gen Intel(R) Processors" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_TGL_GT2_ULT_1,
	  "Intel(R) UHD Graphics for 11th Gen Intel(R) Processors" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_TGL_GT2_ULT_2,
	  "Intel(R) UHD Graphics for 11th Gen Intel(R) Processors" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ADL_S_GT1,
	  "Intel(R) AlderLake-S GT1" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ADL_S_GT1_2,
	  "Intel(R) AlderLake-S GT1" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ADL_S_GT1_3,
	  "Intel(R) AlderLake-S GT1" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ADL_S_GT1_4,
	  "Intel(R) AlderLake-S GT1" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UHD_GRAPHICS,
	  "Intel(R) UHD Graphics" },
};

#ifndef __DARWIN__
static int fd_mem;

void *map_physical(uint64_t phys_addr, size_t len)
{
	void *virt_addr;

	virt_addr = mmap(0, len, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) phys_addr);

	if (virt_addr == MAP_FAILED) {
		printf("Error mapping physical memory 0x%08" PRIx64 "[0x%zx]\n",
			phys_addr, len);
		return NULL;
	}

	return virt_addr;
}

void unmap_physical(void *virt_addr, size_t len)
{
	munmap(virt_addr, len);
}
#endif

static void print_version(void)
{
	printf("inteltool v%s -- ", INTELTOOL_VERSION);
	printf("Copyright (C) 2008 coresystems GmbH\n\n");
	printf(
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, version 2 of the License.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n");
}

static void print_usage(const char *name)
{
	printf("usage: %s [-vh?gGrplmedPMaAsfSRx]\n", name);
	printf("\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n\n"
	     "   -s | --spi:                       dump southbridge spi and bios_cntrl registers\n"
	     "   -f | --gfx:                       dump graphics registers (UNSAFE: may hang system!)\n"
	     "   -R | --ahci:                      dump AHCI registers\n"
	     "   -g | --gpio:                      dump southbridge GPIO registers\n"
	     "   -G | --gpio-diffs:                show GPIO differences from defaults\n"
	     "   -r | --rcba:                      dump southbridge RCBA registers\n"
	     "   -p | --pmbase:                    dump southbridge Power Management registers\n\n"
	     "   -l | --lpc:                       dump southbridge LPC/eSPI Interface registers\n\n"
	     "   -m | --mchbar:                    dump northbridge Memory Controller registers\n"
	     "   -S FILE | --spd=FILE:             create a file storing current timings (implies -m)\n"
	     "   -e | --epbar:                     dump northbridge EPBAR registers\n"
	     "   -d | --dmibar:                    dump northbridge DMIBAR registers\n"
	     "   -P | --pciexpress:                dump northbridge PCIEXBAR registers\n\n"
	     "   -M | --msrs:                      dump CPU MSRs\n"
	     "        --cpu-range <start>[-<end>]: (optional) set CPU cores range for -M (--msrs) option\n"
	     "   -A | --ambs:                      dump AMB registers\n"
	     "   -x | --sgx:                       dump SGX status\n"
	     "   -t | --tme:                       dump TME status\n"
	     "   -a | --all:                       dump all known (safe) registers\n"
	     "        --pcr=PORT_ID:               dump all registers of a PCR port\n"
	     "                                     (may be specified max %d times)\n"
	     "\n", MAX_PCR_PORTS);
	exit(1);
}

static void print_system_info(struct pci_dev *nb, struct pci_dev *sb, struct pci_dev *gfx)
{
	unsigned int id, i;
	char *sbname = "unknown", *nbname = "unknown", *gfxname = "unknown";

	id = cpuid(1);

	/* Determine names */
	for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++) {
		if (nb->device_id == supported_chips_list[i].device_id)
			nbname = supported_chips_list[i].name;
	}
	for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++) {
		if (sb->device_id == supported_chips_list[i].device_id)
			sbname = supported_chips_list[i].name;
	}
	if (gfx) {
		for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++)
			if (gfx->device_id == supported_chips_list[i].device_id)
				gfxname = supported_chips_list[i].name;
	}

	/* Intel has suggested applications to display the family of a CPU as
	 * the sum of the "Family" and the "Extended Family" fields shown
	 * above, and the model as the sum of the "Model" and the 4-bit
	 * left-shifted "Extended Model" fields.
	 * http://download.intel.com/design/processor/applnots/24161832.pdf
	 */
	printf("CPU: ID 0x%x, Processor Type 0x%x, Family 0x%x, Model 0x%x, Stepping 0x%x\n",
			id, (id >> 12) & 0x3, ((id >> 8) & 0xf) + ((id >> 20) & 0xff),
			((id >> 12) & 0xf0) + ((id >> 4) & 0xf), (id & 0xf));

	printf("Northbridge: %04x:%04x (%s)\n",
		nb->vendor_id, nb->device_id, nbname);

	printf("Southbridge: %04x:%04x (%s)\n",
		sb->vendor_id, sb->device_id, sbname);

	if (gfx)
		printf("IGD: %04x:%04x (%s)\n",
		       gfx->vendor_id, gfx->device_id, gfxname);
}

int main(int argc, char *argv[])
{
	struct pci_access *pacc;
	struct pci_dev *sb = NULL, *nb, *gfx = NULL, *ahci = NULL, *dev;
	const char *dump_spd_file = NULL;
	int opt, option_index = 0;

	int dump_gpios = 0, dump_mchbar = 0, dump_rcba = 0;
	int dump_pmbase = 0, dump_epbar = 0, dump_dmibar = 0;
	int dump_pciexbar = 0, dump_coremsrs = 0, dump_ambs = 0;
	int dump_spi = 0, dump_gfx = 0, dump_ahci = 0, dump_sgx = 0, dump_tme = 0;
	int dump_lpc = 0;
	int show_gpio_diffs = 0;
	size_t pcr_count = 0;
	uint8_t dump_pcr[MAX_PCR_PORTS];

	unsigned int cores_range_start = 0, cores_range_end = UINT_MAX;

	static struct option long_options[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"gpios", 0, 0, 'g'},
		{"gpio-diffs", 0, 0, 'G'},
		{"mchbar", 0, 0, 'm'},
		{"rcba", 0, 0, 'r'},
		{"pmbase", 0, 0, 'p'},
		{"lpc", 0, 0, 'l'},
		{"epbar", 0, 0, 'e'},
		{"dmibar", 0, 0, 'd'},
		{"pciexpress", 0, 0, 'P'},
		{"msrs", 0, 0, 'M'},
		{"cpu-range", required_argument, 0, LONG_OPT_RANGE},
		{"ambs", 0, 0, 'A'},
		{"spi", 0, 0, 's'},
		{"spd", 0, 0, 'S'},
		{"all", 0, 0, 'a'},
		{"gfx", 0, 0, 'f'},
		{"ahci", 0, 0, 'R'},
		{"sgx", 0, 0, 'x'},
		{"pcr", required_argument, 0, LONG_OPT_PCR},
		{"tme", 0, 0, 't'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "vh?gGrplmedPMaAsfRS:xt",
                                  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 'S':
			dump_spd_file = optarg;
			dump_mchbar = 1;
			break;
		case 'g':
			dump_gpios = 1;
			break;
		case 'f':
			dump_gfx = 1;
			break;
		case 'R':
			dump_ahci = 1;
			break;
		case 'G':
			show_gpio_diffs = 1;
			break;
		case 'm':
			dump_mchbar = 1;
			break;
		case 'r':
			dump_rcba = 1;
			break;
		case 'p':
			dump_pmbase = 1;
			break;
		case 'l':
			dump_lpc = 1;
			break;
		case 'e':
			dump_epbar = 1;
			break;
		case 'd':
			dump_dmibar = 1;
			break;
		case 'P':
			dump_pciexbar = 1;
			break;
		case 'M':
			dump_coremsrs = 1;
			break;
		case LONG_OPT_RANGE:
			if (strlen(optarg) == 0) {
				print_usage(argv[0]);
				exit(1);
			}
			const int sscanf_ret = sscanf(optarg, "%u-%u", &cores_range_start, &cores_range_end);
			if (sscanf_ret == 1) {
				/* the end of the range is not specified - only for one core */
				cores_range_end = cores_range_start;
			} else if (sscanf_ret != 2) {
				print_usage(argv[0]);
				exit(1);
			} else if (cores_range_end < cores_range_start) {
				printf("Error: invalid cores range <%u-%u>!\n",
						cores_range_start, cores_range_end);
				exit(1);
			}
			break;
		case 'a':
			dump_gpios = 1;
			show_gpio_diffs = 1;
			dump_mchbar = 1;
			dump_rcba = 1;
			dump_pmbase = 1;
			dump_lpc = 1;
			dump_epbar = 1;
			dump_dmibar = 1;
			dump_pciexbar = 1;
			dump_coremsrs = 1;
			dump_ambs = 1;
			dump_spi = 1;
			dump_ahci = 1;
			dump_sgx = 1;
			dump_tme = 1;
			break;
		case 'A':
			dump_ambs = 1;
			break;
		case 's':
			dump_spi = 1;
			break;
		case 'x':
			dump_sgx = 1;
			break;
		case 't':
			dump_tme = 1;
			break;
		case LONG_OPT_PCR:
			if (pcr_count < MAX_PCR_PORTS) {
				errno = 0;
				const unsigned long int pcr =
					strtoul(optarg, NULL, 0);
				if (strlen(optarg) == 0 || errno) {
					print_usage(argv[0]);
					exit(1);
				}
				dump_pcr[pcr_count++] = (uint8_t)pcr;
			} else {
				print_usage(argv[0]);
				exit(1);
			}
			break;
		case 'h':
		case '?':
		default:
			print_usage(argv[0]);
			exit(0);
			break;
		}
	}

#if defined(__FreeBSD__)
	if (open("/dev/io", O_RDWR) < 0) {
		perror("/dev/io");
#elif defined(__NetBSD__)
# ifdef __i386__
	if (i386_iopl(3)) {
		perror("iopl");
# else
	if (x86_64_iopl(3)) {
		perror("iopl");
# endif
#else
	if (iopl(3)) {
		perror("iopl");
#endif
		printf("You need to be root.\n");
		exit(1);
	}

#ifndef __DARWIN__
	if ((fd_mem = open("/dev/mem", O_RDWR)) < 0) {
		perror("Can not open /dev/mem");
		exit(1);
	}
#endif

	pacc = pci_alloc();
	pacc->method = PCI_ACCESS_I386_TYPE1;
	pci_init(pacc);
	pci_scan_bus(pacc);

	/* Find the required devices */
	for (dev = pacc->devices; dev; dev = dev->next) {
		pci_fill_info(dev, PCI_FILL_CLASS);
		/* The ISA/LPC bridge can be 0x1f, 0x07, or 0x04 so we probe. */
		if (dev->device_class == 0x0601) { /* ISA/LPC bridge */
			if (sb == NULL) {
				sb = dev;
			} else {
				fprintf(stderr, "Multiple devices with class ID"
					" 0x0601, using %02x%02x:%02x.%02x\n",
					sb->domain, sb->bus, sb->dev, sb->func);
				break;
			}
		}
	}

	if (!sb) {
		printf("No southbridge found.\n");
		exit(1);
	}

	pci_fill_info(sb, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);

	if (sb->vendor_id != PCI_VENDOR_ID_INTEL) {
		printf("Not an Intel(R) southbridge.\n");
		exit(1);
	}

	nb = pci_get_dev(pacc, 0, 0, 0x00, 0);
	if (!nb) {
		printf("No northbridge found.\n");
		exit(1);
	}

	pci_fill_info(nb, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);

	if (nb->vendor_id != PCI_VENDOR_ID_INTEL) {
		printf("Not an Intel(R) northbridge.\n");
		exit(1);
	}

	gfx = pci_get_dev(pacc, 0, 0, 0x02, 0);
	if (gfx) {
		pci_fill_info(gfx, PCI_FILL_IDENT | PCI_FILL_BASES |
				   PCI_FILL_CLASS);
		if ((gfx->device_class & 0xff00) != 0x0300)
			gfx = NULL;
		else if (gfx->vendor_id != PCI_VENDOR_ID_INTEL)
			gfx = NULL;
	}

	if (sb->device_id == PCI_DEVICE_ID_INTEL_BAYTRAIL_LPC) {
		ahci = pci_get_dev(pacc, 0, 0, 0x13, 0);
	} else {
		ahci = pci_get_dev(pacc, 0, 0, 0x1f, 2);
		if (ahci) {
			pci_fill_info(ahci, PCI_FILL_CLASS);
			if (ahci->device_class != PCI_CLASS_STORAGE_SATA)
				ahci = pci_get_dev(pacc, 0, 0, 0x17, 0);
		}
	}

	if (ahci) {
		pci_fill_info(ahci, PCI_FILL_IDENT | PCI_FILL_BASES |
				    PCI_FILL_CLASS);

		if (ahci->vendor_id != PCI_VENDOR_ID_INTEL ||
		    ahci->device_class != PCI_CLASS_STORAGE_SATA)
			ahci = NULL;
	}

	print_system_info(nb, sb, gfx);

	/* Now do the deed */
	if (dump_gpios) {
		print_gpios(sb, 1, show_gpio_diffs);
		printf("\n\n");
	} else if (show_gpio_diffs) {
		print_gpios(sb, 0, show_gpio_diffs);
		printf("\n\n");
	}

	if (dump_rcba) {
		print_rcba(sb);
		printf("\n\n");
	}

	if (dump_pmbase) {
		print_pmbase(sb, pacc);
		printf("\n\n");
	}

	if (dump_lpc) {
		print_lpc(sb, pacc);
		printf("\n\n");
	}

	if (dump_mchbar) {
		print_mchbar(nb, pacc, dump_spd_file);
		printf("\n\n");
	}

	if (dump_epbar) {
		print_epbar(nb);
		printf("\n\n");
	}

	if (dump_dmibar) {
		print_dmibar(nb);
		printf("\n\n");
	}

	if (dump_pciexbar) {
		print_pciexbar(nb);
		printf("\n\n");
	}

	if (dump_coremsrs) {
		print_intel_msrs(cores_range_start, cores_range_end);
		printf("\n\n");
	}

	if (dump_ambs)
		print_ambs(nb, pacc);

	if (dump_spi)
		print_spi(sb);

	if (dump_gfx)
		print_gfx(gfx);

	if (dump_ahci)
		print_ahci(ahci);

	if (dump_sgx)
		print_sgx();

	if (dump_tme)
		print_tme();

	if (pcr_count)
		print_pcr_ports(sb, dump_pcr, pcr_count);

	/* Clean up */
	pcr_cleanup();
	if (ahci)
		pci_free_dev(ahci);
	if (gfx)
		pci_free_dev(gfx);
	pci_free_dev(nb);
	/* `sb` wasn't allocated by pci_get_dev() */
	pci_cleanup(pacc);

	return 0;
}
