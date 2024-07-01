/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <smbios.h>
#include <soc/ddr.h>
#include <soc/intel/common/smbios.h>
#include <soc/romstage.h>
#include <soc/util.h>
#include <spd.h>

void save_dimm_info(void)
{
	const struct SystemMemoryMapHob *hob = get_system_memory_map();
	assert(hob != NULL);

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	struct memory_info *mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (mem_info == NULL) {
		printk(BIOS_ERR, "CBMEM entry for DIMM info missing\n");
		return;
	}
	memset(mem_info, 0, sizeof(*mem_info));
	mem_info->max_capacity_mib = get_max_capacity_mib();
	mem_info->number_of_devices = CONFIG_DIMM_MAX;
	mem_info->ecc_type = get_error_correction_type(hob->RasModesEnabled);
	int dimm_max = ARRAY_SIZE(mem_info->dimm);

	uint32_t vdd_voltage = get_ddr_millivolt(hob->DdrVoltage);

	int slot_index = 0;
	const int max_dimm_count = get_max_dimm_count();
	int dimm_num = 0;
	for (int skt = 0; skt < CONFIG_MAX_SOCKET; skt++) {
		for (int ch = 0; ch < MAX_CH; ch++) {
			for (int dimm = 0; dimm < max_dimm_count; dimm++) {
				if (slot_index >= dimm_max) {
					printk(BIOS_WARNING, "Too many DIMMs info for %s.\n",
					       __func__);
					return;
				}

				struct dimm_info *dest_dimm;
				MEMMAP_DIMM_DEVICE_INFO_STRUCT src_dimm =
					hob->Socket[skt].ChannelInfo[ch].DimmInfo[dimm];

				if (src_dimm.Present) {
					dest_dimm = &mem_info->dimm[slot_index++];
					dest_dimm->dimm_size = (src_dimm.DimmSize << 6);
					dest_dimm->soc_num = skt;
					dest_dimm->channel_num = ch;
					dest_dimm->dimm_num = dimm;
				} else if (mainboard_dimm_slot_exists(skt, ch, dimm)) {
					dest_dimm = &mem_info->dimm[slot_index++];
					dest_dimm->dimm_size = 0;
					dest_dimm->soc_num = skt;
					dest_dimm->channel_num = ch;
					dest_dimm->dimm_num = dimm;
					continue;
				} else
					continue;

				uint8_t mem_dev_type = get_dram_type(hob);
				uint16_t data_width = 64;

				dimm_info_fill(
					dest_dimm, src_dimm.DimmSize << 6, mem_dev_type,
					hob->memFreq, /* replaced by configured_speed_mts */
					src_dimm.NumRanks,
					ch,   /* for mainboard locator string override */
					dimm, /* for mainboard locator string override */
					(const char *)&src_dimm.PartNumber[0],
					sizeof(src_dimm.PartNumber),
					(const uint8_t *)&src_dimm.serialNumber[0], data_width,
					vdd_voltage, true, /* hard-coded as ECC supported */
					src_dimm.VendorID, src_dimm.actKeyByte2, 0,
					get_max_memory_speed(src_dimm.commonTck));

				dimm_num++;
			}
		}
	}

	mem_info->dimm_cnt = slot_index; /* Number of DIMM slots found */
	printk(BIOS_DEBUG, "%d slots and %d installed DIMMs found\n", slot_index, dimm_num);
}
