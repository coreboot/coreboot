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

#ifndef SUPERIO_WINBOND_COMMON_PRE_RAM_H
#define SUPERIO_WINBOND_COMMON_PRE_RAM_H

#include <device/pnp_type.h>
#include <stdint.h>

void winbond_enable_serial(pnp_devfn_t dev, uint16_t iobase);
void winbond_set_pinmux(pnp_devfn_t dev, uint8_t offset, uint8_t mask, uint8_t state);
void winbond_set_clksel_48(pnp_devfn_t dev);

void pnp_enter_conf_state(pnp_devfn_t dev);
void pnp_exit_conf_state(pnp_devfn_t dev);

#endif /* SUPERIO_WINBOND_COMMON_PRE_RAM_H */
