/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

static const struct soc_amd_gpio fp_spi_gpio_table[] = {
	/* SOC_CLK_FPMCU_R */
	PAD_NF(GPIO_70, SPI2_CLK, PULL_NONE),
	/* SOC_CLK_FPMCU_R_L */
	PAD_NF(GPIO_75, SPI2_CS1_L, PULL_NONE),
	/* SPI_SOC_DO_FPMCU_DI_R */
	PAD_NF(GPIO_104, SPI2_DAT0, PULL_NONE),
	/* SPI_SOC_DI_FPMCU_DO_R */
	PAD_NF(GPIO_105, SPI2_DAT1, PULL_NONE),
};

static const struct soc_amd_gpio fp_uart_gpio_table[] = {
	/* UART1_TXD / FPMCU */
	PAD_NF(GPIO_140, UART1_TXD, PULL_NONE),
	/* UART1_RXD  / FPMCU */
	PAD_NF(GPIO_142, UART1_RXD, PULL_NONE),
};

static void fw_config_handle(void *unused)
{
	if (!fw_config_is_provisioned() || fw_config_probe(FW_CONFIG(FP, DISABLED))) {
		return;
	}
	if (fw_config_probe(FW_CONFIG(FP, SPI))) {
		printk(BIOS_INFO, "Enabling SPI FP.\n");
		gpio_configure_pads(fp_spi_gpio_table, ARRAY_SIZE(fp_spi_gpio_table));
	} else if (fw_config_probe(FW_CONFIG(FP, UART))) {
		printk(BIOS_INFO, "Enabling UART FP.\n");
		gpio_configure_pads(fp_uart_gpio_table, ARRAY_SIZE(fp_uart_gpio_table));
	} else {
		printk(BIOS_INFO, "Invalid Fingerprint Setting, leaving FP disabled.");
	}
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
