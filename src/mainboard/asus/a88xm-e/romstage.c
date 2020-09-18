/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <arch/io.h>
#include <console/console.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/agesa/hudson/smbus.h>

static void smbus_setup(void)
{
	post_code(0x30);

	/* turn on secondary smbus at b20 */
	pm_write8(0x28, pm_read8(0x28) | 0x01);
}

void board_BeforeAgesa(struct sysinfo *cb)
{
	smbus_setup();
}
