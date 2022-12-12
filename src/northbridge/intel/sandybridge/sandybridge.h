/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __NORTHBRIDGE_INTEL_SANDYBRIDGE_SANDYBRIDGE_H__
#define __NORTHBRIDGE_INTEL_SANDYBRIDGE_SANDYBRIDGE_H__

/* Device ID for SandyBridge and IvyBridge */
#define BASE_REV_SNB	0x00
#define BASE_REV_IVB	0x50
#define BASE_REV_MASK	0x50

#include "memmap.h"

/* Everything below this line is ignored in the DSDT */
#ifndef __ASSEMBLER__
#include <types.h>

/* Chipset types */
enum platform_type {
	PLATFORM_MOBILE = 0,
	PLATFORM_DESKTOP_SERVER,
};

/* Device 0:0.0 PCI configuration space (Host Bridge) */
#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

#include "registers/host_bridge.h"

/* Devices 0:1.0, 0:1.1, 0:1.2, 0:6.0 PCI configuration space (PCI Express Graphics) */

#define AFE_PWRON	0xc24	/* PEG Analog Front-End Power-On */

/* Device 0:2.0 PCI configuration space (Graphics Device) */

#define MSAC		0x62	/* Multi Size Aperture Control */

/*
 * MCHBAR
 */

#include <northbridge/intel/common/fixed_bars.h>

/* As there are many registers, define them on a separate file */
#include "registers/mchbar.h"

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#include "registers/epbar.h"

/*
 * DMIBAR
 */

#include "registers/dmibar.h"

bool is_sandybridge(void);

void intel_sandybridge_finalize_smm(void);
void systemagent_early_init(void);
void sandybridge_init_iommu(void);
void sandybridge_late_initialization(void);
void northbridge_romstage_finalize(void);
void early_init_dmi(void);

/* mainboard_early_init: Optional callback, run after console init but before raminit. */
void mainboard_early_init(int s3resume);
int mainboard_should_reset_usb(int s3resume);
void perform_raminit(int s3resume);
void report_memory_config(void);
enum platform_type get_platform_type(void);

#include <device/device.h>

struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(const struct device *device, unsigned long start,
					    struct acpi_rsdp *rsdp);

#endif /* __ASSEMBLER__ */
#endif /* __NORTHBRIDGE_INTEL_SANDYBRIDGE_SANDYBRIDGE_H__ */
