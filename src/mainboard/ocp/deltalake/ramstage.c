/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <bootstate.h>
#include <drivers/ipmi/ipmi_ops.h>
#include <drivers/ocp/dmi/ocp_dmi.h>
#include <gpio.h>
#include <soc/lewisburg_pch_gpio_defs.h>
#include <soc/ramstage.h>
#include <stdio.h>

#include "ipmi.h"

#define SLOT_ID_LEN 2

extern struct fru_info_str fru_strings;
static char slot_id_str[SLOT_ID_LEN];

/* Override SMBIOS 2 Location In Chassis from BMC */
const char *smbios_mainboard_location_in_chassis(void)
{
	uint8_t slot_id = 0;

	if (ipmi_get_slot_id(&slot_id) != CB_SUCCESS) {
		printk(BIOS_ERR, "IPMI get slot_id failed\n");
		return "";
	}
	/* Sanity check, slot_id can only be 1~4 since there are 4 slots in YV3 */
	if (slot_id < PCIE_CONFIG_A || slot_id > PCIE_CONFIG_D) {
		printk(BIOS_ERR, "slot_id %d is not between 1~4\n", slot_id);
		return "";
	}
	snprintf(slot_id_str, SLOT_ID_LEN, "%d", slot_id);
	return slot_id_str;
}

static void dl_oem_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	uint8_t pcie_config = 0;

	/* OEM string 1 to 6 */
	ocp_oem_smbios_strings(dev, t);

	/* TODO: Add real OEM string 7, add TBF for now */
	t->count = smbios_add_oem_string(t->eos, TBF);

	/* Add OEM string 8 */
	if (ipmi_get_pcie_config(&pcie_config) == CB_SUCCESS) {
		switch (pcie_config) {
		case PCIE_CONFIG_UNKNOWN:
			t->count = smbios_add_oem_string(t->eos, "0x0: Unknown");
			break;
		case PCIE_CONFIG_A:
			t->count = smbios_add_oem_string(t->eos, "0x1: YV3 Config-A");
			break;
		case PCIE_CONFIG_B:
			t->count = smbios_add_oem_string(t->eos, "0x2: YV3 Config-B");
			break;
		case PCIE_CONFIG_C:
			t->count = smbios_add_oem_string(t->eos, "0x3: YV3 Config-C");
			break;
		case PCIE_CONFIG_D:
			t->count = smbios_add_oem_string(t->eos, "0x4: YV3 Config-D");
			break;
		default:
			t->count = smbios_add_oem_string(t->eos, "Check BMC return data");
		}
	} else {
		printk(BIOS_ERR, "Failed to get IPMI PCIe config\n");
	}
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->get_smbios_strings = dl_oem_smbios_strings,
	read_fru_areas(CONFIG_BMC_KCS_BASE, CONFIG_FRU_DEVICE_ID, 0, &fru_strings);
}

void mainboard_silicon_init_params(FSPS_UPD *params)
{
}

static void mainboard_final(void *chip_info)
{
	struct ppin_req req = {0};

	req.cpu0_lo = xeon_sp_ppin[0].lo;
	req.cpu0_hi = xeon_sp_ppin[0].hi;
	/* Set PPIN to BMC */
	if (ipmi_set_ppin(&req) != CB_SUCCESS)
		printk(BIOS_ERR, "ipmi_set_ppin failed\n");
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};

static void pull_post_complete_pin(void *unused)
{
	/* Pull Low post complete pin */
	gpio_output(GPP_B20, 0);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, pull_post_complete_pin, NULL);
