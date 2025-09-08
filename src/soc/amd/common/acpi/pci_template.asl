/* SPDX-License-Identifier: GPL-2.0-only */

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
	Device(name) {					\
		Name(_ADR, (dev << 16) + fun)		\
		Name (STAT, 0x0)			\
		Method (_STA, 0, NotSerialized)		\
		{					\
			Return (STAT)			\
		}					\
	}
