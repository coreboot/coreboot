/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <stdint.h>
#include <pc80/mc146818rtc.h>
#include <bootmode.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8772f/it8772f.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <superio/smsc/lpc47n207/lpc47n207.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8772F_SP1)
#define GPIO_DEV   PNP_DEV(0x2e, IT8772F_GPIO)
#define EC_DEV     PNP_DEV(0x2e, IT8772F_EC)

void mainboard_late_rcba_config(void)
{
	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P1IP  WLAN   INTA -> PIRQB
	 * D28IP_P4IP  ETH0   INTB -> PIRQC
	 * D29IP_E1P   EHCI1  INTA -> PIRQD
	 * D26IP_E2P   EHCI2  INTA -> PIRQE
	 * D31IP_SIP   SATA   INTA -> PIRQF (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQG
	 * D31IP_TTIP  THRT   INTC -> PIRQH
	 * D27IP_ZIP   HDA    INTA -> PIRQG (MSI)
	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (INTA << D28IP_P1IP) | (INTC << D28IP_P3IP) |
		(INTB << D28IP_P4IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);

	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQF, PIRQG, PIRQH, PIRQA);
	DIR_ROUTE(D29IR, PIRQD, PIRQE, PIRQF, PIRQG);
	DIR_ROUTE(D28IR, PIRQB, PIRQC, PIRQD, PIRQE);
	DIR_ROUTE(D27IR, PIRQG, PIRQH, PIRQA, PIRQB);
	DIR_ROUTE(D26IR, PIRQE, PIRQF, PIRQG, PIRQH);
	DIR_ROUTE(D25IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D22IR, PIRQA, PIRQB, PIRQC, PIRQD);
}

static void setup_sio_gpios(void)
{
	/*
	 * GPIO10 as USBPWRON12#
	 * GPIO12 as USBPWRON13#
	 */
	it8772f_gpio_setup(GPIO_DEV, 1, 0x05, 0x05, 0x00, 0x05, 0x05);

	/*
	 * GPIO22 as wake SCI#
	 */
	it8772f_gpio_setup(GPIO_DEV, 2, 0x04, 0x04, 0x00, 0x04, 0x04);

	/*
	 * GPIO32 as EXTSMI#
	 */
	it8772f_gpio_setup(GPIO_DEV, 3, 0x04, 0x04, 0x00, 0x04, 0x04);

	/*
	 * GPIO45 as LED_POWER#
	 */
	it8772f_gpio_led(GPIO_DEV, 4 /* set */, (0x1 << 5) /* select */,
		(0x1 << 5) /* polarity */, (0x1 << 5) /* 1 = pullup */,
		(0x1 << 5) /* output */, (0x1 << 5) /* 1 = Simple IO function */,
		SIO_GPIO_BLINK_GPIO45, IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);

	/*
	 * GPIO51 as USBPWRON8#
	 * GPIO52 as USBPWRON1#
	 */
	it8772f_gpio_setup(GPIO_DEV, 5, 0x06, 0x06, 0x00, 0x06, 0x06);
	it8772f_gpio_setup(GPIO_DEV, 6, 0x00, 0x00, 0x00, 0x00, 0x00);
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* TODO: Confirm if nortbridge_fill_pei_data() gets .system_type right (should be 0) */
}

void bootblock_mainboard_early_init(void)
{
	if (CONFIG(DRIVERS_UART_8250IO))
		try_enabling_LPC47N207_uart();

	setup_sio_gpios();

	/* Early SuperIO setup */
	ite_ac_resume_southbridge(EC_DEV);
	ite_kill_watchdog(GPIO_DEV);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
