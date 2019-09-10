/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <pc80/mc146818rtc.h>
#include <cf9_reset.h>
#include <smbios.h>
#include <string.h>
#include "ipmi.h"

/*
 * mainboard_enable is executed as first thing after enumerate_buses().
 * This is the earliest point to add customization.
 */
static void mainboard_enable(struct device *dev)
{
	ipmi_oem_rsp_t rsp;

	if (is_ipmi_clear_cmos_set(&rsp)) {
		/* TODO: Should also try to restore CMOS to cmos.default
		 * if USE_OPTION_TABLE is set */
		cmos_init(1);
		clear_ipmi_flags(&rsp);
		system_reset();
	}
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

void smbios_fill_dimm_locator(const struct dimm_info *dimm, struct smbios_type17 *t)
{

	char locator[64] = {0};

	snprintf(locator, sizeof(locator), "DIMM_%c%u", 'A' + dimm->channel_num,
		 dimm->dimm_num);
	t->device_locator = smbios_add_string(t->eos, locator);

	snprintf(locator, sizeof(locator), "_Node0_Channel%d_Dimm%d", dimm->channel_num,
		 dimm->dimm_num);
	t->bank_locator = smbios_add_string(t->eos, locator);
}
