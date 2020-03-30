/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

/*
 * Datasheet:
 *  - Name: F81803A
 */

#ifndef SUPERIO_FINTEK_F81803_H
#define SUPERIO_FINTEK_F81803_H

#define LDN_REG				0x07	/* Not defined under PNP */
/* Logical Device Numbers (LDN) */
  #define F81803A_SP1			0x01	/* UART1 */
  #define F81803A_SP2			0x02	/* UART2 */
  #define F81803A_HWM			0x04	/* Hardware Monitor */
  #define F81803A_KBC			0x05	/* Keyboard/Mouse */
  #define F81803A_GPIO			0x06	/* General Purpose I/O (GPIO) */
  #define F81803A_WDT			0x07	/* Watch Dog Timer */
  #define F81803A_PME			0x0a	/* Power Management Events (PME) */

/* Global Control Registers */
#define CLOCK_SELECT_REG		0x26
#define   FUNC_PROG_SELECT		(1<<3)
#define PORT_SELECT_REG			0x27

#define TSI_LEVEL_SELECT_REG		0x28	/* FUNC_PROG_SEL = 0 */
#define TSI_PIN_SELECT_REG		0x28	/* FUNC_PROG_SEL = 1 */
#define MULTI_FUNC_SEL_REG1		0x29
#define MULTI_FUNC_SEL_REG2		0x2A
#define MULTI_FUNC_SEL_REG3		0x2B
#define MULTI_FUNC_SEL_REG		0x2C
#define WAKEUP_CONTROL_REG		0x2d

/* LUN A - PME, ACPI, ERP */
#define PME_DEVICE_ENABLE_REG		0x30
#define   PME_ENABLE			(1<<0)
#define PME_ERP_ENABLE_REG		0xE0
#define   ERP_ENABLE			(1<<7)
#define   ERP_PME_EN			(1<<1)
#define   ERP_PSOUT_EN			(1<<0)
#define PME_ERP_CONTROL_1_REG		0xE1
#define PME_ERP_CONTROL_2_REG		0xE2
#define PME_ERP_PSIN_DEBOUNCE_REG	0xE3
#define PME_ERP_WAKEUP_ENABLE_REG	0xE8
#define PME_ERP_MODE_SELECT_REG		0xEC
#define PME_EVENT_ENABLE_1_REG		0xF0
#define PME_EVENT_STATUS_1_REG		0xF1
#define PME_EVENT_ENABLE_2_REG		0xF2
#define PME_EVENT_STATUS_2_REG		0xF3
#define PME_ACPI_CONTROL_1_REG		0xF4
#define PME_ACPI_CONTROL_2_REG		0xF5
#define PME_ACPI_CONTROL_3_REG		0xF6
#define PME_ACPI_CONTROL_4_REG		0xF7
#define PME_ACPI_CONTROL_5_REG		0xFB
#define PME_ACPI_CONTROL_6_REG		0xFC

#endif /* SUPERIO_FINTEK_F81803_H */
