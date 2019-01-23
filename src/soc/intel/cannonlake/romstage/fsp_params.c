/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corp.
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

#include <assert.h>
#include <chip.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/pmclib.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <vendorcode/google/chromeos/chromeos.h>

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg, const config_t *config)
{
	unsigned int i;
	uint32_t mask = 0;
	const struct device *dev = dev_find_slot(0, PCH_DEVFN_ISH);

	/* Set IGD stolen size to 64MB. */
	m_cfg->IgdDvmt50PreAlloc = 2;
	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->IedSize = CONFIG_IED_REGION_SIZE;
	m_cfg->SaGv = config->SaGv;
	if (IS_ENABLED(CONFIG_SOC_INTEL_CANNONLAKE_PCH_H))
		m_cfg->UserBd = BOARD_TYPE_DESKTOP;
	else
		m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->RMT;

	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1 << i);
	}
	m_cfg->PcieRpEnableMask = mask;
	m_cfg->PrmrrSize = config->PrmrrSize;
	m_cfg->EnableC6Dram = config->enable_c6dram;
	m_cfg->PcdSerialIoUartNumber = CONFIG_UART_FOR_CONSOLE;
	/* Disable Vmx if Vt-d is already disabled */
	if (config->VtdDisable)
		m_cfg->VmxEnable = 0;
	else
		m_cfg->VmxEnable = config->VmxEnable;
#if IS_ENABLED(CONFIG_SOC_INTEL_COFFEELAKE)
	m_cfg->SkipMpInit = !chip_get_fsp_mp_init();
#endif

	/* Disable CPU Flex Ratio and SaGv in recovery mode */
	if (vboot_recovery_mode_enabled()) {
		struct chipset_power_state *ps = pmc_get_power_state();

		/*
		 * Only disable when coming from S5 (cold reset) otherwise
		 * the flex ratio may be locked and FSP will return an error.
		 */
		if (ps && ps->prev_sleep_state == ACPI_S5) {
			m_cfg->CpuRatio = 0;
			m_cfg->SaGv = 0;
		}
	}

	/* If ISH is enabled, enable ISH elements */
	if (!dev)
		m_cfg->PchIshEnable = 0;
	else
		m_cfg->PchIshEnable = dev->enabled;

	/* If HDA is enabled, enable HDA elements */
	dev = dev_find_slot(0, PCH_DEVFN_HDA);
	if (!dev)
		m_cfg->PchHdaEnable = 0;
	else
		m_cfg->PchHdaEnable = dev->enabled;

	/* Enable IPU only if the device is enabled */
	m_cfg->SaIpuEnable = 0;
	dev = pcidev_path_on_root(SA_DEVFN_IPU);
	if (dev)
		m_cfg->SaIpuEnable = dev->enabled;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct device *dev = dev_find_slot(0, PCH_DEVFN_LPC);
	const struct device *smbus = dev_find_slot(0, PCH_DEVFN_SMBUS);
	assert(dev != NULL);
	const config_t *config = dev->chip_info;
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	soc_memory_init_params(m_cfg, config);

	/* Enable SMBus controller based on config */
	if (!smbus)
		m_cfg->SmbusEnable = 0;
	else
		m_cfg->SmbusEnable = smbus->enabled;
	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = config->DebugConsent;

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
