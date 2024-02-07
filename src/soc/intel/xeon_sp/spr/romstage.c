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
#include <spd.h>
#include <soc/chip_common.h>
#include <soc/romstage.h>
#include <soc/pci_devs.h>
#include <static.h>
#include <string.h>
#include <soc/config.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <soc/ddr.h>
#include <IioPcieConfigUpd.h>

#include "chip.h"

/* Initialize to all zero first */
static UPD_IIO_PCIE_PORT_CONFIG spr_iio_bifur_table[MAX_SOCKET];
static UINT8 deemphasis_list[MAX_SOCKET * MAX_IIO_PORTS_PER_SOCKET];

void __weak mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Default weak implementation */
}

static void config_upd(FSPM_UPD *mupd)
{
	int cxl_mode = get_cxl_mode();
	if (cxl_mode == XEONSP_CXL_SYS_MEM || cxl_mode == XEONSP_CXL_SP_MEM)
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

	mupd->FspmConfig.IioPcieConfigTablePtr = (uintptr_t)spr_iio_bifur_table;
	/* MAX_SOCKET is the maximal number defined by FSP, currently is 4. */
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

	mupd->FspmConfig.DeEmphasisPtr = (uintptr_t)deemphasis_list;
	mupd->FspmConfig.DeEmphasisNumber = MAX_SOCKET * MAX_IIO_PORTS_PER_SOCKET;
	UINT8 *DeEmphasisConfig = (UINT8 *)deemphasis_list;

	for (port = 0; port < MAX_SOCKET * MAX_IIO_PORTS_PER_SOCKET; port++)
		DeEmphasisConfig[port] = 0x1;
}

void soc_config_iio(FSPM_UPD *mupd, const UPD_IIO_PCIE_PORT_CONFIG_ENTRY
	mb_iio_table[CONFIG_MAX_SOCKET][IIO_PORT_SETTINGS], const UINT8 mb_iio_bifur[CONFIG_MAX_SOCKET][5])
{
	UPD_IIO_PCIE_PORT_CONFIG *PciePortConfig;
	int port, socket;

	PciePortConfig =
		(UPD_IIO_PCIE_PORT_CONFIG *)(UINTN)mupd->FspmConfig.IioPcieConfigTablePtr;
	mupd->FspmConfig.IioPcieConfigTableNumber = CONFIG_MAX_SOCKET; /* Set by mainboard */

	for (socket = 0; socket < CONFIG_MAX_SOCKET; socket++) {
		/* Configures DMI, IOU0 ~ IOU4 */
		for (port = 0; port < IIO_PORT_SETTINGS; port++) {
			const UPD_IIO_PCIE_PORT_CONFIG_ENTRY *port_cfg =
				&mb_iio_table[socket][port];
			PciePortConfig[socket].SLOTIMP[port] = port_cfg->SLOTIMP;
			PciePortConfig[socket].SLOTPSP[port] = port_cfg->SLOTPSP;
			PciePortConfig[socket].SLOTHPCAP[port] = port_cfg->SLOTHPCAP;
			PciePortConfig[socket].SLOTHPSUP[port] = port_cfg->SLOTHPSUP;
			PciePortConfig[socket].SLOTSPLS[port] = port_cfg->SLOTSPLS;
			PciePortConfig[socket].SLOTSPLV[port] = port_cfg->SLOTSPLV;
			PciePortConfig[socket].VppAddress[port] = port_cfg->VppAddress;
			PciePortConfig[socket].SLOTPIP[port] = port_cfg->SLOTPIP;
			PciePortConfig[socket].SLOTAIP[port] = port_cfg->SLOTAIP;
			PciePortConfig[socket].SLOTMRLSP[port] = port_cfg->SLOTMRLSP;
			PciePortConfig[socket].SLOTPCP[port] = port_cfg->SLOTPCP;
			PciePortConfig[socket].SLOTABP[port] = port_cfg->SLOTABP;
			PciePortConfig[socket].VppEnabled[port] = port_cfg->VppEnabled;
			PciePortConfig[socket].VppPort[port] = port_cfg->VppPort;
			PciePortConfig[socket].MuxAddress[port] = port_cfg->MuxAddress;
			PciePortConfig[socket].PciePortEnable[port] = port_cfg->PciePortEnable;
			PciePortConfig[socket].PEXPHIDE[port] = port_cfg->PEXPHIDE;
			PciePortConfig[socket].PcieHotPlugOnPort[port] = port_cfg->PcieHotPlugOnPort;
			PciePortConfig[socket].PcieMaxPayload[port] = port_cfg->PcieMaxPayload;
			PciePortConfig[socket].PciePortLinkSpeed[port] = port_cfg->PciePortLinkSpeed;
			PciePortConfig[socket].DfxDnTxPresetGen3[port] = port_cfg->DfxDnTxPresetGen3;
			PciePortConfig[socket].HidePEXPMenu[port] = port_cfg->HidePEXPMenu;
		}

		/* Socket IOU5 ~ IOU6 are not used, set PEXPHIDE and HidePEXPMenu to 1 */
		for (port = IIO_PORT_SETTINGS; port < MAX_IIO_PORTS_PER_SOCKET;
		     port++) {
			PciePortConfig[socket].PEXPHIDE[port] = 1;
			PciePortConfig[socket].HidePEXPMenu[port] = 1;
		}
		/* Configure IOU0 ~ IOU4 bifurcation */
		for (port = 0; port < 5; port++)
			PciePortConfig[socket].ConfigIOU[port] = mb_iio_bifur[socket][port];
	}
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

	/*
	 * By default FSP will set MMCFG size to 256 buses on 1S and 2S platforms
	 * and 512 buses on 4S platforms. 512 buses are implemented by using multiple
	 * PCI segment groups and is likely incompatible with legacy software stacks.
	 */
	switch (CONFIG_ECAM_MMCONF_BUS_NUMBER) {
	case 2048:
		m_cfg->mmCfgSize = 5;
		break;
	case 1024:
		m_cfg->mmCfgSize = 4;
		break;
	case  512:
		m_cfg->mmCfgSize = 3;
		break;
	case  256:
		m_cfg->mmCfgSize = 2;
		break;
	case  128:
		m_cfg->mmCfgSize = 1;
		break;
	case   64:
		m_cfg->mmCfgSize = 0;
		break;
	default:
		printk(BIOS_ERR, "%s: Unsupported ECAM_MMCONF_BUS_NUMBER = %d\n",
			__func__, CONFIG_ECAM_MMCONF_BUS_NUMBER);
	}

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

	config_upd(mupd);
	initialize_iio_upd(mupd);
	mainboard_memory_init_params(mupd);

	if (CONFIG(ENABLE_IO_MARGINING)) {
		printk(BIOS_INFO, "IO Margining Enabled.\n");
		/* Needed for IO Margining */
		mupd->FspmConfig.DFXEnable = 1;

		UPD_IIO_PCIE_PORT_CONFIG *iio_pcie_cfg;
		int socket;

		iio_pcie_cfg = (UPD_IIO_PCIE_PORT_CONFIG *)(uintptr_t)mupd->FspmConfig.IioPcieConfigTablePtr;

		for (socket = 0; socket < MAX_SOCKET; socket++)
			iio_pcie_cfg[socket].PcieGlobalAspm = 0;

		mupd->FspmConfig.KtiLinkL1En = 0;
		mupd->FspmConfig.KtiLinkL0pEn = 0;
	}

	if (CONFIG(ENABLE_RMT)) {
		printk(BIOS_INFO, "RMT Enabled.\n");
		mupd->FspmConfig.EnableRMT = 0x1;
		/* Set FSP debug message to Max for RMT logs */
		mupd->FspmConfig.serialDebugMsgLvl = 0x3;
		mupd->FspmConfig.AllowedSocketsInParallel = 0x1;
		mupd->FspmConfig.EnforcePopulationPor = 0x1;
	}
	if (CONFIG(MEM_POR_FREQ))
		mupd->FspmConfig.EnforceDdrMemoryFreqPor = 0x0;
}

uint8_t get_error_correction_type(const uint8_t RasModesEnabled)
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

uint8_t get_max_dimm_count(void)
{
	return MAX_DIMM;
}

uint8_t get_dram_type(const struct SystemMemoryMapHob *hob)
{
	if (hob->DramType == SPD_MEMORY_TYPE_DDR5_SDRAM)
		return MEMORY_TYPE_DDR5;

	return MEMORY_TYPE_DDR4;
}

uint32_t get_max_capacity_mib(void)
{
	/* According to EDS doc#611488, it's 4 TB per processor. */
	return 4 * MiB * CONFIG_MAX_SOCKET;
}
