/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
 * Copyright (C) 2009 Carl-Daniel Hailfinger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>

#if defined(__GLIBC__)
#include <sys/io.h>
#endif
#if (defined(__MACH__) && defined(__APPLE__))
/* DirectHW is available here: http://www.coreboot.org/DirectHW */
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
#define PCI_DEVICE_ID_INTEL_ICH10R		0x3a16
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
#define PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP	0x9cc5
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

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

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
msr_t freebsd_rdmsr(int addr);
int freebsd_wrmsr(int addr, msr_t msr);
#endif
typedef struct { uint16_t addr; int size; char *name; } io_register_t;

void *map_physical(uint64_t phys_addr, size_t len);
void unmap_physical(void *virt_addr, size_t len);

unsigned int cpuid(unsigned int op);
int print_intel_core_msrs(void);
int print_mchbar(struct pci_dev *nb, struct pci_access *pacc, const char *dump_spd_file);
int print_pmbase(struct pci_dev *sb, struct pci_access *pacc);
int print_rcba(struct pci_dev *sb);
int print_gpios(struct pci_dev *sb, int show_all, int show_diffs);
int print_epbar(struct pci_dev *nb);
int print_dmibar(struct pci_dev *nb);
int print_pciexbar(struct pci_dev *nb);
int print_ambs(struct pci_dev *nb, struct pci_access *pacc);
int print_spi(struct pci_dev *sb);
int print_gfx(struct pci_dev *gfx);
int print_ahci(struct pci_dev *ahci);
void ivybridge_dump_timings(const char *dump_spd_file);
