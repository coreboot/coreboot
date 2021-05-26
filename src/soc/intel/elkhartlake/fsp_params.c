/* SPDX-License-Identifier: GPL-2.0-only */
#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <intelblocks/lpss.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <string.h>

/*
 * ME End of Post configuration
 * 0 - Disable EOP.
 * 1 - Send in PEI (Applicable for FSP in API mode)
 * 2 - Send in DXE (Not applicable for FSP in API mode)
 */
enum {
	EOP_DISABLE,
	EOP_PEI,
	EOP_DXE,
} EndOfPost;

static const pci_devfn_t serial_io_dev[] = {
	PCH_DEVFN_I2C0,
	PCH_DEVFN_I2C1,
	PCH_DEVFN_I2C2,
	PCH_DEVFN_I2C3,
	PCH_DEVFN_I2C4,
	PCH_DEVFN_I2C5,
	PCH_DEVFN_I2C6,
	PCH_DEVFN_I2C7,
	PCH_DEVFN_GSPI0,
	PCH_DEVFN_GSPI1,
	PCH_DEVFN_GSPI2,
	PCH_DEVFN_UART0,
	PCH_DEVFN_UART1,
	PCH_DEVFN_UART2
};

static void parse_devicetree(FSP_S_CONFIG *params)
{
	const struct soc_intel_elkhartlake_config *config = config_of_soc();

	/* LPSS controllers configuration */

	/* I2C */
	_Static_assert(ARRAY_SIZE(params->SerialIoI2cMode) >=
			ARRAY_SIZE(config->SerialIoI2cMode), "copy buffer overflow!");
	memcpy(params->SerialIoI2cMode, config->SerialIoI2cMode,
		sizeof(config->SerialIoI2cMode));

	_Static_assert(ARRAY_SIZE(params->PchSerialIoI2cPadsTermination) >=
			ARRAY_SIZE(config->SerialIoI2cPadsTermination),
			"copy buffer overflow!");
	memcpy(params->PchSerialIoI2cPadsTermination, config->SerialIoI2cPadsTermination,
		sizeof(config->SerialIoI2cPadsTermination));

	params->PchSerialIoI2cSclPinMux[4] = 0x1B44AC09;	//GPIO native mode for GPP_H9
	params->PchSerialIoI2cSdaPinMux[4] = 0x1B44CC08;	//GPIO native mode for GPP_H8

	/* GSPI */
	_Static_assert(ARRAY_SIZE(params->SerialIoSpiMode) >=
			ARRAY_SIZE(config->SerialIoGSpiMode), "copy buffer overflow!");
	memcpy(params->SerialIoSpiMode, config->SerialIoGSpiMode,
		sizeof(config->SerialIoGSpiMode));

	_Static_assert(ARRAY_SIZE(params->SerialIoSpiCsEnable) >=
			ARRAY_SIZE(config->SerialIoGSpiCsEnable), "copy buffer overflow!");
	memcpy(params->SerialIoSpiCsEnable, config->SerialIoGSpiCsEnable,
		sizeof(config->SerialIoGSpiCsEnable));

	_Static_assert(ARRAY_SIZE(params->SerialIoSpiCsMode) >=
			ARRAY_SIZE(config->SerialIoGSpiCsMode), "copy buffer overflow!");
	memcpy(params->SerialIoSpiCsMode, config->SerialIoGSpiCsMode,
		sizeof(config->SerialIoGSpiCsMode));

	_Static_assert(ARRAY_SIZE(params->SerialIoSpiCsState) >=
			ARRAY_SIZE(config->SerialIoGSpiCsState), "copy buffer overflow!");
	memcpy(params->SerialIoSpiCsState, config->SerialIoGSpiCsState,
		sizeof(config->SerialIoGSpiCsState));

	params->SerialIoSpiCsPolarity[2] = 0;

	/* UART */
	_Static_assert(ARRAY_SIZE(params->SerialIoUartMode) >=
			ARRAY_SIZE(config->SerialIoUartMode), "copy buffer overflow!");
	memcpy(params->SerialIoUartMode, config->SerialIoUartMode,
		sizeof(config->SerialIoUartMode));

	_Static_assert(ARRAY_SIZE(params->SerialIoUartDmaEnable) >=
			ARRAY_SIZE(config->SerialIoUartDmaEnable), "copy buffer overflow!");
	memcpy(params->SerialIoUartDmaEnable, config->SerialIoUartDmaEnable,
		sizeof(config->SerialIoUartDmaEnable));

	params->SerialIoUartCtsPinMuxPolicy[0] = 0x2B01320F;	//GPIO native mode for GPP_T15
	params->SerialIoUartRtsPinMuxPolicy[0] = 0x2B01220E;	//GPIO native mode for GPP_T14
	params->SerialIoUartRxPinMuxPolicy[0] = 0x2B01020C;	//GPIO native mode for GPP_T12
	params->SerialIoUartTxPinMuxPolicy[0] = 0x2B01120D;	//GPIO native mode for GPP_T13

	/* Provide correct UART number for FSP debug logs */
	params->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	FSP_S_CONFIG *params = &supd->FspsConfig;

	/* Parse device tree and fill in FSP UPDs */
	parse_devicetree(params);

	/* TODO: Update with UPD override as FSP matures */

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
