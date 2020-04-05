/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <security/vboot/vboot_common.h>

#include "early_init.h"

void verstage_mainboard_init(void)
{
	mainboard_early_init();
}
