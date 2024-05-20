/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>

smbios_wakeup_type smbios_system_wakeup_type(void)
{
	// TODO: Read wake source from EC.
	return SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
}
