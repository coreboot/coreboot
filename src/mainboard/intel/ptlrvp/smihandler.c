/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/x86/smm.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/smm.h>
#include <elog.h>
#include <intelblocks/smihandler.h>
#include <variant/ec.h>

void mainboard_smi_sleep(u8 slp_typ)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS, MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return 0;

	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS, MAINBOARD_EC_SMI_EVENTS);

	return 0;
}

void elog_gsmi_cb_mainboard_log_wake_source(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	google_chromeec_log_events(MAINBOARD_EC_LOG_EVENTS | MAINBOARD_EC_S0IX_WAKE_EVENTS);
}

void mainboard_smi_espi_handler(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	chromeec_smi_process_events();
}
