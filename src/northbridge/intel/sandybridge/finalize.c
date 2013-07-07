/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <stdlib.h>
#include "sandybridge.h"

#define PCI_DEV_SNB PCI_DEV(0, 0, 0)

void intel_sandybridge_finalize_smm(void)
{
	pcie_or_config16(PCI_DEV_SNB, 0x50, 1 << 0);	/* GGC */
	pcie_or_config32(PCI_DEV_SNB, 0x5c, 1 << 0);	/* DPR */
	pcie_or_config32(PCI_DEV_SNB, 0x78, 1 << 10);	/* ME */
	pcie_or_config32(PCI_DEV_SNB, 0x90, 1 << 0);	/* REMAPBASE */
	pcie_or_config32(PCI_DEV_SNB, 0x98, 1 << 0);	/* REMAPLIMIT */
	pcie_or_config32(PCI_DEV_SNB, 0xa0, 1 << 0);	/* TOM */
	pcie_or_config32(PCI_DEV_SNB, 0xa8, 1 << 0);	/* TOUUD */
	pcie_or_config32(PCI_DEV_SNB, 0xb0, 1 << 0);	/* BDSM */
	pcie_or_config32(PCI_DEV_SNB, 0xb4, 1 << 0);	/* BGSM */
	pcie_or_config32(PCI_DEV_SNB, 0xb8, 1 << 0);	/* TSEGMB */
	pcie_or_config32(PCI_DEV_SNB, 0xbc, 1 << 0);	/* TOLUD */

	MCHBAR32_OR(0x5500, 1 << 0);	/* PAVP */
	MCHBAR32_OR(0x5f00, 1 << 31);	/* SA PM */
	MCHBAR32_OR(0x6020, 1 << 0);	/* UMA GFX */
	MCHBAR32_OR(0x63fc, 1 << 0);	/* VTDTRK */
	MCHBAR32_OR(0x6800, 1 << 31);
	MCHBAR32_OR(0x7000, 1 << 31);
	MCHBAR32_OR(0x77fc, 1 << 0);

	/* Memory Controller Lockdown */
	MCHBAR8(0x50fc) = 0x8f;

	/* Read+write the following */
	MCHBAR32(0x6030) = MCHBAR32(0x6030);
	MCHBAR32(0x6034) = MCHBAR32(0x6034);
	MCHBAR32(0x6008) = MCHBAR32(0x6008);
}
