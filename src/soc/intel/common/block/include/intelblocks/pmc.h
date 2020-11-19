/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_PMC_H
#define SOC_INTEL_COMMON_BLOCK_PMC_H

#include <device/device.h>
#include <stdint.h>

/* PMC controller resource structure */
struct pmc_resource_config {
	/* PMC PCI config offset for MMIO BAR */
	uint8_t pwrmbase_offset;
	/* MMIO BAR address */
	uintptr_t pwrmbase_addr;
	/* MMIO BAR size */
	size_t pwrmbase_size;
	/* PMC PCI config offset for IO BAR */
	uint8_t abase_offset;
	/* IO BAR address */
	uintptr_t abase_addr;
	/* IO BAR size */
	size_t abase_size;
};

/*
 * SoC overrides
 *
 * All new SoCs wishes to make use of common PMC PCI driver
 * must implement below functionality .
 */

/*
 * Function to initialize PMC controller.
 *
 * This initialization may differ between different SoC
 *
 * Input: Device Structure PMC PCI device
 */
void pmc_soc_init(struct device *dev);

/*
 * SoC should fill this structure information based on
 * PMC controller register information like PWRMBASE, ABASE offset
 * BAR and Size
 *
 * Input: PMC config structure
 * Output: -1 = Error, 0 = Success
 */
int pmc_soc_get_resources(struct pmc_resource_config *cfg);

#endif /* SOC_INTEL_COMMON_BLOCK_PMC_H */
