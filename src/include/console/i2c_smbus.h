/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CONSOLE_I2C_SMBUS_H
#define CONSOLE_I2C_SMBUS_H

#include <stdint.h>

void i2c_smbus_console_init(void);
void i2c_smbus_console_tx_byte(unsigned char c);

#define __CONSOLE_SMBUS_ENABLE__	CONFIG(CONSOLE_I2C_SMBUS)

#if __CONSOLE_SMBUS_ENABLE__
static inline void __i2c_smbus_console_init(void)
{
	i2c_smbus_console_init();
}

static inline void __i2c_smbus_console_tx_byte(u8 data)
{
	i2c_smbus_console_tx_byte(data);
}
#else
static inline void __i2c_smbus_console_init(void)	{}
static inline void __i2c_smbus_console_tx_byte(u8 data)	{}
#endif /* __CONSOLE_SMBUS_ENABLE__ */

#endif /* CONSOLE_I2C_SMBUS_H */
