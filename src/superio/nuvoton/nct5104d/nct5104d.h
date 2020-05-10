/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT5104D_H
#define SUPERIO_NUVOTON_NCT5104D_H

#include <device/pnp_type.h>

/* SIO global configuration */
#define IRQ_TYPE_SEL_CR10   0x10     /* UARTA,UARTB */
#define IRQ_TYPE_SEL_CR11   0x11     /* SMI,UARTC,UARTD,WDTO */
#define GLOBAL_OPTION_CR26  0x26
#define CR26_LOCK_REG       (1 << 4) /* required to access CR10/CR11 */

/* LDN 0x07 specific registers */
#define NCT5104D_GPIO0_IO   0xE0
#define NCT5104D_GPIO1_IO   0xE4
#define NCT5104D_GPIO6_IO   0xF8

/* LDN 0x0F specific registers */
#define NCT5104D_GPIO0_PP_OD   0xE0
#define NCT5104D_GPIO1_PP_OD   0xE1
#define NCT5104D_GPIO6_PP_OD   0xE6

/* Logical Device Numbers (LDN). */
#define NCT5104D_FDC        0x00 /* FDC - not pinned out */
#define NCT5104D_SP1        0x02 /* UARTA */
#define NCT5104D_SP2        0x03 /* UARTB */
#define NCT5104D_GPIO_PP_OD 0x0F /* GPIO Push-Pull / Open drain select */
#define NCT5104D_SP3        0x10 /* UARTC */
#define NCT5104D_SP4        0x11 /* UARTD */
#define NCT5104D_PORT80     0x14 /* PORT 80 */

/* Virtual Logical Device Numbers (LDN) */
#define NCT5104D_GPIO_V 0x07 /* GPIO - 0,1,6 Interface */
#define NCT5104D_GPIO_WDT_V 0x08 /* GPIO/WDT Interface */

/* Virtual devices sharing the enables are encoded as follows:
	VLDN = baseLDN[7:0] | [10:8] bitpos of enable in 0x30 of baseLDN
*/
#define NCT5104D_GPIO0 ((0 << 8) | NCT5104D_GPIO_V)
#define NCT5104D_GPIO1 ((1 << 8) | NCT5104D_GPIO_V)
#define NCT5104D_GPIO6 ((6 << 8) | NCT5104D_GPIO_V)

#define NCT5104D_GPIO_WDT ((0 << 8) | NCT5104D_GPIO_WDT_V)
#define NCT5104D_GPIO_IO  ((1 << 8) | NCT5104D_GPIO_WDT_V)

void nct5104d_enable_uartd(pnp_devfn_t dev);

#endif /* SUPERIO_NUVOTON_NCT5104D_H */
