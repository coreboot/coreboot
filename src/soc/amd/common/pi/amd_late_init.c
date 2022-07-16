/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <dimm_info_util.h>
#include <memory_info.h>
#include <lib.h>
#include <string.h>

#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>

/**
 * Populate dimm_info using AGESA TYPE17_DMI_INFO.
 */
static void transfer_memory_info(TYPE17_DMI_INFO *dmi17,
				 struct dimm_info *dimm)
{
	hexstrtobin(dmi17->SerialNumber, dimm->serial, sizeof(dimm->serial));

	dimm->dimm_size =
	    smbios_memory_size_to_mib(dmi17->MemorySize, dmi17->ExtSize);

	dimm->ddr_type = dmi17->MemoryType;

	/*
	 * dimm_info uses ddr_frequency for setting both config speed and max
	 * speed. Lets use config speed so we don't get the false impression
	 * that the RAM is running faster than it actually is.
	 */
	dimm->ddr_frequency = dmi17->ConfigSpeed;

	dimm->rank_per_dimm = dmi17->Attributes;

	dimm->mod_type = smbios_form_factor_to_spd_mod_type(
		(smbios_memory_type)dmi17->MemoryType,
		(smbios_memory_form_factor)dmi17->FormFactor);

	dimm->bus_width = smbios_bus_width_to_spd_width(dmi17->MemoryType, dmi17->TotalWidth,
						dmi17->DataWidth);

	dimm->mod_id = dmi17->ManufacturerIdCode;

	dimm->bank_locator = 0;

	strncpy((char *)dimm->module_part_number, dmi17->PartNumber,
				sizeof(dimm->module_part_number) - 1);
}

static void print_dimm_info(const struct dimm_info *dimm)
{
	printk(RAM_SPEW,
	       "CBMEM_ID_MEMINFO:\n"
	       "  dimm_size: %u\n"
	       "  ddr_type: 0x%hx\n"
	       "  ddr_frequency: %hu\n"
	       "  rank_per_dimm: %hhu\n"
	       "  channel_num: %hhu\n"
	       "  dimm_num: %hhu\n"
	       "  bank_locator: %hhu\n"
	       "  mod_id: %hu\n"
	       "  mod_type: 0x%hhx\n"
	       "  bus_width: %hhu\n"
	       "  serial: %02hhx%02hhx%02hhx%02hhx\n"
	       "  module_part_number(%zu): %s\n",
	       dimm->dimm_size,
	       dimm->ddr_type,
	       dimm->ddr_frequency,
	       dimm->rank_per_dimm,
	       dimm->channel_num,
	       dimm->dimm_num,
	       dimm->bank_locator,
	       dimm->mod_id,
	       dimm->mod_type,
	       dimm->bus_width,
	       dimm->serial[0],
	       dimm->serial[1],
	       dimm->serial[2],
	       dimm->serial[3],
	       strlen((char *)dimm->module_part_number),
	       (char *)dimm->module_part_number
	);
}

static void print_dmi_info(const TYPE17_DMI_INFO *dmi17)
{
	printk(RAM_SPEW,
	       "AGESA TYPE 17 DMI INFO:\n"
	       "  Handle: %hu\n"
	       "  TotalWidth: %hu\n"
	       "  DataWidth: %hu\n"
	       "  MemorySize: %hu\n"
	       "  DeviceSet: %hhu\n"
	       "  Speed: %hu\n"
	       "  ManufacturerIdCode: %llu\n"
	       "  Attributes: %hhu\n"
	       "  ExtSize: %u\n"
	       "  ConfigSpeed: %hu\n"
	       "  MemoryType: 0x%x\n"
	       "  FormFactor: 0x%x\n"
	       "  DeviceLocator: %8s\n"
	       "  BankLocator: %10s\n"
	       "  SerialNumber(%zu): %9s\n"
	       "  PartNumber(%zu): %19s\n",
	       dmi17->Handle,
	       dmi17->TotalWidth,
	       dmi17->DataWidth,
	       dmi17->MemorySize,
	       dmi17->DeviceSet,
	       dmi17->Speed,
	       dmi17->ManufacturerIdCode,
	       dmi17->Attributes,
	       dmi17->ExtSize,
	       dmi17->ConfigSpeed,
	       dmi17->MemoryType,
	       dmi17->FormFactor,
	       dmi17->DeviceLocator,
	       dmi17->BankLocator,
	       strlen((char *)dmi17->SerialNumber),
	       dmi17->SerialNumber,
	       strlen((char *)dmi17->PartNumber),
	       dmi17->PartNumber
	);
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
				print_dmi_info(address);
				print_dimm_info(dimm);
				dimm_cnt++;
			}
		}
	}
	mem_info->dimm_cnt = dimm_cnt;
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, prepare_dmi_17, NULL);

static void agesawrapper_post_device(void *unused)
{
	if (acpi_is_wakeup_s3())
		return;

	do_agesawrapper(AMD_INIT_LATE, "amdinitlate");

	if (!acpi_s3_resume_allowed())
		return;

	do_agesawrapper(AMD_INIT_RTB, "amdinitrtb");
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, agesawrapper_post_device,
		      NULL);
