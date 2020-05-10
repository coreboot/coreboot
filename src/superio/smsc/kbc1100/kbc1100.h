/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SUPERIO_SMSC_KBC1100_H
#define SUPERIO_SMSC_KBC1100_H

#include <device/pnp_type.h>
#include <stdint.h>

#define KBC1100_PM1              1            /* PM1 */
#define SMSCSUPERIO_SP1          4            /* Com1 */
#define SMSCSUPERIO_SP2          5            /* Com2 */
#define KBC1100_KBC              7            /* Keyboard */
#define KBC1100_EC0              8            /* EC Channel 0 */
#define KBC1100_MAILBOX          9            /* Mail Box */
#define KBC1100_GPIO             0x0A         /* GPIO */
#define KBC1100_SPI              0x0B         /* Share flash interface */

#define KBC1100_EC1              0x0D         /* EC Channel 1 */
#define KBC1100_EC2              0x0E         /* EC Channel 2 */

void kbc1100_early_serial(pnp_devfn_t dev, u16 iobase);
void kbc1100_early_init(u16 port);

#endif /* SUPERIO_SMSC_KBC1100_H */
