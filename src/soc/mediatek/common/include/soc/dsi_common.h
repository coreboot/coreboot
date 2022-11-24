/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_DSI_COMMON_H
#define SOC_MEDIATEK_DSI_COMMON_H

#include <commonlib/helpers.h>
#include <edid.h>
#include <mipi/dsi.h>
#include <types.h>
#include <soc/addressmap.h>

enum mipi_dsi_pixel_format {
	MIPI_DSI_FMT_RGB888,
	MIPI_DSI_FMT_RGB666,
	MIPI_DSI_FMT_RGB666_PACKED,
	MIPI_DSI_FMT_RGB565
};

/* video mode */
enum {
	MIPI_DSI_MODE_VIDEO = BIT(0),
	/* video burst mode */
	MIPI_DSI_MODE_VIDEO_BURST = BIT(1),
	/* video pulse mode */
	MIPI_DSI_MODE_VIDEO_SYNC_PULSE = BIT(2),
	/* enable auto vertical count mode */
	MIPI_DSI_MODE_VIDEO_AUTO_VERT = BIT(3),
	/* enable hsync-end packets in vsync-pulse and v-porch area */
	MIPI_DSI_MODE_VIDEO_HSE = BIT(4),
	/* disable hfront-porch area */
	MIPI_DSI_MODE_VIDEO_HFP = BIT(5),
	/* disable hback-porch area */
	MIPI_DSI_MODE_VIDEO_HBP = BIT(6),
	/* disable hsync-active area */
	MIPI_DSI_MODE_VIDEO_HSA = BIT(7),
	/* flush display FIFO on vsync pulse */
	MIPI_DSI_MODE_VSYNC_FLUSH = BIT(8),
	/* disable EoT packets in HS mode */
	MIPI_DSI_MODE_EOT_PACKET = BIT(9),
	/* device supports non-continuous clock behavior (DSI spec 5.6.1) */
	MIPI_DSI_CLOCK_NON_CONTINUOUS = BIT(10),
	/* transmit data in low power */
	MIPI_DSI_MODE_LPM = BIT(11),
	/* dsi per line's data end same time on all lanes */
	MIPI_DSI_MODE_LINE_END = BIT(12),
};

static struct dsi_regs *const dsi0 = (void *)DSI0_BASE;

/* DSI_INTSTA */
enum {
	LPRX_RD_RDY_INT_FLAG = BIT(0),
	CMD_DONE_INT_FLAG    = BIT(1),
	TE_RDY_INT_FLAG      = BIT(2),
	VM_DONE_INT_FLAG     = BIT(3),
	EXT_TE_RDY_INT_FLAG  = BIT(4),
	DSI_BUSY             = BIT(31),
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
void mtk_dsi_reset(void);
void mtk_dsi_configure_mipi_tx(u32 data_rate, u32 lanes);

/* Functions as weak no-ops that can be overridden. */
void mtk_dsi_override_phy_timing(struct mtk_phy_timing *timing);

/* Public API provided in common/dsi.c */
int mtk_dsi_bpp_from_format(u32 format);
int mtk_dsi_init(u32 mode_flags, u32 format, u32 lanes, const struct edid *edid,
		 const u8 *init_commands);

#endif /* SOC_MEDIATEK_DSI_COMMON_H */
