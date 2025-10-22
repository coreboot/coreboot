/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_COMMON_H
#define SUPERIO_NUVOTON_COMMON_H

#ifndef __ACPI__
#include <device/pnp_type.h>
#include <stdint.h>

void nuvoton_pnp_enter_conf_state(pnp_devfn_t dev);
void nuvoton_pnp_exit_conf_state(pnp_devfn_t dev);
void nuvoton_enable_serial(pnp_devfn_t dev, u16 iobase);
#endif

/*
 * Logical Device Numbers (LDN) common to all Nuvoton SIOs.
 * Because they will be incorporated into ACPI device IDs,
 * they must be written as decimal.
 */
#define NCT677X_PP		1 /* Parallel port */
#define NCT677X_SP1		2 /* COM1 */
#define NCT677X_SP2		3 /* COM2 & IR */
#define NCT677X_KBC		5 /* PS/2 keyboard and mouse */
#define NCT677X_ACPI		10 /* ACPI */
#define NCT677X_HWM_FPLED	11 /* Hardware monitor & front LED */
#define NCT677X_MAXLDN		22 /* The highest known LDN of these chips (0x16) */

#endif /* SUPERIO_NUVOTON_COMMON_H */
