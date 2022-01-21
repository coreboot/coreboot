/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <amdblocks/cpu.h>

void variant_update_ddi_descriptors(fsp_ddi_descriptor *ddi_descriptors)
{
	if ((get_cpu_count() == 4 && get_threads_per_core() == 2) || get_cpu_count() == 2)
		ddi_descriptors[1].connector_type = DDI_UNUSED_TYPE;
}
