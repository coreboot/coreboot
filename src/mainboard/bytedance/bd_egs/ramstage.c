/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <soc/ramstage.h>
#include <soc/smmrelocate.h>
#include <soc/gpio.h>

#include "gpio.h"

static void mainboard_chip_init(void *chip_info)
{
	const struct pad_config *pads;
	size_t pads_num;

	pads = get_gpio_table(&pads_num);
	gpio_configure_pads(pads, pads_num);
}

void mainboard_override_fsp_gpio(void)
{
	const struct pad_config *pads;
	size_t pads_num;

	pads = get_override_fsp_gpio_table(&pads_num);
	gpio_configure_pads(pads, pads_num);
}

void smm_mainboard_pci_resource_store_init(struct smm_pci_resource_info *slots, size_t size)
{
	soc_ubox_store_resources(slots, size);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
};
