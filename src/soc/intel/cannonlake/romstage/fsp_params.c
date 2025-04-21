/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/pci.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/pmclib.h>
#include <option.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
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

	m_cfg->HyperThreading = get_uint_option("hyper_threading", CONFIG(FSP_HYPERTHREADING));

	/*
	 * Probe for no IGD and disable InternalGfx and panel power to prevent a
	 * crash in FSP-M.
	 */
	const bool igd_on = !CONFIG(SOC_INTEL_DISABLE_IGD) && is_devfn_enabled(SA_DEVFN_IGD);
	if (igd_on && pci_read_config16(SA_DEV_IGD, PCI_VENDOR_ID) != 0xffff) {
		/* Set IGD stolen size to 64MB. */
		m_cfg->InternalGfx = 1;
		m_cfg->IgdDvmt50PreAlloc = get_uint_option("igd_dvmt_prealloc", 2);
		m_cfg->ApertureSize = get_uint_option("igd_aperture_size", 1);
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

	m_cfg->PcieRpEnableMask = pcie_rp_enable_mask(get_pch_pcie_rp_table());
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

	m_cfg->PchIshEnable = is_devfn_enabled(PCH_DEVFN_ISH);

	m_cfg->PchHdaEnable = is_devfn_enabled(PCH_DEVFN_HDA);

	m_cfg->SaIpuEnable = is_devfn_enabled(SA_DEVFN_IPU);

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

	m_cfg->SmbusEnable = is_devfn_enabled(PCH_DEVFN_SMBUS);

	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = CONFIG_SOC_INTEL_COMMON_DEBUG_CONSENT;

	/* Configure VT-d */
	tconfig->VtdDisable = !get_uint_option("vtd", 1);

	/* Set HECI1 PCI BAR address */
	m_cfg->Heci1BarAddress = HECI1_BASE_ADDRESS;

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
