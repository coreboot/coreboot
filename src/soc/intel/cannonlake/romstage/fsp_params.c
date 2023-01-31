/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/pci.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pmclib.h>
#include <option.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <types.h>

#include "../chip.h"

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct device *dev = pcidev_path_on_root(PCH_DEVFN_LPC);
	assert(dev != NULL);
	const config_t *config = config_of(dev);
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	FSP_M_TEST_CONFIG *tconfig = &mupd->FspmTestConfig;
	unsigned int i;
	uint32_t mask = 0;

	m_cfg->HyperThreading = get_uint_option("hyper_threading", CONFIG(FSP_HYPERTHREADING));

	/*
	 * Probe for no IGD and disable InternalGfx and panel power to prevent a
	 * crash in FSP-M.
	 */
	dev = pcidev_path_on_root(SA_DEVFN_IGD);
	const bool igd_on = !CONFIG(SOC_INTEL_DISABLE_IGD) && dev && dev->enabled;
	if (igd_on && pci_read_config16(SA_DEV_IGD, PCI_VENDOR_ID) != 0xffff) {
		/* Set IGD stolen size to 64MB. */
		m_cfg->InternalGfx = 1;
		m_cfg->IgdDvmt50PreAlloc = 2;
	} else {
		m_cfg->InternalGfx = 0;
		m_cfg->IgdDvmt50PreAlloc = 0;
		tconfig->PanelPowerEnable = 0;
	}
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
	m_cfg->PrmrrSize = get_valid_prmrr_size();
	m_cfg->EnableC6Dram = config->enable_c6dram;
#if CONFIG(SOC_INTEL_COMETLAKE)
	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;
	memcpy(tconfig->PcieRpHotPlug, config->PcieRpHotPlug, sizeof(tconfig->PcieRpHotPlug));
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

	m_cfg->SkipMpInit = !CONFIG(USE_INTEL_FSP_MP_INIT);

	if (config->cpu_ratio_override) {
		m_cfg->CpuRatio = config->cpu_ratio_override;
	} else {
		/* Set CpuRatio to match existing MSR value */
		msr_t flex_ratio;
		flex_ratio = rdmsr(MSR_FLEX_RATIO);
		m_cfg->CpuRatio = (flex_ratio.lo >> 8) & 0xff;
	}

	dev = pcidev_path_on_root(PCH_DEVFN_ISH);
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

	/* SATA Gen3 strength */
	for (i = 0; i < SOC_INTEL_CML_SATA_DEV_MAX; i++) {
		if (config->sata_port[i].RxGen3EqBoostMagEnable) {
			m_cfg->PchSataHsioRxGen3EqBoostMagEnable[i] =
				config->sata_port[i].RxGen3EqBoostMagEnable;
			m_cfg->PchSataHsioRxGen3EqBoostMag[i] =
				config->sata_port[i].RxGen3EqBoostMag;
		}
		if (config->sata_port[i].TxGen3DownscaleAmpEnable) {
			m_cfg->PchSataHsioTxGen3DownscaleAmpEnable[i] =
				config->sata_port[i].TxGen3DownscaleAmpEnable;
			m_cfg->PchSataHsioTxGen3DownscaleAmp[i] =
				config->sata_port[i].TxGen3DownscaleAmp;
		}
		if (config->sata_port[i].TxGen3DeEmphEnable) {
			m_cfg->PchSataHsioTxGen3DeEmphEnable[i] =
				config->sata_port[i].TxGen3DeEmphEnable;
			m_cfg->PchSataHsioTxGen3DeEmph[i] =
				config->sata_port[i].TxGen3DeEmph;
		}
	}
#if !CONFIG(SOC_INTEL_COMETLAKE)
	if (config->DisableHeciRetry)
		tconfig->DisableHeciRetry = config->DisableHeciRetry;
#endif

	/* Enable SMBus controller based on config */
	dev = pcidev_path_on_root(PCH_DEVFN_SMBUS);
	if (!dev)
		m_cfg->SmbusEnable = 0;
	else
		m_cfg->SmbusEnable = dev->enabled;

	/* Set debug probe type */
	m_cfg->PlatformDebugConsent =
		CONFIG_SOC_INTEL_CANNONLAKE_DEBUG_CONSENT;

	/* Configure VT-d */
	tconfig->VtdDisable = 0;

	/* Set HECI1 PCI BAR address */
	m_cfg->Heci1BarAddress = HECI1_BASE_ADDRESS;

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
