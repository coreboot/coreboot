/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include "commands.h"
#include "ec.h"
#include "romstage.h"

void wilco_ec_romstage_init(void)
{
	wilco_ec_send(KB_BIOS_PROGRESS, BIOS_PROGRESS_BEFORE_MEMORY);
}
