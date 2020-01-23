/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_FINTEK_COMMON_PRE_RAM_H
#define SUPERIO_FINTEK_COMMON_PRE_RAM_H

#include <device/pnp_type.h>
#include <stdint.h>

void fintek_enable_serial(pnp_devfn_t dev, u16 iobase);

void pnp_enter_conf_state(pnp_devfn_t dev);
void pnp_exit_conf_state(pnp_devfn_t dev);

#endif /* SUPERIO_FINTEK_COMMON_PRE_RAM_H */
