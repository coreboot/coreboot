/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/smbus_host.h>

uintptr_t smbus_base(void)
{
	return CONFIG_FIXED_SMBUS_IO_BASE;
}
