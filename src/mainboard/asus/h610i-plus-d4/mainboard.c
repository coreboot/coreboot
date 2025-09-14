/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>
#include <identity.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <smbios.h>
#include <string.h>
#include <bootstate.h>
#include <superio/nuvoton/common/hwm.h>

// This is the base address of the HWM registers (set in the device tree)
#define HWM_IOBASE		0x290

// The registers below are in a separate bank for each fan
// On the H610I-PLUS and H610M-K only SYSFAN and CPUFAN are wired up
#define BANK_SYSFAN		1
#define BANK_CPUFAN		2

#define FAN_SOURCE		0x00		// Fan control temperature source
# define FAN_SOURCE_PECI0       0x10		// PECI Agent 0
# define FAN_SOURCE_PECI0_CAL	0x1c		// PECI Agent 0 Calibration

#define FAN_MODE_TEMP_TOLERANCE	0x02		// [7:4] = mode [2:0] temperature tolerance
# define FAN_MODE_SFIV		4		// SmartFan IV
#define FAN_STEP_UP_TIME	0x03		// In 0.1 sec
#define FAN_STEP_DOWN_TIME	0x04		// In 0.1 sec
#define FAN_DUTY_PER_STEP	0x66		// [7:4] step up val [3:0] step down val

#define FAN_TEMP(i)		(0x21 + (i))	// Temperature points on the curve (4 points)
#define FAN_DUTY(i)		(0x27 + (i))	// Corresponding duty for each temperature point

#define FAN_CRIT_TEMP		0x35		// Critical temperature
#define FAN_CRIT_DUTY_EN	0x36		// Use critical duty (or default to 255)
#define FAN_CRIT_DUTY		0x37		// Critical duty
#define FAN_CRIT_TEMP_TOLERANCE	0x38		// Critical temperature tolerance

struct nct_fan {
	const char *name;
	uint8_t bank;

	// Temperature source
	uint8_t source;

	// Temperature x duty cycle curve points
	uint8_t temp[4];
	uint8_t duty[4];

	// Temperature tolerance
	uint8_t temp_tolerance;

	// Step up and down smoothing
	uint8_t step_up_time;
	uint8_t step_down_time;
	uint8_t duty_per_step_up;
	uint8_t duty_per_step_down;

	// Critical mode
	uint8_t crit_temp;
	uint8_t crit_duty_en;
	uint8_t crit_duty;
	uint8_t crit_temp_tolerance;
};

#define PERCENT_TO_DUTY(perc)	((perc) * 255 / 100)

// These fan curves have been adjusted from Mate Kukri's original values (for his i3-12100), and work well with my i7-12700
static const struct nct_fan NCT_FANS[] = {
	{
		.name = "SYSFAN",
		.bank = BANK_SYSFAN,
		.source = FAN_SOURCE_PECI0,
		.temp = {40, 60, 75, 90},
		.duty = {PERCENT_TO_DUTY(20), PERCENT_TO_DUTY(40), PERCENT_TO_DUTY(70), PERCENT_TO_DUTY(100)},
		.crit_temp = 100,
		.crit_duty_en = 1,
		.crit_duty = 255,
		.crit_temp_tolerance = 2,
	},
	{
		.name = "CPUFAN",
		.bank = BANK_CPUFAN,
		.source = FAN_SOURCE_PECI0,
		.temp = {40, 60, 75, 90},
		.duty = {PERCENT_TO_DUTY(20), PERCENT_TO_DUTY(40), PERCENT_TO_DUTY(70), PERCENT_TO_DUTY(100)},
		.crit_temp = 100,
		.crit_duty_en = 1,
		.crit_duty = 255,
		.crit_temp_tolerance = 2,
	},
};

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_DESKTOP;
	fadt->iapc_boot_arch |= ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;
}

static void mainboard_init(void *chip_info)
{

}

static void mainboard_enable(struct device *dev)
{

}

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	uint8_t aspm, aspm_l1;

	/* ASPM L1 sub-states require CLKREQ, so CLK_PM should be enabled as well */
	if (CONFIG(PCIEXP_L1_SUB_STATE) && CONFIG(PCIEXP_CLK_PM))
		aspm_l1 = 2; // 2 - L1.1 and L1.2
	else
		aspm_l1 = 0;

	if (CONFIG(PCIEXP_ASPM)) {
		aspm = CONFIG(PCIEXP_L1_SUB_STATE) ? 3 : 1; // 3 - L0sL1, 1 - L0s
	} else {
		aspm = 0;
		aspm_l1 = 0;
	}

	memset(params->PcieRpEnableCpm, 0, sizeof(params->PcieRpEnableCpm));
	memset(params->CpuPcieRpEnableCpm, 0, sizeof(params->CpuPcieRpEnableCpm));
	memset(params->CpuPcieClockGating, 0, sizeof(params->CpuPcieClockGating));
	memset(params->CpuPciePowerGating, 0, sizeof(params->CpuPciePowerGating));

	params->UsbPdoProgramming = 1;

	params->CpuPcieFiaProgramming = 1;

	params->PcieRpFunctionSwap = 0;
	params->CpuPcieRpFunctionSwap = 0;

	params->PchLegacyIoLowLatency = 1;
	params->PchDmiAspmCtrl = 0;

	params->CpuPcieRpPmSci[2] = 1; // PCI_E1
	params->PcieRpPmSci[1]    = 1; // M2_1
	params->PcieRpPmSci[5]    = 1; // Ethernet
	params->PcieRpPmSci[6]    = 1; // WiFi

	params->PcieRpMaxPayload[0]  = 1; // PCI_E2
	params->PcieRpMaxPayload[1]  = 1; // M2_1
	params->PcieRpMaxPayload[5]  = 1; // Ethernet
	params->PcieRpMaxPayload[6]  = 1; // WiFi

	params->CpuPcieRpTransmitterHalfSwing[2] = 1; // PCI_E1
	params->PcieRpTransmitterHalfSwing[1]    = 1; // M2_1
	params->PcieRpTransmitterHalfSwing[5]    = 1; // Ethernet
	params->PcieRpTransmitterHalfSwing[6]    = 1; // WiFi

	params->CpuPcieRpEnableCpm[2] = CONFIG(PCIEXP_CLK_PM); // PCI_E1
	params->PcieRpEnableCpm[1]    = CONFIG(PCIEXP_CLK_PM); // M2_1
	params->PcieRpEnableCpm[5]    = CONFIG(PCIEXP_CLK_PM); // Ethernet
	params->PcieRpEnableCpm[6]    = CONFIG(PCIEXP_CLK_PM); // WiFi

	params->CpuPcieRpL1Substates[2] = aspm_l1; // PCI_E1
	params->PcieRpL1Substates[1]    = aspm_l1; // M2_1
	params->PcieRpL1Substates[5]    = aspm_l1; // Ethernet
	params->PcieRpL1Substates[6]    = aspm_l1; // WiFi

	params->CpuPcieRpAspm[2] = aspm; // PCI_E1
	params->PcieRpAspm[1]    = aspm; // M2_1
	params->PcieRpAspm[5]    = aspm; // Ethernet
	params->PcieRpAspm[6]    = aspm; // WiFi

	params->PcieRpAcsEnabled[1]  = 1; // M2_1
	params->PcieRpAcsEnabled[5]  = 1; // Ethernet
	params->PcieRpAcsEnabled[6]  = 1; // WiFi

	params->CpuPcieClockGating[2] = CONFIG(PCIEXP_CLK_PM);
	params->CpuPciePowerGating[2] = CONFIG(PCIEXP_CLK_PM);
	params->CpuPcieRpPeerToPeerMode[2] = 1;
	params->CpuPcieRpMaxPayload[2] = 2; // 512B
	params->CpuPcieRpAcsEnabled[2] = 1;

	params->SataLedEnable = 1;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};

static void nct6798d_hwm_init(void *arg)
{
	printk(BIOS_DEBUG, "NCT6798D HWM configuration\n");

	// Setup PECI
	// Devicetree must set pin 121 to PECI mode first
	nuvoton_hwm_select_bank(HWM_IOBASE, 7);
	pnp_write_hwm5_index(HWM_IOBASE, 1, 0x85);
	pnp_write_hwm5_index(HWM_IOBASE, 2, 0x02);
	pnp_write_hwm5_index(HWM_IOBASE, 3, 0x10);
	pnp_write_hwm5_index(HWM_IOBASE, 4, 0x00);
	pnp_write_hwm5_index(HWM_IOBASE, 9, 0x64);

	// Enable PECI temp reading
	nuvoton_hwm_select_bank(HWM_IOBASE, 0);
	pnp_write_hwm5_index(HWM_IOBASE, 0xae, 1);

	// Program PECI Agent 0 Calibration
	nuvoton_hwm_select_bank(HWM_IOBASE, 4);
	pnp_write_hwm5_index(HWM_IOBASE, 0xf8, 0x50);
	pnp_write_hwm5_index(HWM_IOBASE, 0xfa, 0x51);


	// Program fan control profiles
	for (const struct nct_fan *fan = NCT_FANS; fan < NCT_FANS + ARRAY_SIZE(NCT_FANS); ++fan) {
		printk(BIOS_DEBUG, "Configuring NCT6798D fan %s\n", fan->name);

		nuvoton_hwm_select_bank(HWM_IOBASE, fan->bank);

		pnp_write_hwm5_index(HWM_IOBASE, FAN_SOURCE, fan->source);

		for (size_t i = 0; i < 4; ++i)
			pnp_write_hwm5_index(HWM_IOBASE, FAN_TEMP(i), fan->temp[i]);
		for (size_t i = 0; i < 4; ++i)
			pnp_write_hwm5_index(HWM_IOBASE, FAN_DUTY(i), fan->duty[i]);

		pnp_write_hwm5_index(HWM_IOBASE, FAN_CRIT_TEMP, fan->crit_temp);
		pnp_write_hwm5_index(HWM_IOBASE, FAN_CRIT_DUTY_EN, fan->crit_duty_en);
		pnp_write_hwm5_index(HWM_IOBASE, FAN_CRIT_DUTY, fan->crit_duty);
		pnp_write_hwm5_index(HWM_IOBASE, FAN_CRIT_TEMP_TOLERANCE, fan->crit_temp_tolerance);

		pnp_write_hwm5_index(HWM_IOBASE, FAN_STEP_UP_TIME, fan->step_up_time);
		pnp_write_hwm5_index(HWM_IOBASE, FAN_STEP_DOWN_TIME, fan->step_down_time);
		pnp_write_hwm5_index(HWM_IOBASE, FAN_DUTY_PER_STEP, fan->duty_per_step_up << 4 | fan->duty_per_step_down);

		// There are other modes supported by hardware, but always use SmartFan IV mode here
		pnp_write_hwm5_index(HWM_IOBASE, FAN_MODE_TEMP_TOLERANCE, (FAN_MODE_SFIV << 4) | fan->temp_tolerance);
	}
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, nct6798d_hwm_init, NULL);
