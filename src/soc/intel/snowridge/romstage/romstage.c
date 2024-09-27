/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/dram/common.h>
#include <device/dram/ddr4.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <fsp/api.h>
#include <fsp/soc_binding.h>
#include <fsp/util.h>
#include <intelblocks/imc.h>
#include <intelblocks/meminit.h>
#include <intelblocks/p2sblib.h>
#include <intelblocks/rtc.h>
#include <lib.h>
#include <romstage.h>
#include <soc/intel/common/smbios.h>
#include <soc/pci_devs.h>
#include <spd.h>
#include <spd_bin.h>
#include <string.h>

#include "../common/fsp_hob.h"
#include "../common/kti_cache.h"

#define SPD_DIMM_ADDR(ch, dimm) ((ch) * CONFIG_DIMMS_PER_CHANNEL + (dimm))

static void fsp_kti_cache_fill(FSPM_UPD *mupd)
{
	void *data;
	size_t kti_size;

	mupd->FspmConfig.PcdKtiBufferPtr = 0;

	data = kti_cache_load(&kti_size);
	if (data == NULL)
		return;

	mupd->FspmConfig.PcdKtiBufferPtr = (uint32_t)data;

	printk(BIOS_SPEW, "KTI cache found, size %zx bytes\n", kti_size);
	hexdump(data, kti_size);
}

static struct mem_channel_data mem_channel_data;

static void fsp_mem_spd_fill(FSPM_UPD *mupd)
{
	const struct soc_mem_cfg soc_mem_cfg = {
		.num_phys_channels = MRC_CHANNELS,
		.phys_to_mrc_map = {0, 1},
	};
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus[0] = {.addr_dimm[0] = SPD_DIMM_ADDR(0, 0),
			     .addr_dimm[1] = SPD_DIMM_ADDR(0, 1)},
		.smbus[1] = {.addr_dimm[0] = SPD_DIMM_ADDR(1, 0),
			     .addr_dimm[1] = SPD_DIMM_ADDR(1, 1)},
	};

	imc_spd_smbus_init(IMC_SPD_DEV);

	mem_populate_channel_data(mupd, &soc_mem_cfg, &spd_info, false, &mem_channel_data);

	printk(BIOS_SPEW, "SPD block length: 0x%zx\n", mem_channel_data.spd_len);

	mupd->FspmConfig.PcdMemSpdPtr = (uintptr_t)mem_channel_data.spd[0][0];
	printk(BIOS_SPEW, "PcdMemSpdPtr: 0x%08x\n", mupd->FspmConfig.PcdMemSpdPtr);

	mupd->FspmConfig.PcdDdrFreq = 0;
}

static void soc_memory_init_params(FSPM_UPD *mupd)
{
	fsp_kti_cache_fill(mupd);
	fsp_mem_spd_fill(mupd);

	mupd->FspmConfig.PcdVtdSupport = CONFIG(ENABLE_VTD);

	mupd->FspmConfig.PcdFiaMuxOverride = 1;
	mupd->FspmConfig.FiaMuxCfgInvalidate = 0;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	soc_memory_init_params(mupd);

	mainboard_memory_init_params(mupd);
}

static uint8_t spd_ddr_type_to_smbios_memory_type(uint16_t spd_type)
{
	switch (spd_type) {
	case SPD_MEMORY_TYPE_DDR4_SDRAM:
	case SPD_MEMORY_TYPE_DDR4E_SDRAM:
		return MEMORY_TYPE_DDR4;
	case SPD_MEMORY_TYPE_LPDDR4_SDRAM:
		return MEMORY_TYPE_LPDDR4;
	default:
		return MEMORY_TYPE_UNKNOWN;
	}
}

static void smbios_memory_info_save(void)
{
	const FSP_SMBIOS_MEMORY_INFO *fsp_smbios_memory_info = fsp_hob_get_memory_info();
	if (!fsp_smbios_memory_info) {
		printk(BIOS_WARNING, "Couldn't find SMBIOS memory info hob!\n");
		return;
	}

	struct memory_info *mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (!mem_info) {
		printk(BIOS_ERR, "Add memory info to cbmem failed!\n");
		return;
	}

	memset(mem_info, 0, sizeof(*mem_info));

	assert(fsp_smbios_memory_info->ChannelCount <=
	       ARRAY_SIZE(fsp_smbios_memory_info->ChannelInfo));

	uint8_t dimm_cnt = 0;
	for (uint8_t channel = 0; channel < fsp_smbios_memory_info->ChannelCount &&
				  channel < ARRAY_SIZE(fsp_smbios_memory_info->ChannelInfo) &&
				  dimm_cnt < ARRAY_SIZE(mem_info->dimm);
	     channel++) {
		const CHANNEL_INFO *channel_info =
			&fsp_smbios_memory_info->ChannelInfo[channel];

		assert(channel_info->DimmCount <= ARRAY_SIZE(channel_info->DimmInfo));

		for (uint8_t dimm = 0; dimm < channel_info->DimmCount &&
				       dimm < ARRAY_SIZE(channel_info->DimmInfo) &&
				       dimm_cnt < ARRAY_SIZE(mem_info->dimm);
		     dimm++) {
			const DIMM_INFO *fsp_dimm_info = &channel_info->DimmInfo[dimm];
			struct dimm_info *dimm_info = &mem_info->dimm[dimm_cnt];

			struct dimm_attr_ddr4_st dimm_attr;
			enum spd_status status = spd_decode_ddr4(
				&dimm_attr, (uint8_t *)mem_channel_data.spd[channel][dimm]);

			printk(BIOS_INFO, "MemoryType from fsp hob: 0x%02x\n",
			       fsp_smbios_memory_info->MemoryType);

			/* Populate the DIMM information */
			dimm_info_fill(dimm_info, fsp_dimm_info->SizeInMb,
				       spd_ddr_type_to_smbios_memory_type(
					       fsp_smbios_memory_info->MemoryType),
				       fsp_smbios_memory_info->MemoryFrequencyInMHz,
				       status == SPD_STATUS_OK ? dimm_attr.ranks : 0,
				       channel_info->ChannelId, fsp_dimm_info->DimmId,
				       (const char *)fsp_dimm_info->ModulePartNum,
				       sizeof(fsp_dimm_info->ModulePartNum),
				       status == SPD_STATUS_OK ? dimm_attr.serial_number : NULL,
				       fsp_smbios_memory_info->DataWidth,
				       status == SPD_STATUS_OK ? dimm_attr.vdd_voltage : 1200,
				       fsp_smbios_memory_info->ErrorCorrectionType,
				       fsp_dimm_info->MfgId,
				       status == SPD_STATUS_OK ? dimm_attr.dimm_type : 0, 0, 0);

			dimm_cnt++;
		}
	}

	mem_info->dimm_cnt = dimm_cnt;
	printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
}

/**
 * @brief Intel SoCs place this function in `src/soc`.
 */
void mainboard_romstage_entry(void)
{
	p2sb_dev_enable_bar(PCH_DEV_P2SB, CONFIG_PCR_BASE_ADDRESS);
	mainboard_config_gpios();

	fsp_memory_init(false);

	rtc_init();

	smbios_memory_info_save();
}
