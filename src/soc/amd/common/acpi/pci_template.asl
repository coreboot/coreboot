/* SPDX-License-Identifier: GPL-2.0-only */
#include <device/pci_def.h>
/*
 * Template for fixed PCI devices on the SoC. SSDT will emit STAT=1 when enabled
 * in hardware and devicetree.cb. Allows to write DSDT code for such devices or
 * it's children. Possible use cases:
 * - PowerResources for devices behind PCIe bridges (NVVMe D3cold)
 * - _DSM for ACP
 * - BOCO and BACO for GPU
 */
#ifdef ACPI_PCI_DEV
 #undef ACPI_PCI_DEV
#endif
#define ACPI_PCI_DEV(name, dev, fun) \
	Device(name) {						\
		Name(_ADR, (dev << 16) + fun)			\
		Name (STAT, 0x0)				\
		Method (_STA, 0, NotSerialized)			\
		{						\
			Return (STAT)				\
		}						\
		/* Generated in SSDT */				\
		/* Set PM D[0-3] state */			\
		External (SPMD, MethodObj)			\
		/* Backup PCI_PM_CTRL */			\
		External (STPM, MethodObj)			\
		/* Restore PCI_PM_CTRL */			\
		External (RSPM, MethodObj)			\
		/* Clear and disable slot presence detect */	\
		External (DSLP, MethodObj)			\
		/* Enable Link and retrain */			\
		External (RETR, MethodObj)			\
		/* Disable Link */				\
		External (LDIS, MethodObj)			\
		/* Disable ASPM */				\
		External (DASP, MethodObj)			\
	}
