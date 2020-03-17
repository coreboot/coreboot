/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
