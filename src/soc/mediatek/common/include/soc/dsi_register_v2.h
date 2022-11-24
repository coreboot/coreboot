/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DSI_REGISTER_V2_H
#define DSI_REGISTER_V2_H

#include <commonlib/helpers.h>
#include <soc/addressmap.h>
#include <types.h>

struct dsi_regs {
	u32 dsi_start;
	u8 reserved0[4];
	u32 dsi_inten;
	u32 dsi_intsta;
	u32 dsi_con_ctrl;
	u32 dsi_mode_ctrl;
	u32 dsi_txrx_ctrl;
	u32 dsi_psctrl;
	u32 dsi_vsa_nl;
	u32 dsi_vbp_nl;
	u32 dsi_vfp_nl;
	u32 dsi_vact_nl;
	u32 dsi_lfr_con;  /* Available since MT8183 */
	u32 dsi_lfr_sta;  /* Available since MT8183 */
	u32 dsi_size_con;  /* Available since MT8183 */
	u32 dsi_vfp_early_stop;  /* Available since MT8183 */
	u32 reserved1[4];
	u32 dsi_hsa_wc;
	u32 dsi_hbp_wc;
	u32 dsi_hfp_wc;
	u32 dsi_bllp_wc;
	u32 dsi_cmdq_size;
	u32 dsi_hstx_cklp_wc;
	u8 reserved2[156];
	u32 dsi_phy_lccon;
	u32 dsi_phy_ld0con;
	u8 reserved3[4];
	u32 dsi_phy_timecon0;
	u32 dsi_phy_timecon1;
	u32 dsi_phy_timecon2;
	u32 dsi_phy_timecon3;
	u8 reserved4[16];
	u32 dsi_vm_cmd_con;
	u8 reserved5[92];
	u32 dsi_force_commit;  /* Available since MT8183 */
	u8 reserved6[2924];
	u32 dsi_cmdq[128];
};

check_member(dsi_regs, dsi_phy_lccon, 0x104);
check_member(dsi_regs, dsi_phy_timecon3, 0x11c);
check_member(dsi_regs, dsi_vm_cmd_con, 0x130);
check_member(dsi_regs, dsi_force_commit, 0x190);
check_member(dsi_regs, dsi_cmdq, 0xd00);

#endif /* DSI_REGISTER_V2_H */
