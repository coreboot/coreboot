/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <soc/soc.h>
#include <soc/spi.h>
#include <soc/uart.h>
#include <soc/gpio.h>
#include <spi_flash.h>
#include <console/console.h>
#include <fmap.h>
#include "mainboard.h"

void bootblock_mainboard_early_init(void)
{
	/* Route UART0 to CON1 */
	gpio_output(ELGON_GPIO_UART_SEL, 0);

	/* Turn off error LED */
	gpio_output(ELGON_GPIO_ERROR_LED, 0);

	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		if (!uart_is_enabled(CONFIG_UART_FOR_CONSOLE))
			uart_setup(CONFIG_UART_FOR_CONSOLE, CONFIG_TTYS0_BAUD);
	}
}

static void configure_spi_flash(void)
{
	/* The maximum SPI frequency for error-free transmission is at 30 MHz */
	spi_init_custom(0, // bus
			28000000, // speed Hz
			0, // idle low disabled
			0, // zero idle cycles between transfers
			0, // MSB first
			0, // Chip select 0
			1); // assert is high

	/* Route SPI to SoC */
	gpio_output(ELGON_GPIO_SPI_MUX, 1);
}

/**
 * Handle flash write protection.
 * This code verifies the write-protection on each boot.
 * Enabling the write protection does only run on the first boot.
 * An error is fatal as it breaks the Chain Of Trust.
 */
static void protect_ro_rgn_spi_flash(void)
{
	const struct spi_flash *flash = boot_device_spi_flash();
	const char *fmapname = "WP_RO";
	struct region ro_rgn;

	if (fmap_locate_area(fmapname, &ro_rgn)) {
		printk(BIOS_ERR, "%s: No %s FMAP section.\n", __func__,
			fmapname);
		die("Can't verify flash protections!");
	}

	u8 reg8 = 0;
	spi_flash_status(flash, &reg8);

	/* Check if SRP0 is set and RO region is protected */
	if (!(reg8 & 0x80) ||
	    spi_flash_is_write_protected(flash, &ro_rgn) != 1) {
		printk(BIOS_WARNING, "%s: FMAP section %s is not write-protected\n",
			 __func__, fmapname);

		/*
		* Need to protect flash region :
		* WP_RO read only and use /WP pin
		* non-volatile programming
		*/
		if (spi_flash_set_write_protected(flash, &ro_rgn, 1,
		    SPI_WRITE_PROTECTION_PIN) != 0)
			die("Failed to write-protect WP_RO region!");
	}
	printk(BIOS_INFO, "%s: FMAP section %s is write-protected\n",
	       __func__, fmapname);
}

void bootblock_mainboard_init(void)
{
	configure_spi_flash();
	protect_ro_rgn_spi_flash();
}
