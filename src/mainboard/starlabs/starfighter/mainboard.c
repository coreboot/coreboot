/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <device/device.h>
#include <soc/ramstage.h>
#include <variants.h>

static void starlabs_configure_gpios(void *unused)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, starlabs_configure_gpios, NULL);

struct chip_operations mainboard_ops = {};
