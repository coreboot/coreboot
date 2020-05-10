/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include "haswell.h"

void intel_northbridge_haswell_finalize_smm(void)
{
	pci_or_config16(HOST_BRIDGE, 0x50, 1 << 0);	/* GGC */
	pci_or_config32(HOST_BRIDGE, 0x5c, 1 << 0);	/* DPR */
	pci_or_config32(HOST_BRIDGE, 0x78, 1 << 10);	/* ME */
	pci_or_config32(HOST_BRIDGE, 0x90, 1 << 0);	/* REMAPBASE */
	pci_or_config32(HOST_BRIDGE, 0x98, 1 << 0);	/* REMAPLIMIT */
	pci_or_config32(HOST_BRIDGE, 0xa0, 1 << 0);	/* TOM */
	pci_or_config32(HOST_BRIDGE, 0xa8, 1 << 0);	/* TOUUD */
	pci_or_config32(HOST_BRIDGE, 0xb0, 1 << 0);	/* BDSM */
	pci_or_config32(HOST_BRIDGE, 0xb4, 1 << 0);	/* BGSM */
	pci_or_config32(HOST_BRIDGE, 0xb8, 1 << 0);	/* TSEGMB */
	pci_or_config32(HOST_BRIDGE, 0xbc, 1 << 0);	/* TOLUD */

	MCHBAR32_OR(MMIO_PAVP_MSG, 1 << 0);	/* PAVP */
	MCHBAR32_OR(SAPMCTL, 1UL << 31);	/* SA PM */
	MCHBAR32_OR(UMAGFXCTL, 1 << 0);		/* UMA GFX */
	MCHBAR32_OR(VTDTRKLCK, 1 << 0);		/* VTDTRK */
	MCHBAR32_OR(REQLIM, 1UL << 31);
	MCHBAR32_OR(DMIVCLIM, 1UL << 31);
	MCHBAR32_OR(CRDTLCK, 1 << 0);

	/* Memory Controller Lockdown */
	MCHBAR8(MC_LOCK) = 0x8f;

	/* Read+write the following */
	MCHBAR32(VDMBDFBARKVM)  = MCHBAR32(VDMBDFBARKVM);
	MCHBAR32(VDMBDFBARPAVP) = MCHBAR32(VDMBDFBARPAVP);
	MCHBAR32(HDAUDRID)      = MCHBAR32(HDAUDRID);
}
