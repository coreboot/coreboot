/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <cbfs.h>
#include <gpio.h>
#include <smbios.h>
#include <variant/gpio.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <drivers/generic/bayhub/bh720.h>

uint32_t sku_id(void)
{
	static int sku = -1;

	if (sku == -1)
		sku = google_chromeec_get_sku_id();

	return sku;
}

uint8_t variant_board_sku(void)
{
	return sku_id();
}

void variant_mainboard_suspend_resume(void)
{
	/* Enable backlight - GPIO 133 active low */
	gpio_set(GPIO_133, 0);
}

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

	/* Set Base clock to 200MHz(PCR 0x304[31:16] = 0x2510) */
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_READ | BH720_PCR_DrvStrength_PLL);
	bh720_pcr_data = read32((void *)(sdbar + BH720_MEM_RW_DATA));
	bh720_pcr_data &= 0x0000FFFF;
	bh720_pcr_data |= 0x2510 << 16;
	write32((void *)(sdbar + BH720_MEM_RW_DATA), bh720_pcr_data);
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_WRITE | BH720_PCR_DrvStrength_PLL);

	/* Use PLL Base clock PCR 0x3E4[22] = 1 */
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_READ | BH720_PCR_CSR);
	bh720_pcr_data = read32((void *)(sdbar + BH720_MEM_RW_DATA));
	write32((void *)(sdbar + BH720_MEM_RW_DATA),
		bh720_pcr_data | BH720_PCR_CSR_EMMC_MODE_SEL);
	write32((void *)(sdbar + BH720_MEM_RW_ADR),
		BH720_MEM_RW_WRITE | BH720_PCR_CSR);

	/* Disable Memory Access */
	write32((void *)(sdbar + BH720_MEM_RW_DATA), 0x80000001);
	write32((void *)(sdbar + BH720_MEM_RW_ADR), 0x800000D0);
	write32((void *)(sdbar + BH720_MEM_ACCESS_EN), 0x80000000);
}

const char *smbios_mainboard_manufacturer(void)
{
	static char oem_bin_data[11];
	static const char *manuf;

	if (!CONFIG(USE_OEM_BIN))
		return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	if (manuf)
		return manuf;

	if (cbfs_load("oem.bin", oem_bin_data, sizeof(oem_bin_data) - 1))
		manuf = &oem_bin_data[0];
	else
		manuf = CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	return manuf;
}
