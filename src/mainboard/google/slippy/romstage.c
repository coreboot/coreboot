/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include "variant.h"


void mainboard_romstage_entry(void)
{
	variant_romstage_entry();
}
