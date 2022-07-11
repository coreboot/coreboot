/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/pll.h>
#include <soc/usb.h>

void mtk_usb_prepare(void)
{
	mt_pll_set_usb_clock();
}

void mtk_usb_adjust_phy_shift(void)
{
	u32 phy_set_val, write_val;
	struct ssusb_sif_port *phy = (void *)(SSUSB_SIF_BASE);

	SET32_BITFIELDS(&phy->u3phyd.phyd_reserved,
			AUTO_LOAD_DIS, 1);

	phy_set_val = read32((void *)USB_PHY_SETTING_REG);

	/* TX imp */
	write_val = (phy_set_val & TX_IMP_MASK) >> TX_IMP_SHIFT;
	SET32_BITFIELDS(&phy->u3phyd.phyd_cal0,
			TX_IMP_CAL, write_val,
			TX_IMP_CAL_EN, 1);

	/* RX imp */
	write_val = (phy_set_val & RX_IMP_MASK) >> RX_IMP_SHIFT;
	SET32_BITFIELDS(&phy->u3phyd.phyd_cal1,
			RX_IMP_CAL, write_val,
			RX_IMP_CAL_EN, 1);

	/* Intr_cal */
	write_val = (phy_set_val & INTR_CAL_MASK) >> INTR_CAL_SHIFT;
	SET32_BITFIELDS(&phy->u3phya.phya_reg0,
			INTR_CAL, write_val);
}
