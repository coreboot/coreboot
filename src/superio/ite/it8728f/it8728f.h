/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_IT8728F_H
#define SUPERIO_ITE_IT8728F_H

#define IT8728F_FDC  0x00 /* Floppy */
#define IT8728F_SP1  0x01 /* Com1 */
#define IT8728F_SP2  0x02 /* Com2 */
#define IT8728F_PP   0x03 /* Parallel port */
#define IT8728F_EC   0x04 /* Environment controller */
#define IT8728F_KBCK 0x05 /* PS/2 keyboard */
#define IT8728F_KBCM 0x06 /* PS/2 mouse */
#define IT8728F_GPIO 0x07 /* GPIO */
#define IT8728F_IR   0x0a /* Consumer IR */

/* Global configuration registers. */
#define IT8728F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8728F_CONFIG_REG_CHIPVERS  0x22 /* Chip version */
#define IT8728F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8728F_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. 'Special register' */

#endif /* SUPERIO_ITE_IT8728F_H */
