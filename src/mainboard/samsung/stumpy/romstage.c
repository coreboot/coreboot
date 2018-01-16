/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <bootmode.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8772f/it8772f.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <halt.h>
#include <security/tpm/tis.h>
#if IS_ENABLED(CONFIG_DRIVERS_UART_8250IO)
#include <superio/smsc/lpc47n207/lpc47n207.h>
#endif

/* Stumpy USB Reset Disable defined in cmos.layout */
#if IS_ENABLED(CONFIG_USE_OPTION_TABLE)
#include "option_table.h"
#define CMOS_USB_RESET_DISABLE  (CMOS_VSTART_stumpy_usb_reset_disable >> 3)
#else
#define CMOS_USB_RESET_DISABLE  (400 >> 3)
#endif
#define USB_RESET_DISABLE_MAGIC (0xdd) /* Disable if set to this */

#define DUMMY_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, IT8772F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8772F_GPIO)

void pch_enable_lpc(void)
{
	/* Set COM1/COM2 decode range */
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);

#if IS_ENABLED(CONFIG_DRIVERS_UART_8250IO)
	/* Enable SuperIO + PS/2 Keyboard/Mouse + COM1 + lpc47n207 config*/
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF1_LPC_EN | KBC_LPC_EN |\
			   CNF2_LPC_EN | COMA_LPC_EN);

	/* map full 256 bytes at 0x1600 to the LPC bus */
	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0xfc1601);

	try_enabling_LPC47N207_uart();
#else
	/* Enable SuperIO + PS/2 Keyboard/Mouse */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF1_LPC_EN | KBC_LPC_EN);
#endif
}

void mainboard_rcba_config(void)
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

	/* Enable IOAPIC (generic) */
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void) RCBA16(OIC);
}

static void setup_sio_gpios(void)
{
	/*
	 * GPIO10 as USBPWRON12#
	 * GPIO12 as USBPWRON13#
	 */
	it8772f_gpio_setup(DUMMY_DEV, 1, 0x05, 0x05, 0x00, 0x05, 0x05);

	/*
	 * GPIO22 as wake SCI#
	 */
	it8772f_gpio_setup(DUMMY_DEV, 2, 0x04, 0x04, 0x00, 0x04, 0x04);

	/*
	 * GPIO32 as EXTSMI#
	 */
	it8772f_gpio_setup(DUMMY_DEV, 3, 0x04, 0x04, 0x00, 0x04, 0x04);

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
	it8772f_gpio_setup(DUMMY_DEV, 5, 0x06, 0x06, 0x00, 0x06, 0x06);
	it8772f_gpio_setup(DUMMY_DEV, 6, 0x00, 0x00, 0x00, 0x00, 0x00);
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	struct pei_data pei_data_template = {
		.pei_version = PEI_VERSION,
		.mchbar = (uintptr_t)DEFAULT_MCHBAR,
		.dmibar = (uintptr_t)DEFAULT_DMIBAR,
		.epbar = DEFAULT_EPBAR,
		.pciexbar = CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar = SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = CONFIG_HPET_ADDRESS,
		.rcba = (uintptr_t)DEFAULT_RCBABASE,
		.pmbase = DEFAULT_PMBASE,
		.gpiobase = DEFAULT_GPIOBASE,
		.thermalbase = 0xfed08000,
		.system_type = 0, // 0 Mobile, 1 Desktop/Server
		.tseg_size = CONFIG_SMM_TSEG_SIZE,
		.spd_addresses = { 0xa0, 0x00,0xa4,0x00 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 0,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1333,
		.usb_port_config = {
			{ 1, 0, 0x0080 }, /* P0: Front port  (OC0) */
			{ 1, 1, 0x0040 }, /* P1: Back port   (OC1) */
			{ 1, 0, 0x0040 }, /* P2: MINIPCIE1   (no OC) */
			{ 1, 0, 0x0040 }, /* P3: MMC         (no OC) */
			{ 1, 2, 0x0080 }, /* P4: Front port  (OC2) */
			{ 0, 0, 0x0000 }, /* P5: Empty */
			{ 0, 0, 0x0000 }, /* P6: Empty */
			{ 0, 0, 0x0000 }, /* P7: Empty */
			{ 1, 4, 0x0040 }, /* P8: Back port   (OC4) */
			{ 1, 4, 0x0040 }, /* P9: MINIPCIE3   (no OC) */
			{ 1, 4, 0x0040 }, /* P10: BLUETOOTH  (no OC) */
			{ 0, 4, 0x0000 }, /* P11: Empty */
			{ 1, 6, 0x0040 }, /* P12: Back port  (OC6) */
			{ 1, 5, 0x0040 }, /* P13: Back port  (OC5) */
		},
	};
	*pei_data = pei_data_template;
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled power  usb oc pin  */
	{ 1, 1, 0 }, /* P0: Front port  (OC0) */
	{ 1, 0, 1 }, /* P1: Back port   (OC1) */
	{ 1, 0, -1 }, /* P2: MINIPCIE1   (no OC) */
	{ 1, 0, -1 }, /* P3: MMC         (no OC) */
	{ 1, 1, 2 }, /* P4: Front port  (OC2) */
	{ 0, 0, -1 }, /* P5: Empty */
	{ 0, 0, -1 }, /* P6: Empty */
	{ 0, 0, -1 }, /* P7: Empty */
	{ 1, 0, 4 }, /* P8: Back port   (OC4) */
	{ 1, 0, -1 }, /* P9: MINIPCIE3   (no OC) */
	{ 1, 0, -1 }, /* P10: BLUETOOTH  (no OC) */
	{ 0, 0, -1 }, /* P11: Empty */
	{ 1, 0, 6 }, /* P12: Back port  (OC6) */
	{ 1, 0, 5 }, /* P13: Back port  (OC5) */
};

void mainboard_early_init(int s3resume)
{
	init_bootmode_straps();
}

int mainboard_should_reset_usb(int s3resume)
{
	if (s3resume) {
		/*
		 * For Stumpy the back USB ports are reset on resume
		 * so default to resetting the controller to make the
		 * kernel happy.  There is a CMOS flag to disable the
		 * controller reset in case the kernel can tolerate
		 * the device power loss better in the future.
		 */
		u8 magic = cmos_read(CMOS_USB_RESET_DISABLE);
		if (magic == USB_RESET_DISABLE_MAGIC) {
			printk(BIOS_DEBUG, "USB Controller Reset Disabled\n");
			return 0;
		} else {
			printk(BIOS_DEBUG, "USB Controller Reset Enabled\n");
			return 1;
		}
	} else {
		/* Ensure USB reset on resume is enabled at boot */
		cmos_write(0, CMOS_USB_RESET_DISABLE);
		return 1;
	}
}

void mainboard_config_superio(void)
{
	setup_sio_gpios();

	/* Early SuperIO setup */
	it8772f_ac_resume_southbridge(DUMMY_DEV);
	ite_kill_watchdog(GPIO_DEV);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
