/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/smm.h>
#include <intelblocks/smihandler.h>
#include <soc/pm.h>
#include <soc/gpio.h>
#include <variant/ec.h>
#include <variant/gpio.h>

void mainboard_smi_gpi_handler(const struct gpi_status *sts)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		if (gpi_status_get(sts, EC_SMI_GPI))
			chromeec_smi_process_events();
}

void mainboard_smi_sleep(u8 slp_typ)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_sleep_gpio_table(&num);
	gpio_configure_pads(pads, num);

	if (CONFIG(EC_GOOGLE_CHROMEEC))
		chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS,
					MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS,
					MAINBOARD_EC_SMI_EVENTS);
	return 0;
}

void mainboard_smi_espi_handler(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		chromeec_smi_process_events();
}
