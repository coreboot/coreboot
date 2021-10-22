/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INTELTOOL_H
#define INTELTOOL_H 1

#include <arch/mmio.h>
#include <commonlib/helpers.h>

#include <stdint.h>

#if defined(__GLIBC__)
#include <sys/io.h>
#endif
#if (defined(__MACH__) && defined(__APPLE__))
/* DirectHW is available here: https://www.coreboot.org/DirectHW */
#define __DARWIN__
#include <DirectHW/DirectHW.h>
#endif
#ifdef __NetBSD__
#include <pciutils/pci.h>
#else
#include <pci/pci.h>
#endif

/* This #include is needed for freebsd_{rd,wr}msr. */
#if defined(__FreeBSD__)
#include <machine/cpufunc.h>
#endif

#ifdef __NetBSD__
static inline uint8_t inb(unsigned port)
{
	uint8_t data;
	__asm volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}
static inline uint16_t inw(unsigned port)
{
	uint16_t data;
	__asm volatile("inw %w1,%0": "=a" (data) : "d" (port));
	return data;
}
static inline uint32_t inl(unsigned port)
{
	uint32_t data;
	__asm volatile("inl %w1,%0": "=a" (data) : "d" (port));
	return data;
}
#endif

#define INTELTOOL_VERSION "1.0"

/* Tested chipsets: */
#define PCI_VENDOR_ID_INTEL			0x8086
#define PCI_DEVICE_ID_INTEL_ICH			0x2410
#define PCI_DEVICE_ID_INTEL_ICH0		0x2420
#define PCI_DEVICE_ID_INTEL_ICH2		0x2440
#define PCI_DEVICE_ID_INTEL_ICH4		0x24c0
#define PCI_DEVICE_ID_INTEL_ICH4M		0x24cc
#define PCI_DEVICE_ID_INTEL_ICH5		0x24d0
#define PCI_DEVICE_ID_INTEL_ICH6		0x2640
#define PCI_DEVICE_ID_INTEL_ICH7DH		0x27b0
#define PCI_DEVICE_ID_INTEL_ICH7		0x27b8
#define PCI_DEVICE_ID_INTEL_ICH7M		0x27b9
#define PCI_DEVICE_ID_INTEL_ICH7MDH		0x27bd
#define PCI_DEVICE_ID_INTEL_NM10		0x27bc
#define PCI_DEVICE_ID_INTEL_ICH8		0x2810
#define PCI_DEVICE_ID_INTEL_ICH8M		0x2815
#define PCI_DEVICE_ID_INTEL_ICH8ME		0x2811
#define PCI_DEVICE_ID_INTEL_ICH9DH		0x2912
#define PCI_DEVICE_ID_INTEL_ICH9DO		0x2914
#define PCI_DEVICE_ID_INTEL_ICH9R		0x2916
#define PCI_DEVICE_ID_INTEL_ICH9		0x2918
#define PCI_DEVICE_ID_INTEL_ICH9M		0x2919
#define PCI_DEVICE_ID_INTEL_ICH9ME		0x2917
#define PCI_DEVICE_ID_INTEL_ICH10DO		0x3a14
#define PCI_DEVICE_ID_INTEL_ICH10R		0x3a16
#define PCI_DEVICE_ID_INTEL_ICH10		0x3a18
#define PCI_DEVICE_ID_INTEL_ICH10D		0x3a1a
#define PCI_DEVICE_ID_INTEL_3400_DESKTOP	0x3b00
#define PCI_DEVICE_ID_INTEL_3400_MOBILE		0x3b01
#define PCI_DEVICE_ID_INTEL_P55			0x3b02
#define PCI_DEVICE_ID_INTEL_PM55		0x3b03
#define PCI_DEVICE_ID_INTEL_H55			0x3b06
#define PCI_DEVICE_ID_INTEL_QM57		0x3b07
#define PCI_DEVICE_ID_INTEL_H57			0x3b08
#define PCI_DEVICE_ID_INTEL_HM55		0x3b09
#define PCI_DEVICE_ID_INTEL_Q57			0x3b0a
#define PCI_DEVICE_ID_INTEL_HM57		0x3b0b
#define PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF	0x3b0d
#define PCI_DEVICE_ID_INTEL_B55_A		0x3b0e
#define PCI_DEVICE_ID_INTEL_QS57		0x3b0f
#define PCI_DEVICE_ID_INTEL_3400		0x3b12
#define PCI_DEVICE_ID_INTEL_3420		0x3b14
#define PCI_DEVICE_ID_INTEL_3450		0x3b16
#define PCI_DEVICE_ID_INTEL_B55_B		0x3b1e
#define PCI_DEVICE_ID_INTEL_SCH_POULSBO_LPC	0x8119
#define PCI_DEVICE_ID_INTEL_ICELAKE_LP_U	0x3482
#define PCI_DEVICE_ID_INTEL_Z68			0x1c44
#define PCI_DEVICE_ID_INTEL_P67			0x1c46
#define PCI_DEVICE_ID_INTEL_UM67		0x1c47
#define PCI_DEVICE_ID_INTEL_HM65		0x1c49
#define PCI_DEVICE_ID_INTEL_H67			0x1c4a
#define PCI_DEVICE_ID_INTEL_HM67		0x1c4b
#define PCI_DEVICE_ID_INTEL_Q65			0x1c4c
#define PCI_DEVICE_ID_INTEL_QS67		0x1c4d
#define PCI_DEVICE_ID_INTEL_Q67			0x1c4e
#define PCI_DEVICE_ID_INTEL_QM67		0x1c4f
#define PCI_DEVICE_ID_INTEL_B65			0x1c50
#define PCI_DEVICE_ID_INTEL_C202		0x1c52
#define PCI_DEVICE_ID_INTEL_C204		0x1c54
#define PCI_DEVICE_ID_INTEL_C206		0x1c56
#define PCI_DEVICE_ID_INTEL_H61			0x1c5c
#define PCI_DEVICE_ID_INTEL_Z77			0x1e44
#define PCI_DEVICE_ID_INTEL_Z75			0x1e46
#define PCI_DEVICE_ID_INTEL_Q77			0x1e47
#define PCI_DEVICE_ID_INTEL_Q75			0x1e48
#define PCI_DEVICE_ID_INTEL_B75			0x1e49
#define PCI_DEVICE_ID_INTEL_H77			0x1e4a
#define PCI_DEVICE_ID_INTEL_C216		0x1e53
#define PCI_DEVICE_ID_INTEL_QM77		0x1e55
#define PCI_DEVICE_ID_INTEL_QS77		0x1e56
#define PCI_DEVICE_ID_INTEL_HM77		0x1e57
#define PCI_DEVICE_ID_INTEL_UM77		0x1e58
#define PCI_DEVICE_ID_INTEL_HM76		0x1e59
#define PCI_DEVICE_ID_INTEL_HM75		0x1e5d
#define PCI_DEVICE_ID_INTEL_HM70		0x1e5e
#define PCI_DEVICE_ID_INTEL_NM70		0x1e5f
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL	0x9c41
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM	0x9c43
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE	0x9c45
#define PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP_PREM	0x9cc3
#define PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP		0x9cc5
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_SATA		0xa102
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_P2SB		0xa120
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_PRE		0xa141
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_SATA	0x9d03
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_PRE		0x9d41
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_SKL	0x9d43
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_SKL	0x9d46
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_SKL	0x9d48
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_KBL	0x9d53
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_KBL	0x9d56
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_KBL	0x9d58
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_BASE	0x9d50
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_PREM	0x9d4e
#define PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_IHDCP_PREM	0x9d4b
#define PCI_DEVICE_ID_INTEL_CANNONPOINT_LP_U_PREM	0x9d84
#define PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_PREM	0x0284
#define PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_BASE	0x0285
#define PCI_DEVICE_ID_INTEL_TIGERPOINT_U_SUPER	0xa081
#define PCI_DEVICE_ID_INTEL_TIGERPOINT_U_PREM	0xa082
#define PCI_DEVICE_ID_INTEL_TIGERPOINT_U_BASE	0xa083
#define PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_SUPER	0xa086
#define PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_PREM	0xa087
#define PCI_DEVICE_ID_INTEL_H110		0xa143
#define PCI_DEVICE_ID_INTEL_H170		0xa144
#define PCI_DEVICE_ID_INTEL_Z170		0xa145
#define PCI_DEVICE_ID_INTEL_Q170		0xa146
#define PCI_DEVICE_ID_INTEL_Q150		0xa147
#define PCI_DEVICE_ID_INTEL_B150		0xa148
#define PCI_DEVICE_ID_INTEL_C236		0xa149
#define PCI_DEVICE_ID_INTEL_C232		0xa14a
#define PCI_DEVICE_ID_INTEL_QM170		0xa14d
#define PCI_DEVICE_ID_INTEL_HM170		0xa14e
#define PCI_DEVICE_ID_INTEL_CM236		0xa150
#define PCI_DEVICE_ID_INTEL_HM175		0xa152
#define PCI_DEVICE_ID_INTEL_QM175		0xa153
#define PCI_DEVICE_ID_INTEL_CM238		0xa154

#define PCI_DEVICE_ID_INTEL_C621		0xa1c1
#define PCI_DEVICE_ID_INTEL_C622		0xa1c2
#define PCI_DEVICE_ID_INTEL_C624		0xa1c3
#define PCI_DEVICE_ID_INTEL_C625		0xa1c4
#define PCI_DEVICE_ID_INTEL_C626		0xa1c5
#define PCI_DEVICE_ID_INTEL_C627		0xa1c6
#define PCI_DEVICE_ID_INTEL_C628		0xa1c7
#define PCI_DEVICE_ID_INTEL_C629		0xa1ca
#define PCI_DEVICE_ID_INTEL_C621A		0xa1cb
#define PCI_DEVICE_ID_INTEL_C627A		0xa1cc
#define PCI_DEVICE_ID_INTEL_C629A		0xa1cd
#define PCI_DEVICE_ID_INTEL_C624_SUPER		0xa242
#define PCI_DEVICE_ID_INTEL_C627_SUPER_1	0xa243
#define PCI_DEVICE_ID_INTEL_C621_SUPER		0xa244
#define PCI_DEVICE_ID_INTEL_C627_SUPER_2	0xa245
#define PCI_DEVICE_ID_INTEL_C628_SUPER		0xa246
#define PCI_DEVICE_ID_INTEL_C621A_SUPER		0xa24a
#define PCI_DEVICE_ID_INTEL_C627A_SUPER		0xa24b
#define PCI_DEVICE_ID_INTEL_C629A_SUPER		0xa24c

#define PCI_DEVICE_ID_INTEL_EBG		0x1b81

#define PCI_DEVICE_ID_INTEL_H270		0xa2c4
#define PCI_DEVICE_ID_INTEL_Z270		0xa2c5
#define PCI_DEVICE_ID_INTEL_Q270		0xa2c6
#define PCI_DEVICE_ID_INTEL_Q250		0xa2c7
#define PCI_DEVICE_ID_INTEL_B250		0xa2c8
#define PCI_DEVICE_ID_INTEL_Z370		0xa2c9
#define PCI_DEVICE_ID_INTEL_H310C		0xa2ca
#define PCI_DEVICE_ID_INTEL_X299		0xa2d2

#define PCI_DEVICE_ID_INTEL_H310		0xa303
#define PCI_DEVICE_ID_INTEL_H370		0xa304
#define PCI_DEVICE_ID_INTEL_Z390		0xa305
#define PCI_DEVICE_ID_INTEL_Q370		0xa306
#define PCI_DEVICE_ID_INTEL_B360		0xa308
#define PCI_DEVICE_ID_INTEL_C246		0xa309
#define PCI_DEVICE_ID_INTEL_C242		0xa30a
#define PCI_DEVICE_ID_INTEL_QM370		0xa30c
#define PCI_DEVICE_ID_INTEL_HM370		0xa30d
#define PCI_DEVICE_ID_INTEL_CM246		0xa30e

#define PCI_DEVICE_ID_INTEL_Q570		0x4384
#define PCI_DEVICE_ID_INTEL_Z590		0x4385
#define PCI_DEVICE_ID_INTEL_H570		0x4386
#define PCI_DEVICE_ID_INTEL_B560		0x4387
#define PCI_DEVICE_ID_INTEL_H510		0x4388
#define PCI_DEVICE_ID_INTEL_WM590		0x4389
#define PCI_DEVICE_ID_INTEL_QM580		0x438a
#define PCI_DEVICE_ID_INTEL_HM570		0x438b
#define PCI_DEVICE_ID_INTEL_C252		0x438c
#define PCI_DEVICE_ID_INTEL_C256		0x438d
#define PCI_DEVICE_ID_INTEL_W580		0x438f

#define PCI_DEVICE_ID_INTEL_82810		0x7120
#define PCI_DEVICE_ID_INTEL_82810_DC	0x7122
#define PCI_DEVICE_ID_INTEL_82810E_DC	0x7124
#define PCI_DEVICE_ID_INTEL_82830M		0x3575
#define PCI_DEVICE_ID_INTEL_82845		0x1a30
#define PCI_DEVICE_ID_INTEL_82865		0x2570
#define PCI_DEVICE_ID_INTEL_82915		0x2580
#define PCI_DEVICE_ID_INTEL_82945P		0x2770
#define PCI_DEVICE_ID_INTEL_82945GM		0x27a0
#define PCI_DEVICE_ID_INTEL_82945GSE	0x27ac
#define PCI_DEVICE_ID_INTEL_82946		0x2970
#define PCI_DEVICE_ID_INTEL_82965PM		0x2a00
#define PCI_DEVICE_ID_INTEL_82Q965		0x2990
#define PCI_DEVICE_ID_INTEL_82975X		0x277c
#define PCI_DEVICE_ID_INTEL_82Q35		0x29b0
#define PCI_DEVICE_ID_INTEL_82G33		0x29c0
#define PCI_DEVICE_ID_INTEL_82Q33		0x29d0
#define PCI_DEVICE_ID_INTEL_82X38 		0x29e0
#define PCI_DEVICE_ID_INTEL_32X0		0x29f0
#define PCI_DEVICE_ID_INTEL_82XX4X		0x2a40
#define PCI_DEVICE_ID_INTEL_82Q45		0x2e10
#define PCI_DEVICE_ID_INTEL_82G45		0x2e20
#define PCI_DEVICE_ID_INTEL_82G41		0x2e30
#define PCI_DEVICE_ID_INTEL_82B43		0x2e40
#define PCI_DEVICE_ID_INTEL_82B43_2		0x2e90

#define PCI_DEVICE_ID_INTEL_C8_MOBILE		0x8c41
#define PCI_DEVICE_ID_INTEL_C8_DESKTOP		0x8c42
#define PCI_DEVICE_ID_INTEL_Z87			0x8c44
#define PCI_DEVICE_ID_INTEL_Z85			0x8c46
#define PCI_DEVICE_ID_INTEL_HM86		0x8c49
#define PCI_DEVICE_ID_INTEL_H87			0x8c4a
#define PCI_DEVICE_ID_INTEL_HM87		0x8c4b
#define PCI_DEVICE_ID_INTEL_Q85			0x8c4c
#define PCI_DEVICE_ID_INTEL_Q87			0x8c4e
#define PCI_DEVICE_ID_INTEL_QM87		0x8c4f
#define PCI_DEVICE_ID_INTEL_B85			0x8c50
#define PCI_DEVICE_ID_INTEL_C222		0x8c52
#define PCI_DEVICE_ID_INTEL_C224		0x8c54
#define PCI_DEVICE_ID_INTEL_C226		0x8c56
#define PCI_DEVICE_ID_INTEL_H81			0x8c5c

#define PCI_DEVICE_ID_INTEL_82X58		0x3405
#define PCI_DEVICE_ID_INTEL_SCH_POULSBO	0x8100
#define PCI_DEVICE_ID_INTEL_ATOM_DXXX	0xa000
#define PCI_DEVICE_ID_INTEL_I63XX		0x2670

#define PCI_DEVICE_ID_INTEL_I5000X		0x25c0
#define PCI_DEVICE_ID_INTEL_I5000Z		0x25d0
#define PCI_DEVICE_ID_INTEL_I5000V		0x25d4
#define PCI_DEVICE_ID_INTEL_I5000P		0x25d8

/* untested, but almost identical to D-series */
#define PCI_DEVICE_ID_INTEL_ATOM_NXXX	0xa010

#define PCI_DEVICE_ID_INTEL_82443LX		0x7180
/* 82443BX has a different device ID if AGP is disabled (hardware-wise). */
#define PCI_DEVICE_ID_INTEL_82443BX		0x7190
#define PCI_DEVICE_ID_INTEL_82443BX_NO_AGP	0x7192

/* 82371AB/EB/MB use the same device ID value. */
#define PCI_DEVICE_ID_INTEL_82371XX		0x7110

/* Bay Trail */
#define PCI_DEVICE_ID_INTEL_BAYTRAIL		0x0f00 /* SOC Transaction Router */
#define PCI_DEVICE_ID_INTEL_BAYTRAIL_LPC	0x0f1c
#define PCI_DEVICE_ID_INTEL_BAYTRAIL_GFX	0x0f31
#define CPUID_BAYTRAIL						0x30670

#define PCI_DEVICE_ID_INTEL_APL_LPC		0x5ae8
#define PCI_DEVICE_ID_INTEL_DNV_LPC		0x19dc
#define PCI_DEVICE_ID_INTEL_GLK_LPC		0x31E8

/* Intel starts counting these generations with the integration of the DRAM controller */
#define PCI_DEVICE_ID_INTEL_CORE_0TH_GEN	0xd132 /* Nehalem */
#define PCI_DEVICE_ID_INTEL_CORE_1ST_GEN	0x0044 /* Westmere */
#define PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D	0x0100 /* Sandy Bridge (Desktop) */
#define PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M	0x0104 /* Sandy Bridge (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3	0x0108 /* Sandy Bridge (Xeon E3) */
#define PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D	0x0150 /* Ivy Bridge (Desktop) */
#define PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M	0x0154 /* Ivy Bridge (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3	0x0158 /* Ivy Bridge (Xeon E3 v2) */
#define PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c	0x015c /* Ivy Bridge (?) */
#define PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_D	0x0c00 /* Haswell (Desktop) */
#define PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_M	0x0c04 /* Haswell (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_E3	0x0c08 /* Haswell (Xeon E3 v3) */
#define PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_U	0x0a04 /* Haswell-ULT */
#define PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_U	0x1604 /* Broadwell-ULT */
#define PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D2	0x190f /* Skylake (Desktop) */
#define PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_U	0x1904 /* Skylake (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_Y	0x190c /* Skylake (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_M	0x1910 /* Skylake (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_WST	0x1918 /* Skylake (Workstation) */
#define PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D	0x191f /* Skylake (Desktop) */
#define PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_E	0x2020 /* Skylake-E (Server) */
#define PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U	0x5904 /* Kabylake (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_Y	0x590C /* Kabylake (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U_Q	0x5914 /* Kabylake (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_E3	0x5918 /* Kabylake Xeon E3 */
#define PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_1	0x3ed0 /* Coffeelake (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_2	0x3e34 /* Whiskeylake (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_10TH_GEN_U	0x8a12 /* Icelake U */
#define PCI_DEVICE_ID_INTEL_CORE_CML_U1		0x9b51 /* Cometlake U (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_CML_U2		0x9b61 /* Cometlake U (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_CML_U3		0x9b71 /* Cometlake U (Mobile) */
#define PCI_DEVICE_ID_INTEL_CORE_TGL_ID_U_2	0x9a04 /* Tigerlake UP3 2 Cores */
#define PCI_DEVICE_ID_INTEL_CORE_TGL_ID_U_4	0x9a14 /* Tigerlake UP3 4 Cores */
#define PCI_DEVICE_ID_INTEL_CORE_TGL_ID_Y_2	0x9a02 /* Tigerlake UP4 2 Cores */
#define PCI_DEVICE_ID_INTEL_CORE_TGL_ID_Y_4	0x9a12 /* Tigerlake UP4 4 Cores */
#define PCI_DEVICE_ID_INTEL_CORE_TGL_ID_H_4	0x9a16 /* Tigerlake H 4 Cores */
#define PCI_DEVICE_ID_INTEL_CORE_TGL_ID_H_6	0x9a26 /* Tigerlake H 6 Cores */
#define PCI_DEVICE_ID_INTEL_CORE_TGL_ID_H_8	0x9a36 /* Tigerlake H 8 Cores */
#define PCI_DEVICE_ID_INTEL_HEWITTLAKE		0x6f00 /* Hewitt Lake */
#define PCI_DEVICE_ID_INTEL_SAPPHIRERAPIDS_SP	0x09a2 /* Sapphire Rapids SP */


/* Intel GPUs */
#define PCI_DEVICE_ID_INTEL_G35_EXPRESS		0x2982
#define PCI_DEVICE_ID_INTEL_G35_EXPRESS_1	0x2983
#define PCI_DEVICE_ID_INTEL_965_EXPRESS		0x2a02
#define PCI_DEVICE_ID_INTEL_965_EXPRESS_1	0x2a03
#define PCI_DEVICE_ID_INTEL_965_EXPRESS_2	0x2a12
#define PCI_DEVICE_ID_INTEL_965_EXPRESS_3	0x2a13
#define PCI_DEVICE_ID_INTEL_4_SERIES		0x2a42
#define PCI_DEVICE_ID_INTEL_4_SERIES_1		0x2a43
#define PCI_DEVICE_ID_INTEL_G45			0x2e22
#define PCI_DEVICE_ID_INTEL_G45_1		0x2e23
#define PCI_DEVICE_ID_INTEL_Q45			0x2e12
#define PCI_DEVICE_ID_INTEL_Q45_1		0x2e13
#define PCI_DEVICE_ID_INTEL_G41			0x2e32
#define PCI_DEVICE_ID_INTEL_G41_1		0x2e33
#define PCI_DEVICE_ID_INTEL_B43			0x2e42
#define PCI_DEVICE_ID_INTEL_B43_1		0x2e43
#define PCI_DEVICE_ID_INTEL_B43_2		0x2e92
#define PCI_DEVICE_ID_INTEL_B43_3		0x2e93
#define PCI_DEVICE_ID_INTEL_HD_GRAPHICS		0x0046
#define PCI_DEVICE_ID_INTEL_HD_GRAPHICS_1	0x0042
#define PCI_DEVICE_ID_INTEL_HD_GRAPHICS_2	0x0106
#define PCI_DEVICE_ID_INTEL_HD_2000		0x0102
#define PCI_DEVICE_ID_INTEL_HD_2000_1		0x0106
#define PCI_DEVICE_ID_INTEL_HD_3000		0x0116
#define PCI_DEVICE_ID_INTEL_HD_3000_1		0x0112
#define PCI_DEVICE_ID_INTEL_HD_3000_2		0x0116
#define PCI_DEVICE_ID_INTEL_HD_3000_3		0x0122
#define PCI_DEVICE_ID_INTEL_HD_3000_4		0x0126
#define PCI_DEVICE_ID_INTEL_HD_3000_5		0x0116
#define PCI_DEVICE_ID_INTEL_HD_2500		0x0152
#define PCI_DEVICE_ID_INTEL_HD_2500_1		0x0156
#define PCI_DEVICE_ID_INTEL_HD_2500_2		0x015A
#define PCI_DEVICE_ID_INTEL_HD_4000		0x0162
#define PCI_DEVICE_ID_INTEL_HD_4000_1		0x0166
#define PCI_DEVICE_ID_INTEL_HD_4000_2		0x016A
#define PCI_DEVICE_ID_INTEL_HD_4600		0x0412
#define PCI_DEVICE_ID_INTEL_HD_4600_1		0x0416
#define PCI_DEVICE_ID_INTEL_HD_4400		0x041E
#define PCI_DEVICE_ID_INTEL_HD_5000		0x0422
#define PCI_DEVICE_ID_INTEL_HD_5000_1		0x0426
#define PCI_DEVICE_ID_INTEL_HD_5000_2		0x042A
#define PCI_DEVICE_ID_INTEL_HD_510		0x1902
#define PCI_DEVICE_ID_INTEL_HD_515		0x191E
#define PCI_DEVICE_ID_INTEL_HD_520		0x1916
#define PCI_DEVICE_ID_INTEL_HD_530_1		0x191B
#define PCI_DEVICE_ID_INTEL_HD_530_2		0x1912
#define PCI_DEVICE_ID_INTEL_UHD_615_1		0x591C
#define PCI_DEVICE_ID_INTEL_UHD_615_2		0x591E
#define PCI_DEVICE_ID_INTEL_UHD_617		0x87C0
#define PCI_DEVICE_ID_INTEL_UHD_620_1		0x5917
#define PCI_DEVICE_ID_INTEL_UHD_620_2		0x3EA0
#define PCI_DEVICE_ID_INTEL_UHD_620_3		0x5916
#define PCI_DEVICE_ID_INTEL_UHD_630_1		0x3E92
#define PCI_DEVICE_ID_INTEL_UHD_630_2		0x3E9B
#define PCI_DEVICE_ID_INTEL_UHD_630_3		0x3E91
#define PCI_DEVICE_ID_INTEL_UHD_630_4		0x5912
#define PCI_DEVICE_ID_INTEL_UHD_630_5		0x591B
#define PCI_DEVICE_ID_INTEL_UHD_630_6		0x5902
#define PCI_DEVICE_ID_INTEL_UHD_630_7		0x3E98
#define PCI_DEVICE_ID_INTEL_UHD_640		0x5926
#define PCI_DEVICE_ID_INTEL_IRIS_540		0x1926
#define PCI_DEVICE_ID_INTEL_IRIS_550		0x1927
#define PCI_DEVICE_ID_INTEL_IRIS_PRO_580	0x193B
#define PCI_DEVICE_ID_INTEL_IRIS_PLUS_650	0x5927
#define PCI_DEVICE_ID_INTEL_IRIS_PLUS_655	0x3EA5
#define PCI_DEVICE_ID_INTEL_IRIS_PLUS_G7	0x8A52
#define PCI_DEVICE_ID_INTEL_UHD_GRAPHICS	0x9b41
#define PCI_DEVICE_ID_INTEL_TGL_GT2_Y		0x9A40
#define PCI_DEVICE_ID_INTEL_TGL_GT2_UY		0x9A49
#define PCI_DEVICE_ID_INTEL_TGL_GT1		0x9A60
#define PCI_DEVICE_ID_INTEL_TGL_GT1_2		0x9A68
#define PCI_DEVICE_ID_INTEL_TGL_GT2_ULT_1	0x9A78
#define PCI_DEVICE_ID_INTEL_TGL_GT2_ULT_2	0x9A70

#if !defined(__DARWIN__) && !defined(__FreeBSD__)
typedef struct { uint32_t hi, lo; } msr_t;
#endif
#if defined (__FreeBSD__)
/* FreeBSD already has conflicting definitions for wrmsr/rdmsr. */
#undef rdmsr
#undef wrmsr
#define rdmsr freebsd_rdmsr
#define wrmsr freebsd_wrmsr
typedef struct { uint32_t hi, lo; } msr_t;
#endif
typedef struct { uint16_t addr; int size; char *name; } io_register_t;
typedef struct {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
} cpuid_result_t;

void *map_physical(uint64_t phys_addr, size_t len);
void unmap_physical(void *virt_addr, size_t len);

unsigned int cpuid(unsigned int op);
int print_intel_msrs(unsigned int range_start, unsigned int range_end);
int print_mchbar(struct pci_dev *nb, struct pci_access *pacc, const char *dump_spd_file);
int print_pmbase(struct pci_dev *sb, struct pci_access *pacc);
int print_lpc(struct pci_dev *sb, struct pci_access *pacc);
int print_rcba(struct pci_dev *sb);
void print_iobp(struct pci_dev *sb, volatile uint8_t *rcba);
int print_gpios(struct pci_dev *sb, int show_all, int show_diffs);
const struct gpio_community *const *get_gpio_communities(struct pci_dev *const sb,
						size_t* community_count,
						size_t* pad_stepping);
void print_gpio_groups(struct pci_dev *sb);
int print_epbar(struct pci_dev *nb);
int print_dmibar(struct pci_dev *nb);
int print_pciexbar(struct pci_dev *nb);
int print_ambs(struct pci_dev *nb, struct pci_access *pacc);
int print_spi(struct pci_dev *sb);
int print_gfx(struct pci_dev *gfx);
int print_ahci(struct pci_dev *ahci);
int print_sgx(void);
void print_tme(void);
void ivybridge_dump_timings(const char *dump_spd_file);

#endif
