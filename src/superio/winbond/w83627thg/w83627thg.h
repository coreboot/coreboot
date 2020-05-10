/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_WINBOND_W83627THG_W83627THG_H
#define SUPERIO_WINBOND_W83627THG_W83627THG_H

#include <device/pnp_type.h>

#define W83627THG_FDC              0   /* Floppy */
#define W83627THG_PP               1   /* Parallel port */
#define W83627THG_SP1              2   /* Com1 */
#define W83627THG_SP2              3   /* Com2 */
#define W83627THG_KBC              5   /* PS/2 keyboard & mouse */
#define W83627THG_GAME_MIDI_GPIO1  7
#define W83627THG_GPIO2            8
#define W83627THG_GPIO3            9
#define W83627THG_ACPI            10
#define W83627THG_HWM             11   /* Hardware monitor */

void pnp_enter_ext_func_mode(pnp_devfn_t dev);
void pnp_exit_ext_func_mode(pnp_devfn_t dev);

void w83627thg_set_clksel_48(pnp_devfn_t dev);

#endif /* SUPERIO_WINBOND_W83627THG_W83627THG_H */
