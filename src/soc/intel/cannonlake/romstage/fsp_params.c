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
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pmclib.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "../chip.h"

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg, const config_t *config)
{
	unsigned int i;
	uint32_t mask = 0;
	const struct device *dev = pcidev_path_on_root(PCH_DEVFN_ISH);

	/* Set IGD stolen size to 64MB. */
	m_cfg->IgdDvmt50PreAlloc = 2;
	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->IedSize = CONFIG_IED_REGION_SIZE;
	m_cfg->SaGv = config->SaGv;
	if (CONFIG(SOC_INTEL_CANNONLAKE_PCH_H))
		m_cfg->UserBd = BOARD_TYPE_DESKTOP;
	else
		m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->RMT;

	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1 << i);
	}
	m_cfg->PcieRpEnableMask = mask;
	m_cfg->PrmrrSize = get_prmrr_size();
	m_cfg->EnableC6Dram = config->enable_c6dram;
#if CONFIG(SOC_INTEL_COMETLAKE)
	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;
#else
	m_cfg->PcdSerialIoUartNumber = CONFIG_UART_FOR_CONSOLE;
#endif
	/*
	 * PcdDebugInterfaceFlags
	 * This config will allow coreboot to pass information to the FSP
	 * regarding which debug interface is being used.
	 * Debug Interfaces:
	 * BIT0-RAM, BIT1-Legacy Uart BIT3-USB3, BIT4-LPSS Uart, BIT5-TraceHub
	 * BIT2 - Not used.
	 */
	m_cfg->PcdDebugInterfaceFlags =
				CONFIG(DRIVERS_UART_8250IO) ? 0x02 : 0x10;

	/* Change VmxEnable UPD value according to ENABLE_VMX Kconfig */
	m_cfg->VmxEnable = CONFIG(ENABLE_VMX);

#if CONFIG(SOC_INTEL_CANNONLAKE_ALTERNATE_HEADERS)
	m_cfg->SkipMpInit = !CONFIG_USE_INTEL_FSP_MP_INIT;
#endif

	if (config->cpu_ratio_override) {
		m_cfg->CpuRatio = config->cpu_ratio_override;
	} else {
		/* Set CpuRatio to match existing MSR value */
		msr_t flex_ratio;
		flex_ratio = rdmsr(MSR_FLEX_RATIO);
		m_cfg->CpuRatio = (flex_ratio.lo >> 8) & 0xff;
	}

	/* If ISH is enabled, enable ISH elements */
	if (!dev)
		m_cfg->PchIshEnable = 0;
	else
		m_cfg->PchIshEnable = dev->enabled;

	/* If HDA is enabled, enable HDA elements */
	dev = pcidev_path_on_root(PCH_DEVFN_HDA);
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
	const struct device *dev = pcidev_path_on_root(PCH_DEVFN_LPC);
	const struct device *smbus = pcidev_path_on_root(PCH_DEVFN_SMBUS);
	assert(dev != NULL);
	const config_t *config = config_of(dev);
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	FSP_M_TEST_CONFIG *tconfig = &mupd->FspmTestConfig;

	soc_memory_init_params(m_cfg, config);

	/* Enable SMBus controller based on config */
	if (!smbus)
		m_cfg->SmbusEnable = 0;
	else
		m_cfg->SmbusEnable = smbus->enabled;

	/* Set debug probe type */
	m_cfg->PlatformDebugConsent =
		CONFIG_SOC_INTEL_CANNONLAKE_DEBUG_CONSENT;

	/* Configure VT-d */
	tconfig->VtdDisable = 0;

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
