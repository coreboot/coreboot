/* SPDX-License-Identifier: GPL-2.0-only */

#include <security/vboot/vboot_common.h>

#include "early_init.h"

void verstage_mainboard_init(void)
{
	mainboard_early_init();
}
