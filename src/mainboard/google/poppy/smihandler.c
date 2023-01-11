/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/smm.h>
#include <elog.h>
#include <gpio.h>
#include <intelblocks/smihandler.h>

#include <baseboard/variants.h>
#include <variant/gpio.h>
#include <variant/ec.h>

void mainboard_smi_espi_handler(void)
{
	chromeec_smi_process_events();
}

void __weak variant_smi_sleep(u8 slp_typ) {}

void mainboard_smi_sleep(u8 slp_typ)
{
	const struct google_chromeec_event_info *info;

	info = variant_get_event_info();

	variant_smi_sleep(slp_typ);
	chromeec_smi_sleep(slp_typ, info->s3_wake_events, info->s5_wake_events);
}

int mainboard_smi_apmc(u8 apmc)
{
	const struct google_chromeec_event_info *info;

	info = variant_get_event_info();

	chromeec_smi_apmc(apmc, info->sci_events, info->smi_events);

	return 0;
}

void elog_gsmi_cb_mainboard_log_wake_source(void)
{
	const struct google_chromeec_event_info *info;

	info = variant_get_event_info();

	google_chromeec_log_events(info->log_events | info->s0ix_wake_events);
}
