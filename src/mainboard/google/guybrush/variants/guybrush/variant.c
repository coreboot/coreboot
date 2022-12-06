/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <amdblocks/cpu.h>
#include <gpio.h>

bool variant_has_pcie_wwan(void)
{
	return is_dev_enabled(DEV_PTR(gpp_bridge_2));
}

uint8_t variant_sd_aux_reset_gpio(void)
{
	return board_id() == 1 ? GPIO_70 : GPIO_69;
}

void variant_update_ddi_descriptors(fsp_ddi_descriptor *ddi_descriptors)
{
	if ((get_cpu_count() == 4 && get_threads_per_core() == 2) || get_cpu_count() == 2)
		ddi_descriptors[1].connector_type = DDI_UNUSED_TYPE;
}
