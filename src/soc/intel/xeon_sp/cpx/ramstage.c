/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <smbios.h>

int soc_fsp_multi_phase_init_is_enable(void)
{
	return 0;
}

unsigned int smbios_cpu_get_max_speed_mhz(void)
{
	return 3900;
}
