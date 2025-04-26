/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootmode.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/ec.h>
#include <elog.h>
#include <intelblocks/smihandler.h>
#include <vendorcode/google/chromeos/chromeos.h>

void google_chromeec_events_init(const struct google_chromeec_event_info *info,
				 bool is_s3_wakeup)
{
	/* no-op */
}

int get_lid_switch(void)
{
	/* Lid always open */
	return 1;
}

int get_recovery_mode_switch(void)
{
	return 0;
}

bool chromeos_device_branded_plus_hard(void)
{
	return false;
}

bool chromeos_device_branded_plus_soft(void)
{
	return false;
}
