/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT5525D_H
#define SUPERIO_NUVOTON_NCT5525D_H

/* Logical Device Numbers (LDN). */
#define NCT5525D_SP1		0x02 /* UART A */
#define NCT5525D_KBC		0x05 /* Keyboard Controller */
#define NCT5525D_CIR		0x06 /* Consumer IR */
#define NCT5525D_GPIO056	0x07 /* GPIO 0, 5 & 6 */
#define NCT5525D_WDT1_GPIO	0x08 /* WDT1, GPIO direct access, multi-function select */
#define NCT5525D_GPIO89AB	0x09 /* GPIO 8, 9, A & B, GPIO reset select */
#define NCT5525D_ACPI		0x0A /* ACPI */
#define NCT5525D_HWM_FPLED	0x0B /* HW Monitor, SMBus master, Front Panel LED */
#define NCT5525D_WDT2		0x0D /* WDT2 */
#define NCT5525D_CIRWUP		0x0E /* CIR Wake-Up, eSPI */
#define NCT5525D_GPIO_PP_OD	0x0F /* GPIO Push-Pull/Open-Drain, I2C to Port 80 */
#define NCT5525D_SP3		0x10 /* UART C */
#define NCT5525D_PORT80_IR	0x14 /* Port 80 UART, IR */
#define NCT5525D_FLED		0x15 /* Fading LED */
#define NCT5525D_DS		0x16 /* Deep Sleep */

/* Virtual LDNs, selecting a bit in CR30 of the LDN */
#define NCT5525D_GPIO0		((0 << 8) | NCT5525D_GPIO056)
#define NCT5525D_GPIO5		((5 << 8) | NCT5525D_GPIO056)
#define NCT5525D_GPIO6		((6 << 8) | NCT5525D_GPIO056)
#define NCT5525D_WDT1		((0 << 8) | NCT5525D_WDT1_GPIO)
#define NCT5525D_GPIOBASE	((1 << 8) | NCT5525D_WDT1_GPIO)
#define NCT5525D_GPIO8		((0 << 8) | NCT5525D_GPIO89AB)
#define NCT5525D_GPIO9		((1 << 8) | NCT5525D_GPIO89AB)
#define NCT5525D_GPIOA		((2 << 8) | NCT5525D_GPIO89AB)
#define NCT5525D_GPIOB		((3 << 8) | NCT5525D_GPIO89AB)
#define NCT5525D_DS5		((0 << 8) | NCT5525D_DS)

#define NCT5525D_CHIP_ID	0xd290
#define NCT5525D_CHIP_ID_MASK	0xfff0

/* Global configuration registers */
#define NCT5525D_CR_MFS_1A	0x1a /* Pin 39 function */
#define NCT5525D_CR_MFS_1B	0x1b /* Pin 39/40, 46/48 function */
#define NCT5525D_CR_MFS_1C	0x1c /* Pin 39, 57-64 UART C function */
#define NCT5525D_CR_CHIP_ID_HI	0x20
#define NCT5525D_CR_CHIP_ID_LO	0x21
#define NCT5525D_CR_PWR_DOWN	0x22 /* AUXFANOUT2 output type */
#define NCT5525D_CR_GLOBAL_24	0x24 /* SYSFANOUT/CPUFANOUT output type, beep */
#define NCT5525D_CR_GLOBAL_2A	0x2a /* Pin 46/48 function */
#define NCT5525D_CR_UART_OPT	0x2c /* Pin 14-22 UART A function */

/* UART pin modes, CR1C[3:2] for UART C and CR2C[1:0] for UART A */
#define NCT5525D_UART_PINS_GPIO		0x0
#define NCT5525D_UART_PINS_RS422	0x1
#define NCT5525D_UART_PINS_RS485	0x2
#define NCT5525D_UART_PINS_RS232	0x3

#endif /* SUPERIO_NUVOTON_NCT5525D_H */
