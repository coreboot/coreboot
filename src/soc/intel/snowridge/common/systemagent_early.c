/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/systemagent_server.h>
#include <soc/systemagent.h>

uint32_t sa_server_soc_reg_to_pci_offset(enum sa_server_reg reg)
{
	switch (reg) {
	case MMCFG_BASE_REG:
		return PCIE_MMCFG_BASE;
	case MMCFG_LIMIT_REG:
		return PCIE_MMCFG_LIMIT;
	case TSEG_BASE_REG:
		return TSEG;
	case TSEG_LIMIT_REG:
		return TSEG_LIMIT;
	case TOCM_REG:
		return TOCM;
	case TOUUD_REG:
		return TOUUD;
	case TOLUD_REG:
		return TOLUD;
	case MMIO_L_REG:
		return MMIOL;
	case VT_BAR_REG:
		return VTBAR;
	case DPR_REG:
		return DPR;
	default:
		return 0;
	}
}
