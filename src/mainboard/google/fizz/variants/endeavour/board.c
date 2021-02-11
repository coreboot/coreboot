/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <nhlt.h>
#include <soc/nhlt.h>

void variant_nhlt_oem_overrides(const char **oem_id,
					const char **oem_table_id,
					uint32_t *oem_revision)
{
	*oem_id = "GOOGLE";
	*oem_table_id = "ENDEAVOUR";
	*oem_revision = 0;
}

void variant_chip_display_init(void)
{

}
