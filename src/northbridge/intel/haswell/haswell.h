/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __NORTHBRIDGE_INTEL_HASWELL_HASWELL_H__
#define __NORTHBRIDGE_INTEL_HASWELL_HASWELL_H__

/* Chipset types */
#define HASWELL_MOBILE	0
#define HASWELL_DESKTOP	1
#define HASWELL_SERVER	2

#include "memmap.h"

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

/* Device 0:0.0 PCI configuration space (Host Bridge) */
#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

#include "registers/host_bridge.h"

/* Device 0:1.0 PCI configuration space (PCIe Graphics) */

#define PEG_CAP		0xa2
#define PEG_DCAP	0xa4

#define PEG_LCAP	0xac

#define PEG_DSTS	0xaa

#define PEG_SLOTCAP	0xb4

#define PEG_DCAP2	0xc4	/* 32bit */

#define PEG_LCTL2	0xd0

#define PEG_VC0RCTL	0x114

#define PEG_ESD		0x144	/* 32bit */
#define PEG_LE1D	0x150	/* 32bit */
#define PEG_LE1A	0x158	/* 64bit */

#define PEG_UESTS	0x1c4
#define PEG_UESEV	0x1cc
#define PEG_CESTS	0x1d0

#define PEG_L0SLAT	0x22c

#define PEG_AFE_PM_TMR	0xc28

/* Device 0:2.0 PCI configuration space (Graphics Device) */

#define MSAC		0x62	/* Multi Size Aperture Control */

/*
 * MCHBAR
 */

#include <northbridge/intel/common/fixed_bars.h>

/* As there are many registers, define them on a separate file */
#include "registers/mchbar.h"

#define ARCHDIS		0xff0	/* DMA Remap Engine Policy Control */
#define  DMAR_LCKDN	(1 << 31)
#define  SPCAPCTRL	(1 << 25)
#define  L3HIT2PEND_DIS	(1 << 20)
#define  PRSCAPDIS	(1 << 2)
#define  GLBIOTLBINV	(1 << 1)
#define  GLBCTXTINV	(1 << 0)

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#include "registers/epbar.h"

/*
 * DMIBAR
 */

#include "registers/dmibar.h"

#ifndef __ASSEMBLER__

void mb_late_romstage_setup(void); /* optional */

void haswell_early_initialization(void);
void haswell_late_initialization(void);
void haswell_unhide_peg(void);

void report_platform_info(void);

#include <device/device.h>

struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(const struct device *device, unsigned long start,
					    struct acpi_rsdp *rsdp);

#endif /* __ASSEMBLER__ */
#endif /* __ACPI__ */
#endif /* __NORTHBRIDGE_INTEL_HASWELL_HASWELL_H__ */
