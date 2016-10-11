/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SUPERIO_ITE_COMMON_ROMSTAGE_H
#define SUPERIO_ITE_COMMON_ROMSTAGE_H

#include <arch/io.h>
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

#endif /* SUPERIO_ITE_COMMON_ROMSTAGE_H */
