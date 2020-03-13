/* SPDX-License-Identifier: GPL-2.0-only */

#include <mboot.h>
#include "board_verified_boot.h"

const mboot_measure_item_t mb_log_list[] = {
	{ "config", CBFS_TYPE_RAW, MBOOT_PCR_INDEX_0, EV_NO_ACTION, NULL },
	{ "revision", CBFS_TYPE_RAW, MBOOT_PCR_INDEX_0, EV_NO_ACTION, NULL },
	{ "cmos_layout.bin", CBFS_TYPE_CMOS_LAYOUT, MBOOT_PCR_INDEX_0,
		EV_NO_ACTION, NULL },
#if CONFIG(VENDORCODE_ELTAN_VBOOT)
	{ "oemmanifest.bin", CBFS_TYPE_RAW, MBOOT_PCR_INDEX_7, EV_NO_ACTION,
		NULL },
#endif
};
