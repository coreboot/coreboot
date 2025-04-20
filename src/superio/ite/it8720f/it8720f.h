/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_IT8720F_H
#define SUPERIO_ITE_IT8720F_H

/* Logical Device Numbers (LDN). */
#define IT8720F_FDC  0x00 /* Floppy */
#define IT8720F_SP1  0x01 /* Serial port 1 */
#define IT8720F_SP2  0x02 /* Serial port 2 */
#define IT8720F_PP   0x03 /* Parallel port */
#define IT8720F_EC   0x04 /* Environment controller */
#define IT8720F_KBCK 0x05 /* PS/2 keyboard */
#define IT8720F_KBCM 0x06 /* PS/2 mouse */
#define IT8720F_GPIO 0x07 /* GPIO (including SPI flash interface) */
#define IT8720F_CIR  0x0a /* Consumer IR */

#endif /* SUPERIO_ITE_IT8720F_H */
