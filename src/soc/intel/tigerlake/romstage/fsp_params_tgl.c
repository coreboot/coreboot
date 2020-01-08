/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Intel Corp.
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
#include <console/console.h>
#include <fsp/util.h>
#include <soc/gpio_soc_defs.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>

/* Debug interface flag */
enum debug_interface_flag {
	DEBUG_INTERFACE_RAM  = 0x1,
	DEBUG_INTERFACE_UART = 0x2,
	DEBUG_INTERFACE_USB3 = 0x4,
	DEBUG_INTERFACE_SERIAL_IO  = 0x8,
	DEBUG_INTERFACE_TRACEHUB = 0x10
};

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_tigerlake_config *config)
{
	unsigned int i;
	uint32_t mask = 0;

	/* Set IGD stolen size to 60MB. */
	m_cfg->IgdDvmt50PreAlloc = 0xFE;
	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->IedSize = CONFIG_IED_REGION_SIZE;
	m_cfg->SaGv = config->SaGv;
	m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->RMT;

	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1 << i);
	}
	m_cfg->PcieRpEnableMask = mask;

	memcpy(m_cfg->PcieClkSrcUsage, config->PcieClkSrcUsage,
		sizeof(config->PcieClkSrcUsage));

	for (i = 0; i < CONFIG_MAX_PCIE_CLOCKS; i++) {
		if (config->PcieClkSrcUsage[i] == 0)
			m_cfg->PcieClkSrcUsage[i] = 0xff;
	}

	memcpy(m_cfg->PcieClkSrcClkReq, config->PcieClkSrcClkReq,
		sizeof(config->PcieClkSrcClkReq));

	m_cfg->PrmrrSize = config->PrmrrSize;
	m_cfg->EnableC6Dram = config->enable_c6dram;
	/* Disable BIOS Guard */
	m_cfg->BiosGuard = 0;

	/* UART Debug Log */
	m_cfg->PcdDebugInterfaceFlags = CONFIG(DRIVERS_UART_8250IO) ?
			DEBUG_INTERFACE_UART : DEBUG_INTERFACE_TRACEHUB;
	m_cfg->PcdIsaSerialUartBase = 0x0;
	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;

	/*
	 * Skip IGD initialization in FSP if device
	 * is disable in devicetree.cb.
	 */
	const struct device *dev = pcidev_path_on_root(SA_DEVFN_IGD);
	if (!dev || !dev->enabled)
		m_cfg->InternalGfx = 0;
	else
		m_cfg->InternalGfx = 0x1;

	/* Enable Hyper Threading */
	m_cfg->HyperThreading = 1;
	/* Disable Lock PCU Thermal Management registers */
	m_cfg->LockPTMregs = 0;
	/* Channel Hash Mask:0x0001=BIT6 set(Minimal), 0x3FFF=BIT[19:6] set(Maximum) */
	m_cfg->ChHashMask = 0x30CC;
	/* Enable SMBus controller based on config */
	m_cfg->SmbusEnable = config->SmbusEnable;
	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = config->DebugConsent;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct soc_intel_tigerlake_config *config;
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	config = config_of_soc();

	soc_memory_init_params(m_cfg, config);
	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
