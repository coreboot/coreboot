/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootmode.h>
#include <chip.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <intelblocks/power_limit.h>
#include <soc/pci_devs.h>
#include <timer.h>

#define GPIO_HDMI_HPD		GPP_E13
#define GPIO_DP_HPD		GPP_E14

/* TODO: This can be moved to common directory */
static void wait_for_hpd(gpio_t gpio, long timeout)
{
	struct stopwatch sw;

	printk(BIOS_INFO, "Waiting for HPD\n");
	stopwatch_init_msecs_expire(&sw, timeout);
	while (!gpio_get(gpio)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING,
			       "HPD not ready after %ldms. Abort.\n", timeout);
			return;
		}
		mdelay(200);
	}
	printk(BIOS_INFO, "HPD ready after %lu ms\n",
	       stopwatch_duration_msecs(&sw));
}

/*
 * For type-C chargers, set PL2 to 97% of max power to account for
 * cable loss and FET Rdson loss in the path from the source.
 */
#define SET_PSYSPL2(w)               (97 * (w) / 100)
#define PUFF_U22_PL2                 (35)
#define PUFF_U62_U42_PL2             (51)
#define PUFF_CELERON_PENTIUM_PSYSPL2 (65)
#define PUFF_CORE_CPU_PSYSPL2        (90)
#define PUFF_MAX_TIME_WINDOW         6
#define PUFF_MIN_DUTYCYCLE           4

/*
 * mainboard_set_power_limits
 *
 * Set Pl2 and SysPl2 values based on detected charger.
 * Values are defined below but we use U22 value for all SKUs for now.
 * definitions:
 * x = no value entered. Use default value in parenthesis.
 *     will set 0 to anything that shouldn't be set.
 * n = max value of power adapter.
 * +-------------+-----+---------+-----------+-------+
 * | sku_id      | PL2 | PsysPL2 |  PsysPL3  |  PL4  |
 * +-------------+-----+---------+-----------+-------+
 * | i7 U42      |  51 |   90    | x(.85PL4) | x(82) |
 * | i3 U22      |  35 |   65    | x(.85PL4) | x(51) |
 * +-------------+-----+---------+-----------+-------+
 * For USB C charger:
 * +-------------+-----------------+---------+---------+-------+
 * | Max Power(W)|       PL2       | PsysPL2 | PsysPL3 |  PL4  |
 * +-------------+-----+-----------+---------+---------+-------+
 * | n           | min(0.97n, PL2) |  0.97n  |  0.97n  | 0.97n |
 * +-------------+-----+-----------+---------+---------+-------+
 */

/*
 * Psys_pmax considerations
 *
 * Given the hardware design in puff, the serial shunt resistor is 0.01ohm.
 * The full scale of hardware PSYS signal 0.8v maps to system current 9.6A
 * instead of real system power. The equation is shown below:
 * PSYS = 0.8v = (0.01ohm x Iinput) x 50 (INA213, gain 50V/V) x 15k/(15k + 75k)
 * Hence, Iinput (Amps) = 9.6A
 * Since there is no voltage information from PSYS, different voltage input
 * would map to different Psys_pmax settings:
 * For Type-C 15V, the Psys_pmax sholud be 15v x 9.6A = 144W
 * For Type-C 20V, the Psys_pmax should be 20v x 9.6A = 192W
 * For a barral jack, the Psys_pmax should be 19v x 9.6A = 182.4W
 */
#define PSYS_IMAX    9600
#define BJ_VOLTS_MV  19000

static void mainboard_set_power_limits(struct soc_power_limits_config *conf)
{
	enum usb_chg_type type;
	u32 watts;
	u16 volts_mv, current_ma;
	u32 psyspl2 = PUFF_CELERON_PENTIUM_PSYSPL2; // default BJ value
	u32 pl2 = PUFF_U22_PL2; // default PL2 for U22
	int rv = google_chromeec_get_usb_pd_power_info(&type, &current_ma, &volts_mv);

	struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
	u16 mch_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	dev = pcidev_path_on_root(SA_DEVFN_IGD);
	u16 igd_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;

	/* use SoC default value for PsysPL3 and PL4 unless we're on USB-PD*/
	conf->tdp_psyspl3 = 0;
	conf->tdp_pl4 = 0;

	if (rv == 0 && type == USB_CHG_TYPE_PD) {
		/* Detected USB-PD.  Base on max value of adapter */
		watts = ((u32)current_ma * volts_mv) / 1000000;
		/* set psyspl2 to 90% of adapter rating */
		psyspl2 = SET_PSYSPL2(watts);

		/* Limit PL2 if the adapter is with lower capability */
		if (mch_id == PCI_DID_INTEL_CML_ULT ||
			mch_id == PCI_DID_INTEL_CML_ULT_6_2)
			pl2 = (psyspl2 > PUFF_U62_U42_PL2) ? PUFF_U62_U42_PL2 : psyspl2;
		else
			pl2 = (psyspl2 > PUFF_U22_PL2) ? PUFF_U22_PL2 : psyspl2;

		conf->tdp_psyspl3 = psyspl2;
		/* set max possible time window */
		conf->tdp_psyspl3_time = PUFF_MAX_TIME_WINDOW;
		/* set minimum duty cycle */
		conf->tdp_psyspl3_dutycycle = PUFF_MIN_DUTYCYCLE;
		/* No data about an arbitrary Type-C adapter, set pl4 conservatively. */
		conf->tdp_pl4 = psyspl2;
	} else {
		/*
		 * Input type is barrel jack, from the SKU matrix:
		 * 1. i3/i5/i7 SKUs use 90W BJ
		 * 2. Celeron and Pentium use 65W BJ (default)
		 */
		volts_mv = BJ_VOLTS_MV;
		/* Use IGD ID to check if CPU is Core SKUs */
		if (igd_id != PCI_DID_INTEL_CML_GT1_ULT_1 &&
			igd_id != PCI_DID_INTEL_CML_GT2_ULT_5) {
			psyspl2 = PUFF_CORE_CPU_PSYSPL2;
			if (mch_id == PCI_DID_INTEL_CML_ULT ||
				mch_id == PCI_DID_INTEL_CML_ULT_6_2)
				pl2 = PUFF_U62_U42_PL2;
		}
	}
	/* voltage unit is milliVolts and current is in milliAmps */
	conf->psys_pmax = (u16)(((u32)PSYS_IMAX * volts_mv) / 1000000);

	conf->tdp_pl2_override = pl2;
	conf->tdp_psyspl2 = psyspl2;
}

void variant_ramstage_init(void)
{
	static const long display_timeout_ms = 3000;
	struct soc_power_limits_config *soc_config;
	config_t *conf = config_of_soc();

	/* This is reconfigured back to whatever FSP-S expects by gpio_configure_pads. */
	gpio_input(GPIO_HDMI_HPD);
	gpio_input(GPIO_DP_HPD);
	if (display_init_required()
		&& !gpio_get(GPIO_HDMI_HPD)
		&& !gpio_get(GPIO_DP_HPD)) {
		/* This has to be done before FSP-S runs. */
		if (google_chromeec_wait_for_displayport(display_timeout_ms))
			wait_for_hpd(GPIO_DP_HPD, display_timeout_ms);
	}
	/* Psys_pmax needs to be setup before FSP-S */
	soc_config = &conf->power_limits_config;
	mainboard_set_power_limits(soc_config);
}
