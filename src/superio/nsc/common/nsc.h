/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NSC_COMMON_PRE_RAM_H
#define SUPERIO_NSC_COMMON_PRE_RAM_H

#include <stdint.h>
#include <device/pnp_type.h>

void nsc_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif /* SUPERIO_NSC_COMMON_PRE_RAM_H */
