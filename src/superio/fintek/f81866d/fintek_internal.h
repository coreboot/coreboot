/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_FINTEK_F81866D_INTERNAL_H
#define SUPERIO_FINTEK_F81866D_INTERNAL_H

#include <device/pnp.h>

void f81866d_hwm_init(struct device *dev);
void f81866d_uart_init(struct device *dev);

#endif /* SUPERIO_FINTEK_F81866D_INTERNAL_H */
