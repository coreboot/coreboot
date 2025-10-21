/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/dsi.h>

void mtk_dsi_reset(void)
{
	write32(&dsi0->dsi_force_commit,
		DSI_FORCE_COMMIT_USE_MMSYS | DSI_FORCE_COMMIT_ALWAYS);
	write32(&dsi0->dsi_con_ctrl, 1);
	write32(&dsi0->dsi_con_ctrl, 0);
}
