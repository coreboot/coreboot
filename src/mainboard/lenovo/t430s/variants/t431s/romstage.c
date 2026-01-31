/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <drivers/i2c/at24rf08c/lenovo.h>

void mb_get_spd_map(struct spd_info *spdi)
{
	/* C1S0 is a soldered RAM with no real SPD. Use stored SPD. */
	spdi->addresses[0] = SPD_MEMORY_DOWN;
	spdi->addresses[2] = 0x51;
	spdi->spd_index = 0;
}

void mainboard_early_init(bool s3resume)
{
	lenovo_mainboard_eeprom_lock();
}
