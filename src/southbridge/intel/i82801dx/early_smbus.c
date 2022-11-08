/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/smbus_host.h>
#include "i82801dx.h"

void i82801dx_early_init(void)
{
	enable_smbus();
}
