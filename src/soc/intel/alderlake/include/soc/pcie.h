/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_ALDERLAKE_PCIE_H__
#define __SOC_ALDERLAKE_PCIE_H__

#include <intelblocks/pcie_rp.h>

const struct pcie_rp_group *get_pch_pcie_rp_table(void);
const struct pcie_rp_group *get_cpu_pcie_rp_table(void);

#endif /* __SOC_ALDERLAKE_PCIE_H__ */
