/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Vladimir Serbinenko
 * Copyright (C) 2017 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <halt.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <arch/acpi.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/raminit_native.h>

#include "superio.h"
#include "thermal.h"

#if IS_ENABLED(CONFIG_DISABLE_UART_ON_TESTPADS)
#define DEBUG_UART_EN 0
#else
#define DEBUG_UART_EN COMA_LPC_EN
#endif

void pch_enable_lpc(void)
{
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN,
			CNF2_LPC_EN | DEBUG_UART_EN);
	/* Decode SuperIO 0x0a00 */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), LPC_GEN1_DEC, 0x00fc0a01);
}

void mainboard_rcba_config(void)
{
	/* Disable devices */
	RCBA32(FD) |= PCH_DISABLE_P2P | PCH_DISABLE_XHCI;

#if IS_ENABLED(CONFIG_USE_NATIVE_RAMINIT)
	/* Enable Gigabit Ethernet */
	if (RCBA32(BUC) & PCH_DISABLE_GBE) {
		RCBA32(BUC) &= ~PCH_DISABLE_GBE;
		/* Datasheet says clearing the bit requires a reset after */
		printk(BIOS_DEBUG, "Enabled gigabit ethernet, reset once.\n");
		outb(0xe, 0xcf9);
		halt();
	}
#endif

	/* Set "mobile" bit in MCH (which makes sense layout-wise). */
	/* Note sure if this has any effect at all though. */
	MCHBAR32(0x0004) |= 0x00001000;
	MCHBAR32(0x0104) |= 0x00001000;
}

void mainboard_early_init(int s3resume)
{
}

static const u16 hwm_initvals[] = {
	HWM_BANK(0),
	HWM_INITVAL(0xae, 0x01), /* Enable PECI Agent0 */

	HWM_BANK(7), /* PECI */
	HWM_INITVAL(0x01, 0x95), /* Enable PECI */
	HWM_INITVAL(0x03, 0x10), /* Enable Agent 0 */
	/*
	 * PECI temperatures are negative, going up to 0.
	 * 0 represents the maximum allowable junction temperature, Tjmax.
	 * There is also Tcontrol, which is the temperature at which the
	 * system cooling should run at full speed.
	 * Since the NCT5577D fan control only supports positive values,
	 * Tbase0 is used as an offset.
	 */
	HWM_INITVAL(0x09, CRITICAL_TEMPERATURE), /* Tbase0 */

	HWM_BANK(2), /* CPUFAN control */
	HWM_INITVAL(0x00, 0x0c), /* PECI Agent 0 as CPUFAN monitoring source */
	HWM_INITVAL(0x01, 50),   /* Target temperature */
	HWM_INITVAL(0x02, 0x40), /* Enable Smart Fan IV mode */
	HWM_INITVAL(0x03, 0x01), /* Step-up time */
	HWM_INITVAL(0x04, 0x01), /* Step-down time */
	HWM_INITVAL(0x05, 0x10), /* Stop PWM value */
	HWM_INITVAL(0x06, 0x20), /* Start PWM value */
	HWM_INITVAL(0x21, 45),   /* Smart Fan IV Temp1 */
	HWM_INITVAL(0x22, 46),   /* Smart Fan IV Temp2 */
	HWM_INITVAL(0x23, 47),   /* Smart Fan IV Temp3 */
	HWM_INITVAL(0x24, PASSIVE_TEMPERATURE), /* Smart Fan IV Temp4 */
	HWM_INITVAL(0x27, 0x01), /* Smart Fan IV PWM1 */
	HWM_INITVAL(0x28, 0x02), /* Smart Fan IV PWM2 */
	HWM_INITVAL(0x29, 0x03), /* Smart Fan IV PWM3 */
	HWM_INITVAL(0x2a, 0xff), /* Smart Fan IV PWM4 */
	/* Smart Fan IV Critical temp */
	HWM_INITVAL(0x35, CRITICAL_TEMPERATURE),
	HWM_INITVAL(0x38, 3),    /* Smart Fan IV Critical temp tolerance */
	HWM_INITVAL(0x39, 0x81), /* Enable SYSTIN weight value */
	HWM_INITVAL(0x3a, 1),    /* SYSTIN temperature step */
	HWM_INITVAL(0x3b, 2),    /* SYSTIN step tolerance */
	HWM_INITVAL(0x3c, 1),    /* SYSTIN weight step */
	HWM_INITVAL(0x3d, 40),   /* SYSTIN temperature base */
	HWM_INITVAL(0x3e, 0x00), /* SYSTIN fan duty base */

	HWM_BANK(0),
};

static void hwm_init(void)
{
	/* Set up fan control */
	for (int i = 0; i < ARRAY_SIZE(hwm_initvals); i++)
		HWM_WRITE_INITVAL(hwm_initvals[i]);
}

static const u16 superio_initvals[] = {
	/* Global config registers */
	SUPERIO_INITVAL(0x1a, 0x02),
	SUPERIO_INITVAL(0x1b, 0x6a),
	SUPERIO_INITVAL(0x27, 0x80),
#if IS_ENABLED(CONFIG_DISABLE_UART_ON_TESTPADS)
	SUPERIO_INITVAL(0x2a, 0x80),
#else
	SUPERIO_INITVAL(0x2a, 0x00),
#endif
	SUPERIO_INITVAL(0x2c, 0x00),

	SUPERIO_BANK(2), /* UART A */
	SUPERIO_INITVAL(0x30, 0x01),
	SUPERIO_INITVAL(0x60, 0x03),
	SUPERIO_INITVAL(0x61, 0xf8),
	SUPERIO_INITVAL(0x70, 0x04),

	SUPERIO_BANK(7), /* GPIO config */
	SUPERIO_INITVAL(0x30, 0x01),
	SUPERIO_INITVAL(0xe0, 0xcf),
	SUPERIO_INITVAL(0xe1, 0x0f),
	SUPERIO_INITVAL(0xe4, 0xed),
	SUPERIO_INITVAL(0xe5, 0x4d),
	SUPERIO_INITVAL(0xec, 0x30),
	SUPERIO_INITVAL(0xee, 0xff),

	SUPERIO_BANK(8),
	SUPERIO_INITVAL(0x30, 0x0a),
	SUPERIO_INITVAL(0x60, GPIO_PORT >> 8),
	SUPERIO_INITVAL(0x61, GPIO_PORT & 0xff),

	SUPERIO_BANK(9),
	SUPERIO_INITVAL(0x30, 0x8c),
	SUPERIO_INITVAL(0xe1, 0x90),

	SUPERIO_BANK(0xa),
	SUPERIO_INITVAL(0xe4, 0x20),
	SUPERIO_INITVAL(0xe6, 0x4c),

	SUPERIO_BANK(0xb), /* HWM & LED */
	SUPERIO_INITVAL(0x30, 0x01),
	SUPERIO_INITVAL(0x60, HWM_PORT >> 8),
	SUPERIO_INITVAL(0x61, HWM_PORT & 0xff),
	SUPERIO_INITVAL(0xf7, 0x67),
	SUPERIO_INITVAL(0xf8, 0x60),

	SUPERIO_BANK(0x16),
	SUPERIO_INITVAL(0x30, 0x00),
};

static void superio_init(void)
{
	SUPERIO_UNLOCK;
	for (int i = 0; i < ARRAY_SIZE(superio_initvals); i++)
		SUPERIO_WRITE_INITVAL(superio_initvals[i]);
	SUPERIO_LOCK;
}

void mainboard_config_superio(void)
{
	superio_init();
	hwm_init();
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x51, id_only);
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
#define USB_CONFIG(enabled, current, ocpin) { enabled, current, ocpin }
#include "usb.h"
};
