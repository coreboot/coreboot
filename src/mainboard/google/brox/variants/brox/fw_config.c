/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <fw_config.h>
#include <gpio.h>

#define GPIO_PADBASED_OVERRIDE(b, a) gpio_padbased_override(b, a, ARRAY_SIZE(a))

static const struct pad_config ish_enable_pads[] = {
	/* GPP_B5 : ISH I2C0_SDA */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B5, NONE, DEEP, NF1),
	/* GPP_B6 : ISH_I2C0_SCL */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B6, NONE, DEEP, NF1),
	/* GPP_D13 : [NF1: ISH_UART0_RXD ==> UART0_ISH_RX_DBG_TX */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF1),
	/* GPP_D14 : [NF1: ISH_UART0_TXD ==> UART0_ISH_TX_DBG_RX */
	PAD_CFG_NF(GPP_D14, NONE, DEEP, NF1),
	/* GPP_D2 : ISH_GP2, SOC_ISH_ACCEL_INT_L */
	PAD_CFG_NF(GPP_D2, NONE, DEEP, NF1),
	/* GPP_D3 : ISH_GP3, SOC_ISH_IMU_INT_L */
	PAD_CFG_NF(GPP_D3, NONE, DEEP, NF1),
	/* GPP_B15 : ISH_GP7, LID_OPEN_1V8 */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF5),
	/* GPP_A16 : ISH_GP5, TABLET_MODE_ODL */
	PAD_CFG_NF(GPP_A16, NONE, DEEP, NF4),
};

static void fw_config_handle(void *unused)
{
	if (fw_config_probe(FW_CONFIG(ISH, ISH_ENABLE))) {
		printk(BIOS_INFO, "Configure GPIOs for ISH.\n");
		gpio_configure_pads(ish_enable_pads, ARRAY_SIZE(ish_enable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
