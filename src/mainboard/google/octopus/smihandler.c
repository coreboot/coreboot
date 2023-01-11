/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/smm.h>
#include <elog.h>
#include <gpio.h>
#include <intelblocks/smihandler.h>
#include <soc/pm.h>
#include <variant/ec.h>
#include <variant/gpio.h>

struct gpio_with_delay {
	gpio_t gpio;
	unsigned int delay_msecs;
};

void mainboard_smi_gpi_handler(const struct gpi_status *sts)
{
	if (gpi_status_get(sts, EC_SMI_GPI))
		chromeec_smi_process_events();
}

void mainboard_smi_sleep(u8 slp_typ)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_sleep_gpio_table(&num, slp_typ);
	gpio_configure_pads(pads, num);

	variant_smi_sleep(slp_typ);

	chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS,
				MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS,
				MAINBOARD_EC_SMI_EVENTS);
	return 0;
}

void mainboard_smi_espi_handler(void)
{
	chromeec_smi_process_events();
}

void elog_gsmi_cb_mainboard_log_wake_source(void)
{
	google_chromeec_log_events(MAINBOARD_EC_LOG_EVENTS |
					MAINBOARD_EC_S0IX_WAKE_EVENTS);
}

void __weak variant_smi_sleep(u8 slp_typ)
{
	/* Leave for the variant to implement if necessary. */
}

void power_off_lte_module(void)
{

	const struct gpio_with_delay lte_power_off_gpios[] = {
		{
			GPIO_161, /* AVS_I2S1_MCLK -- PLT_RST_LTE_L */
			30,
		},
		{
			GPIO_117, /* PCIE_WAKE1_B -- FULL_CARD_POWER_OFF */
			100
		},
		{
			GPIO_67, /* UART2-CTS_B -- EN_PP3300_DX_LTE_SOC */
			0
		}
	};

	for (int i = 0; i < ARRAY_SIZE(lte_power_off_gpios); i++) {
		gpio_output(lte_power_off_gpios[i].gpio, 0);
		mdelay(lte_power_off_gpios[i].delay_msecs);
	}
}
