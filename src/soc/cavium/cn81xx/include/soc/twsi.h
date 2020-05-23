/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/i2c.h>

#ifndef __SOC_CAVIUM_CN81XX_INCLUDE_SOC_TWSI_H
#define __SOC_CAVIUM_CN81XX_INCLUDE_SOC_TWSI_H

int twsi_init(unsigned int bus, enum i2c_speed hz);

#endif
