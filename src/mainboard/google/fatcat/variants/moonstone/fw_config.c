/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>
#include <inttypes.h>

/* t: base table; o: override table */
#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))
/* t: table */
#define GPIO_CONFIGURE_PADS(t) gpio_configure_pads(t, ARRAY_SIZE(t))

static const struct pad_config fp_spi_disable_pads[] = {
	PAD_NC(GPP_C15, NONE),
	PAD_NC(GPP_E11, NONE),
	PAD_NC(GPP_E17, NONE),
	PAD_NC(GPP_E20, NONE),
	PAD_NC(GPP_E22, NONE),
	PAD_NC(GPP_F14, NONE),
	PAD_NC(GPP_F15, NONE),
};

static const struct pad_config fp_spi_enable_pads[] = {
	/* GPP_C15:     FPS_RST_N */
	PAD_CFG_GPO_LOCK(GPP_C15, 1, LOCK_CONFIG),
	/* GPP_E11:     GPSI0_CLK */
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF5),
	/* GPP_E17:     GSPI0_CS0 */
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF5),
	/* GPP_E20:     FPMCU_FW_UPDATE */
	PAD_CFG_GPO_LOCK(GPP_E20, 0, LOCK_CONFIG),
	/* GPP_E22:     FPS_SOC_INT_L */
	PAD_CFG_GPI_IRQ_WAKE(GPP_E22, NONE, PWROK, LEVEL, INVERT),
	/* GPP_F14:     GPSI0A_MOSI */
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF8),
	/* GPP_F15:     GSPI0A_MISO */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF8),
};

static const struct pad_config use_usb3c_enable_pads[] = {
	/* GPP_C20:     NC */
	PAD_NC(GPP_C20, NONE),
	/* GPP_C21:     NC */
	PAD_NC(GPP_C21, NONE),
	/* GPP_C22:     TBT_LSX3_TXD  */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF1),
	/* GPP_C23:     TBT_LSX3_RXD  */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF1),
	/* GPP_B14:     NC */
	PAD_NC(GPP_B14, NONE),
	/* GPP_B11:     NC */
	PAD_NC(GPP_B11, NONE),
	/* GPP_E09:     NC */
	PAD_NC(GPP_E09, NONE),
};

static const struct pad_config use_usb2a2c_hdmi_enable_pads[] = {
	/* GPP_C20:     TBT_LSX2_TXD */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* GPP_C21:     TBT_LSX2_RXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	/* GPP_C22:     DDP3_CTRLCLK */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF2),
	/* GPP_C23:     DDP3_CTRLDATA */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF2),
	/* GPP_B14:     DISP_HPD4 */
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF2),
	/* GPP_B11:     USB_OC1 */
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF1),
	/* GPP_E09:     USB_OC0 */
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),
};

static const struct pad_config pre_mem_fp_spi_enable_pads[] = {
	/* GPP_C15:     FPS_RST_N */
	PAD_CFG_GPO(GPP_C15, 0, DEEP),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(FPMCU, FP_SPI)))
		GPIO_CONFIGURE_PADS(pre_mem_fp_spi_enable_pads);

}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}
	/*
	 *+-------+--------+-------------+
	 *|       | FP_SPI | FP_USB      |
	 *+-------+--------+-------------+
	 *| FPMCU | GSPI0  | usb2_port6  |
	 *+-------+--------+-------------+
	 */
	if (fw_config_probe(FW_CONFIG(FPMCU, FP_SPI))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, fp_spi_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, fp_spi_disable_pads);
	}
	/* Probe fw_config : "IO_PORT" to reconfigure port settings accordingly.
	 * proto0  : IO_PORT => "USB2A2C_HDMI:0"
	 * porot1.5: IO_PORT => "USB3C:1"
	 * +-----------------+------------------+------------------+
	 *| IO_PORT         | USB2A2C_HDMI     |  USB3C           |
	 *+-----------------+------------------+------------------+
	 *| tcss_usb3_port0 | USB4_C0 (MB-TBT) | USB4_C0 (MB-TBT) |
	 *+-----------------+------------------+------------------+
	 *| tcss_usb3_port1 | N/A              | USB3.2 C2 (DB)   |
	 *+-----------------+------------------+------------------+
	 *| tcss_usb3_port2 | USB4_C1 (MB-TBT) | N/A              |
	 *+-----------------+------------------+------------------+
	 *| tcss_usb3_port3 | Fixed HDMI       | USB4_C1 (MB-TBT) |
	 *+-----------------+------------------+------------------+
	 *| usb3_port1      | USB3_A0          | N/A              |
	 *+-----------------+------------------+------------------+
	 *| usb3_port2      | USB3_A1          | N/A              |
	 *+-----------------+------------------+------------------+
	 *| usb2_port5      | USB2_A0          | USB2_C2 (DB)     |
	 *+-----------------+------------------+------------------+
	 *| usb2_port6      | USB2_A1          | USB_FP           |
	 *+-----------------+------------------+------------------+
	 */
	if (fw_config_probe(FW_CONFIG(IO_PORT, USB3C)))
		GPIO_PADBASED_OVERRIDE(padbased_table, use_usb3c_enable_pads);
	else
		GPIO_PADBASED_OVERRIDE(padbased_table, use_usb2a2c_hdmi_enable_pads);
}
