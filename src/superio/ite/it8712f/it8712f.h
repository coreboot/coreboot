/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_IT8712F_H
#define SUPERIO_ITE_IT8712F_H

/* Datasheet: http://www.ite.com.tw/product_info/PC/Brief-IT8712_2.asp */

#define IT8712F_FDC  0x00 /* Floppy */
#define IT8712F_SP1  0x01 /* Com1 */
#define IT8712F_SP2  0x02 /* Com2 */
#define IT8712F_PP   0x03 /* Parallel port */
#define IT8712F_EC   0x04 /* Environment controller */
#define IT8712F_KBCK 0x05 /* Keyboard */
#define IT8712F_KBCM 0x06 /* Mouse */
#define IT8712F_GPIO 0x07 /* GPIO */
#define IT8712F_MIDI 0x08 /* MIDI port */
#define IT8712F_GAME 0x09 /* GAME port */
#define IT8712F_IR   0x0a /* Consumer IR */

#endif /* SUPERIO_ITE_IT8712F_H */
