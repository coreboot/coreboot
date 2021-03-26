/* SPDX-License-Identifier: GPL-2.0-only */

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

	mchbar_setbits32(PAVP_MSG,  1 <<  0);	/* PAVP */
	mchbar_setbits32(SAPMCTL,   1 << 31);	/* SA PM */
	mchbar_setbits32(UMAGFXCTL, 1 <<  0);	/* UMA GFX */
	mchbar_setbits32(VTDTRKLCK, 1 <<  0);	/* VTDTRK */
	mchbar_setbits32(REQLIM,    1 << 31);
	mchbar_setbits32(DMIVCLIM,  1 << 31);
	mchbar_setbits32(CRDTLCK,   1 <<  0);

	/* Memory Controller Lockdown */
	mchbar_write8(MC_LOCK, 0x8f);

	/* Read+write the following */
	mchbar_setbits32(VDMBDFBARKVM, 0);
	mchbar_setbits32(VDMBDFBARPAVP, 0);
	mchbar_setbits32(HDAUDRID, 0);
}
