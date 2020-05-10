/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SUPERIO_SMSC_SIO1306_H
#define SUPERIO_SMSC_SIO1306_H

#define SIO1036_SP1		0 /* Com1 */

#define UART_POWER_DOWN		(1 << 7)
#define LPT_POWER_DOWN		(1 << 2)
#define IR_OUTPUT_MUX		(1 << 6)

#include <device/pnp_type.h>
#include <stdint.h>

void sio1036_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif /* SUPERIO_SMSC_SIO1306_H */
