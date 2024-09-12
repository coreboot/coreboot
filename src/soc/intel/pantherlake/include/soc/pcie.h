/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_PANTHERLAKE_PCIE_H__
#define __SOC_PANTHERLAKE_PCIE_H__

#include <intelblocks/pcie_rp.h>

const struct pcie_rp_group *get_pcie_rp_table(void);
const struct pcie_rp_group *get_tbt_pcie_rp_table(void);

#endif
