/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <soc/ramstage.h>

unsigned int smbios_cpu_get_voltage(void)
{
	return 16; /* Per SMBIOS spec, voltage times 10 */
}
