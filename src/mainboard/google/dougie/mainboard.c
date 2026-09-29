/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/qcom_qup_se.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qupv3_config_common.h>

static void mainboard_init(struct device *dev)
{
	gpi_firmware_load(QUP_0_GSI_BASE);
	gpi_firmware_load(QUP_1_GSI_BASE);

	/*
	 * When coreboot firmware disables serial output,
	 * we still need to load console UART QUP FW for OS.
	 */
	if (!CONFIG(CONSOLE_SERIAL))
		qupv3_se_fw_load_and_init(QUPV3_0_SE5, SE_PROTOCOL_UART, FIFO);

	qupv3_se_fw_load_and_init(QUPV3_0_SE0, SE_PROTOCOL_I2C, MIXED);	 /* Trackpad I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE5, SE_PROTOCOL_I2C, MIXED);	 /* Touch I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE7, SE_PROTOCOL_UART, FIFO);	 /* BT UART */
	qupv3_se_fw_load_and_init(QUPV3_1_SE4, SE_PROTOCOL_I2C, MIXED);	 /* HDMI bridge I2C */
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
