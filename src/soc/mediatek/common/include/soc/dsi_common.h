/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_DSI_COMMON_H
#define SOC_MEDIATEK_DSI_COMMON_H

#include <commonlib/helpers.h>
#include <edid.h>
#include <mipi/dsi.h>
#include <types.h>
#include <soc/addressmap.h>
#include <soc/display_dsi.h>
#include <soc/dsi_reg.h>

/* DSI_INTSTA */
enum {
	LPRX_RD_RDY_INT_FLAG = BIT(0),
	CMD_DONE_INT_FLAG    = BIT(1),
	TE_RDY_INT_FLAG      = BIT(2),
	VM_DONE_INT_FLAG     = BIT(3),
	EXT_TE_RDY_INT_FLAG  = BIT(4),
	DSI_BUSY             = BIT(31),
};

/* DSI_CMD_TYPE1_HS */
enum {
	CMD_CPHY_6BYTE_EN = BIT(18),
};

/* DSI_CON_CTRL */
enum {
	DSI_RESET = BIT(0),
	DSI_EN = BIT(1),
	DPHY_RESET = BIT(2),
	DSI_DUAL = BIT(4),
};

/* DSI_MODE_CTRL */
enum {
	MODE = 3,
	CMD_MODE = 0,
	SYNC_PULSE_MODE = 1,
	SYNC_EVENT_MODE = 2,
	BURST_MODE = 3,
	FRM_MODE = BIT(16),
	MIX_MODE = BIT(17)
};

/* DSI_TXRX_CTRL */
enum {
	EOTP_DISABLE = BIT(6),
	NON_CONTINUOUS_CLK = BIT(16),
};

/* DSI_PSCTRL */
enum {
	DSI_PS_WC = 0x3fff,
	DSI_PS_SEL = (3 << 16),
	PACKED_PS_16BIT_RGB565 = (0 << 16),
	LOOSELY_PS_18BIT_RGB666 = (1 << 16),
	PACKED_PS_18BIT_RGB666 = (2 << 16),
	PACKED_PS_24BIT_RGB888 = (3 << 16),
	COMPRESSED_PIXEL_STREAM_V2 = (5 << 16),

	DSI_PSCON_CUSTOM_HEADER_SHIFT = 26,
};

/* DSI_SIZE_CON */
enum {
	DSI_SIZE_CON_HEIGHT_SHIFT = 16,
	DSI_SIZE_CON_WIDTH_SHIFT = 0,
};

/* DSI_CMDQ_SIZE */
enum {
	CMDQ_SIZE = 0x3f,
	CMDQ_SIZE_SEL = BIT(15),
};

/* DSI_PHY_LCCON */
enum {
	LC_HS_TX_EN = BIT(0),
	LC_ULPM_EN = BIT(1),
	LC_WAKEUP_EN = BIT(2)
};

/*DSI_PHY_LD0CON */
enum {
	LD0_RM_TRIG_EN = BIT(0),
	LD0_ULPM_EN = BIT(1),
	LD0_WAKEUP_EN = BIT(2)
};

enum {
	LPX = (0xff << 0),
	HS_PRPR = (0xff << 8),
	HS_ZERO = (0xff << 16),
	HS_TRAIL = (0xff << 24)
};

enum {
	TA_GO = (0xff << 0),
	TA_SURE = (0xff << 8),
	TA_GET = (0xff << 16),
	DA_HS_EXIT = (0xff << 24)
};

enum {
	CONT_DET = (0xff << 0),
	CLK_ZERO = (0xf << 16),
	CLK_TRAIL = (0xff << 24)
};

enum {
	CLK_HS_PRPR = (0xff << 0),
	CLK_HS_POST = (0xff << 8),
	CLK_HS_EXIT = (0xf << 16)
};

/* DSI_VM_CMD_CON */
enum {
	VM_CMD_EN = BIT(0),
	TS_VFP_EN = BIT(5),
};

/* DSI_CMDQ0 */
enum {
	CONFIG         = (0xff << 0),
	SHORT_PACKET   = 0,
	LONG_PACKET    = 2,
	BTA            = BIT(2),
	DATA_ID        = (0xff << 8),
	DATA_0         = (0xff << 16),
	DATA_1         = (0xff << 24),
};

/* DSI_FORCE_COMMIT */
enum {
	DSI_FORCE_COMMIT_USE_MMSYS = BIT(0),
	DSI_FORCE_COMMIT_ALWAYS = BIT(1),
};

struct mtk_phy_timing {
	u8 lpx;
	u8 da_hs_prepare;
	u8 da_hs_zero;
	u8 da_hs_trail;

	u8 ta_go;
	u8 ta_sure;
	u8 ta_get;
	u8 da_hs_exit;

	u8 da_hs_sync;
	u8 clk_hs_zero;
	u8 clk_hs_trail;

	u8 clk_hs_prepare;
	u8 clk_hs_post;
	u8 clk_hs_exit;

	u32 d_phy;
};

/* Functions that each SOC should provide. */
void mtk_dsi_reset(struct dsi_regs *dsi_reg);

/* Functions as weak no-ops that can be overridden. */
void mtk_dsi_override_phy_timing(struct mtk_phy_timing *timing);

/*
 * DSI-internal APIs provided in common/dsi_common.c, common/dsi_v1.c,
 * and common/mtk_mipi_{c/d}phy.c
 */
void mtk_dsi_cphy_enable(struct mipi_tx_regs *mipi_tx_reg);
void mtk_dsi_cphy_enable_cmdq_6byte(struct dsi_regs *dsi_reg);
void mtk_dsi_cphy_lane_sel_setting(struct mipi_tx_regs *mipi_tx_reg);
void mtk_dsi_cphy_timing(struct dsi_regs *dsi_reg, u32 data_rate,
			 struct mtk_phy_timing *timing);
void mtk_dsi_cphy_vdo_timing(const u32 lanes, const struct edid *edid,
			     const struct mtk_phy_timing *phy_timing,
			     const u32 bytes_per_pixel, const u32 hbp, const u32 hfp,
			     s32 *hbp_byte, s32 *hfp_byte, u32 *hsync_active_byte);
void mtk_dsi_cphy_disable_ck_mode(struct mipi_tx_regs *mipi_tx_reg);
void mtk_dsi_dphy_disable_ck_mode(struct mipi_tx_regs *mipi_tx_reg);
void mtk_dsi_dphy_timing_calculation(u32 data_rate_mhz, struct mtk_phy_timing *timing);
void mtk_dsi_configure_mipi_tx(struct mipi_tx_regs *mipi_tx_reg,
			       u32 data_rate, u32 lanes, bool is_cphy);

#endif /* SOC_MEDIATEK_DSI_COMMON_H */
