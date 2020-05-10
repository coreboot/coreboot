/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_SMSC_SMSCSUPERIO_H
#define SUPERIO_SMSC_SMSCSUPERIO_H

#include <device/pnp_type.h>
#include <stdint.h>

/* All known/supported SMSC Super I/Os have the same logical device IDs
 * for the serial ports (COM1, COM2).
 */
#define SMSCSUPERIO_SP1 4	/* Com1 */
#define SMSCSUPERIO_SP2 5	/* Com2 */

void smscsuperio_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif /* SUPERIO_SMSC_SMSCSUPERIO_H */
