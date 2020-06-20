/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_KONTRON_KEMPLD_CHIP_H
#define EC_KONTRON_KEMPLD_CHIP_H

#define KEMPLD_NUM_UARTS 2

enum kempld_uart_io {
	KEMPLD_UART_3F8 = 0,
	KEMPLD_UART_2F8 = 1,
	KEMPLD_UART_3E8 = 2,
	KEMPLD_UART_2E8 = 3,
};

enum kempld_i2c_frequency {
	KEMPLD_I2C_FREQ_STANDARD_MODE_100KHZ	= 100,
	KEMPLD_I2C_FREQ_FAST_MODE_400KHZ	= 400,
	KEMPLD_I2C_FREQ_FAST_PLUS_MODE_1MHZ	= 1000,
	KEMPLD_I2C_FREQ_MAX			= 2700,
};

struct kempld_uart {
	enum kempld_uart_io io;
	unsigned int irq;
};

struct ec_kontron_kempld_config {
	struct kempld_uart uart[KEMPLD_NUM_UARTS];
	unsigned short i2c_frequency;
};

#endif /* EC_KONTRON_KEMPLD_CHIP_H */
