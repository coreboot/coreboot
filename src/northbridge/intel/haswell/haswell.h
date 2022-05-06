/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_HASWELL_HASWELL_H
#define NORTHBRIDGE_INTEL_HASWELL_HASWELL_H

#include <device/device.h>
#include <northbridge/intel/common/fixed_bars.h>

#include "memmap.h"
#include "registers/dmibar.h"
#include "registers/epbar.h"
#include "registers/host_bridge.h"
#include "registers/mchbar.h"
#include "registers/pcie_graphics.h"

/* Device 0:0.0 PCI configuration space (Host Bridge) */
#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

/* Device 0:2.0 PCI configuration space (Graphics Device) */

#define MSAC		0x62	/* Multi Size Aperture Control */

#define ARCHDIS		0xff0	/* DMA Remap Engine Policy Control */
#define  DMAR_LCKDN	(1 << 31)
#define  SPCAPCTRL	(1 << 25)
#define  L3HIT2PEND_DIS	(1 << 20)
#define  PRSCAPDIS	(1 << 2)
#define  GLBIOTLBINV	(1 << 1)
#define  GLBCTXTINV	(1 << 0)

void mb_late_romstage_setup(void); /* optional */

void haswell_early_initialization(void);
void haswell_late_initialization(void);
void haswell_unhide_peg(void);

void dmi_early_init(void);
void peg_dmi_recipe(const bool is_peg, const pci_devfn_t dev);

void report_platform_info(void);

struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(const struct device *device, unsigned long start,
					    struct acpi_rsdp *rsdp);

#endif /* NORTHBRIDGE_INTEL_HASWELL_HASWELL_H */
