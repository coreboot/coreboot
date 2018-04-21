/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 - 2017 Intel Corp.
 * Copyright (C) 2017 Online SAS.
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

#include <cbmem.h>
#include <compiler.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <reset.h>
#include <soc/fiamux.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pcr.h>
#include <soc/pmc.h>
#include <soc/romstage.h>
#include <soc/smbus.h>
#include <soc/smm.h>
#include <soc/soc_util.h>

void __weak mainboard_config_gpios(void) {}

#define FSP_SMBIOS_MEMORY_INFO_GUID	\
{	\
	0x8c, 0x10, 0xa1, 0x01, 0xee, 0x9d, 0x84, 0x49,	\
	0x88, 0xc3, 0xee, 0xe8, 0xc4, 0x9e, 0xfb, 0x89	\
}

#if IS_ENABLED(CONFIG_DISPLAY_HOBS)
static void display_fsp_smbios_memory_info_hob(void)
{
	int channel, dimm;
	size_t hob_size;
	const DIMM_INFO *dimm_info;
	const CHANNEL_INFO *channel_info;
	const FSP_SMBIOS_MEMORY_INFO *memory_info_hob;
	const uint8_t smbios_memory_info_guid[16] =
			FSP_SMBIOS_MEMORY_INFO_GUID;

	/* Locate the memory info HOB */
	memory_info_hob = fsp_find_extension_hob_by_guid(
				smbios_memory_info_guid,
				&hob_size);

	if (memory_info_hob == NULL || hob_size == 0) {
		printk(BIOS_ERR, "SMBIOS MEMORY_INFO_DATA_HOB not found\n");
		return;
	}

	/* Display the data in the FSP_SMBIOS_MEMORY_INFO HOB */
	printk(BIOS_DEBUG, "FSP_SMBIOS_MEMORY_INFO HOB\n");
	printk(BIOS_DEBUG, "    0x%02x: Revision\n",
		memory_info_hob->Revision);
	printk(BIOS_DEBUG, "    0x%02x: MemoryType\n",
		memory_info_hob->MemoryType);
	printk(BIOS_DEBUG, "    %d: MemoryFrequencyInMHz\n",
		memory_info_hob->MemoryFrequencyInMHz);
	printk(BIOS_DEBUG, "    %d: DataWidth in bits\n",
		memory_info_hob->DataWidth);
	printk(BIOS_DEBUG, "    0x%02x: ErrorCorrectionType\n",
		memory_info_hob->ErrorCorrectionType);
	printk(BIOS_DEBUG, "    0x%02x: ChannelCount\n",
		memory_info_hob->ChannelCount);
	for (channel = 0; channel < memory_info_hob->ChannelCount;
		channel++) {
		channel_info = &memory_info_hob->ChannelInfo[channel];
		printk(BIOS_DEBUG, "  Channel %d\n", channel);
		printk(BIOS_DEBUG, "      0x%02x: ChannelId\n",
			channel_info->ChannelId);
		printk(BIOS_DEBUG, "      0x%02x: DimmCount\n",
			channel_info->DimmCount);
		for (dimm = 0; dimm < channel_info->DimmCount;
			dimm++) {
			dimm_info = &channel_info->DimmInfo[dimm];
			printk(BIOS_DEBUG, "   DIMM %d\n", dimm);
			printk(BIOS_DEBUG, "      0x%02x: DimmId\n",
				dimm_info->DimmId);
			printk(BIOS_DEBUG, "      %d: SizeInMb\n",
				dimm_info->SizeInMb);
			printk(BIOS_DEBUG, "    0x%04x: MfgId\n",
				dimm_info->MfgId);
			printk(BIOS_DEBUG, "%*.*s: ModulePartNum\n",
				(int)sizeof(dimm_info->ModulePartNum),
				(int)sizeof(dimm_info->ModulePartNum),
				dimm_info->ModulePartNum);
		}
	}
}
#endif

static void early_pmc_init(void)
{
	/* PMC (B0:D31:F2). */
	device_t dev = PCH_PMC_DEV;

	/* Is PMC present */
	if (pci_read_config16(dev, 0) == 0xffff) {
		printk(BIOS_ERR, "PMC controller (B0:D31:F2) does not present!\n");
		return;
	}

	uint32_t pwrm_base =
		pci_read_config32(dev, PMC_PWRM_BASE) & MASK_PMC_PWRM_BASE;
	if (!pwrm_base) {
		printk(BIOS_ERR, "PWRM base address is not configured!\n");
		return;
	}

	/* Workaround for sighting report (doc#: 560805) v1.86.
	   42. System Might Hang In AC Power Loss
	   Problem :
	       When removing and reapplying AC power to the board,
	       the system might hang at serial output
	       'RESET required : change of frequency'
	       due to PMC ROM change on B0.
	   Implication :
	       1. This issue is only shown in B0 stepping.
	       2. This issue does not impact a system without an RTC battery.
	   Alternative workaround :
	       Remove RTC battery on the board if possible.
	   Status : Plan Fix.
	*/
	if (silicon_stepping() == SILICON_REV_DENVERTON_B0) {
		if (!(pci_read_config32(dev, PMC_GEN_PMCON_B)
					& PMC_GEN_PMCON_B_RTC_PWR_STS)) {
			if (read32((void *)(pwrm_base + 0x124))
				   & ((1 << 11) | (1 << 12))) {
				/* Performs a global reset */
				printk(BIOS_DEBUG,
					"Requesting Global Reset...\n");
				pci_write_config32(dev, PMC_ETR3,
					pci_read_config32(dev, PMC_ETR3)
					| PMC_ETR3_CF9GR);
				hard_reset();
			}
		}
	}
}

static void early_tco_init(void)
{
	/* SMBUS (B0:D31:F4). */
	device_t dev = PCI_DEV(0, SMBUS_DEV, SMBUS_FUNC);

	/* Configure TCO base address */
	if (pci_read_config16(dev, TCOBASE) == 0xffff) {
		printk(BIOS_ERR, "SMBus controller (B0:D31:F4) does not present!\n");
		return;
	}
	uint16_t tco_ctl = pci_read_config16(dev, TCOCTL);
	if (tco_ctl & TCOBASE_LOCK) {
		printk(BIOS_ERR, "TCO base register already has been locked!\n");
	} else {
		pci_write_config16(dev, TCOCTL, tco_ctl & (~TCOBASE_EN));
		pci_write_config16(dev, TCOBASE, DEFAULT_TCO_BASE | 0x1);
		pci_write_config16(dev, TCOCTL, tco_ctl | TCOBASE_EN);
	}

	uint16_t tco_base = pci_read_config16(dev, TCOBASE) & MASK_TCOBASE;
	printk(BIOS_DEBUG, "TCO base address set to 0x%x!\n", tco_base);

	/* Disable the TCO timer expiration from causing a system reset */
	MMIO32_OR(PCH_PCR_ADDRESS(PID_SMB, PCR_SMBUS_GC),
		(uint32_t)PCR_SMBUS_GC_NR);

	/*  Halt the TCO timer */
	uint16_t reg16 = inw(tco_base + TCO1_CNT);
	reg16 |= TCO_TMR_HLT;
	outw(reg16, tco_base + TCO1_CNT);

	/* Clear the Second TCO status bit */
	reg16 = inw(tco_base + TCO2_STS);
	reg16 |= TCO2_STS_SECOND_TO;
	outw(reg16, tco_base + TCO2_STS);
}

asmlinkage void car_stage_entry(void)
{

	struct postcar_frame pcf;
	uintptr_t top_of_ram;

#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	void *smm_base;
	size_t smm_size;
	uintptr_t tseg_base;
#endif

	console_init();

	printk(BIOS_DEBUG, "FSP TempRamInit was successful...\n");

	mainboard_config_gpios();
	early_tco_init();
	early_pmc_init();

	fsp_memory_init(false);

#if IS_ENABLED(CONFIG_DISPLAY_HOBS)
	display_fsp_smbios_memory_info_hob();
#endif

	if (postcar_frame_init(&pcf, 1 * KiB))
		die("Unable to initialize postcar frame.\n");

	/*
	 * We need to make sure ramstage will be run cached. At this point exact
	 * location of ramstage in cbmem is not known. Instruct postcar to cache
	 * 16 megs under cbmem top which is a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t)cbmem_top();
	postcar_frame_add_mtrr(&pcf, top_of_ram - 16 * MiB, 16 * MiB,
			       MTRR_TYPE_WRBACK);

	/* Cache the memory-mapped boot media. */
	if (IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED))
		postcar_frame_add_mtrr(&pcf, -CONFIG_ROM_SIZE, CONFIG_ROM_SIZE,
				       MTRR_TYPE_WRPROT);
#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	/*
	 * Cache the TSEG region at the top of ram. This region is
	 * not restricted to SMM mode until SMM has been relocated.
	 * By setting the region to cacheable it provides faster access
	 * when relocating the SMM handler as well as using the TSEG
	 * region for other purposes.
	 */
	smm_region(&smm_base, &smm_size);
	tseg_base = (uintptr_t)smm_base;
	postcar_frame_add_mtrr(&pcf, tseg_base, smm_size, MTRR_TYPE_WRBACK);
#endif

	run_postcar_phase(&pcf);
}

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg)
{
	FSPM_UPD *mupd = container_of(m_cfg, FSPM_UPD, FspmConfig);
	size_t num;
	uint16_t supported_hsio_lanes;
	BL_HSIO_INFORMATION *hsio_config;

	/* Set the parameters for MemoryInit */
	m_cfg->PcdEnableIQAT = IS_ENABLED(CONFIG_IQAT_ENABLE);

	/* if ME HECI communication is disabled, apply default one*/
	if (mupd->FspmConfig.PcdMeHeciCommunication == 0) {

		/* Configure FIA MUX PCD */
		/* Assume the validating silicon has max lanes. */
		supported_hsio_lanes = BL_ME_FIA_MUX_LANE_NUM_MAX;

		num = mainboard_get_hsio_config(&hsio_config);

		if (get_fiamux_hsio_info(supported_hsio_lanes, num,
					 &hsio_config))
			die("HSIO Configuration is invalid, please correct "
			    "it!");

		/* Check the requested FIA MUX Configuration */
		if (!(&hsio_config->FiaConfig)) {
			die("Requested FIA MUX Configuration is invalid,"
			    " please correct it!");
		}

		mupd->FspmConfig.PcdHsioLanesNumber =
		    (uint32_t)hsio_config->NumLanesSupported;
		mupd->FspmConfig.PcdFiaMuxConfigPtr =
		    (uint32_t)&hsio_config->FiaConfig;
	}
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Do nothing */
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	soc_memory_init_params(m_cfg);

	mainboard_memory_init_params(mupd);
}
