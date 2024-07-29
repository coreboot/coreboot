/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_ASPM_H
#define SOC_INTEL_COMMON_BLOCK_ASPM_H

#include <intelblocks/pcie_rp.h>

void configure_pch_rp_power_management(FSP_S_CONFIG *s_cfg,
				       const struct pcie_rp_config *rp_cfg,
				       unsigned int index);

#if CONFIG(HAS_INTEL_CPU_ROOT_PORTS)
void configure_cpu_rp_power_management(FSP_S_CONFIG *s_cfg,
				       const struct pcie_rp_config *rp_cfg,
				       unsigned int index);
#endif

#endif	/* SOC_INTEL_COMMON_BLOCK_ASPM_H */
