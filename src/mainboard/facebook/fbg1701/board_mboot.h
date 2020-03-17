/*
 * This file is part of the coreboot project.
 *
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

#include <mboot.h>
#include "board_verified_boot.h"

const mboot_measure_item_t mb_log_list[] = {
	{ "config", CBFS_TYPE_RAW, MBOOT_PCR_INDEX_0, EV_NO_ACTION, NULL },
	{ "revision", CBFS_TYPE_RAW, MBOOT_PCR_INDEX_0, EV_NO_ACTION, NULL },
	{ "cmos_layout.bin", CBFS_COMPONENT_CMOS_LAYOUT, MBOOT_PCR_INDEX_0,
		EV_NO_ACTION, NULL },
#if CONFIG(VENDORCODE_ELTAN_VBOOT)
	{ "oemmanifest.bin", CBFS_TYPE_RAW, MBOOT_PCR_INDEX_7, EV_NO_ACTION,
		NULL },
#endif
};
