/* SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * This code was adapted from src/soc/amd/common/block/pi/amd_late_init.c
 */

#include <fsp/util.h>
#include <memory_info.h>
#include <console/console.h>
#include <cbmem.h>
#include <string.h>
#include <ec/google/chromeec/ec.h>
#include <bootstate.h>
#include <lib.h>
#include <dimm_info_util.h>
#include <dmi_info.h>
#include <device/dram/ddr4.h>
#include <device/dram/lpddr4.h>

/**
 * Convert DDR clock speed (based on memory type) in MHz to the standard reported speed in MT/s
 */
static uint16_t ddr_speed_mhz_to_reported_mts(uint16_t ddr_type, uint16_t speed)
{
	switch (ddr_type) {
	case MEMORY_TYPE_DDR4:
		return ddr4_speed_mhz_to_reported_mts(speed);
	case MEMORY_TYPE_LPDDR4:
		return lpddr4_speed_mhz_to_reported_mts(speed);
	default:
		printk(BIOS_ERR, "Unknown memory type %x\n", ddr_type);
		return 0;
	}
}

/**
 * Populate dimm_info using AGESA TYPE17_DMI_INFO.
 */
static void transfer_memory_info(const TYPE17_DMI_INFO *dmi17,
				 struct dimm_info *dimm)
{
	hexstrtobin(dmi17->SerialNumber, dimm->serial, sizeof(dimm->serial));

	dimm->dimm_size = smbios_memory_size_to_mib(dmi17->MemorySize, dmi17->ExtSize);

	dimm->ddr_type = dmi17->MemoryType;

	dimm->configured_speed_mts = ddr_speed_mhz_to_reported_mts(
		dmi17->MemoryType, dmi17->ConfigSpeed);

	dimm->max_speed_mts = ddr_speed_mhz_to_reported_mts(dmi17->MemoryType, dmi17->Speed);

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
	printk(BIOS_DEBUG,
	       "CBMEM_ID_MEMINFO:\n"
	       "  dimm_size: %u\n"
	       "  ddr_type: 0x%hx\n"
	       "  ddr_frequency: %hu\n"
	       "  rank_per_dimm: %hhu\n"
	       "  channel_num: %hhu\n"
	       "  dimm_num: %hhu\n"
	       "  bank_locator: %hhu\n"
	       "  mod_id: %hx\n"
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
	       strlen((const char *)dimm->module_part_number),
	       (char *)dimm->module_part_number);
}

static void print_dmi_info(const TYPE17_DMI_INFO *dmi17)
{
	printk(BIOS_DEBUG,
	       "AGESA TYPE 17 DMI INFO:\n"
	       "  Handle: %hu\n"
	       "  TotalWidth: %hu\n"
	       "  DataWidth: %hu\n"
	       "  MemorySize: %hu\n"
	       "  DeviceSet: %hhu\n"
	       "  Speed: %hu\n"
	       "  ManufacturerIdCode: %llx\n"
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
	       strlen((const char *)dmi17->SerialNumber),
	       dmi17->SerialNumber,
	       strlen((const char *)dmi17->PartNumber),
	       dmi17->PartNumber);
}

/**
 * Marshalls dimm info from AMD_FSP_DMI_HOB into CBMEM_ID_MEMINFO
 */
static void prepare_dmi_17(void *unused)
{
	const DMI_INFO *dmi_table;
	const TYPE17_DMI_INFO *type17_dmi_info;
	struct memory_info *mem_info;
	struct dimm_info *dimm_info;
	char cbi_part_number[DIMM_INFO_PART_NUMBER_SIZE];
	bool use_cbi_part_number = false;
	size_t dimm_cnt = 0;
	size_t amd_fsp_dmi_hob_size;
	const EFI_GUID amd_fsp_dmi_hob_guid = AMD_FSP_DMI_HOB_GUID;

	printk(BIOS_DEBUG, "Saving dimm info for smbios type 17\n");

	/* Allocate meminfo in cbmem. */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(struct memory_info));
	if (!mem_info) {
		printk(BIOS_ERR,
		       "Failed to add memory info to CBMEM, DMI tables will be incomplete\n");
		return;
	}
	memset(mem_info, 0, sizeof(struct memory_info));

	/* Locate the memory info HOB. */
	dmi_table = fsp_find_extension_hob_by_guid(
		(const uint8_t *)&amd_fsp_dmi_hob_guid, &amd_fsp_dmi_hob_size);

	if (dmi_table == NULL || amd_fsp_dmi_hob_size == 0) {
		printk(BIOS_ERR,
		       "AMD_FSP_DMI_HOB not found, DMI table 17 will be incomplete\n");
		return;
	}
	printk(BIOS_DEBUG, "AMD_FSP_DMI_HOB found\n");

	if (CONFIG(EC_GOOGLE_CHROMEEC)) {
		/* Prefer DRAM part number from CBI. */
		if (google_chromeec_cbi_get_dram_part_num(
			cbi_part_number, sizeof(cbi_part_number)) == 0) {
			use_cbi_part_number = true;
		} else {
			printk(BIOS_ERR, "Could not obtain DRAM part number from CBI\n");
		}
	}

	for (unsigned int channel = 0; channel < MAX_CHANNELS_PER_SOCKET; channel++) {
		for (unsigned int dimm = 0; dimm < MAX_DIMMS_PER_CHANNEL; dimm++) {
			type17_dmi_info = &dmi_table->T17[0][channel][dimm];
			/* DIMMs that are present will have a non-zero
			   handle. */
			if (type17_dmi_info->Handle == 0)
				continue;
			print_dmi_info(type17_dmi_info);
			dimm_info = &mem_info->dimm[dimm_cnt];
			dimm_info->channel_num = channel;
			dimm_info->dimm_num = channel;
			transfer_memory_info(type17_dmi_info, dimm_info);
			if (use_cbi_part_number) {
				/* mem_info is memset to 0 above, so it's
				   safe to assume module_part_number will be
				   null terminated */
				strncpy((char *)dimm_info->module_part_number, cbi_part_number,
					sizeof(dimm_info->module_part_number) - 1);
			}
			print_dimm_info(dimm_info);
			dimm_cnt++;
		}
	}
	mem_info->dimm_cnt = dimm_cnt;
}

/* AMD_FSP_DMI_HOB is initialized very late, so check it just in time for writing tables. */
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, prepare_dmi_17, NULL);
