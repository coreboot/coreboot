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
	PCH_DEVFN_GSPI0,
	PCH_DEVFN_GSPI1,
	PCH_DEVFN_GSPI2,
	PCH_DEVFN_UART0,
	PCH_DEVFN_UART1,
	PCH_DEVFN_UART2
};

/* List of Minimum Assertion durations in microseconds */
enum min_assrt_dur {
	MinAssrtDur0s		= 0,
	MinAssrtDur60us		= 60,
	MinAssrtDur1ms		= 1000,
	MinAssrtDur50ms		= 50000,
	MinAssrtDur98ms		= 98000,
	MinAssrtDur500ms	= 500000,
	MinAssrtDur1s		= 1000000,
	MinAssrtDur2s		= 2000000,
	MinAssrtDur3s		= 3000000,
	MinAssrtDur4s		= 4000000,
};

/* Signal Assertion duration values */
struct cfg_assrt_dur {
	/* Minimum assertion duration of SLP_A signal */
	enum min_assrt_dur slp_a;

	/* Minimum assertion duration of SLP_4 signal */
	enum min_assrt_dur slp_s4;

	/* Minimum assertion duration of SLP_3 signal */
	enum min_assrt_dur slp_s3;

	/* PCH PM Power Cycle duration */
	enum min_assrt_dur pm_pwr_cyc_dur;
};

/* Default value of PchPmPwrCycDur */
#define PCH_PM_PWR_CYC_DUR	0

static void parse_devicetree(FSP_S_CONFIG *params)
{
	const struct soc_intel_jasperlake_config *config = config_of_soc();

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

/* This function returns the highest assertion duration of the SLP_Sx assertion widths */
static enum min_assrt_dur get_high_assrt_width(const struct cfg_assrt_dur *cfg_assrt_dur)
{
	enum min_assrt_dur max_assert_dur = cfg_assrt_dur->slp_s4;

	if (max_assert_dur < cfg_assrt_dur->slp_s3)
		max_assert_dur = cfg_assrt_dur->slp_s3;

	if (max_assert_dur < cfg_assrt_dur->slp_a)
		max_assert_dur = cfg_assrt_dur->slp_a;

	return max_assert_dur;
}

/* This function converts assertion durations from register-encoded to microseconds */
static void get_min_assrt_dur(uint8_t slp_s4_min_assrt, uint8_t slp_s3_min_assrt,
		uint8_t slp_a_min_assrt, uint8_t pm_pwr_cyc_dur,
		struct cfg_assrt_dur *cfg_assrt_dur)
{
	/*
	 * Ensure slp_x_dur_list[] elements in the devicetree config are in sync with
	 * FSP encoded values.
	 */

	/* slp_s4_assrt_dur_list : 1s, 1s(default), 2s, 3s, 4s */
	const enum min_assrt_dur slp_s4_assrt_dur_list[] = {
		MinAssrtDur1s, MinAssrtDur1s, MinAssrtDur2s, MinAssrtDur3s, MinAssrtDur4s
	};

	/* slp_s3_assrt_dur_list: 50ms, 60us, 1ms, 50ms (Default), 2s */
	const enum min_assrt_dur slp_s3_assrt_dur_list[] = {
		MinAssrtDur50ms, MinAssrtDur60us, MinAssrtDur1ms, MinAssrtDur50ms, MinAssrtDur2s
	};

	/* slp_a_assrt_dur_list: 2s, 0s, 4s, 98ms, 2s(Default) */
	const enum min_assrt_dur slp_a_assrt_dur_list[] = {
		MinAssrtDur2s, MinAssrtDur0s, MinAssrtDur4s, MinAssrtDur98ms, MinAssrtDur2s
	};

	/* pm_pwr_cyc_dur_list: 4s(Default), 1s, 2s, 3s, 4s */
	const enum min_assrt_dur pm_pwr_cyc_dur_list[] = {
		MinAssrtDur4s, MinAssrtDur1s, MinAssrtDur2s, MinAssrtDur3s, MinAssrtDur4s
	};

	/* Get signal assertion width */
	if (slp_s4_min_assrt < ARRAY_SIZE(slp_s4_assrt_dur_list))
		cfg_assrt_dur->slp_s4 = slp_s4_assrt_dur_list[slp_s4_min_assrt];

	if (slp_s3_min_assrt < ARRAY_SIZE(slp_s3_assrt_dur_list))
		cfg_assrt_dur->slp_s3 = slp_s3_assrt_dur_list[slp_s3_min_assrt];

	if (slp_a_min_assrt < ARRAY_SIZE(slp_a_assrt_dur_list))
		cfg_assrt_dur->slp_a = slp_a_assrt_dur_list[slp_a_min_assrt];

	if (pm_pwr_cyc_dur < ARRAY_SIZE(pm_pwr_cyc_dur_list))
		cfg_assrt_dur->pm_pwr_cyc_dur = pm_pwr_cyc_dur_list[pm_pwr_cyc_dur];
}

/* This function ensures that the duration programmed in the PchPmPwrCycDur will never be
 * smaller than the SLP_Sx assertion widths.
 * If the pm_pwr_cyc_dur is less than any of the SLP_Sx assertion widths then it returns the
 * default value PCH_PM_PWR_CYC_DUR.
 */
static uint8_t get_pm_pwr_cyc_dur(uint8_t slp_s4_min_assrt, uint8_t slp_s3_min_assrt,
		uint8_t slp_a_min_assrt, uint8_t pm_pwr_cyc_dur)
{
	/* Set default values for the minimum assertion duration */
	struct cfg_assrt_dur cfg_assrt_dur = {
		.slp_a          = MinAssrtDur2s,
		.slp_s4         = MinAssrtDur1s,
		.slp_s3         = MinAssrtDur50ms,
		.pm_pwr_cyc_dur = MinAssrtDur4s
	};

	enum min_assrt_dur high_assrt_width;

	/* Convert assertion durations from register-encoded to microseconds */
	get_min_assrt_dur(slp_s4_min_assrt, slp_s3_min_assrt, slp_a_min_assrt, pm_pwr_cyc_dur,
			&cfg_assrt_dur);

	/* Get the highest assertion duration among PCH EDS specified signals for pwr_cyc_dur */
	high_assrt_width = get_high_assrt_width(&cfg_assrt_dur);

	if (cfg_assrt_dur.pm_pwr_cyc_dur >= high_assrt_width)
		return pm_pwr_cyc_dur;

	printk(BIOS_DEBUG,
			"Set PmPwrCycDur to 4s as configured PmPwrCycDur (%d) violates PCH EDS "
			"spec\n", pm_pwr_cyc_dur);

	return PCH_PM_PWR_CYC_DUR;
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	unsigned int i;
	struct device *dev;
	FSP_S_CONFIG *params = &supd->FspsConfig;
	struct soc_intel_jasperlake_config *config = config_of_soc();

	/* Parse device tree and fill in FSP UPDs */
	parse_devicetree(params);

	/* Load VBT before devicetree-specific config. */
	params->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Check if IGD is present and fill Graphics init param accordingly */
	dev = pcidev_path_on_root(SA_DEVFN_IGD);
	params->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_dev_enabled(dev);

	/* Use coreboot MP PPI services if Kconfig is enabled */
	if (CONFIG(USE_INTEL_FSP_TO_CALL_COREBOOT_PUBLISH_MP_PPI))
		params->CpuMpPpi = (uintptr_t) mp_fill_ppi_services_data();

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

	/* Enable End of Post in PEI phase */
	params->EndOfPostMessage = EOP_PEI;

	/* Legacy 8254 timer support */
	params->Enable8254ClockGating = !CONFIG(USE_LEGACY_8254_TIMER);
	params->Enable8254ClockGatingOnS3 = 1;

	/* disable Legacy PME */
	memset(params->PcieRpPmSci, 0, sizeof(params->PcieRpPmSci));

	/* Enable ClkReqDetect for enabled port */
	memcpy(params->PcieRpClkReqDetect, config->PcieRpClkReqDetect,
		sizeof(config->PcieRpClkReqDetect));

	/* USB configuration */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		params->Usb2PhyPetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		params->Usb2PhyTxiset[i] = config->usb2_ports[i].tx_bias;
		params->Usb2PhyPredeemp[i] = config->usb2_ports[i].tx_emp_enable;
		params->Usb2PhyPehalfbit[i] = config->usb2_ports[i].pre_emp_bit;

		if (config->usb2_ports[i].enable)
			params->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		else
			params->Usb2OverCurrentPin[i] = 0xff;
	}

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		params->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		if (config->usb3_ports[i].enable) {
			params->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		} else {
			params->Usb3OverCurrentPin[i] = 0xff;
		}
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

	/* SATA */
	dev = pcidev_path_on_root(PCH_DEVFN_SATA);
	params->SataEnable = is_dev_enabled(dev);
	if (params->SataEnable) {
		params->SataMode = config->SataMode;
		params->SataSalpSupport = config->SataSalpSupport;

		_Static_assert(ARRAY_SIZE(params->SataPortsEnable) >=
				ARRAY_SIZE(config->SataPortsEnable), "copy buffer overflow!");
		memcpy(params->SataPortsEnable, config->SataPortsEnable,
				sizeof(params->SataPortsEnable));

		_Static_assert(ARRAY_SIZE(params->SataPortsDevSlp) >=
				ARRAY_SIZE(config->SataPortsDevSlp), "copy buffer overflow!");
		memcpy(params->SataPortsDevSlp, config->SataPortsDevSlp,
				sizeof(params->SataPortsDevSlp));
	}

	/* SDCard related configuration */
	dev = pcidev_path_on_root(PCH_DEVFN_SDCARD);
	params->ScsSdCardEnabled = is_dev_enabled(dev);
	if (params->ScsSdCardEnabled)
		params->SdCardPowerEnableActiveHigh = config->SdCardPowerEnableActiveHigh;

	params->Device4Enable = config->Device4Enable;

	/* Set TccActivationOffset */
	params->TccActivationOffset = config->tcc_offset;

	/* eMMC configuration */
	dev = pcidev_path_on_root(PCH_DEVFN_EMMC);
	params->ScsEmmcEnabled = is_dev_enabled(dev);
	if (params->ScsEmmcEnabled)
		params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;

	/* Enable xDCI controller if enabled in devicetree and allowed */
	dev = pcidev_path_on_root(PCH_DEVFN_USBOTG);
	if (dev) {
		if (!xdci_can_enable())
			dev->enabled = 0;

		params->XdciEnable = dev->enabled;
	} else {
		params->XdciEnable = 0;
	}

	/* Disable Pavp */
	params->PavpEnable = 0;

	/* Provide correct UART number for FSP debug logs */
	params->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;

	/* Apply minimum assertion width settings if non-zero */
	if (config->PchPmSlpS3MinAssert)
		params->PchPmSlpS3MinAssert = config->PchPmSlpS3MinAssert;
	if (config->PchPmSlpS4MinAssert)
		params->PchPmSlpS4MinAssert = config->PchPmSlpS4MinAssert;
	if (config->PchPmSlpSusMinAssert)
		params->PchPmSlpSusMinAssert = config->PchPmSlpSusMinAssert;
	if (config->PchPmSlpAMinAssert)
		params->PchPmSlpAMinAssert = config->PchPmSlpAMinAssert;

	/* Set Power Cycle Duration */
	if (config->PchPmPwrCycDur)
		params->PchPmPwrCycDur = get_pm_pwr_cyc_dur(config->PchPmSlpS4MinAssert,
				config->PchPmSlpS3MinAssert, config->PchPmSlpAMinAssert,
				config->PchPmPwrCycDur);

	/* Override/Fill FSP Silicon Param for mainboard */
	mainboard_silicon_init_params(params);
}

/* Disable Multiphase Si init */
int soc_fsp_multi_phase_init_is_enable(void)
{
	return 0;
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
