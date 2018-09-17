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

static uint8_t calc_oem_id(void)
{
	return variant_board_sku() / 0x10;
}

/* "oem.bin" in cbfs contains an array of records using the following structure. */
struct oem_mapping {
	uint8_t oem_id;
	char oem_name[10];
} __packed;

/* Local buffer to read "oem.bin" */
static char oem_bin_data[200];

const char *smbios_mainboard_manufacturer(void)
{
	uint8_t oem_id;
	const struct oem_mapping *oem_entry = (void *)&oem_bin_data;
	size_t oem_data_size;
	unsigned int i, oem_entries_count;
	static const char *manuf;

	if (!IS_ENABLED(CONFIG_USE_OEM_BIN))
		return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	if (manuf)
		return manuf;

	oem_data_size = cbfs_boot_load_file("oem.bin", oem_bin_data,
					    sizeof(oem_bin_data),
					    CBFS_TYPE_RAW);
	oem_id = calc_oem_id();
	oem_entries_count = oem_data_size / sizeof(*oem_entry);
	for (i = 0; i < oem_entries_count; i++) {
		if (oem_id == oem_entry->oem_id) {
			manuf = oem_entry->oem_name;
			break;
		}
		oem_entry++;
	}

	if (manuf == NULL)
		manuf = CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	return manuf;
}
