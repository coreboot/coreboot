/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_FINTEK_F81966D_INTERNAL_H
#define SUPERIO_FINTEK_F81966D_INTERNAL_H

#include <device/pnp.h>

void f81966d_hwm_init(struct device *dev);
void f81966d_uart_init(struct device *dev);
void f81966d_gpio_init(struct device *dev);

void set_reg(struct device *dev, u8 port, u8 mask, u8 val);
u8 select_bank(struct device *dev, u8 bank);

#endif /* SUPERIO_FINTEK_F81966D_INTERNAL_H */
