/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_ITE_COMMON_PRE_RAM_H
#define SUPERIO_ITE_COMMON_PRE_RAM_H

#include <device/pnp_type.h>
#include <stdint.h>

#define ITE_UART_CLK_PREDIVIDE_48 0x00 /* default */
#define ITE_UART_CLK_PREDIVIDE_24 0x01

void ite_conf_clkin(pnp_devfn_t dev, u8 predivide);
void ite_enable_serial(pnp_devfn_t dev, u16 iobase);

/* Some boards need to init wdt+gpio's very early */
void ite_reg_write(pnp_devfn_t dev, u8 reg, u8 value);
void ite_enable_3vsbsw(pnp_devfn_t dev);
void ite_kill_watchdog(pnp_devfn_t dev);

void pnp_enter_conf_state(pnp_devfn_t dev);
void pnp_exit_conf_state(pnp_devfn_t dev);

#endif /* SUPERIO_ITE_COMMON_PRE_RAM_H */
