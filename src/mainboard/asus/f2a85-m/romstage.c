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

#include <amdblocks/acpimmio.h>
#include <arch/io.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/agesa/hudson/smbus.h>
#include <stdint.h>

void board_BeforeAgesa(struct sysinfo *cb)
{
	u8 byte;

	post_code(0x30);

	/* turn on secondary smbus at b20 */
	pm_write8(0x28, pm_read8(0x28) | 1);

	/* set DDR3 voltage */
	byte = CONFIG_BOARD_ASUS_F2A85_M_DDR3_VOLT_VAL;

	/* default is byte = 0x0, so no need to set it in this case */
	if (byte)
		do_smbus_write_byte(0xb20, 0x15, 0x3, byte);
}
