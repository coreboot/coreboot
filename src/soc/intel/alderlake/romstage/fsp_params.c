/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/cpu_ids.h>
#include <device/device.h>
#include <drivers/wifi/generic/wifi.h>
#include <fsp/fsp_debug_event.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pcie_rp.h>
#include <option.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>

#define FSP_CLK_NOTUSED			0xFF
#define FSP_CLK_LAN			0x70
#define FSP_CLK_FREE_RUNNING		0x80

#define CPU_PCIE_BASE			0x40

enum vtd_base_index_type {
	VTD_GFX,
	VTD_IPU,
	VTD_VTVCO,
	VTD_TBT0,
	VTD_TBT1,
	VTD_TBT2,
	VTD_TBT3,
};

static uint8_t clk_src_to_fsp(enum pcie_rp_type type, int rp_number)
{
	assert(type == PCIE_RP_PCH || type == PCIE_RP_CPU);

	if (type == PCIE_RP_PCH)
		return rp_number;
	else // type == PCIE_RP_CPU
		return CPU_PCIE_BASE + rp_number;
}

static void pcie_rp_init(FSP_M_CONFIG *m_cfg, uint32_t en_mask, enum pcie_rp_type type,
			const struct pcie_rp_config *cfg, size_t cfg_count)
{
	size_t i;
	/* bitmask to save the status of clkreq assignment */
	static unsigned int clk_req_mapping = 0;

	for (i = 0; i < cfg_count; i++) {
		if (!(en_mask & BIT(i)))
			continue;
		if (cfg[i].flags & PCIE_RP_CLK_SRC_UNUSED)
			continue;
		if (!cfg[i].flags && cfg[i].clk_src == 0 && cfg[i].clk_req == 0) {
			printk(BIOS_WARNING, "Missing root port clock structure definition\n");
			continue;
		}
		if (clk_req_mapping & (1 << cfg[i].clk_req))
			printk(BIOS_WARNING, "Found overlapped clkreq assignment on clk req %d\n"
				, cfg[i].clk_req);
		if (!(cfg[i].flags & PCIE_RP_CLK_REQ_UNUSED)) {
			m_cfg->PcieClkSrcClkReq[cfg[i].clk_src] = cfg[i].clk_req;
			clk_req_mapping |= 1 << cfg[i].clk_req;
		}
		m_cfg->PcieClkSrcUsage[cfg[i].clk_src] = clk_src_to_fsp(type, i);
	}
}

static void fill_fspm_pcie_rp_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Disable all PCIe clock sources by default. And set RP irrelevant clock. */
	unsigned int i;

	for (i = 0; i < CONFIG_MAX_PCIE_CLOCK_SRC; i++) {
		if (config->pcie_clk_config_flag[i] & PCIE_CLK_FREE_RUNNING)
			m_cfg->PcieClkSrcUsage[i] = FSP_CLK_FREE_RUNNING;
		else if (config->pcie_clk_config_flag[i] & PCIE_CLK_LAN)
			m_cfg->PcieClkSrcUsage[i] = FSP_CLK_LAN;
		else
			m_cfg->PcieClkSrcUsage[i] = FSP_CLK_NOTUSED;
		m_cfg->PcieClkSrcClkReq[i] = FSP_CLK_NOTUSED;
	}

	/* Configure PCH PCIE ports */
	m_cfg->PcieRpEnableMask = pcie_rp_enable_mask(get_pch_pcie_rp_table());
	pcie_rp_init(m_cfg, m_cfg->PcieRpEnableMask, PCIE_RP_PCH, config->pch_pcie_rp,
			CONFIG_MAX_PCH_ROOT_PORTS);

	/* Configure CPU PCIE ports */
	m_cfg->CpuPcieRpEnableMask = pcie_rp_enable_mask(get_cpu_pcie_rp_table());
	pcie_rp_init(m_cfg, m_cfg->CpuPcieRpEnableMask, PCIE_RP_CPU, config->cpu_pcie_rp,
			CONFIG_MAX_CPU_ROOT_PORTS);
}

static void fill_fspm_igd_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	unsigned int i;
	const struct ddi_port_upds {
		uint8_t *ddc;
		uint8_t *hpd;
	} ddi_port_upds[] = {
		[DDI_PORT_A] = {&m_cfg->DdiPortADdc, &m_cfg->DdiPortAHpd},
		[DDI_PORT_B] = {&m_cfg->DdiPortBDdc, &m_cfg->DdiPortBHpd},
		[DDI_PORT_C] = {&m_cfg->DdiPortCDdc, &m_cfg->DdiPortCHpd},
		[DDI_PORT_1] = {&m_cfg->DdiPort1Ddc, &m_cfg->DdiPort1Hpd},
		[DDI_PORT_2] = {&m_cfg->DdiPort2Ddc, &m_cfg->DdiPort2Hpd},
		[DDI_PORT_3] = {&m_cfg->DdiPort3Ddc, &m_cfg->DdiPort3Hpd},
		[DDI_PORT_4] = {&m_cfg->DdiPort4Ddc, &m_cfg->DdiPort4Hpd},
	};
	m_cfg->InternalGfx = !CONFIG(SOC_INTEL_DISABLE_IGD) && is_devfn_enabled(SA_DEVFN_IGD);
	if (m_cfg->InternalGfx) {
		/* IGD is enabled, set IGD stolen size to 60MB. */
		m_cfg->IgdDvmt50PreAlloc = IGD_SM_60MB;
		/* DP port config */
		m_cfg->DdiPortAConfig = config->ddi_portA_config;
		m_cfg->DdiPortBConfig = config->ddi_portB_config;
		for  (i = 0; i < ARRAY_SIZE(ddi_port_upds); i++) {
			*ddi_port_upds[i].ddc = !!(config->ddi_ports_config[i] &
								DDI_ENABLE_DDC);
			*ddi_port_upds[i].hpd = !!(config->ddi_ports_config[i] &
								DDI_ENABLE_HPD);
		}
	} else {
		/* IGD is disabled, skip IGD init in FSP. */
		m_cfg->IgdDvmt50PreAlloc = 0;
		/* DP port config */
		m_cfg->DdiPortAConfig = 0;
		m_cfg->DdiPortBConfig = 0;
		for  (i = 0; i < ARRAY_SIZE(ddi_port_upds); i++) {
			*ddi_port_upds[i].ddc = 0;
			*ddi_port_upds[i].hpd = 0;
		}
	}
}
static void fill_fspm_mrc_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	m_cfg->SaGv = config->sagv;
	m_cfg->RMT = config->RMT;
	if (config->max_dram_speed_mts)
		m_cfg->DdrFreqLimit = config->max_dram_speed_mts;
}

static void fill_fspm_cpu_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	/* CpuRatio Settings */
	if (config->cpu_ratio_override)
		m_cfg->CpuRatio = config->cpu_ratio_override;
	else
		/* Set CpuRatio to match existing MSR value */
		m_cfg->CpuRatio = (rdmsr(MSR_FLEX_RATIO).lo >> 8) & 0xff;

	m_cfg->PrmrrSize = get_valid_prmrr_size();
	m_cfg->EnableC6Dram = config->enable_c6dram;
	m_cfg->HyperThreading = get_uint_option("hyper_threading", CONFIG(FSP_HYPERTHREADING));
}

static void fill_fspm_security_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Disable BIOS Guard */
	m_cfg->BiosGuard = 0;
	m_cfg->TmeEnable = CONFIG(INTEL_TME);
}

static void fill_fspm_uart_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* UART Debug Log */
	m_cfg->PcdDebugInterfaceFlags = CONFIG(DRIVERS_UART_8250IO) ?
			DEBUG_INTERFACE_UART_8250IO : DEBUG_INTERFACE_LPSS_SERIAL_IO;
	if (CONFIG(DRIVERS_UART_8250IO))
		m_cfg->PcdIsaSerialUartBase = ISA_SERIAL_BASE_ADDR_3F8;
	m_cfg->SerialIoUartDebugMode = PchSerialIoSkipInit;
	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;
}

static void fill_fspm_ipu_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Image clock: disable all clocks for bypassing FSP pin mux */
	memset(m_cfg->ImguClkOutEn, 0, sizeof(m_cfg->ImguClkOutEn));
	/* IPU */
	m_cfg->SaIpuEnable = is_devfn_enabled(SA_DEVFN_IPU);
}

static void fill_fspm_smbus_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	m_cfg->SmbusEnable = is_devfn_enabled(PCH_DEVFN_SMBUS);
}

static void fill_fspm_misc_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Disable Lock PCU Thermal Management registers */
	m_cfg->LockPTMregs = 0;

	/* Skip CPU replacement check */
	m_cfg->SkipCpuReplacementCheck = !config->cpu_replacement_check;

	/* Skip GPIO configuration from FSP */
	m_cfg->GpioOverride = 0x1;

	/* CNVi DDR RFI Mitigation */
	const struct device_path path[] = {
		{ .type = DEVICE_PATH_PCI, .pci.devfn = PCH_DEVFN_CNVI_WIFI },
		{ .type = DEVICE_PATH_GENERIC, .generic.id = 0 } };
	const struct device *dev = find_dev_nested_path(pci_root_bus(), path,
							ARRAY_SIZE(path));
	if (is_dev_enabled(dev))
		m_cfg->CnviDdrRfim = wifi_generic_cnvi_ddr_rfim_enabled(dev);
}

static void fill_fspm_audio_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Audio: HDAUDIO_LINK_MODE I2S/SNDW */
	m_cfg->PchHdaEnable = is_devfn_enabled(PCH_DEVFN_HDA);
	m_cfg->PchHdaDspEnable = config->pch_hda_dsp_enable;
	m_cfg->PchHdaIDispLinkTmode = config->pch_hda_idisp_link_tmode;
	m_cfg->PchHdaIDispLinkFrequency = config->pch_hda_idisp_link_frequency;
	m_cfg->PchHdaIDispCodecDisconnect = !config->pch_hda_idisp_codec_enable;
	/*
	 * All the PchHdaAudioLink{Hda|Dmic|Ssp|Sndw}Enable UPDs are used by FSP only to
	 * configure GPIO pads for audio. Mainboard is expected to perform all GPIO
	 * configuration in coreboot and hence these UPDs are set to 0 to skip FSP GPIO
	 * configuration for audio pads.
	 */
	m_cfg->PchHdaAudioLinkHdaEnable = 0;
	memset(m_cfg->PchHdaAudioLinkDmicEnable, 0, sizeof(m_cfg->PchHdaAudioLinkDmicEnable));
	memset(m_cfg->PchHdaAudioLinkSspEnable, 0, sizeof(m_cfg->PchHdaAudioLinkSspEnable));
	memset(m_cfg->PchHdaAudioLinkSndwEnable, 0, sizeof(m_cfg->PchHdaAudioLinkSndwEnable));
}

static void fill_fspm_ish_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	m_cfg->PchIshEnable = is_devfn_enabled(PCH_DEVFN_ISH);
}

static void fill_fspm_tcss_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Tcss USB */
	m_cfg->TcssXhciEn = is_devfn_enabled(SA_DEVFN_TCSS_XHCI);
	m_cfg->TcssXdciEn = is_devfn_enabled(SA_DEVFN_TCSS_XDCI);

	/* TCSS DMA */
	m_cfg->TcssDma0En = is_devfn_enabled(SA_DEVFN_TCSS_DMA0);
	m_cfg->TcssDma1En = is_devfn_enabled(SA_DEVFN_TCSS_DMA1);
}

static void fill_fspm_usb4_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	m_cfg->TcssItbtPcie0En = is_devfn_enabled(SA_DEVFN_TBT0);
	m_cfg->TcssItbtPcie1En = is_devfn_enabled(SA_DEVFN_TBT1);
	m_cfg->TcssItbtPcie2En = is_devfn_enabled(SA_DEVFN_TBT2);
	m_cfg->TcssItbtPcie3En = is_devfn_enabled(SA_DEVFN_TBT3);
}

static void fill_fspm_vtd_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	const uint32_t cpuid = cpu_get_cpuid();

	/* Disable VT-d for early silicon steppings as it results in a CPU hard hang */
	if (cpuid == CPUID_ALDERLAKE_J0 || cpuid == CPUID_ALDERLAKE_Q0) {
		m_cfg->VtdDisable = 1;
		return;
	}

	m_cfg->VtdBaseAddress[VTD_GFX] = GFXVT_BASE_ADDRESS;
	m_cfg->VtdBaseAddress[VTD_IPU] = IPUVT_BASE_ADDRESS;
	m_cfg->VtdBaseAddress[VTD_VTVCO] = VTVC0_BASE_ADDRESS;

	m_cfg->VtdDisable = 0;
	m_cfg->VtdIopEnable = !m_cfg->VtdDisable;
	m_cfg->VtdIgdEnable = m_cfg->InternalGfx;
	m_cfg->VtdIpuEnable = m_cfg->SaIpuEnable;

	if (m_cfg->VtdIgdEnable && m_cfg->VtdBaseAddress[VTD_GFX] == 0) {
		m_cfg->VtdIgdEnable = 0;
		printk(BIOS_ERR, "Requested IGD VT-d, but GFXVT_BASE_ADDRESS is 0\n");
	}

	if (m_cfg->VtdIpuEnable && m_cfg->VtdBaseAddress[VTD_IPU] == 0) {
		m_cfg->VtdIpuEnable = 0;
		printk(BIOS_ERR, "Requested IPU VT-d, but IPUVT_BASE_ADDRESS is 0\n");
	}

	if (!m_cfg->VtdDisable && m_cfg->VtdBaseAddress[VTD_VTVCO] == 0) {
		m_cfg->VtdDisable = 1;
		printk(BIOS_ERR, "Requested VT-d, but VTVCO_BASE_ADDRESS is 0\n");
	}

	if (m_cfg->TcssDma0En || m_cfg->TcssDma1En)
		m_cfg->VtdItbtEnable = 1;

	if (m_cfg->TcssItbtPcie0En)
		m_cfg->VtdBaseAddress[VTD_TBT0] = TBT0_BASE_ADDRESS;

	if (m_cfg->TcssItbtPcie1En)
		m_cfg->VtdBaseAddress[VTD_TBT1] = TBT1_BASE_ADDRESS;

	if (m_cfg->TcssItbtPcie2En)
		m_cfg->VtdBaseAddress[VTD_TBT2] = TBT2_BASE_ADDRESS;

	if (m_cfg->TcssItbtPcie3En)
		m_cfg->VtdBaseAddress[VTD_TBT3] = TBT3_BASE_ADDRESS;

	/* Change VmxEnable UPD value according to ENABLE_VMX Kconfig */
	m_cfg->VmxEnable = CONFIG(ENABLE_VMX);
}

static void fill_fspm_trace_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = CONFIG_SOC_INTEL_ALDERLAKE_DEBUG_CONSENT;

	/* CrashLog config */
	m_cfg->CpuCrashLogDevice = CONFIG(SOC_INTEL_CRASHLOG) && is_devfn_enabled(SA_DEVFN_TMT);
	m_cfg->CpuCrashLogEnable = m_cfg->CpuCrashLogDevice;
}

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_alderlake_config *config)
{
	void (*const fill_fspm_params[])(FSP_M_CONFIG *m_cfg,
			const struct soc_intel_alderlake_config *config) = {
		fill_fspm_igd_params,
		fill_fspm_mrc_params,
		fill_fspm_cpu_params,
		fill_fspm_security_params,
		fill_fspm_uart_params,
		fill_fspm_ipu_params,
		fill_fspm_smbus_params,
		fill_fspm_misc_params,
		fill_fspm_audio_params,
		fill_fspm_pcie_rp_params,
		fill_fspm_ish_params,
		fill_fspm_tcss_params,
		fill_fspm_usb4_params,
		fill_fspm_vtd_params,
		fill_fspm_trace_params,
	};

	for (size_t i = 0; i < ARRAY_SIZE(fill_fspm_params); i++)
		fill_fspm_params[i](m_cfg, config);
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct soc_intel_alderlake_config *config;
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	FSPM_ARCH_UPD *arch_upd = &mupd->FspmArchUpd;

	if (CONFIG(FSP_USES_CB_DEBUG_EVENT_HANDLER)) {
		if (CONFIG(CONSOLE_SERIAL) && CONFIG(FSP_ENABLE_SERIAL_DEBUG)) {
			enum fsp_log_level log_level = fsp_map_console_log_level();
			arch_upd->FspEventHandler = (UINT32)((FSP_EVENT_HANDLER *)
					fsp_debug_event_handler);
			/* Set Serial debug message level */
			m_cfg->PcdSerialDebugLevel = log_level;
			/* Set MRC debug level */
			m_cfg->SerialDebugMrcLevel = log_level;
		} else {
			/* Disable Serial debug message */
			m_cfg->PcdSerialDebugLevel = 0;
			/* Disable MRC debug message */
			m_cfg->SerialDebugMrcLevel = 0;
		}
	}
	config = config_of_soc();

	soc_memory_init_params(m_cfg, config);
	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
