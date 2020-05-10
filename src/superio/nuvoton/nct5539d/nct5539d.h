/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT5539D_H
#define SUPERIO_NUVOTON_NCT5539D_H

/* Logical Device Numbers (LDN). */
#define NCT5539D_SP1			0x02 /* UART A */
#define NCT5539D_KBC			0x05 /* Keyboard Controller */
#define NCT5539D_CIR			0x06 /* Consumer IR */
#define NCT5539D_GPIO78			0x07 /* GPIO 7 & 8 */
#define NCT5539D_WDT1_WDT3_GPIO0	0x08 /* WDT1, WDT3, GPIO 0 & KBC P20 */
#define NCT5539D_GPIO2345		0x09 /* GPIO 2, 3, 4 & 5 */
#define NCT5539D_ACPI			0x0A /* ACPI */
#define NCT5539D_HWM_FPLED		0x0B /* HW Monitor, Front Panel LED */
#define NCT5539D_WDT2			0x0D /* WDT2 */
#define NCT5539D_CIRWUP			0x0E /* CIR Wake-Up */
#define NCT5539D_GPIO_PP_OD		0x0F /* GPIO Push-Pull/Open-Drain */
#define NCT5539D_GPIO_PSO		0x11 /* GPIO, RI PSOUT Wake-Up Status */
#define NCT5539D_SWEC			0x12 /* SW Error Control */
#define NCT5539D_FLED			0x15 /* Fading LED */
#define NCT5539D_DS			0x16 /* Deep Sleep */

/* Virtual LDNs */
#define NCT5539D_WDT1		((0 << 8) | NCT5539D_WDT1_WDT3_GPIO0)
#define NCT5539D_WDT3		((4 << 8) | NCT5539D_WDT1_WDT3_GPIO0)
#define NCT5539D_GPIOBASE	((3 << 8) | NCT5539D_WDT1_WDT3_GPIO0)
#define NCT5539D_GPIO0		((1 << 8) | NCT5539D_WDT1_WDT3_GPIO0)
#define NCT5539D_GPIO2		((0 << 8) | NCT5539D_GPIO2345)
#define NCT5539D_GPIO3		((1 << 8) | NCT5539D_GPIO2345)
#define NCT5539D_GPIO4		((2 << 8) | NCT5539D_GPIO2345)
#define NCT5539D_GPIO5		((3 << 8) | NCT5539D_GPIO2345)
#define NCT5539D_GPIO7		((1 << 8) | NCT5539D_GPIO78)
#define NCT5539D_GPIO8		((2 << 8) | NCT5539D_GPIO78)
#define NCT5539D_DS5		((0 << 8) | NCT5539D_DS)
#define NCT5539D_DS3		((1 << 8) | NCT5539D_DS)

#endif /* SUPERIO_NUVOTON_NCT5539D_H */
