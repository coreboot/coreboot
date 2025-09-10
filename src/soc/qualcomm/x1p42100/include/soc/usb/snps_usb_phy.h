/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>

struct hs_usb_phy_reg {
	/* Revision ID registers */
	u32 usb_phy_revision_id0;		/* 0x000 */
	u32 usb_phy_revision_id1;		/* 0x004 */
	u32 usb_phy_revision_id2;		/* 0x008 */
	u32 usb_phy_revision_id3;		/* 0x00c */

	/* Debug bus status registers */
	u32 usb_phy_debug_bus_stat0;		/* 0x010 */
	u32 usb_phy_debug_bus_stat1;		/* 0x014 */
	u32 usb_phy_debug_bus_stat2;		/* 0x018 */
	u32 usb_phy_debug_bus_stat3;		/* 0x01c */

	/* Test and status registers */
	u32 usb_phy_hs_rx_tester_out_1;	/* 0x020 */
	u32 usb_phy_charging_det_output;	/* 0x024 */
	u32 usb_phy_hs_phy_test_out_1;		/* 0x028 */
	u32 usb_phy_refclk_rxtap_test_status;	/* 0x02c */

	/* UTMI RX status registers */
	u32 usb_phy_utmi_rx_datal_status;	/* 0x030 */
	u32 usb_phy_utmi_rx_datah_status;	/* 0x034 */
	u32 usb_phy_utmi_rx_port_status;	/* 0x038 */

	/* Original UTMI control registers */
	u32 utmi_ctrl0;				/* 0x03c */
	u32 utmi_ctrl1;				/* 0x040 */

	/* Extended UTMI control registers */
	u32 utmi_ctrl2;				/* 0x044 */
	u32 utmi_ctrl3;				/* 0x048 */
	u32 utmi_ctrl4;				/* 0x04c */
	u32 utmi_ctrl5;				/* 0x050 */

	/* HS PHY control registers */
	u32 hs_phy_ctrl_common0;		/* 0x054 */

	/* Configuration control registers */
	u32 cfg_ctrl_1;				/* 0x058 */
	u32 cfg_ctrl_2;				/* 0x05c */
	u32 cfg_ctrl_3;				/* 0x060 */
	u32 hs_phy_ctrl2;			/* 0x064 */
	u32 cfg_ctrl_4;				/* 0x068 */

	/* Additional configuration control registers */
	u32 cfg_ctrl_5;				/* 0x06c */
	u32 cfg_ctrl_6;				/* 0x070 */
	u32 cfg_ctrl_7;				/* 0x074 */
	u32 cfg_ctrl_8;				/* 0x078 */
	u32 cfg_ctrl_9;				/* 0x07c */
	u32 cfg_ctrl_10;			/* 0x080 */

	u32 hs_phy_test1;		/* 0x084 */
	u32 rx_tester_1_reg0;	/* 0x088 */
	u32 rx_tester_1_reg1;	/* 0x08c */
	u32 rx_tester_1_reg2;	/* 0x090 */

	/* Original cfg0 register */
	u32 cfg0;				/* 0x094 */

	/* Common control registers */
	u32 utmi_phy_cmn_ctrl0;			/* 0x098 */
	u32 utmi_phy_cmn_ctrl1;			/* 0x09c */

	/* Original refclk_ctrl register */
	u32 refclk_ctrl;			/* 0x0a0 */

	/* Power and reset control registers */
	u32 usb_phy_pwrdown_ctrl;		/* 0x0a4 */
	u32 usb_phy_test_debug_ctrl;		/* 0x0a8 */
	u32 usb_phy_reset_ctrl;			/* 0x0ac */
	u32 usb_phy_ac_en;			/* 0x0b0 */
	u32 cfg_ctrl_11;			/* 0x0b4 */
	u32 usb_phy_fsel_sel;			/* 0x0b8 */

	/* Skitter control registers */
	u32 usb_phy_skitter_ctrl_1;		/* 0x0bc */
	u32 usb_phy_skitter_ctrl_2;		/* 0x0c0 */
	u32 usb_phy_skitter_ctrl_3;		/* 0x0c4 */
	u32 usb_phy_skitter_sticky_no_lsb;	/* 0x0c8 */
	u32 usb_phy_skitter_sticky_no_msb;	/* 0x0cc */
	u32 usb_phy_skitter_calib_counter_lsb;	/* 0x0d0 */
	u32 usb_phy_skitter_calib_counter_msb;	/* 0x0d4 */
	u32 usb_phy_skitter_mul_cons_lsb;	/* 0x0d8 */
	u32 usb_phy_skitter_mul_cons_msb;	/* 0x0dc */
	u32 usb_phy_skitter_mfs_misc_1;		/* 0x0e0 */
	u32 usb_phy_skitter_mfs_misc_2;		/* 0x0e4 */
	u32 usb_phy_skitter_spread_detect_range; /* 0x0e8 */
	u32 usb_phy_skitter_jitter_thrhold;	/* 0x0ec */
	u32 usb_phy_skitter_status;		/* 0x0f0 */
	u32 usb_phy_inv_dly_lsb;		/* 0x0f4 */
	u32 usb_phy_inv_dly_msb;		/* 0x0f8 */
	u32 usb_phy_eud_connected;		/* 0x0fc */
	u32 usb_phy_skitter_jitter;		/* 0x100 */
	u32 usb_phy_test_debug_ctrl_2;		/* 0x104 */
	u32 eud_present_sel;			/* 0x108 */
	u32 usb_phy_skitter_tie_jitter_lsb;	/* 0x10c */
	u32 usb_phy_skitter_tie_jitter_msb;	/* 0x110 */
	u32 usb_phy_skitter_calib_rodly_1;	/* 0x114 */
	u32 usb_phy_skitter_calib_rodly_2;	/* 0x118 */
	u32 usb_phy_skitter_calib_rodly_3;	/* 0x11c */

	/* Reserved registers */
	u32 usb_phy_reserved_0;			/* 0x120 */
	u32 usb_phy_reserved_1;			/* 0x124 */
	u32 usb_phy_reserved_2;			/* 0x128 */
	u32 usb_phy_reserved_3;			/* 0x12c */

	/* APB access registers */
	u32 usb_phy_apb_access_cmd;		/* 0x130 */
	u32 usb_phy_apb_access_status;		/* 0x134 */
	u32 usb_phy_apb_address;		/* 0x138 */
	u32 usb_phy_apb_wrdata_lsb;		/* 0x13c */
	u32 usb_phy_apb_wrdata_msb;		/* 0x140 */
	u32 usb_phy_apb_rddata_lsb;		/* 0x144 */
	u32 usb_phy_apb_rddata_msb;		/* 0x148 */

	/* Final skitter registers */
	u32 usb_phy_skitter_insertion_dly_corr;	/* 0x14c */
	u32 usb_phy_skitter_pos_edge_corr1;	/* 0x150 */
};
