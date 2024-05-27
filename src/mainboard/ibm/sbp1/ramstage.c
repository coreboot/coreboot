/* SPDX-License-Identifier: GPL-2.0-only */
#include <cpu/x86/smm.h>
#include <soc/ramstage.h>
#include <soc/smmrelocate.h>
#include <stdio.h>

#include "include/spr_sbp1_gpio.h"
#include <bootstate.h>

void mainboard_silicon_init_params(FSPS_UPD *params)
{
	/* configure Emmitsburg PCH GPIO controller after FSP-M */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

void smbios_fill_dimm_locator(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	const u8 so = dimm->soc_num;
	const u8 ch = dimm->channel_num;
	const u8 mm = dimm->dimm_num;

	char dev_loc[10] = { "\x00" };
	snprintf(dev_loc, sizeof(dev_loc), "DIMM C%u%c%u", so, 'A' + ch, mm);
	t->device_locator = smbios_add_string(t->eos, dev_loc);

	char bnk_loc[10] = { "\x00" };
	snprintf(bnk_loc, sizeof(bnk_loc), "BANK C%u%c%u", so, 'A' + ch, mm);
	t->bank_locator = smbios_add_string(t->eos, bnk_loc);
}

static void finalize_boot(void *unused)
{
	printk(BIOS_DEBUG, "FM_BIOS_POST_CMPLT_N cleared.\n");
	/* Clear FM_BIOS_POST_CMPLT_N */
	gpio_output(GPPC_C17, 0);
}

void smm_mainboard_pci_resource_store_init(struct smm_pci_resource_info *slots, size_t size)
{
	soc_ubox_store_resources(slots, size);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, finalize_boot, NULL);
