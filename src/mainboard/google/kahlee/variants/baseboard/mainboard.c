/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#include <ec/google/chromeec/ec.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <variant/gpio.h>
#include <device/pci.h>
#include <drivers/generic/bayhub/bh720.h>

uint8_t variant_board_sku(void)
{
	static int sku = -1;

	if (sku == -1)
		sku = google_chromeec_get_sku_id();

	return sku;
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
void variant_mainboard_suspend_resume(void)
{
	/* Enable backlight - GPIO 133 active low */
	gpio_set(GPIO_133, 0);
}
#endif

void bh720_driving_strength(struct device *dev)
{
	u32 sdbar;
	u32 bh720_pcr_data;

	sdbar = pci_read_config32(dev, PCI_BASE_ADDRESS_1);

	/* Enable Memory Access Function */
	write32((void *)(sdbar + BH720_MEM_ACCESS_EN), 0x40000000);
	write32((void *)(sdbar + BH720_MEM_RW_DATA), 0x80000000);
	write32((void *)(sdbar + BH720_MEM_RW_ADR), 0x800000D0);

	/* Read current EMMC 1.8V CLK/DATA,CMD driving strength */
	write32((void *)(sdbar + BH720_MEM_RW_ADR), 0x40000304);
	bh720_pcr_data = read32((void *)(sdbar + BH720_MEM_RW_DATA));

	/* set EMMC 1.8V CLK/DATA,CMD the max level */
	write32((void *)(sdbar + BH720_MEM_RW_DATA),
		bh720_pcr_data | (BH720_PCR_CLK_DRV_MAX << 4) |
		(BH720_PCR_DATA_CMD_DRV_MAX << 1));
	write32((void *)(sdbar + BH720_MEM_RW_ADR), 0x80000304);
}
