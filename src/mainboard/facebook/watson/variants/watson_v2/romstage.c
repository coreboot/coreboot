/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) Facebook, Inc. and its affiliates
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci_ops.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <variants.h>

void variant_romstage_fsp_init_params(UPD_DATA_REGION *UpdData)
{
	/* Configure IOU1 as 4*4 lanes */
	UpdData->ConfigIOU1_PciPort3 = 0;

	/* Configure IOU2 as 2*4 lanes */
	UpdData->ConfigIOU2_PciPort1 = 0;

	/* Configure PCH PCIe ports as 8*1 lanes */
	UpdData->PchPciPort1 = 1;
	UpdData->PchPciPort2 = 1;
	UpdData->PchPciPort3 = 1;
	UpdData->PchPciPort4 = 1;
	UpdData->PchPciPort5 = 1;
	UpdData->PchPciPort6 = 1;
	UpdData->PchPciPort7 = 1;
	UpdData->PchPciPort8 = 1;

	/* Enable hotplug for PCH PCIe ports */
	UpdData->HotPlug_PchPciPort1 = 1;
	UpdData->HotPlug_PchPciPort2 = 1;
	UpdData->HotPlug_PchPciPort3 = 1;
	UpdData->HotPlug_PchPciPort4 = 1;
	UpdData->HotPlug_PchPciPort5 = 1;
	UpdData->HotPlug_PchPciPort6 = 1;
	UpdData->HotPlug_PchPciPort7 = 1;
	UpdData->HotPlug_PchPciPort8 = 1;
}

void variant_early_mainboard_romstage_entry(void)
{
	// Enable LPC IO ports 0xca2, 0xca8 for IPMI
	pci_write_config32(PCH_DEV_LPC, LPC_GEN2_DEC,
		(0 << 16) | ALIGN_DOWN(0xca2, 4) | 1);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN3_DEC,
		(0 << 16) | ALIGN_DOWN(0xca8, 4) | 1);
}
