/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_ITE_IT8623E_H
#define SUPERIO_ITE_IT8623E_H

/* Datasheet not available. The list of LDNs might not be complete. */

#define IT8623E_FDC  0x00 /* Floppy Controller */
#define IT8623E_SP1  0x01 /* Com1 */
#define IT8623E_SP2  0x02 /* Com2 */
#define IT8623E_PP   0x03 /* Parallel port */
#define IT8623E_EC   0x04 /* Environment controller */
#define IT8623E_KBCK 0x05 /* PS/2 keyboard */
#define IT8623E_KBCM 0x06 /* PS/2 mouse */
#define IT8623E_GPIO 0x07 /* GPIO */

#endif /* SUPERIO_ITE_IT8623E_H */
