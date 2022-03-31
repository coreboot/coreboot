/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/smm.h>
#include <elog.h>
#include <variant/ec.h>

void mainboard_smi_gpi(u32 gpi_sts)
{
	printk(BIOS_WARNING, "No GPIO is set up as PAD_SMI, so %s should never end up being "
			     "called. gpi_status is %x.\n", __func__, gpi_sts);
}

void mainboard_smi_sleep(u8 slp_typ)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS, MAINBOARD_EC_S5_WAKE_EVENTS);

	gpios = variant_sleep_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}

int mainboard_smi_apmc(u8 apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS, MAINBOARD_EC_SMI_EVENTS);

	return 0;
}
