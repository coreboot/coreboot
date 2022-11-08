/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_SMSC_LPC47M10X_H
#define SUPERIO_SMSC_LPC47M10X_H

#include <device/pnp_type.h>
#include <stdint.h>

#define LPC47M10X2_FDC	0	/* Floppy */
#define LPC47M10X2_PP	3	/* Parallel Port */
#define LPC47M10X2_SP1	4	/* Com1 */
#define LPC47M10X2_SP2	5	/* Com2 */
#define LPC47M10X2_KBC	7	/* Keyboard & Mouse */
#define LPC47M10X2_GAME	9	/* GAME */
#define LPC47M10X2_PME	10	/* PME reg*/
#define LPC47M10X2_MPU	11	/* MPU-401 MIDI */

#define LPC47M10X2_MAX_CONFIG_REGISTER	0x5F

void lpc47m10x_enable_serial(pnp_devfn_t dev, u16 iobase);

void pnp_enter_conf_state(pnp_devfn_t dev);
void pnp_exit_conf_state(pnp_devfn_t dev);

#endif /* SUPERIO_SMSC_LPC47M10X_H */
