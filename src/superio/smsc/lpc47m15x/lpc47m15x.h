/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_SMSC_LPC47M15X_H
#define SUPERIO_SMSC_LPC47M15X_H

#define LPC47M15X_FDC              0   /* Floppy */
#define LPC47M15X_PP               3   /* Parallel Port */
#define LPC47M15X_SP1              4   /* Com1 */
#define LPC47M15X_SP2              5   /* Com2 */
#define LPC47M15X_KBC              7   /* Keyboard & Mouse */
#define LPC47M15X_GAME             9   /* GAME */
#define LPC47M15X_PME             10   /* PME  reg*/
#define LPC47M15X_MPU             11   /* MPE -- who knows --   reg*/

#define LPC47M15X2_MAX_CONFIG_REGISTER	0x5F

#include <device/pnp_type.h>
#include <stdint.h>

void lpc47m15x_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif /* SUPERIO_SMSC_LPC47M15X_H */
