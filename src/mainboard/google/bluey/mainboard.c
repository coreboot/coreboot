/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <device/device.h>
#include <gpio.h>
#include <soc/pcie.h>
#include <soc/qupv3_config_common.h>
#include <soc/qup_se_handlers_common.h>
#include "board.h"

bool mainboard_needs_pcie_init(void)
{
	/* Placeholder */
	return false;
}

static void display_startup(void)
{
	if (!display_init_required()) {
		printk(BIOS_INFO, "Skipping display init.\n");
		return;
	}

	/* TODO: add logic for display init */
}

static void mainboard_init(struct device *dev)
{
	gpi_firmware_load(QUP_0_GSI_BASE);
	gpi_firmware_load(QUP_1_GSI_BASE);
	gpi_firmware_load(QUP_2_GSI_BASE);

	/*
	 * Load console UART QUP firmware.
	 * This is required even if coreboot's serial output is disabled.
	 */
	if (!CONFIG(CONSOLE_SERIAL))
		qupv3_se_fw_load_and_init(QUPV3_2_SE5, SE_PROTOCOL_UART, FIFO);

	qupv3_se_fw_load_and_init(QUPV3_1_SE0, SE_PROTOCOL_I2C, MIXED); /* Touch I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE6, SE_PROTOCOL_UART, FIFO); /* BT UART */
	qupv3_se_fw_load_and_init(QUPV3_0_SE0, SE_PROTOCOL_I2C, MIXED); /* Trackpad I2C */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI))
		qupv3_se_fw_load_and_init(QUPV3_2_SE2, SE_PROTOCOL_SPI, MIXED); /* Fingerprint SPI */

	/*
	 * Deassert FPMCU reset. Power applied in romstage
	 * has now stabilized.
	 */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT))
		gpio_output(GPIO_FP_RST_L, 1);

	display_startup();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
