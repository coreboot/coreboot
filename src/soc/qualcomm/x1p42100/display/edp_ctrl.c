// SPDX-License-Identifier: GPL-2.0-only

#include <device/mmio.h>
#include <commonlib/rational.h>
#include <console/console.h>
#include <edid.h>
#include <delay.h>
#include <types.h>
#include <string.h>
#include <soc/clock.h>
#include <gpio.h>
#include <soc/display/edp_reg.h>
#include <soc/display/edp_aux.h>
#include <soc/display/edp_ctrl.h>
#include <soc/display/edp_phy.h>
#include <soc/display/edp_link_train.h>

#include <soc/display/mdssreg.h>
#include <soc/qcom_spmi.h>

static void edp_ctrl_phy_enable(int enable)
{
	write32(&edp_ahbclk->sw_reset, 1);
	write32(&edp_ahbclk->sw_reset, 0);

	write32(&edp_ahbclk->phy_ctrl, 0x4);
	write32(&edp_ahbclk->phy_ctrl, 0x0);

	uint32_t active_status = read32(&edp_ahbclk->clk_active);

	printk(BIOS_INFO, "edp_ahbclk->phy_ctrl active = %d\n", active_status);

	early_phy_enable();

	/* early link-side initialization */
	write32(&edp_auxclk->hpd_int_ack, EDP_HPD_INT_ACK_CLEAR_ALL);
	write32(&edp_auxclk->aux_ctrl, EDP_AUX_CTRL_ENABLE_BASIC);
	write32(&edp_auxclk->hpd_reftimer, EDP_HPD_REFTIMER_ENABLE_0013);
	write32(&edp_auxclk->hpd_int_ack, EDP_HPD_INT_ACK_CLEAR_ALL);
	write32(&edp_auxclk->hpd_ctrl, EDP_HPD_CTRL_ENABLE);
	write32(&mdp_periph_top1->dp_hpd_select, MDP_DP_HPD_SELECT_HPDDPT3);
	write32(&mdp_periph_top1->hdmi_dp_core_select, MDP_HDMI_DP_CORE_SELECT_DP);
	write32(&edp_auxclk->aux_ctrl, EDP_AUX_CTRL_ENABLE_RST);
	write32(&edp_auxclk->aux_ctrl, EDP_AUX_CTRL_ENABLE_BASIC);
	write32(&edp_auxclk->timeout_count, EDP_AUX_TIMEOUT_COUNT_FFFF);
	write32(&edp_auxclk->aux_limits, EDP_AUX_LIMITS_FFFF);
	write32((uint32_t *)AHB2EDPPHY_AHB2PHY_AHB2PHY_TOP_CFG, READ_WAIT2_WR_WAIT1);
}

static void edp_ctrl_irq_enable(int enable)
{
	if (enable) {
		write32(&edp_ahbclk->interrupt_status, EDP_INTERRUPT_STATUS1_MASK);
		write32(&edp_ahbclk->interrupt_status2, EDP_INTERRUPT_STATUS2_MASK);
	} else {
		write32(&edp_ahbclk->interrupt_status, EDP_INTERRUPT_STATUS1_ACK);
		write32(&edp_ahbclk->interrupt_status2, EDP_INTERRUPT_STATUS2_ACK);
	}
}

enum cb_err edp_ctrl_init(struct edid *edid)
{
	uint8_t value;

	uint32_t ver = edp_ahbclk->hw_version;

	printk(BIOS_DEBUG, " eDP HW_VERSION = 0x%08x\n", ver);

	uint8_t dpcd[DP_RECEIVER_CAP_SIZE];
	int ret;

	mdss_clock_enable(DISP_CC_MDSS_DPTX3_AUX_CBCR);

	edp_ctrl_phy_enable(1);

	edp_ctrl_irq_enable(1);

	ret = edp_aux_transfer(DP_SET_POWER, DP_AUX_NATIVE_READ, &value, 1);
	printk(BIOS_DEBUG, "DPCD power read address=%x\n", DP_SET_POWER);
	if (ret < 0) {
		printk(BIOS_ERR, "edp native read failure\n");
		return CB_ERR;
	}

	value &= ~DP_SET_POWER_MASK;
	value |= DP_SET_POWER_D0;

	ret = edp_aux_transfer(DP_SET_POWER, DP_AUX_NATIVE_WRITE, &value, 1);
	printk(BIOS_DEBUG, "DPCD power Set address=%x : %x\n", DP_SET_POWER, value);
	if (ret < 0) {
		printk(BIOS_ERR, "edp native read failure\n");
		return CB_ERR;
	}

	udelay(1000);

	ret = edp_aux_transfer(DP_DPCD_REV, DP_AUX_NATIVE_READ, dpcd, DP_RECEIVER_CAP_SIZE);
	if (ret < 0) {
		printk(BIOS_ERR, " DPCD[0x00000] read failed : ret=%d\n", ret);
	} else {
		int dump_len = ret;
		if (dump_len > 16)
			dump_len = 16;

		printk(BIOS_DEBUG, "[DPCD] 0000..000F: ");

		for (int i = 0; i <= dump_len; i++) {
			printk(BIOS_INFO, "%02x ", (uint32_t)dpcd[i]);
		}
	}

	edp_read_edid(edid);

	u8 v[5];
	ret = edp_aux_transfer(0xF0000, DP_AUX_NATIVE_READ, v, 5);
	u8 bright[3];
	ret = edp_aux_transfer(0x724, DP_AUX_NATIVE_READ, bright, 3);
	u8 br2[1];
	br2[0] = bright[2];
	edp_aux_transfer(0x724, DP_AUX_NATIVE_WRITE, br2, 1);

	printk(BIOS_INFO, "LTTPR probe: addr=0x%05x ret=%d val=0x%02x", 0xF0000, ret, v[0]);
	printk(BIOS_INFO, ":%02x", v[1]);
	printk(BIOS_INFO, ":%02x", v[2]);
	printk(BIOS_INFO, ":%02x", v[3]);
	printk(BIOS_INFO, ":%02x", v[4]);

	struct edp_ctrl ctrl;
	if (edp_ctrl_on(&ctrl, edid, dpcd) < 0)
		return CB_ERR;

	return CB_SUCCESS;
}

void edp_backlight_aux(void)
{
	/* eDP DPCD AUX transactions: enable backlight control and set brightness */
	uint8_t rx_buf[4], tx_buf[4];
	int ret;

	ret = edp_aux_transfer(0x721, DP_AUX_NATIVE_READ, rx_buf, 1);
	if (ret < 0)
		printk(BIOS_DEBUG, " Error\n");

	tx_buf[0] = 0x02; /* Set DPCD 0x721 bits (per original logic) */
	ret = edp_aux_transfer(0x721, DP_AUX_NATIVE_WRITE, tx_buf, 1);
	if (ret < 0)
		printk(BIOS_DEBUG, " Error\n");

	ret = edp_aux_transfer(0x720, DP_AUX_NATIVE_READ, rx_buf, 1);
	if (ret < 0)
		printk(BIOS_DEBUG, " Error\n");

	tx_buf[0] = 0x01; /* Enable backlight via DPCD 0x720 */
	ret = edp_aux_transfer(0x720, DP_AUX_NATIVE_WRITE, tx_buf, 1);
	if (ret < 0)
		printk(BIOS_DEBUG, " Error\n");

	/* Brightness: MSB @ 0x722, LSB @ 0x723 (0x0400) */
	tx_buf[0] = 0x04;
	tx_buf[1] = 0x00;
	ret = edp_aux_transfer(0x722, DP_AUX_NATIVE_WRITE, tx_buf, 2);
	if (ret < 0)
		printk(BIOS_DEBUG, " Error\n");

	mdelay(50);
}
