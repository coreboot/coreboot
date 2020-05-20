/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <intelblocks/lpss.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <soc/gpio_soc_defs.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <string.h>

/* THC assignment definition */
#define THC_NONE	0
#define THC_0		1
#define THC_1		2

/*
 * Chip config parameter PcieRpL1Substates uses (UPD value + 1)
 * because UPD value of 0 for PcieRpL1Substates means disabled for FSP.
 * In order to ensure that mainboard setting does not disable L1 substates
 * incorrectly, chip config parameter values are offset by 1 with 0 meaning
 * use FSP UPD default. get_l1_substate_control() ensures that the right UPD
 * value is set in fsp_params.
 * 0: Use FSP UPD default
 * 1: Disable L1 substates
 * 2: Use L1.1
 * 3: Use L1.2 (FSP UPD default)
 */
static int get_l1_substate_control(enum L1_substates_control ctl)
{
	if ((ctl > L1_SS_L1_2) || (ctl == L1_SS_FSP_DEFAULT))
		ctl = L1_SS_L1_2;
	return ctl - 1;
}

static void parse_devicetree(FSP_S_CONFIG *params)
{
	const struct soc_intel_tigerlake_config *config;
	config = config_of_soc();

	for (int i = 0; i < CONFIG_SOC_INTEL_I2C_DEV_MAX; i++)
		params->SerialIoI2cMode[i] = config->SerialIoI2cMode[i];

	for (int i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++) {
		params->SerialIoSpiMode[i] = config->SerialIoGSpiMode[i];
		params->SerialIoSpiCsMode[i] = config->SerialIoGSpiCsMode[i];
		params->SerialIoSpiCsState[i] = config->SerialIoGSpiCsState[i];
	}

	for (int i = 0; i < CONFIG_SOC_INTEL_UART_DEV_MAX; i++)
		params->SerialIoUartMode[i] = config->SerialIoUartMode[i];
}

static const pci_devfn_t serial_io_dev[] = {
	PCH_DEVFN_I2C0,
	PCH_DEVFN_I2C1,
	PCH_DEVFN_I2C2,
	PCH_DEVFN_I2C3,
	PCH_DEVFN_I2C4,
	PCH_DEVFN_I2C5,
	PCH_DEVFN_GSPI0,
	PCH_DEVFN_GSPI1,
	PCH_DEVFN_GSPI2,
	PCH_DEVFN_GSPI3,
	PCH_DEVFN_UART0,
	PCH_DEVFN_UART1,
	PCH_DEVFN_UART2
};

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	int i;
	FSP_S_CONFIG *params = &supd->FspsConfig;

	struct device *dev;
	struct soc_intel_tigerlake_config *config;
	config = config_of_soc();

	/* Parse device tree and enable/disable Serial I/O devices */
	parse_devicetree(params);

	/* Load VBT before devicetree-specific config. */
	params->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Check if IGD is present and fill Graphics init param accordingly */
	dev = pcidev_path_on_root(SA_DEVFN_IGD);
	if (CONFIG(RUN_FSP_GOP) && dev && dev->enabled)
		params->PeiGraphicsPeimInit = 1;
	else
		params->PeiGraphicsPeimInit = 0;

	/* Use coreboot MP PPI services if Kconfig is enabled */
	if (CONFIG(USE_INTEL_FSP_TO_CALL_COREBOOT_PUBLISH_MP_PPI)) {
		params->CpuMpPpi = (uintptr_t) mp_fill_ppi_services_data();
		params->SkipMpInit = 0;
	} else {
		params->SkipMpInit = !CONFIG_USE_INTEL_FSP_MP_INIT;
	}

	/* D3Hot and D3Cold for TCSS */
	params->D3HotEnable = config->TcssD3HotEnable;
	params->D3ColdEnable = config->TcssD3ColdEnable;

	params->TcssAuxOri = config->TcssAuxOri;
	for (i = 0; i < 8; i++)
		params->IomTypeCPortPadCfg[i] = config->IomTypeCPortPadCfg[i];

	/* Chipset Lockdown */
	if (get_lockdown_config() == CHIPSET_LOCKDOWN_COREBOOT) {
		params->PchLockDownGlobalSmi = 0;
		params->PchLockDownBiosInterface = 0;
		params->PchUnlockGpioPads = 1;
		params->RtcMemoryLock = 0;
	} else {
		params->PchLockDownGlobalSmi = 1;
		params->PchLockDownBiosInterface = 1;
		params->PchUnlockGpioPads = 0;
		params->RtcMemoryLock = 1;
	}

	/* USB */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		params->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		params->Usb2PhyPetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		params->Usb2PhyTxiset[i] = config->usb2_ports[i].tx_bias;
		params->Usb2PhyPredeemp[i] = config->usb2_ports[i].tx_emp_enable;
		params->Usb2PhyPehalfbit[i] = config->usb2_ports[i].pre_emp_bit;
	}

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		params->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		params->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		if (config->usb3_ports[i].tx_de_emp) {
			params->Usb3HsioTxDeEmphEnable[i] = 1;
			params->Usb3HsioTxDeEmph[i] = config->usb3_ports[i].tx_de_emp;
		}
		if (config->usb3_ports[i].tx_downscale_amp) {
			params->Usb3HsioTxDownscaleAmpEnable[i] = 1;
			params->Usb3HsioTxDownscaleAmp[i] =
				config->usb3_ports[i].tx_downscale_amp;
		}
	}

	/* RP Configs */
	for (i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		params->PcieRpL1Substates[i] =
			get_l1_substate_control(config->PcieRpL1Substates[i]);
		params->PcieRpLtrEnable[i] = config->PcieRpLtrEnable[i];
		params->PcieRpAdvancedErrorReporting[i] =
			config->PcieRpAdvancedErrorReporting[i];
	}

	/* Enable ClkReqDetect for enabled port */
	memcpy(params->PcieRpClkReqDetect, config->PcieRpClkReqDetect,
		sizeof(config->PcieRpClkReqDetect));

	/* Enable xDCI controller if enabled in devicetree and allowed */
	dev = pcidev_path_on_root(PCH_DEVFN_USBOTG);
	if (dev) {
		if (!xdci_can_enable())
			dev->enabled = 0;
		params->XdciEnable = dev->enabled;
	} else {
		params->XdciEnable = 0;
	}

	/* PCH UART selection for FSP Debug */
	params->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
	ASSERT(ARRAY_SIZE(params->SerialIoUartAutoFlow) > CONFIG_UART_FOR_CONSOLE);
	params->SerialIoUartAutoFlow[CONFIG_UART_FOR_CONSOLE] = 0;

	/* SATA */
	dev = pcidev_path_on_root(PCH_DEVFN_SATA);
	if (!dev)
		params->SataEnable = 0;
	else {
		params->SataEnable = dev->enabled;
		params->SataMode = config->SataMode;
		params->SataSalpSupport = config->SataSalpSupport;
		memcpy(params->SataPortsEnable, config->SataPortsEnable,
			sizeof(params->SataPortsEnable));
		memcpy(params->SataPortsDevSlp, config->SataPortsDevSlp,
			sizeof(params->SataPortsDevSlp));
	}

	/*
	 * Power Optimizer for DMI and SATA.
	 * DmiPwrOptimizeDisable and SataPwrOptimizeDisable is default to 0.
	 * Boards not needing the optimizers explicitly disables them by setting
	 * these disable variables to 1 in devicetree overrides.
	 */
	params->PchPwrOptEnable = !(config->DmiPwrOptimizeDisable);
	params->SataPwrOptEnable = !(config->SataPwrOptimizeDisable);

	/* Enable TCPU for processor thermal control */
	params->Device4Enable = config->Device4Enable;

	/* LAN */
	dev = pcidev_path_on_root(PCH_DEVFN_GBE);
	if (!dev)
		params->PchLanEnable = 0;
	else
		params->PchLanEnable = dev->enabled;

	/* CNVi */
	dev = pcidev_path_on_root(PCH_DEVFN_CNVI_WIFI);
	if (dev)
		params->CnviMode = dev->enabled;
	else
		params->CnviMode = 0;

	/* VMD */
	dev = pcidev_path_on_root(SA_DEVFN_VMD);
	if (dev)
		params->VmdEnable = dev->enabled;
	else
		params->VmdEnable = 0;

	/* THC */
	dev = pcidev_path_on_root(PCH_DEVFN_THC0);
	if (!dev)
		params->ThcPort0Assignment = 0;
	else
		params->ThcPort0Assignment = dev->enabled ? THC_0 : THC_NONE;

	dev =  pcidev_path_on_root(PCH_DEVFN_THC1);
	if (!dev)
		params->ThcPort1Assignment = 0;
	else
		params->ThcPort1Assignment = dev->enabled ? THC_1 : THC_NONE;

	/* Legacy 8254 timer support */
	params->Enable8254ClockGating = !CONFIG_USE_LEGACY_8254_TIMER;
	params->Enable8254ClockGatingOnS3 = !CONFIG_USE_LEGACY_8254_TIMER;

	/* Enable Hybrid storage auto detection */
	params->HybridStorageMode = config->HybridStorageMode;

	/* USB4/TBT */
	for (i = 0; i < ARRAY_SIZE(params->ITbtPcieRootPortEn); i++) {
		dev = pcidev_on_root(SA_DEV_SLOT_TBT, i);
		if (dev)
			params->ITbtPcieRootPortEn[i] = dev->enabled;
		else
			params->ITbtPcieRootPortEn[i] = 0;
	}

	mainboard_silicon_init_params(params);
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Return list of SOC LPSS controllers */
const pci_devfn_t *soc_lpss_controllers_list(size_t *size)
{
	*size = ARRAY_SIZE(serial_io_dev);
	return serial_io_dev;
}
