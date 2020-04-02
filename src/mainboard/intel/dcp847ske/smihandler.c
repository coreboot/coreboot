/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <types.h>
#include <cpu/x86/smm.h>
#include "superio.h"

void mainboard_smi_sleep(u8 slp_typ)
{
	if (slp_typ > 0 && slp_typ < 4) {
		/* Enable GRN_LED [Power LED] fading */
		SUPERIO_UNLOCK;
		SUPERIO_WRITE(0xf7, 0x68);
		SUPERIO_LOCK;
	}
}
