/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include "ec.h"

void mainboard_pch_lpc_setup(void)
{
	/* Memory map KB9012 EC registers */
	pci_write_config32(
		PCH_LPC_DEV, LGMR,
		CONFIG_EC_BASE_ADDRESS | 1);
	pci_write_config16(PCH_LPC_DEV, BIOS_DEC_EN1, 0xffc0);

	/* Enable external USB port power. */
	if (CONFIG(USBDEBUG))
		ec_mm_set_bit(0x3b, 4);
}

static const char *mainboard_spd_names[9] = {
	"ELPIDA 4GB",
	"SAMSUNG 4GB",
	"HYNIX 4GB",
	"ELPIDA 8GB",
	"SAMSUNG 8GB",
	"HYNIX 8GB",
	"ELPIDA 2GB",
	"SAMSUNG 2GB",
	"HYNIX 2GB",
};

static unsigned int get_spd_index(void)
{
	const int spd_gpios[] = {71, 70, 16, 48, -1};

	unsigned int spd_index = get_gpios(spd_gpios);
	if (spd_index >= ARRAY_SIZE(mainboard_spd_names)) {
		/* Fallback to pessimistic 2GB image (ELPIDA 2GB) */
		spd_index = 6;
	}

	return spd_index;
}

void mb_get_spd_map(struct spd_info *spdi)
{
	unsigned int spd_index = get_spd_index();

	printk(BIOS_INFO, "SPD index %d (%s)\n",
		spd_index, mainboard_spd_names[spd_index]);

	/* C0S0 is a soldered RAM with no real SPD. Use stored SPD. */
	spdi->addresses[0] = SPD_MEMORY_DOWN;
	spdi->spd_index = spd_index;
}
