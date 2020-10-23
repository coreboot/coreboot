/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include "haswell.h"

void intel_northbridge_haswell_finalize_smm(void)
{
	pci_or_config16(HOST_BRIDGE, GGC,         1 << 0);
	pci_or_config32(HOST_BRIDGE, DPR,         1 << 0);
	pci_or_config32(HOST_BRIDGE, MESEG_LIMIT, 1 << 10);
	pci_or_config32(HOST_BRIDGE, REMAPBASE,   1 << 0);
	pci_or_config32(HOST_BRIDGE, REMAPLIMIT,  1 << 0);
	pci_or_config32(HOST_BRIDGE, TOM,         1 << 0);
	pci_or_config32(HOST_BRIDGE, TOUUD,       1 << 0);
	pci_or_config32(HOST_BRIDGE, BDSM,        1 << 0);
	pci_or_config32(HOST_BRIDGE, BGSM,        1 << 0);
	pci_or_config32(HOST_BRIDGE, TSEG,        1 << 0);
	pci_or_config32(HOST_BRIDGE, TOLUD,       1 << 0);

	/* Memory Controller Lockdown */
	MCHBAR32(MC_LOCK) |= 0x8f;

	MCHBAR32_OR(MMIO_PAVP_MSG, 1 << 0);	/* PAVP */
	MCHBAR32_OR(PCU_DDR_PTM_CTL, 1 << 5);	/* DDR PTM */
	MCHBAR32_OR(UMAGFXCTL, 1 << 0);		/* UMA GFX */
	MCHBAR32_OR(VTDTRKLCK, 1 << 0);		/* VTDTRK */
	MCHBAR32_OR(REQLIM, 1UL << 31);
	MCHBAR32_OR(DMIVCLIM, 1UL << 31);
	MCHBAR32_OR(CRDTLCK, 1 << 0);
	MCHBAR32_OR(MCARBLCK, 1 << 0);

	/* Read+write the following */
	MCHBAR32(VDMBDFBARKVM)  = MCHBAR32(VDMBDFBARKVM);
	MCHBAR32(VDMBDFBARPAVP) = MCHBAR32(VDMBDFBARPAVP);
	MCHBAR32(HDAUDRID)      = MCHBAR32(HDAUDRID);
}
