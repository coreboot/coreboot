/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _INTELPCH_SMBUS_H_
#define _INTELPCH_SMBUS_H_

#include <soc/intel/common/tco.h>

/*
 * Default slave address value for PCH. This value is set to match default
 * value set by hardware. It is useful since PCH is able to respond even
 * before CPU is up. This is reset by RSMRST# but not by PLTRST#.
 */
#define SMBUS_SLAVE_ADDR		0x44

#endif
