/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <arch/acpi.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <memory_info.h>

#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>

static void transfer_memory_info(TYPE17_DMI_INFO *dmi17, struct dimm_info *dimm)
{
	size_t len, destlen;
	uint32_t offset;


	len = strnlen(dmi17->SerialNumber, sizeof(dmi17->SerialNumber)) + 1;
	destlen = sizeof(dimm->serial);

	if (len > destlen) {
		offset = len - destlen;
		len = destlen;
	} else
		offset = 0;

	strncpy((char *)dimm->serial, &dmi17->SerialNumber[offset], len);
	dimm->dimm_size = dmi17->ExtSize;
	dimm->ddr_type = dmi17->MemoryType;
	dimm->ddr_frequency = dmi17->Speed;
	dimm->rank_per_dimm = dmi17->Attributes;
	dimm->mod_type = dmi17->MemoryType;
	dimm->bus_width = dmi17->DataWidth;
	dimm->mod_id = dmi17->ManufacturerIdCode;
	dimm->bank_locator = 0;
	strncpy((char *)dimm->module_part_number, dmi17->PartNumber,
				sizeof(dimm->module_part_number));
}

static void prepare_dmi_17(void *unused)
{
	DMI_INFO *DmiTable;
	TYPE17_DMI_INFO *address;
	struct memory_info *mem_info;
	struct dimm_info *dimm;
	int i, j, dimm_cnt = 0;

	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(struct memory_info));
	if (!mem_info) {
		printk(BIOS_NOTICE, "Failed to add memory info to CBMEM.\n");
		return;
	}
	memset(mem_info, 0, sizeof(struct memory_info));

	DmiTable = agesawrapper_getlateinitptr(PICK_DMI);
	for (i = 0; i < MAX_CHANNELS_PER_SOCKET; i++) {
		for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j++) {
			address = &DmiTable->T17[0][i][j];
			if (address->Handle > 0) {
				dimm = &mem_info->dimm[dimm_cnt];
				dimm->channel_num = i;
				dimm->dimm_num = j;
				transfer_memory_info(address, dimm);
				dimm_cnt++;
			}
		}
	}
	mem_info->dimm_cnt = dimm_cnt;
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY,
			prepare_dmi_17, NULL);

static void agesawrapper_post_device(void *unused)
{
	if (acpi_is_wakeup_s3())
		return;

	do_agesawrapper(agesawrapper_amdinitlate, "amdinitlate");

	if (!acpi_s3_resume_allowed())
		return;

	do_agesawrapper(agesawrapper_amdinitrtb, "amdinitrtb");
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT,
			agesawrapper_post_device, NULL);
