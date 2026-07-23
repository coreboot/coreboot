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

	/* PHY configuration registers (verified against IP catalog) */
	u32 cfg1;				/* 0x154 - USB_PHY_CFG1 (PLL_EN[0], APB_EN[1]) */
	u32 cfg2;				/* 0x158 - USB_PHY_CFG2 */

	/* XCFGI interface registers (directly memory-mapped, not via APB) */
	u32 xcfgi_7_0;				/* 0x15c - XCFGI[7:0]   - PLL lock time [1:0] */
	u32 xcfgi_15_8;				/* 0x160 - XCFGI[15:8] */
	u32 xcfgi_23_16;			/* 0x164 - XCFGI[23:16] */
	u32 xcfgi_31_24;			/* 0x168 - XCFGI[31:24] - HSTX slew rate [2:0] */
	u32 xcfgi_39_32;			/* 0x16c - XCFGI[39:32] - HSTX de-emphasis [3:2] */
	u32 xcfgi_47_40;			/* 0x170 - XCFGI[47:40] */
	u32 xcfgi_55_48;			/* 0x174 - XCFGI[55:48] */
	u32 xcfgi_63_56;			/* 0x178 - XCFGI[63:56] */
	u32 xcfgi_71_64;			/* 0x17c - XCFGI[71:64] - HSTX swing [3:0] */
	u32 xcfgi_79_72;			/* 0x180 - XCFGI[79:72] */
	u32 xcfgi_87_80;			/* 0x184 - XCFGI[87:80] */
	u32 xcfgi_95_88;			/* 0x188 - XCFGI[95:88] */
	u32 xcfgi_103_96;			/* 0x18c - XCFGI[103:96] */
	u32 xcfgi_111_104;			/* 0x190 - XCFGI[111:104] */
	u32 xcfgi_119_112;			/* 0x194 - XCFGI[119:112] */
	u32 xcfgi_127_120;			/* 0x198 - XCFGI[127:120] */
	u32 xcfgi_135_128;			/* 0x19c - XCFGI[135:128] */
	u32 xcfgi_143_136;			/* 0x1a0 - XCFGI[143:136] */
	u32 xcfgi_151_144;			/* 0x1a4 - XCFGI[151:144] */
	u32 xcfgi_159_152;			/* 0x1a8 - XCFGI[159:152] - SE0 counter [7] */
	u32 xcfgi_167_160;			/* 0x1ac - XCFGI[167:160] */
	u32 xcfgi_175_168;			/* 0x1b0 - XCFGI[175:168] */
	u32 xcfgi_183_176;			/* 0x1b4 - XCFGI[183:176] */
	u32 xcfgi_191_184;			/* 0x1b8 - XCFGI[191:184] */
	u32 xcfgi_199_192;			/* 0x1bc - XCFGI[199:192] - HS_DISCONNECT_EN_TX [4] */
	u32 xcfgi_207_200;			/* 0x1c0 - XCFGI[207:200] */

	/* XCFG PLL registers */
	u32 xcfg_pll_1;				/* 0x1c4 */
	u32 xcfg_pll_2;				/* 0x1c8 */

	/* Additional PHY config registers */
	u32 cfg3;				/* 0x1cc */
	u32 cfg4;				/* 0x1d0 */

	/* CM RAP interface */
	u32 usb_phy_cm_rap_init;		/* 0x1d4 */
	u32 usb_phy_reserved_8;			/* 0x1d8 */
	u32 usb_phy_cm_rap_wdata;		/* 0x1dc */
	u32 usb_phy_cm_rap_addr;		/* 0x1e0 */
	u32 usb_phy_cm_rap_start;		/* 0x1e4 */

	/* XCFG extended host parameters */
	u32 xcfg_ext_host_para_7_0;		/* 0x1e8 */
	u32 xcfg_ext_host_para_15_8;		/* 0x1ec */
	u32 xcfg_ext_host_para_23_16;		/* 0x1f0 */
	u32 xcfg_ext_host_para_31_24;		/* 0x1f4 */
	u32 xcfg_ext_host_para_39_32;		/* 0x1f8 */
	u32 xcfg_ext_host_para_47_40;		/* 0x1fc */
	u32 xcfg_ext_host_para_55_48;		/* 0x200 */
	u32 xcfg_ext_host_para_63_56;		/* 0x204 */
	u32 xcfg_ext_host_para_71_64;		/* 0x208 */
	u32 xcfg_ext_host_para_79_72;		/* 0x20c */
	u32 xcfg_ext_host_para_87_80;		/* 0x210 */
	u32 xcfg_ext_host_para_95_88;		/* 0x214 */
	u32 xcfg_ext_host_para_103_96;		/* 0x218 */
	u32 xcfg_ext_host_para_111_104;	/* 0x21c */
	u32 xcfg_ext_host_para_119_112;	/* 0x220 */
	u32 xcfg_ext_host_para_127_120;	/* 0x224 */
	u32 xcfg_ext_host_para_135_128;	/* 0x228 */
	u32 xcfg_ext_host_para_143_136;	/* 0x22c */
	u32 xcfg_ext_host_para_151_144;	/* 0x230 */
	u32 xcfg_ext_host_para_159_152;	/* 0x234 */

	/* XCFG extended device parameters */
	u32 xcfg_ext_device_para_7_0;		/* 0x238 */
	u32 xcfg_ext_device_para_15_8;		/* 0x23c */
	u32 xcfg_ext_device_para_23_16;	/* 0x240 */
	u32 xcfg_ext_device_para_31_24;	/* 0x244 */
	u32 xcfg_ext_device_para_39_32;	/* 0x248 */
	u32 xcfg_ext_device_para_47_40;	/* 0x24c */
	u32 xcfg_ext_device_para_55_48;	/* 0x250 */
	u32 xcfg_ext_device_para_63_56;	/* 0x254 */
	u32 xcfg_ext_device_para_71_64;	/* 0x258 */
	u32 xcfg_ext_device_para_79_72;	/* 0x25c */

	/* CM timing registers */
	u32 usb_phy_cm_timing_7_0;		/* 0x260 */
	u32 usb_phy_cm_timing_15_8;		/* 0x264 */
	u32 usb_phy_cm_timing_23_16;		/* 0x268 */
	u32 usb_phy_cm_timing_31_24;		/* 0x26c */

	/* XCFGO output registers */
	u32 usb_phy_xcfgo_0;			/* 0x270 */
	u32 usb_phy_xcfgo_1;			/* 0x274 */

	/* Reserved */
	u32 usb_phy_reserved_10;		/* 0x278 */
	u32 usb_phy_reserved_11;		/* 0x27c */
	u32 usb_phy_reserved_12;		/* 0x280 */
	u32 usb_phy_reserved_13;		/* 0x284 */
	u32 usb_phy_reserved_14;		/* 0x288 */
	u32 usb_phy_reserved_15;		/* 0x28c */

	/* CM RAP status */
	u32 usb_phy_cm_rap_status;		/* 0x290 */
	u32 usb_phy_reserved_16;		/* 0x294 */
	u32 usb_phy_cm_rap_rdata;		/* 0x298 */
};
