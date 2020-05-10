/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_IT8718F_H
#define SUPERIO_ITE_IT8718F_H

#include <device/pnp_type.h>

/* Datasheet: http://www.ite.com.tw/product_info/PC/Brief-IT8718_2.asp */

#define IT8718F_FDC  0x00 /* Floppy */
#define IT8718F_SP1  0x01 /* Com1 */
#define IT8718F_SP2  0x02 /* Com2 */
#define IT8718F_PP   0x03 /* Parallel port */
#define IT8718F_EC   0x04 /* Environment controller */
#define IT8718F_KBCK 0x05 /* PS/2 keyboard */
#define IT8718F_KBCM 0x06 /* PS/2 mouse */
#define IT8718F_GPIO 0x07 /* GPIO */
#define IT8718F_IR   0x0a /* Consumer IR */

void it8718f_disable_reboot(pnp_devfn_t dev);

#endif /* SUPERIO_ITE_IT8718F_H */
