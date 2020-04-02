/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef EC_KONTRON_KEMPLD_CHIP_H
#define EC_KONTRON_KEMPLD_CHIP_H

#define KEMPLD_NUM_UARTS 2

enum kempld_uart_io {
	KEMPLD_UART_3F8 = 0,
	KEMPLD_UART_2F8 = 1,
	KEMPLD_UART_3E8 = 2,
	KEMPLD_UART_2E8 = 3,
};

struct kempld_uart {
	enum kempld_uart_io io;
	unsigned int irq;
};

struct ec_kontron_kempld_config {
	struct kempld_uart uart[KEMPLD_NUM_UARTS];
};

#endif /* EC_KONTRON_KEMPLD_CHIP_H */
