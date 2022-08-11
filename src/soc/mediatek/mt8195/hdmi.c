/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/hdmi.h>

void hdmi_low_power_setting(void)
{
	printk(BIOS_INFO, "%s: Enable HDMI low power setting\n", __func__);

	setbits32((void *)HDMI_PROTECT_REG, BIT(0) | BIT(1));

	/* HDMI-RX powerdown */
	write32((void *)HDMI_RX_PDN_0_REG, HDMI_RX_PDN_0_VAL);
	write32((void *)HDMI_RX_PDN_1_REG, HDMI_RX_PDN_1_VAL);
	write32((void *)HDMI_RX_PDN_2_REG, HDMI_RX_PDN_2_VAL);
	write32((void *)HDMI_RX_PDN_3_REG, HDMI_RX_PDN_3_VAL);
	write32((void *)HDMI_RX_PDN_4_REG, HDMI_RX_PDN_4_VAL);
	write32((void *)HDMI_RX_PDN_5_REG, HDMI_RX_PDN_5_VAL);
	write32((void *)HDMI_RX_PDN_6_REG, HDMI_RX_PDN_6_VAL);
	write32((void *)HDMI_RX_PDN_7_REG, HDMI_RX_PDN_7_VAL);

	/* HDMI-TX powerdown */
	write32((void *)HDMI_TX_PDN_REG, HDMI_TX_PDN_VAL);
}
