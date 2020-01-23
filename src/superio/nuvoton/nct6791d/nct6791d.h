/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_NUVOTON_NCT6791D_H
#define SUPERIO_NUVOTON_NCT6791D_H

/* Logical Device Numbers (LDN). */
#define NCT6791D_PP			0x01 /* Parallel port */
#define NCT6791D_SP1			0x02 /* UART A */
#define NCT6791D_SP2			0x03 /* UART B, IR */
#define NCT6791D_KBC			0x05 /* Keyboard Controller */
#define NCT6791D_CIR			0x06 /* Consumer IR */
#define NCT6791D_GPIO678		0x07 /* GPIO 6, 7 & 8 */
#define NCT6791D_WDT1_WDTMEM_GPIO01	0x08 /* WDT1, WDT_MEM, GPIO 0 & 1 */
#define NCT6791D_GPIO2345		0x09 /* GPIO 2, 3, 4 & 5 */
#define NCT6791D_ACPI			0x0A /* ACPI */
#define NCT6791D_HWM_FPLED		0x0B /* HW Monitor, Front Panel LED */
#define NCT6791D_BCLK_WDT2_WDTMEM	0x0D /* BCLK, WDT2, WDT_MEM */
#define NCT6791D_CIRWUP			0x0E /* CIR Wake-Up */
#define NCT6791D_GPIO_PP_OD		0x0F /* GPIO Push-Pull/Open-Drain */
#define NCT6791D_PORT80			0x14 /* Port 80 UART */
#define NCT6791D_DS			0x16 /* Deep Sleep */

/* Virtual LDNs */
#define NCT6791D_WDT1		((0 << 8) | NCT6791D_WDT1_WDTMEM_GPIO01)
#define NCT6791D_WDTMEM		((4 << 8) | NCT6791D_WDT1_WDTMEM_GPIO01)
#define NCT6791D_GPIOBASE	((3 << 8) | NCT6791D_WDT1_WDTMEM_GPIO01)
#define NCT6791D_GPIO0		((1 << 8) | NCT6791D_WDT1_WDTMEM_GPIO01)
#define NCT6791D_GPIO1		((7 << 8) | NCT6791D_WDT1_WDTMEM_GPIO01)
#define NCT6791D_GPIO2		((0 << 8) | NCT6791D_GPIO2345)
#define NCT6791D_GPIO3		((1 << 8) | NCT6791D_GPIO2345)
#define NCT6791D_GPIO4		((2 << 8) | NCT6791D_GPIO2345)
#define NCT6791D_GPIO5		((3 << 8) | NCT6791D_GPIO2345)
#define NCT6791D_GPIO6		((0 << 8) | NCT6791D_GPIO678)
#define NCT6791D_GPIO7		((1 << 8) | NCT6791D_GPIO678)
#define NCT6791D_GPIO8		((2 << 8) | NCT6791D_GPIO678)
#define NCT6791D_DS5		((0 << 8) | NCT6791D_DS)
#define NCT6791D_DS3		((1 << 8) | NCT6791D_DS)
#define NCT6791D_PCHDSW		((3 << 8) | NCT6791D_DS)
#define NCT6791D_DSWWOPT	((4 << 8) | NCT6791D_DS)
#define NCT6791D_DS3OPT		((5 << 8) | NCT6791D_DS)
#define NCT6791D_DSDSS		((6 << 8) | NCT6791D_DS)
#define NCT6791D_DSPU		((7 << 8) | NCT6791D_DS)

#endif /* SUPERIO_NUVOTON_NCT6791D_H */
