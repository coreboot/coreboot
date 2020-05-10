/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_SERVERENGINES_PILOT_PILOT_H
#define SUPERIO_SERVERENGINES_PILOT_PILOT_H

#include <device/pnp_type.h>

/* PILOT Super I/O is only based on LPC observation done on factory system. */

#define PILOT_LD1 0x01 /* Logical device 1 */
#define PILOT_SP1 0x02 /* Com1 */
#define PILOT_LD4 0x04 /* Logical device 4 */
#define PILOT_LD5 0x05 /* Logical device 5 */
#define PILOT_LD7 0x07 /* Logical device 7 */

/* should not expose these however early_init needs love */
void pnp_enter_ext_func_mode(pnp_devfn_t dev);
void pnp_exit_ext_func_mode(pnp_devfn_t dev);

void pilot_early_init(pnp_devfn_t dev);

void pilot_enable_serial(pnp_devfn_t dev, u16 iobase);
void pilot_disable_serial(pnp_devfn_t dev);

#endif /* SUPERIO_SERVERENGINES_PILOT_PILOT_H */
