/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/i2c_smbus.h>
#include <device/smbus_host.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include "sc16is7xx_init.h"

void i2c_smbus_console_init(void)
{
	if (CONFIG(SC16IS7XX_INIT))
		sc16is7xx_init();
}

void i2c_smbus_console_tx_byte(unsigned char c)
{
	do_smbus_write_byte(CONFIG_FIXED_SMBUS_IO_BASE,
		CONFIG_CONSOLE_I2C_SMBUS_SLAVE_ADDRESS,
		CONFIG_CONSOLE_I2C_SMBUS_SLAVE_DATA_REGISTER, c);
}
