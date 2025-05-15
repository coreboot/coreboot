/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <soc/ramstage.h>
#include <soc/smmrelocate.h>
#include <device/i2c_simple.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSPS_UPD *params)
{
	/* configure Emmitsburg PCH GPIO controller after FSP-M */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

void smm_mainboard_pci_resource_store_init(struct smm_pci_resource_info *slots, size_t size)
{
	soc_ubox_store_resources(slots, size);
}

void smbios_fill_dimm_locator(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	const u8 so = dimm->soc_num;
	const u8 ch = dimm->channel_num;
	const u8 mm = dimm->dimm_num;

	// generate locator strings matching OEM firmware (CPU1_DIMM_A1, CPU1_DIMM_B1, ..., CPU1_DIMM_H1)
	char dev_loc[40] = { "\x00" };
	snprintf(dev_loc, sizeof(dev_loc), "CPU%u_DIMM_%c%u", so + 1, 'A' + ch, mm + 1);
	t->device_locator = smbios_add_string(t->eos, dev_loc);

	// memory controllers are numbered 0-3 (e.g. "NODE 0" used for DIMMs A1 and B1, "NODE 1" for C1 and D1, ...)
	char bnk_loc[40] = { "\x00" };
	snprintf(bnk_loc, sizeof(bnk_loc), "NODE %u", ch / 2);
	t->bank_locator = smbios_add_string(t->eos, bnk_loc);
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments, int count)
{
	// GENERIC_LINEAR_FRAMEBUFFER enables I2C compilation for the Aspeed VGA controller.
	// This is not supported (or needed) on the SPC741D8, so stub the function.
	return -1;
}
