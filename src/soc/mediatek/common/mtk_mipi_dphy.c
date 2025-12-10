/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/dsi.h>

void mtk_dsi_dphy_disable_ck_mode(struct mipi_tx_regs *mipi_tx_reg)
{
	setbits32(&mipi_tx_reg->ck_ckmode_en, DSI_CK_CKMODE_EN);
}
