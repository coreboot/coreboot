/* SPDX-License-Identifier: GPL-2.0-or-later */

/* Both NCT6687DD and NCT6687DF package variants are supported. */

#ifndef SUPERIO_NUVOTON_NCT6687D_H
#define SUPERIO_NUVOTON_NCT6687D_H

/* Logical Device Numbers (LDN). */
#define NCT6687D_PP		0x01 /* Parallel port */
#define NCT6687D_SP1		0x02 /* Com1 */
#define NCT6687D_SP2		0x03 /* Com2 & IR */
#define NCT6687D_KBC		0x05 /* PS/2 keyboard and mouse */
#define NCT6687D_CIR		0x06
#define NCT6687D_GPIO_0_7	0x07 /* GPIO0 - GPIO7 */
#define NCT6687D_P80_UART	0x08 /* Port 0x 80 UART*/
#define NCT6687D_GPIO_8_9_AF	0x09 /* GPIO8 - GPIO9, GPIO1 - GPIO8 Alternate Function */
#define NCT6687D_ACPI		0x0A
#define NCT6687D_EC		0x0B /* EC space*/
#define NCT6687D_RTC		0x0C
#define NCT6687D_SLEEP_PWR	0x0D /* Deep Sleep, Power Fault */
#define NCT6687D_TACH_PWM	0x0E /* TACHIN/PWMOUT Assignment */
#define NCT6687D_FREG		0x0F /* Function Register */

#endif /* SUPERIO_NUVOTON_NCT6687D_H */
