/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation
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

#include <soc/ramstage.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	u8 i;
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
	params->CdClock = 3;

	/* Set proper OC for various USB ports*/
	u8 usb2_oc[] = { 0x0, 0x2, 0x8, 0x8, 0x2, 0x8, 0x8, 0x8, 0x1, 0x8};
	u8 usb3_oc[] = { 0x0, 0x8, 0x8, 0x1, 0x8, 0x8 };

	for (i = 0; i < ARRAY_SIZE(usb2_oc); i++)
		params->Usb2OverCurrentPin[i] = usb2_oc[i];

	for (i = 0; i < ARRAY_SIZE(usb3_oc); i++)
		params->Usb3OverCurrentPin[i] = usb3_oc[i];
}
