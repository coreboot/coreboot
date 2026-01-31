/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/sandybridge.h>
#include <drivers/i2c/at24rf08c/lenovo.h>

void mainboard_early_init(bool s3resume)
{
	lenovo_mainboard_eeprom_lock();
}
