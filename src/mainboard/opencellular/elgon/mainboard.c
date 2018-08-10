/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-2018 Facebook, Inc.
 * Copyright 2003-2017 Cavium Inc. (support@cavium.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <device/device.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-hal/bdk-twsi.h>
#include <soc/gpio.h>
#include <delay.h>
#include <soc/uart.h>
#include <console/console.h>
#include <soc/clock.h>
#include <soc/gpio.h>
#include <soc/timer.h>
#include <soc/cpu.h>
#include <soc/sdram.h>
#include <soc/spi.h>
#include <spi_flash.h>
#include <fmap.h>
#include <libbdk-hal/bdk-config.h>

static void mainboard_print_info(void)
{
	struct region region;

	if (fmap_locate_area("WP_RO", &region) < 0) {
		printk(BIOS_ERR, "MB: Could not find region '%s'\n", "WP_RO");
	} else {
		const struct spi_flash *flash = boot_device_spi_flash();
		const bool prot = (flash != NULL) &&
			    (spi_flash_is_write_protected(flash, &region) == 1);
		printk(BIOS_INFO, "MB: WP_RO is %swrite protected\n",
		       prot ? "" : "not ");
	}

	printk(BIOS_INFO, "MB: trusted boot    : %s\n",
	       gpio_strap_value(10) ? "yes" : "no");

	const size_t boot_method = gpio_strap_value(0) |
		(gpio_strap_value(1) << 1) |
		(gpio_strap_value(2) << 2) |
		(gpio_strap_value(3) << 3);

	printk(BIOS_INFO, "MB: boot method     : ");
	switch (boot_method) {
	case 0x2:
	case 0x3:
		printk(BIOS_INFO, "EMMC\n");
		break;
	case 0x5:
	case 0x6:
		printk(BIOS_INFO, "SPI\n");
		break;
	case 0x8:
		printk(BIOS_INFO, "REMOTE\n");
		break;
	case 0xc:
	case 0xd:
		printk(BIOS_INFO, "PCIe\n");
		break;
	default:
		printk(BIOS_INFO, "unknown\n");
	}

	printk(BIOS_INFO, "MB: REFclk          : %llu MHz\n",
	       thunderx_get_ref_clock() / 1000000ULL);

	printk(BIOS_INFO, "MB: IOclk           : %llu MHz\n",
	       thunderx_get_io_clock() / 1000000ULL);

	printk(BIOS_INFO, "MB: COREclk         : %llu MHz\n",
	       thunderx_get_core_clock() / 1000000ULL);

	printk(BIOS_INFO, "MB: #CPU cores      : %zu\n",
	       cpu_get_num_available_cores());

	printk(BIOS_INFO, "MB: RAM             : %zu MiB\n",
		sdram_size_mb());

	printk(BIOS_INFO, "MB: SPIclk          : %llu kHz\n",
	       spi_get_clock(0) >> 10);
}

extern const struct bdk_devicetree_key_value devtree[];

static void mainboard_init(struct device *dev)
{
	size_t i;

	/* Init UARTs */
	for (i = 0; i < 4; i++) {
		if (!uart_is_enabled(i))
			uart_setup(i, 0);
	}

	/* Init timer */
	soc_timer_init();

	/* Init CPUs */
	for (i = 1; i < CONFIG_MAX_CPUS; i++)
		start_cpu(i, NULL);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;

	bdk_config_set_fdt(devtree);

	mainboard_print_info();
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
