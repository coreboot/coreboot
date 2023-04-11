/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cpu/cpu.h>
#include <cpu/intel/cpu_ids.h>
#include <arch/romstage.h>
#include <console/console.h>
#include <cbmem.h>
#include <drivers/vpd/vpd.h>
#include <drivers/ocp/include/vpd.h>
#include <security/intel/txt/txt.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <soc/romstage.h>
#include <soc/pci_devs.h>
#include <soc/intel/common/smbios.h>
#include <string.h>
#include <soc/soc_util.h>
#include <soc/ddr.h>

#include "chip.h"

/* Initialize to all zero first */
static UPD_IIO_PCIE_PORT_CONFIG spr_iio_bifur_table[MAX_SOCKET];
static UINT8 deemphasis_list[MAX_SOCKET * MAX_IIO_PORTS_PER_SOCKET];

void __weak mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Default weak implementation */
}

bool __weak mainboard_dimm_slot_exists(uint8_t socket, uint8_t channel, uint8_t dimm)
{
	return false;
}

/*
 * Search from VPD_RW first then VPD_RO for UPD config variables,
 * overwrites them from VPD if it's found.
 */
static void config_upd_from_vpd(FSPM_UPD *mupd)
{
	uint8_t val;
	int val_int, cxl_mode;

	/* Send FSP log message to SOL */
	if (vpd_get_bool(FSP_LOG, VPD_RW_THEN_RO, &val))
		mupd->FspmConfig.SerialIoUartDebugEnable = val;
	else {
		printk(BIOS_INFO,
		       "Not able to get VPD %s, default set "
		       "SerialIoUartDebugEnable to %d\n",
		       FSP_LOG, FSP_LOG_DEFAULT);
		mupd->FspmConfig.SerialIoUartDebugEnable = FSP_LOG_DEFAULT;
	}

	if (mupd->FspmConfig.SerialIoUartDebugEnable) {
		/* FSP memory debug log level */
		if (vpd_get_int(FSP_MEM_LOG_LEVEL, VPD_RW_THEN_RO, &val_int)) {
			if (val_int < 0 || val_int > 4) {
				printk(BIOS_DEBUG,
				       "Invalid serialDebugMsgLvl value from VPD: "
				       "%d\n",
				       val_int);
				val_int = FSP_MEM_LOG_LEVEL_DEFAULT;
			}
			printk(BIOS_DEBUG, "Setting serialDebugMsgLvl to %d\n", val_int);
			mupd->FspmConfig.serialDebugMsgLvl = (uint8_t)val_int;
		} else {
			printk(BIOS_INFO,
			       "Not able to get VPD %s, default set "
			       "DebugPrintLevel to %d\n",
			       FSP_MEM_LOG_LEVEL, FSP_MEM_LOG_LEVEL_DEFAULT);
			mupd->FspmConfig.serialDebugMsgLvl = FSP_MEM_LOG_LEVEL_DEFAULT;
		}
		/* If serialDebugMsgLvl less than 1, disable FSP memory train results */
		if (mupd->FspmConfig.serialDebugMsgLvl <= 1) {
			printk(BIOS_DEBUG, "Setting serialDebugMsgLvlTrainResults to 0\n");
			mupd->FspmConfig.serialDebugMsgLvlTrainResults = 0x0;
		}
	}

	/* FSP Dfx PMIC Secure mode */
	if (vpd_get_int(FSP_PMIC_SECURE_MODE, VPD_RW_THEN_RO, &val_int)) {
		if (val_int < 0 || val_int > 2) {
			printk(BIOS_DEBUG,
			       "Invalid PMIC secure mode value from VPD: "
			       "%d\n",
			       val_int);
			val_int = FSP_PMIC_SECURE_MODE_DEFAULT;
		}
		printk(BIOS_DEBUG, "Setting PMIC secure mode to %d\n", val_int);
		mupd->FspmConfig.DfxPmicSecureMode = (uint8_t)val_int;
	} else {
		printk(BIOS_INFO,
		       "Not able to get VPD %s, default set "
		       "PMIC secure mode to %d\n",
		       FSP_PMIC_SECURE_MODE, FSP_PMIC_SECURE_MODE_DEFAULT);
		mupd->FspmConfig.DfxPmicSecureMode = FSP_PMIC_SECURE_MODE_DEFAULT;
	}

	cxl_mode = get_cxl_mode_from_vpd();
	if (cxl_mode == CXL_SYSTEM_MEMORY || cxl_mode == CXL_SPM)
		mupd->FspmConfig.DfxCxlType3LegacyEn = 1;
	else /* Disable CXL */
		mupd->FspmConfig.DfxCxlType3LegacyEn = 0;

	if (CONFIG(INTEL_TXT)) {
		/* Configure for error injection test */
		mupd->FspmConfig.DFXEnable = skip_intel_txt_lockdown() ? 1 : 0;
	}
}

/* Initialize non-zero default UPD values for IIO */
static void initialize_iio_upd(FSPM_UPD *mupd)
{
	unsigned int port, socket;

	mupd->FspmConfig.IioPcieConfigTablePtr = (UINT32)spr_iio_bifur_table;
	mupd->FspmConfig.IioPcieConfigTableNumber = MAX_SOCKET;
	UPD_IIO_PCIE_PORT_CONFIG *PciePortConfig =
		(UPD_IIO_PCIE_PORT_CONFIG *)spr_iio_bifur_table;

	/* Initialize non-zero default UPD values */
	for (socket = 0; socket < MAX_SOCKET; socket++) {
		for (port = 0; port < MAX_IIO_PORTS_PER_SOCKET; port++) {
			PciePortConfig[socket].PcieMaxPayload[port] = 0x7;     /* Auto */
			PciePortConfig[socket].DfxDnTxPresetGen3[port] = 0xff; /* Auto */
		}
		PciePortConfig[socket].PcieGlobalAspm = 0x1; /* Enable ASPM */
		PciePortConfig[socket].PcieMaxReadRequestSize = 0x5;
	}

	mupd->FspmConfig.DeEmphasisPtr = (UINT32)deemphasis_list;
	mupd->FspmConfig.DeEmphasisNumber = MAX_SOCKET * MAX_IIO_PORTS_PER_SOCKET;
	UINT8 *DeEmphasisConfig = (UINT8 *)deemphasis_list;

	for (port = 0; port < MAX_SOCKET * MAX_IIO_PORTS_PER_SOCKET; port++)
		DeEmphasisConfig[port] = 0x1;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	const config_t *config = config_of_soc();

	m_cfg->DebugPrintLevel = 0xF;

	m_cfg->DirectoryModeEn = 0x2;
	const u8 KtiFpgaEnable[] = {0x2, 0x2, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1};
	memcpy(m_cfg->KtiFpgaEnable, KtiFpgaEnable, sizeof(m_cfg->KtiFpgaEnable));

	m_cfg->TscSyncEn = 0x1;

	m_cfg->mmiohBase = 0x2000;
	m_cfg->mmiohSize = 0x3;

	m_cfg->BoardTypeBitmask = 0x11111133;

	/*
	 * Let coreboot configure LAPIC based on Kconfig.
	 * coreboot currently can only switch from XAPIC to X2APIC,
	 * so always select XAPIC mode here.
	 */
	m_cfg->X2apic = 0;

	m_cfg->serialDebugMsgLvl = 0x3;

	m_cfg->VtdSupport = config->vtd_support;

	m_cfg->SerialIoUartDebugIoBase = CONFIG_TTYS0_BASE;

	mupd->FspmConfig.AttemptFastBoot = 1;
	mupd->FspmConfig.AttemptFastBootCold = 1;

	/* Set Patrol Scrub UPD */
	mupd->FspmConfig.PatrolScrubNotify = 0x1;  /* 1:Enable at ReadyToBootFsp() */
	mupd->FspmConfig.PatrolScrub = 0x2;	   /* 2:Enable during
				    NotifyPhase(EnumInitPhaseReadyToBoot) */
	mupd->FspmConfig.ErrorCheckScrub = 1;	   /* Enable/Disable DDR5 Error Check
				    and Scrub (ECS) in FSP */
	mupd->FspmConfig.PatrolScrubAddrMode = 1;  /* 1:System Physical Address */
	mupd->FspmConfig.PatrolScrubDuration = 24; /* unit is hour */

	/* Disable below UPDs because those features should be implemented by coreboot */
	mupd->FspmConfig.LockChipset = 0;
	mupd->FspmConfig.ProcessorMsrLockControl = 0;
	/* Don't set and signal MSR_BIOS_DONE in FSP since it should be done by coreboot */
	mupd->FspmConfig.DfxDisableBiosDone = 1;

	u32 cpu_id = cpu_get_cpuid();
	if (cpu_id == (u32)CPUID_SAPPHIRERAPIDS_SP_D) {
		printk(BIOS_DEBUG, "CPU is D stepping, setting package C state to C0/C1\n");
		mupd->FspmConfig.CpuPmPackageCState = 0;
	}
	/* Set some common UPDs from VPD, mainboard can still override them if needed */
	if (CONFIG(VPD))
		config_upd_from_vpd(mupd);
	initialize_iio_upd(mupd);
	mainboard_memory_init_params(mupd);
}

static uint8_t get_error_correction_type(const uint8_t RasModesEnabled)
{
	switch (RasModesEnabled) {
	case CH_INDEPENDENT:
		return MEMORY_ARRAY_ECC_SINGLE_BIT;
	case FULL_MIRROR_1LM:
	case PARTIAL_MIRROR_1LM:
	case FULL_MIRROR_2LM:
	case PARTIAL_MIRROR_2LM:
		return MEMORY_ARRAY_ECC_MULTI_BIT;
	case RK_SPARE:
		return MEMORY_ARRAY_ECC_SINGLE_BIT;
	case CH_LOCKSTEP:
		return MEMORY_ARRAY_ECC_SINGLE_BIT;
	default:
		return MEMORY_ARRAY_ECC_MULTI_BIT;
	}
}

/* Save the DIMM information for SMBIOS table 17 */
void save_dimm_info(void)
{
	struct dimm_info *dest_dimm;
	struct memory_info *mem_info;
	const struct SystemMemoryMapHob *hob;
	MEMMAP_DIMM_DEVICE_INFO_STRUCT src_dimm;
	int dimm_max, dimm_num = 0;
	int index = 0;
	uint8_t mem_dev_type;
	uint16_t data_width;
	uint32_t vdd_voltage;

	hob = get_system_memory_map();
	assert(hob != NULL);

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (mem_info == NULL) {
		printk(BIOS_ERR, "CBMEM entry for DIMM info missing\n");
		return;
	}
	memset(mem_info, 0, sizeof(*mem_info));
	/* According to EDS doc#611488, it's 4 TB per processor. */
	mem_info->max_capacity_mib = 4 * MiB * CONFIG_MAX_SOCKET;
	mem_info->number_of_devices = CONFIG_DIMM_MAX;
	mem_info->ecc_type = get_error_correction_type(hob->RasModesEnabled);
	dimm_max = ARRAY_SIZE(mem_info->dimm);
	vdd_voltage = get_ddr_millivolt(hob->DdrVoltage);
	for (int soc = 0; soc < CONFIG_MAX_SOCKET; soc++) {
		for (int ch = 0; ch < MAX_CH; ch++) {
			for (int dimm = 0; dimm < MAX_DIMM; dimm++) {
				if (index >= dimm_max) {
					printk(BIOS_WARNING, "Too many DIMMs info for %s.\n",
					       __func__);
					return;
				}

				src_dimm = hob->Socket[soc].ChannelInfo[ch].DimmInfo[dimm];
				if (src_dimm.Present) {
					dest_dimm = &mem_info->dimm[index];
					index++;
				} else if (mainboard_dimm_slot_exists(soc, ch, dimm)) {
					dest_dimm = &mem_info->dimm[index];
					index++;
					/* Save DIMM Locator information for SMBIOS Type 17 */
					dest_dimm->dimm_size = 0;
					dest_dimm->soc_num = soc;
					dest_dimm->channel_num = ch;
					dest_dimm->dimm_num = dimm;
					continue;
				} else {
					/* Ignore DIMM that isn't present and doesn't exist on
					   the board. */
					continue;
				}
				dest_dimm->max_speed_mts =
					get_max_memory_speed(src_dimm.commonTck);
				dest_dimm->configured_speed_mts = hob->memFreq;
				dest_dimm->soc_num = soc;

				if (hob->DramType == SPD_TYPE_DDR5) {
					/* hard-coded memory device type as DDR5 */
					mem_dev_type = 0x22;
					data_width = 64;
				} else {
					/* hard-coded memory device type as DDR4 */
					mem_dev_type = 0x1A;
					data_width = 64;
				}
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
					src_dimm.VendorID, src_dimm.actKeyByte2, 0);
				dimm_num++;
			}
		}
	}

	mem_info->dimm_cnt = index; /* Number of DIMM slots found */
	printk(BIOS_DEBUG, "%d Installed DIMMs found\n", dimm_num);
}
