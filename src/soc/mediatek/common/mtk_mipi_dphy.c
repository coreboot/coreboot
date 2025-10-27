/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/dsi.h>

void mtk_dsi_dphy_disable_ck_mode(void)
{
	setbits32(&mipi_tx->ck_ckmode_en, DSI_CK_CKMODE_EN);
}
