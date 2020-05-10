/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT5572D
#define SUPERIO_NUVOTON_NCT5572D

/* Logical Device Numbers (LDN). */
#define NCT5572D_FDC		0x00
#define NCT5572D_PP		0x01
#define NCT5572D_SP1		0x02 /* Com1 */
#define NCT5572D_IR		0x03
#define NCT5572D_KBC		0x05
#define NCT5572D_CIR		0x06
#define NCT5572D_GPIO689_V	0x07
#define NCT5572D_WDT1		0x08
#define NCT5572D_GPIO235_V	0x09
#define NCT5572D_ACPI		0x0A
#define NCT5572D_HWM_TSI_FPLED	0x0B /* HW monitor/SB-TSI/front LED/deep S5 */
#define NCT5572D_PECI		0x0C
#define NCT5572D_SUSLED		0x0D /* marked as reserved, but described */
#define NCT5572D_CIRWKUP	0x0E /* CIR wakeup */
#define NCT5572D_GPIO_PP_OD	0x0F /* GPIO Push-Pull/Open drain select */

/* virtual LDN for GPIO */
#define NCT5572D_GPIO2		((0 << 8) | NCT5572D_GPIO235_V)
#define NCT5572D_GPIO3		((1 << 8) | NCT5572D_GPIO235_V)
#define NCT5572D_GPIO4		((2 << 8) | NCT5572D_GPIO235_V)
#define NCT5572D_GPIO5		((3 << 8) | NCT5572D_GPIO235_V)
#define NCT5572D_GPIO6		((1 << 8) | NCT5572D_GPIO689_V)
#define NCT5572D_GPIO7		((2 << 8) | NCT5572D_GPIO689_V)
#define NCT5572D_GPIO8		((3 << 8) | NCT5572D_GPIO689_V)
#define NCT5572D_GPIO9		((4 << 8) | NCT5572D_GPIO689_V)

#endif /* SUPERIO_NUVOTON_NCT5572D */
