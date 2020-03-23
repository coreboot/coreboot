/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/pci_ops.h>
#include "sandybridge.h"

void intel_sandybridge_finalize_smm(void)
{
	pci_or_config16(HOST_BRIDGE, GGC,    1 << 0);
	pci_or_config16(HOST_BRIDGE, PAVPC,  1 << 2);
	pci_or_config32(HOST_BRIDGE, DPR,    1 << 0);
	pci_or_config32(HOST_BRIDGE, MESEG_MASK, MELCK);
	pci_or_config32(HOST_BRIDGE, REMAPBASE,  1 << 0);
	pci_or_config32(HOST_BRIDGE, REMAPLIMIT, 1 << 0);
	pci_or_config32(HOST_BRIDGE, TOM,    1 << 0);
	pci_or_config32(HOST_BRIDGE, TOUUD,  1 << 0);
	pci_or_config32(HOST_BRIDGE, BDSM,   1 << 0);
	pci_or_config32(HOST_BRIDGE, BGSM,   1 << 0);
	pci_or_config32(HOST_BRIDGE, TSEGMB, 1 << 0);
	pci_or_config32(HOST_BRIDGE, TOLUD,  1 << 0);

	MCHBAR32_OR(PAVP_MSG,  1 <<  0);	/* PAVP */
	MCHBAR32_OR(SAPMCTL,   1 << 31);	/* SA PM */
	MCHBAR32_OR(UMAGFXCTL, 1 <<  0);	/* UMA GFX */
	MCHBAR32_OR(VTDTRKLCK, 1 <<  0);	/* VTDTRK */
	MCHBAR32_OR(REQLIM,    1 << 31);
	MCHBAR32_OR(DMIVCLIM,  1 << 31);
	MCHBAR32_OR(CRDTLCK,   1 <<  0);

	/* Memory Controller Lockdown */
	MCHBAR8(MC_LOCK) = 0x8f;

	/* Read+write the following */
	MCHBAR32(VDMBDFBARKVM)  = MCHBAR32(VDMBDFBARKVM);
	MCHBAR32(VDMBDFBARPAVP) = MCHBAR32(VDMBDFBARPAVP);
	MCHBAR32(HDAUDRID)      = MCHBAR32(HDAUDRID);
}
