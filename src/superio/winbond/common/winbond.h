/* SPDX-License-Identifier: GPL-2.0-or-later */

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
