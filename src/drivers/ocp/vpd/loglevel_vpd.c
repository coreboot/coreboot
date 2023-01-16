/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <drivers/ocp/include/vpd.h>

int get_loglevel_from_vpd(const char *const key, const int fallback)
{
	int log_level = fallback;

	if (vpd_get_int(key, VPD_RW_THEN_RO, &log_level)) {
		if (log_level < 0 || log_level >= BIOS_NEVER)
			log_level = fallback;
	}
	return log_level;
}

int get_console_loglevel(void)
{
	return get_loglevel_from_vpd(COREBOOT_LOG_LEVEL, COREBOOT_LOG_LEVEL_DEFAULT);
}

#if ENV_RAMSTAGE && CONFIG(RUNTIME_CONFIGURABLE_SMM_LOGLEVEL)
/* Read VPD for SMM settings in ramstage because we don't want to do this in SMM */
int mainboard_set_smm_log_level(void)
{
	return get_loglevel_from_vpd(SMM_LOG_LEVEL, SMM_LOG_LEVEL_DEFAULT);
}
#endif
