/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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
#define __SIMPLE_DEVICE__

#include <arch/early_variables.h>
#include <console/console.h>
#include "../chip.h"
#include <fsp/memmap.h>
#include <fsp/util.h>
#include <soc/pci_devs.h>
#include <soc/QuarkNcSocId.h>
#include <soc/romstage.h>
#include <string.h>

extern void asmlinkage light_sd_led(void);

asmlinkage void *car_stage_c_entry(void)
{
	FSP_INFO_HEADER *fih;
	struct cache_as_ram_params car_params = {0};
	void *top_of_stack;

	post_code(0x20);

	/* Copy the FSP binary into ESRAM */
	memcpy((void *)CONFIG_FSP_ESRAM_LOC, (void *)CONFIG_FSP_LOC,
		0x00040000);

	/* Locate the FSP header in ESRAM */
	fih = find_fsp(CONFIG_FSP_ESRAM_LOC);
	if (IS_ENABLED(CONFIG_ENABLE_DEBUG_LED_FINDFSP))
		light_sd_led();

	/* Start the early verstage/romstage code */
	post_code(0x2A);
	car_params.fih = fih;
	top_of_stack = cache_as_ram_main(&car_params);

	/* Initialize MTRRs and switch stacks after RAM initialized */
	return top_of_stack;
}

static struct chipset_power_state power_state CAR_GLOBAL;

struct chipset_power_state *get_power_state(void)
{
	return (struct chipset_power_state *)car_get_var_ptr(&power_state);
}

struct chipset_power_state *fill_power_state(void)
{
	struct chipset_power_state *ps = get_power_state();

	ps->prev_sleep_state = 0;
	printk(BIOS_SPEW, "prev_sleep_state %d\n", ps->prev_sleep_state);
	return ps;
}

size_t mmap_region_granularity(void)
{
	/* Align to 8 MiB by default */
	return 8 << 20;
}

/* Initialize the UPD parameters for MemoryInit */
void soc_memory_init_params(struct romstage_params *params,
			    MEMORY_INIT_UPD *upd)
{
	const struct device *dev;
	const struct soc_intel_quark_config *config;
	void *rmu_data;
	size_t rmu_data_len;

	/* Locate the configuration data from devicetree.cb */
	dev = dev_find_slot(0, LPC_DEV_FUNC);
	if (!dev) {
		printk(BIOS_CRIT,
			"Error! Device (PCI:0:%02x.%01x) not found, "
			"soc_memory_init_params!\n", PCI_DEVICE_NUMBER_QNC_LPC,
			PCI_FUNCTION_NUMBER_QNC_LPC);
		return;
	}
	config = dev->chip_info;

	/* Clear SMI and wake events */
	clear_smi_and_wake_events();

	/* Locate the RMU data file in flash */
	rmu_data = locate_rmu_file(&rmu_data_len);
	if (!rmu_data)
		die("Microcode file (rmu.bin) not found.");

	/* Display the ESRAM layout */
	if (IS_ENABLED(CONFIG_DISPLAY_ESRAM_LAYOUT)) {
		printk(BIOS_SPEW, "\nESRAM Layout:\n\n");
		printk(BIOS_SPEW,
			"+-------------------+ 0x80080000 - ESRAM end\n");
		if (_car_relocatable_data_end != (void *)0x80080000) {
			printk(BIOS_SPEW, "|                   |\n");
			printk(BIOS_SPEW, "+-------------------+ 0x%p\n",
				_car_relocatable_data_end);
		}
		printk(BIOS_SPEW, "| coreboot data     |\n");
		printk(BIOS_SPEW, "+-------------------+ 0x%p\n",
			_car_stack_end);
		printk(BIOS_SPEW, "| coreboot stack    |\n");
		printk(BIOS_SPEW, "+-------------------+ 0x%p",
			_car_stack_start);
		if (IS_ENABLED(CONFIG_SEPARATE_VERSTAGE)) {
			printk(BIOS_SPEW, "\n");
			printk(BIOS_SPEW, "| vboot data        |\n");
			printk(BIOS_SPEW, "+-------------------+ 0x%08x",
				CONFIG_DCACHE_RAM_BASE);
		}
		printk(BIOS_SPEW, " (CONFIG_DCACHE_RAM_BASE)\n");

		printk(BIOS_SPEW, "| FSP data          |\n");
		printk(BIOS_SPEW, "+-------------------+\n");
		printk(BIOS_SPEW, "| FSP stack         |\n");
		printk(BIOS_SPEW, "+-------------------+\n");
		printk(BIOS_SPEW, "| FSP binary        |\n");
		printk(BIOS_SPEW,
			"+-------------------+ 0x80000000 - ESRAM start\n\n");
	}

	/* Update the UPD data for MemoryInit */
	upd->AddrMode = config->AddrMode;
	upd->ChanMask = config->ChanMask;
	upd->ChanWidth = config->ChanWidth;
	upd->DramDensity = config->DramDensity;
	upd->DramRonVal = config->DramRonVal;
	upd->DramRttNomVal = config->DramRttNomVal;
	upd->DramRttWrVal = config->DramRttWrVal;
	upd->DramSpeed = config->DramSpeed;
	upd->DramType = config->DramType;
	upd->DramWidth = config->DramWidth;
	upd->EccScrubBlkSize = config->EccScrubBlkSize;
	upd->EccScrubInterval = config->EccScrubInterval;
	upd->Flags = config->Flags;
	upd->FspReservedMemoryLength = config->FspReservedMemoryLength;
	upd->RankMask = config->RankMask;
	upd->RmuBaseAddress = (uintptr_t)rmu_data;
	upd->RmuLength = rmu_data_len;
	upd->SerialPortWriteChar = console_log_level(BIOS_SPEW)
		? (uintptr_t)fsp_write_line : 0;
	upd->SmmTsegSize = IS_ENABLED(CONFIG_HAVE_SMI_HANDLER) ?
		config->SmmTsegSize : 0;
	upd->SocRdOdtVal = config->SocRdOdtVal;
	upd->SocWrRonVal = config->SocWrRonVal;
	upd->SocWrSlewRate = config->SocWrSlewRate;
	upd->SrInt = config->SrInt;
	upd->SrTemp = config->SrTemp;
	upd->tCL = config->tCL;
	upd->tFAW = config->tFAW;
	upd->tRAS = config->tRAS;
	upd->tRRD = config->tRRD;
	upd->tWTR = config->tWTR;
}

void soc_display_memory_init_params(const MEMORY_INIT_UPD *old,
	MEMORY_INIT_UPD *new)
{
	/* Display the parameters for MemoryInit */
	printk(BIOS_SPEW, "UPD values for MemoryInit at: 0x%p\n", new);
	fsp_display_upd_value("AddrMode", sizeof(old->AddrMode),
		old->AddrMode, new->AddrMode);
	fsp_display_upd_value("ChanMask", sizeof(old->ChanMask),
		old->ChanMask, new->ChanMask);
	fsp_display_upd_value("ChanWidth", sizeof(old->ChanWidth),
		old->ChanWidth, new->ChanWidth);
	fsp_display_upd_value("DramDensity", sizeof(old->DramDensity),
		old->DramDensity, new->DramDensity);
	fsp_display_upd_value("DramRonVal", sizeof(old->DramRonVal),
		old->DramRonVal, new->DramRonVal);
	fsp_display_upd_value("DramRttNomVal", sizeof(old->DramRttNomVal),
		old->DramRttNomVal, new->DramRttNomVal);
	fsp_display_upd_value("DramRttWrVal", sizeof(old->DramRttWrVal),
		old->DramRttWrVal, new->DramRttWrVal);
	fsp_display_upd_value("DramSpeed", sizeof(old->DramSpeed),
		old->DramSpeed, new->DramSpeed);
	fsp_display_upd_value("DramType", sizeof(old->DramType),
		old->DramType, new->DramType);
	fsp_display_upd_value("DramWidth", sizeof(old->DramWidth),
		old->DramWidth, new->DramWidth);
	fsp_display_upd_value("EccScrubBlkSize", sizeof(old->EccScrubBlkSize),
		old->EccScrubBlkSize, new->EccScrubBlkSize);
	fsp_display_upd_value("EccScrubInterval", sizeof(old->EccScrubInterval),
		old->EccScrubInterval, new->EccScrubInterval);
	fsp_display_upd_value("Flags", sizeof(old->Flags), old->Flags,
		new->Flags);
	fsp_display_upd_value("FspReservedMemoryLength",
		sizeof(old->FspReservedMemoryLength),
		old->FspReservedMemoryLength, new->FspReservedMemoryLength);
	fsp_display_upd_value("RankMask", sizeof(old->RankMask), old->RankMask,
		new->RankMask);
	fsp_display_upd_value("RmuBaseAddress", sizeof(old->RmuBaseAddress),
		old->RmuBaseAddress, new->RmuBaseAddress);
	fsp_display_upd_value("RmuLength", sizeof(old->RmuLength),
		old->RmuLength, new->RmuLength);
	fsp_display_upd_value("SerialPortPollForChar",
		sizeof(old->SerialPortPollForChar),
		old->SerialPortPollForChar, new->SerialPortPollForChar);
	fsp_display_upd_value("SerialPortReadChar",
		sizeof(old->SerialPortReadChar),
		old->SerialPortReadChar, new->SerialPortReadChar);
	fsp_display_upd_value("SerialPortWriteChar",
		sizeof(old->SerialPortWriteChar),
		old->SerialPortWriteChar, new->SerialPortWriteChar);
	fsp_display_upd_value("SmmTsegSize", sizeof(old->SmmTsegSize),
		old->SmmTsegSize, new->SmmTsegSize);
	fsp_display_upd_value("SocRdOdtVal", sizeof(old->SocRdOdtVal),
		old->SocRdOdtVal, new->SocRdOdtVal);
	fsp_display_upd_value("SocWrRonVal", sizeof(old->SocWrRonVal),
		old->SocWrRonVal, new->SocWrRonVal);
	fsp_display_upd_value("SocWrSlewRate", sizeof(old->SocWrSlewRate),
		old->SocWrSlewRate, new->SocWrSlewRate);
	fsp_display_upd_value("SrInt", sizeof(old->SrInt), old->SrInt,
		new->SrInt);
	fsp_display_upd_value("SrTemp", sizeof(old->SrTemp), old->SrTemp,
		new->SrTemp);
	fsp_display_upd_value("tCL", sizeof(old->tCL), old->tCL, new->tCL);
	fsp_display_upd_value("tFAW", sizeof(old->tFAW), old->tFAW, new->tFAW);
	fsp_display_upd_value("tRAS", sizeof(old->tRAS), old->tRAS, new->tRAS);
	fsp_display_upd_value("tRRD", sizeof(old->tRRD), old->tRRD, new->tRRD);
	fsp_display_upd_value("tWTR", sizeof(old->tWTR), old->tWTR, new->tWTR);
}

void soc_after_ram_init(struct romstage_params *params)
{
	/* Disable the ROM shadow 0x000e0000 - 0x000fffff */
	disable_rom_shadow();

	/* Initialize the PCIe bridges */
	pcie_init();
}
