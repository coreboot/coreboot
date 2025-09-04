/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_COMMON_BLOCK_CNVI_H_
#define _SOC_INTEL_COMMON_BLOCK_CNVI_H_

/**
 * struct soc_intel_common_block_cnvi_config - Configuration structure for CNVi
 * @wake: Specifies the wake pin used for ACPI Power Resources for Wake (_PRW).
 *
 * This structure holds the configuration data required for setting up the
 * Connectivity Integration (CNVi) block in Intel SoCs. The wake pin is
 * utilized to enable wake-up capabilities from sleep states as defined
 * by the ACPI specification.
 */
struct soc_intel_common_block_cnvi_config {
	unsigned int wake;
};

#endif /* SOC_INTEL_COMMON_BLOCK_CNVI_H_ */
