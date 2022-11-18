/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/usb/qusb_phy.h>
#include <soc/efuse.h>
#include <timer.h>
#include <soc/usb/usb_common.h>

static struct qfprom_corr * const qfprom_corr_efuse = (void *)QFPROM_BASE;

struct hs_usb_phy_reg qusb_phy = {
	.phy_pll = (void *)QUSB_PRIM_PHY_BASE,

	.phy_dig = (void *)QUSB_PRIM_PHY_DIG_BASE,

	.efuse_offset = 25,
};

static void qusb2_phy_override_phy_params(struct hs_usb_phy_reg *hs_phy_reg)
{
	/* Override preemphasis value */
	write32(&hs_phy_reg->phy_dig->tune1,
		hs_phy_reg->board_data->port_tune1);

	/* Override BIAS_CTRL_2 to reduce the TX swing overshooting. */
	write32(&hs_phy_reg->phy_pll->bias_ctrl_2,
		hs_phy_reg->board_data->pll_bias_control_2);

	/* Override IMP_RES_OFFSET value */
	write32(&hs_phy_reg->phy_dig->imp_ctrl1,
		hs_phy_reg->board_data->imp_ctrl1);
}

/*
 * Fetches HS Tx tuning value from efuse register and sets the
 * QUSB2PHY_PORT_TUNE1/2 register.
 * For error case, skip setting the value and use the default value.
 */

static void qusb2_phy_set_tune_param(struct hs_usb_phy_reg *hs_phy_reg)
{
	/*
	 * Efuse registers 3 bit value specifies tuning for HSTX
	 * output current in TUNE1 Register. Hence Extract 3 bits from
	 * EFUSE at correct position.
	 */

	const int efuse_bits = 3;
	int bit_pos = hs_phy_reg->efuse_offset;

	u32 bit_mask = (1 << efuse_bits) - 1;
	u32 tune_val =
		(read32(&qfprom_corr_efuse->qusb_hstx_trim_lsb) >> bit_pos)
		& bit_mask;
	/*
	 * if efuse reg is updated (i.e non-zero) then use it to program
	 * tune parameters.
	 */
	if (tune_val)
		clrsetbits32(&hs_phy_reg->phy_dig->tune1,
				PORT_TUNE1_MASK, tune_val << 4);
}

static void tune_phy(struct hs_usb_phy_reg *hs_phy_reg)
{
	write32(&hs_phy_reg->phy_dig->pwr_ctrl2, QUSB2PHY_PWR_CTRL2);
	/* IMP_CTRL1: Control the impedance reduction */
	write32(&hs_phy_reg->phy_dig->imp_ctrl1, QUSB2PHY_IMP_CTRL1);
	/* IMP_CTRL2: Impedance offset/mapping slope */
	write32(&hs_phy_reg->phy_dig->imp_ctrl2, QUSB2PHY_IMP_CTRL1);
	write32(&hs_phy_reg->phy_dig->chg_ctrl2, QUSB2PHY_IMP_CTRL2);
	/*
	 * TUNE1: Sets HS Impedance to approx 45 ohms
	 * then override with efuse value.
	 */
	write32(&hs_phy_reg->phy_dig->tune1, QUSB2PHY_PORT_TUNE1);
	/* TUNE2: Tuning for HS Disconnect Level */
	write32(&hs_phy_reg->phy_dig->tune2, QUSB2PHY_PORT_TUNE2);
	/* TUNE3: Tune squelch range */
	write32(&hs_phy_reg->phy_dig->tune3, QUSB2PHY_PORT_TUNE3);
	/* TUNE4: Sets EOP_DLY(Squelch rising edge to linestate falling edge) */
	write32(&hs_phy_reg->phy_dig->tune4, QUSB2PHY_PORT_TUNE4);
	write32(&hs_phy_reg->phy_dig->tune5, QUSB2PHY_PORT_TUNE5);

	if (hs_phy_reg->board_data) {
		/* Override board specific PHY tuning values */
		qusb2_phy_override_phy_params(hs_phy_reg);

		/* Set efuse value for tuning the PHY */
		qusb2_phy_set_tune_param(hs_phy_reg);
	}
}

void hs_usb_phy_init(void *board_data)
{
	struct hs_usb_phy_reg *hs_phy_reg;

	hs_phy_reg = &qusb_phy;

	hs_phy_reg->board_data = (struct usb_board_data *)board_data;

	/* PWR_CTRL: set the power down bit to disable the PHY */

	setbits32(&hs_phy_reg->phy_dig->pwr_ctrl1, POWER_DOWN);

	write32(&hs_phy_reg->phy_pll->analog_controls_two,
			QUSB2PHY_PLL_ANALOG_CONTROLS_TWO);
	write32(&hs_phy_reg->phy_pll->clock_inverters,
			QUSB2PHY_PLL_CLOCK_INVERTERS);
	write32(&hs_phy_reg->phy_pll->cmode,
			QUSB2PHY_PLL_CMODE);
	write32(&hs_phy_reg->phy_pll->lock_delay,
			QUSB2PHY_PLL_LOCK_DELAY);
	write32(&hs_phy_reg->phy_pll->dig_tim,
			QUSB2PHY_PLL_DIGITAL_TIMERS_TWO);
	write32(&hs_phy_reg->phy_pll->bias_ctrl_1,
			QUSB2PHY_PLL_BIAS_CONTROL_1);
	write32(&hs_phy_reg->phy_pll->bias_ctrl_2,
			QUSB2PHY_PLL_BIAS_CONTROL_2);

	tune_phy(hs_phy_reg);

	/* PWR_CTRL1: Clear the power down bit to enable the PHY */
	clrbits32(&hs_phy_reg->phy_dig->pwr_ctrl1, POWER_DOWN);

	write32(&hs_phy_reg->phy_dig->debug_ctrl2,
				DEBUG_CTRL2_MUX_PLL_LOCK_STATUS);

	/*
	 * DEBUG_STAT5: wait for 160uS for PLL lock;
	 * vstatus[0] changes from 0 to 1.
	 */
	long lock_us = wait_us(160, read32(&hs_phy_reg->phy_dig->debug_stat5) &
						VSTATUS_PLL_LOCK_STATUS_MASK);
	if (!lock_us)
		printk(BIOS_ERR, "QUSB PHY PLL LOCK fails\n");
	else
		printk(BIOS_DEBUG, "QUSB PHY initialized and locked in %ldus\n",
				lock_us);
}
