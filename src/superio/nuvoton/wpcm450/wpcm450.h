/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_WPCM450_WPCM450_H
#define SUPERIO_NUVOTON_WPCM450_WPCM450_H

#define WPCM450_SP2  0x02 /* Com2 */
#define WPCM450_SP1  0x03 /* Com1 */
#define WPCM450_KBCK 0x06 /* Keyboard */

void wpcm450_enable_dev(u8 dev, u16 port, u16 iobase);

#endif
