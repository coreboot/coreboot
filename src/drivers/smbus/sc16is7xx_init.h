/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_SMBUS_SC16IS7XX_INIT__
#define __DRIVERS_SMBUS_SC16IS7XX_INIT__

#include <types.h>

void sc16is7xx_write_byte(uint8_t reg, unsigned char c);
void sc16is7xx_init(void);

#endif /* __DRIVERS_SMBUS_SC16IS7XX_INIT__ */
