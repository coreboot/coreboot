/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ramstage.h>
#include <bootstate.h>
#include <drivers/ipmi/ipmi_ops.h>
#include <drivers/ocp/dmi/ocp_dmi.h>
#include <gpio.h>
#include <soc/ramstage.h>
#include <soc/lewisburg_pch_gpio_defs.h>

#include "ipmi.h"

extern struct fru_info_str fru_strings;

void mainboard_silicon_init_params(FSPS_UPD *params)
{
}

static void pull_post_complete_pin(void *unused)
{
	/* Pull Low post complete pin */
	gpio_output(GPP_B20, 0);
}


static void tp_oem_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	/* OEM string 1 to 6 */
	ocp_oem_smbios_strings(dev, t);

	/* OEM string 7 */
	if (fru_strings.board_info.custom_count > 1 &&
			*(fru_strings.board_info.board_custom + 1) != NULL)
		t->count = smbios_add_oem_string(t->eos,
			*(fru_strings.board_info.board_custom + 1));
	else
		t->count = smbios_add_oem_string(t->eos, TBF);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->get_smbios_strings = tp_oem_smbios_strings,
	read_fru_areas(CONFIG_BMC_KCS_BASE, CONFIG_FRU_DEVICE_ID, 0, &fru_strings);
}

static void mainboard_final(void *chip_info)
{
	struct ppin_req req;

	req.cpu0_lo = xeon_sp_ppin[0].lo;
	req.cpu0_hi = xeon_sp_ppin[0].hi;
	req.cpu1_lo = xeon_sp_ppin[1].lo;
	req.cpu1_hi = xeon_sp_ppin[1].hi;
	/* Set PPIN to BMC */
	ipmi_set_ppin(&req);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, pull_post_complete_pin, NULL);
