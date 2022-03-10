/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"
#include <boardid.h>
#include <bootblock_common.h>
#include <device/device.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/qupv3_config_common.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qcom_qup_se.h>
#include <soc/usb/usb_common.h>
#include <soc/usb/snps_usb_phy.h>

static struct usb_board_data usb0_board_data = {
	.parameter_override_x0 = 0xe6,
	.parameter_override_x1 = 0x8b,
	.parameter_override_x2 = 0x16,
	.parameter_override_x3 = 0x03,
};

static void setup_usb(void)
{
	/* Assert EN_PP3300_HUB for those board variants that use it. */
	gpio_output(USB_HUB_LDO_EN, 1);

	setup_usb_host0(&usb0_board_data);
}

static void configure_sdhci(void)
{
	/* Program eMMC drive strength to 16/10/10 mA */
	write32((void *)SDC1_TLMM_CFG_ADDR, 0x9FE4);
	/* Program SD card drive strength to 16/10/10 mA */
	write32((void *)SDC2_TLMM_CFG_ADDR, 0x1FE4);
}

static void qi2s_configure_gpios(void)
{
	gpio_configure(GPIO_MI2S1_SCK, GPIO106_FUNC_MI2S1_SCK,
		GPIO_NO_PULL, GPIO_16MA, GPIO_OUTPUT);

	gpio_configure(GPIO_MI2S1_WS, GPIO108_FUNC_MI2S1_WS,
		GPIO_NO_PULL, GPIO_16MA, GPIO_OUTPUT);

	gpio_configure(GPIO_MI2S1_DATA0, GPIO107_FUNC_MI2S1_DATA0,
		GPIO_NO_PULL, GPIO_16MA, GPIO_OUTPUT);
}

static void mainboard_init(struct device *dev)
{
	/* Configure clock for eMMC */
	clock_configure_sdcc(1, 384 * MHz);
	/* Configure clock for SD card */
	clock_configure_sdcc(2, 50 * MHz);
	configure_sdhci();

	gpi_firmware_load(QUP_0_GSI_BASE);
	gpi_firmware_load(QUP_1_GSI_BASE);

	/*
	 * When coreboot firmware disables serial output,
	 * we still need to load console UART QUP FW for OS.
	 */
	if (!CONFIG(CONSOLE_SERIAL))
		qupv3_se_fw_load_and_init(QUPV3_0_SE5, SE_PROTOCOL_UART, FIFO);

	qupv3_se_fw_load_and_init(QUPV3_1_SE5, SE_PROTOCOL_I2C, MIXED);	 /* Touch I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE7, SE_PROTOCOL_UART, FIFO);  /* BT UART */

	if (CONFIG(BOARD_GOOGLE_HEROBRINE_REV0)) {
		/* Audio I2C */
		qupv3_se_fw_load_and_init(QUPV3_0_SE0, SE_PROTOCOL_I2C, MIXED);
		/* Trackpad I2C */
		qupv3_se_fw_load_and_init(QUPV3_0_SE1, SE_PROTOCOL_I2C, MIXED);
		/* SAR sensor I2C */
		qupv3_se_fw_load_and_init(QUPV3_0_SE2, SE_PROTOCOL_I2C, MIXED);
		/* Fingerprint SPI */
		qupv3_se_fw_load_and_init(QUPV3_1_SE3, SE_PROTOCOL_SPI, MIXED);
	} else if (CONFIG(BOARD_GOOGLE_SENOR) || CONFIG(BOARD_GOOGLE_PIGLIN)) {
		/* APPS I2C */
		qupv3_se_fw_load_and_init(QUPV3_0_SE1, SE_PROTOCOL_I2C, GSI);
		/* ESIM SPI */
		qupv3_se_fw_load_and_init(QUPV3_1_SE4, SE_PROTOCOL_SPI, MIXED);
		/* Trackpad I2C */
		qupv3_se_fw_load_and_init(QUPV3_0_SE0, SE_PROTOCOL_I2C, MIXED);
	} else {
		/* Trackpad I2C */
		qupv3_se_fw_load_and_init(QUPV3_0_SE0, SE_PROTOCOL_I2C, MIXED);
		/* SAR sensor I2C */
		qupv3_se_fw_load_and_init(QUPV3_0_SE1, SE_PROTOCOL_I2C, MIXED);
		/* Audio I2C */
		qupv3_se_fw_load_and_init(QUPV3_0_SE2, SE_PROTOCOL_I2C, MIXED);
		/* Fingerprint SPI */
		if (CONFIG(HEROBRINE_HAS_FINGERPRINT))
			qupv3_se_fw_load_and_init(QUPV3_1_SE1, SE_PROTOCOL_SPI, MIXED);
	}

	/* Take FPMCU out of reset. Power was already applied
	   in romstage and should have stabilized by now. */
	if (CONFIG(HEROBRINE_HAS_FINGERPRINT))
		gpio_output(GPIO_FP_RST_L, 1);

	setup_usb();
	qi2s_configure_gpios();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
