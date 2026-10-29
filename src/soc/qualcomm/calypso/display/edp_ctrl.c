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

	printk(BIOS_INFO, "\n");

	uint8_t edp_rev;
	ret = edp_aux_transfer(DP_EDP_DPCD_REV,
						DP_AUX_NATIVE_READ,
						&edp_rev,
						sizeof(edp_rev));

	if (ret < 0) {
		printk(BIOS_ERR,
			"eDP revision read failed: address=0x%05x ret=%d\n",
			DP_EDP_DPCD_REV, ret);
	} else if (ret != sizeof(edp_rev)) {
		printk(BIOS_ERR,
			"eDP revision short read: expected=%zu received=%d\n",
			sizeof(edp_rev), ret);
	} else {
		printk(BIOS_INFO,
			"eDP revision: address=0x%05x value=0x%02x\n",
			DP_EDP_DPCD_REV, edp_rev);

		if (edp_rev >= DP_EDP_15)
			printk(BIOS_INFO, "Sink supports eDP 1.5 or later\n");
	}

	edp_read_edid(edid);

	struct edp_ctrl ctrl;

	ctrl.edp_rev = edp_rev;
	if (edp_ctrl_on(&ctrl, edid, dpcd) < 0)
		return CB_ERR;

	return CB_SUCCESS;
}

/*
 * eDP DPCD AUX backlight control (VESA eDP spec), mirroring the capability
 * probe + mode-set + brightness-write sequence used by the Linux msm/dp
 * driver's dp_panel_read_edp_backlight_caps()/edp_set_dpcd_brightness()
 * (dp_panel.c). Only DPCD 0x701/0x702 capability bits gate this path; the
 * backlight-enable/power GPIOs are handled separately by the mainboard
 * (they are independent of AUX brightness control, per the vendor driver).
 *
 * @level:     desired brightness, in the sink's native brightness units.
 * @max_level: the sink's advertised maximum (e.g. from a devicetree-style
 *             "brightness max level" board constant); level is clamped to
 *             this if nonzero.
 *
 * Returns CB_SUCCESS if the brightness was written over AUX, CB_ERR if the
 * sink does not advertise DPCD AUX backlight support (caller should then
 * rely on GPIO/PWM backlight control only).
 */
enum cb_err edp_backlight_aux(uint16_t level, uint16_t max_level)
{
	uint8_t dpcd_buf;
	uint8_t buf[2];
	bool use_16bit;
	int ret;

	ret = edp_aux_transfer(DP_EDP_GENERAL_CAP_1, DP_AUX_NATIVE_READ, &dpcd_buf, 1);
	if (ret < 0 || !(dpcd_buf & DP_EDP_TCON_BACKLIGHT_ADJUSTMENT_CAP)) {
		printk(BIOS_INFO, "eDP: sink has no TCON backlight adjustment cap\n");
		return CB_ERR;
	}

	ret = edp_aux_transfer(DP_EDP_BACKLIGHT_ADJUSTMENT_CAP, DP_AUX_NATIVE_READ, &dpcd_buf, 1);
	if (ret < 0 || !(dpcd_buf & DP_EDP_BACKLIGHT_BRIGHTNESS_AUX_SET_CAP)) {
		printk(BIOS_INFO, "eDP: sink does not support AUX brightness set\n");
		return CB_ERR;
	}
	use_16bit = !!(dpcd_buf & DP_EDP_BACKLIGHT_BRIGHTNESS_BYTE_COUNT);

	ret = edp_aux_transfer(DP_EDP_BACKLIGHT_MODE_SET_REGISTER, DP_AUX_NATIVE_READ, &dpcd_buf, 1);
	if (ret < 0) {
		printk(BIOS_ERR, "eDP: backlight mode register read failed\n");
		return CB_ERR;
	}

	dpcd_buf &= ~DP_EDP_BACKLIGHT_CONTROL_MODE_MASK;
	dpcd_buf |= DP_EDP_BACKLIGHT_CONTROL_MODE_DPCD;

	ret = edp_aux_transfer(DP_EDP_BACKLIGHT_MODE_SET_REGISTER, DP_AUX_NATIVE_WRITE, &dpcd_buf, 1);
	if (ret < 0) {
		printk(BIOS_ERR, "eDP: backlight mode register write failed\n");
		return CB_ERR;
	}

	if (max_level && level > max_level)
		level = max_level;

	buf[0] = (level >> 8) & 0xff;
	buf[1] = level & 0xff;

	if (use_16bit)
		ret = edp_aux_transfer(DP_EDP_BACKLIGHT_BRIGHTNESS_MSB, DP_AUX_NATIVE_WRITE, buf, 2);
	else
		ret = edp_aux_transfer(DP_EDP_BACKLIGHT_BRIGHTNESS_MSB, DP_AUX_NATIVE_WRITE, buf, 1);

	if (ret < 0) {
		printk(BIOS_ERR, "eDP: AUX brightness write failed\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "eDP: AUX backlight set to %u/%u (%u-bit)\n", level, max_level,
	       use_16bit ? 16 : 8);

	return CB_SUCCESS;
}
