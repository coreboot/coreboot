/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_WINBOND_WPCD376I_WPCD376I_H
#define SUPERIO_WINBOND_WPCD376I_WPCD376I_H

/* Logical Device Numbers (LDN). */
#define WPCD376I_FDC	0x00  /* Floppy */
#define WPCD376I_LPT	0x01  /* Parallel port */
/*			0x02     Undefined */
#define WPCD376I_SP1	0x03  /* UART1 */
#define WPCD376I_SWC	0x04  /* System wake-up control */
#define WPCD376I_KBCM	0x05  /* PS/2 mouse */
#define WPCD376I_KBCK	0x06  /* PS/2 keyboard */
#define WPCD376I_GPIO	0x07  /* General Purpose I/O */
#define WPCD376I_ECIR	0x15  /* Enhanced Consumer Infrared Functions (ECIR) */
#define WPCD376I_IR	0x16  /* UART3 & Infrared port */

#endif
