/*
 * This file is part of the coreboot project.
 *
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
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <intelblocks/lpss.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/xdci.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <string.h>

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
	PCH_DEVFN_UART0,
	PCH_DEVFN_UART1,
	PCH_DEVFN_UART2
};

static void parse_devicetree(FSP_S_CONFIG *params)
{
	const struct soc_intel_tigerlake_config *config = config_of_soc();

	/* LPSS controllers configuration */

	/* I2C */
	_Static_assert(ARRAY_SIZE(params->SerialIoI2cMode) >=
			ARRAY_SIZE(config->SerialIoI2cMode), "copy buffer overflow!");
	memcpy(params->SerialIoI2cMode, config->SerialIoI2cMode,
		sizeof(config->SerialIoI2cMode));

	/* GSPI */
	_Static_assert(ARRAY_SIZE(params->SerialIoSpiMode) >=
			ARRAY_SIZE(config->SerialIoGSpiMode), "copy buffer overflow!");
	memcpy(params->SerialIoSpiMode, config->SerialIoGSpiMode,
		sizeof(config->SerialIoGSpiMode));

	_Static_assert(ARRAY_SIZE(params->SerialIoSpiCsMode) >=
			ARRAY_SIZE(config->SerialIoGSpiCsMode), "copy buffer overflow!");
	memcpy(params->SerialIoSpiCsMode, config->SerialIoGSpiCsMode,
		sizeof(config->SerialIoGSpiCsMode));

	_Static_assert(ARRAY_SIZE(params->SerialIoSpiCsState) >=
			ARRAY_SIZE(config->SerialIoGSpiCsState), "copy buffer overflow!");
	memcpy(params->SerialIoSpiCsState, config->SerialIoGSpiCsState,
		sizeof(config->SerialIoGSpiCsState));

	/* UART */
	_Static_assert(ARRAY_SIZE(params->SerialIoUartMode) >=
			ARRAY_SIZE(config->SerialIoUartMode), "copy buffer overflow!");
	memcpy(params->SerialIoUartMode, config->SerialIoUartMode,
		sizeof(config->SerialIoUartMode));
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	unsigned int i;
	struct device *dev;
	FSP_S_CONFIG *params = &supd->FspsConfig;
	struct soc_intel_tigerlake_config *config = config_of_soc();

	/* Parse device tree and fill in FSP UPDs */
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

	/* Unlock upper 8 bytes of RTC RAM */
	params->RtcMemoryLock = 0;

	/* Legacy 8254 timer support */
	params->Enable8254ClockGating = !CONFIG_USE_LEGACY_8254_TIMER;
	params->Enable8254ClockGatingOnS3 = 1;

	/* disable Legacy PME */
	memset(params->PcieRpPmSci, 0, sizeof(params->PcieRpPmSci));

	/* USB configuration */
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

	/* SDCard related configuration */
	dev = pcidev_path_on_root(PCH_DEVFN_SDCARD);
	if (!dev)
		params->ScsSdCardEnabled = 0;
	else
		params->ScsSdCardEnabled = dev->enabled;

	params->Device4Enable = config->Device4Enable;

	/* eMMC configuration */
	dev = pcidev_path_on_root(PCH_DEVFN_EMMC);
	if (!dev) {
		params->ScsEmmcEnabled = 0;
	} else {
		params->ScsEmmcEnabled = dev->enabled;
		params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;
	}

	/* Enable xDCI controller if enabled in devicetree and allowed */
	dev = pcidev_path_on_root(PCH_DEVFN_USBOTG);
	if (dev) {
		if (!xdci_can_enable())
			dev->enabled = 0;

		params->XdciEnable = dev->enabled;
	} else {
		params->XdciEnable = 0;
	}

	/* Provide correct UART number for FSP debug logs */
	params->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;

	/* Override/Fill FSP Silicon Param for mainboard */
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
