/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/qupv3_config.h>
#include <soc/usb.h>

static struct usb_board_data usb0_board_data = {
	.pll_bias_control_2 = 0x22,
	.imp_ctrl1 = 0x08,
	.port_tune1 = 0xc5,
};

static void setup_usb(void)
{
	/* Assert EN_PP3300_HUB for those board variants that use it. */
	gpio_output(GPIO(84), 1);

	setup_usb_host0(&usb0_board_data);
}

static void qi2s_configure_gpios(void)
{

	gpio_configure(GPIO(49), GPIO49_FUNC_MI2S_1_SCK,
				GPIO_PULL_UP, GPIO_8MA, GPIO_OUTPUT);

	gpio_configure(GPIO(50), GPIO50_FUNC_MI2S_1_WS,
				GPIO_PULL_UP, GPIO_8MA, GPIO_OUTPUT);

	gpio_configure(GPIO(51), GPIO51_FUNC_MI2S_1_DATA0,
				GPIO_PULL_UP, GPIO_8MA, GPIO_OUTPUT);
}

static void load_qup_fw(void)
{
	qupv3_se_fw_load_and_init(QUPV3_0_SE1, SE_PROTOCOL_SPI, MIXED);  /* ESIM SPI */
	qupv3_se_fw_load_and_init(QUPV3_0_SE2, SE_PROTOCOL_I2C, MIXED);  /* EDP Bridge I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE3, SE_PROTOCOL_UART, FIFO);  /* BT UART */
	qupv3_se_fw_load_and_init(QUPV3_0_SE4, SE_PROTOCOL_I2C, MIXED);  /* Pen Detect I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE5, SE_PROTOCOL_I2C, MIXED);  /* SAR I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE1, SE_PROTOCOL_I2C, MIXED);  /* Trackpad I2C */
	/*
	 * When coreboot firmware disables serial output,
	 * we still need to load console UART QUP FW for OS.
	 */
	if (!CONFIG(CONSOLE_SERIAL))
		qupv3_se_fw_load_and_init(QUPV3_1_SE2, SE_PROTOCOL_UART, FIFO);

	qupv3_se_fw_load_and_init(QUPV3_1_SE3, SE_PROTOCOL_I2C, MIXED);  /* Speaker Amps I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE4, SE_PROTOCOL_SPI, MIXED);  /* Fingerprint SPI */
	qupv3_se_fw_load_and_init(QUPV3_1_SE5, SE_PROTOCOL_I2C, MIXED);  /* Codec I2C */
}

static void mainboard_init(struct device *dev)
{
	setup_usb();
	qi2s_configure_gpios();
	load_qup_fw();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
