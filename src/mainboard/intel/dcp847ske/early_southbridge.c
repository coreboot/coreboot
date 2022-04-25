/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <stdint.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>

#include "superio.h"
#include "thermal.h"

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
#if CONFIG(DISABLE_UART_ON_TESTPADS)
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

void bootblock_mainboard_early_init(void)
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
