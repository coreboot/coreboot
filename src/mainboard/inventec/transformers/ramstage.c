/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <sprsp_gpio.h>
#include <intelblocks/cse.h>
#include <memory_info.h>

#include "ipmi.h"

#define SLOT_ID_LEN 2
static char slot_id_str[SLOT_ID_LEN];
extern uint32_t heci_fw_sts(void);
extern uint32_t  heci_cse_normal(void);
extern uint32_t heci_cse_done(void);

//extern void cse_init(uintptr_t tempbar);

u8 smbios_mainboard_feature_flags(void)
{
	return SMBIOS_FEATURE_FLAGS_HOSTING_BOARD | SMBIOS_FEATURE_FLAGS_REPLACEABLE;
}

const char *smbios_mainboard_location_in_chassis(void)
{
	uint8_t slot_id = 0;
	if (ipmi_get_slot_id(&slot_id) != CB_SUCCESS) {
		printk(BIOS_ERR, "IPMI get slot_id failed\n");
		return "Part Component";
	}
	return slot_id_str;
}

void mainboard_silicon_init_params(FSPS_UPD *params)
{
    /* configure Emmitsburg PCH GPIO controller after FSP-M */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
