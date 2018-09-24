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
#include <cbfs.h>
#include <gpio.h>
#include <smbios.h>
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

void board_bh720(struct device *dev)
{
	u32 sdbar;
	u32 bh720_pcr_data;

	sdbar = pci_read_config32(dev, PCI_BASE_ADDRESS_1);

	/* Enable Memory Access Function */
	write32((void *)(sdbar + BH720_MEM_ACCESS_EN), 0x40000000);
	write32((void *)(sdbar + BH720_MEM_RW_DATA), 0x80000000);
	write32((void *)(sdbar + BH720_MEM_RW_ADR), 0x800000D0);

	/* Set EMMC VCCQ 1.8V PCR 0x308[4] */
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_READ | BH720_PCR_EMMC_SETTING);
	bh720_pcr_data = read32((void *)(sdbar + BH720_MEM_RW_DATA));
	write32((void *)(sdbar + BH720_MEM_RW_DATA),
		bh720_pcr_data | BH720_PCR_EMMC_SETTING_1_8V);
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_WRITE | BH720_PCR_EMMC_SETTING);

	/* Set Bayhub SD base CLK 50MHz: case#1 PCR 0x3E4[22] = 0 */
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_READ | BH720_PCR_CSR);
	bh720_pcr_data = read32((void *)(sdbar + BH720_MEM_RW_DATA));
	write32((void *)(sdbar + BH720_MEM_RW_DATA),
		bh720_pcr_data & ~BH720_PCR_CSR_EMMC_MODE_SEL);
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_WRITE | BH720_PCR_CSR);

	/* Disable Memory Access */
	write32((void *)(sdbar + BH720_MEM_RW_DATA), 0x80000001);
	write32((void *)(sdbar + BH720_MEM_RW_ADR), 0x800000D0);
	write32((void *)(sdbar + BH720_MEM_ACCESS_EN), 0x80000000);
}


const char *smbios_mainboard_manufacturer(void)
{
	static char oem_bin_data[10];
	static const char *manuf;

	if (!IS_ENABLED(CONFIG_USE_OEM_BIN))
		return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	if (manuf)
		return manuf;

	if (cbfs_boot_load_file("oem.bin", oem_bin_data,
					    sizeof(oem_bin_data) - 1,
					    CBFS_TYPE_RAW))
		manuf = &oem_bin_data[0];
	else
		manuf = CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	return manuf;
}
