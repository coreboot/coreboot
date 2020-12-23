/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_DSI_COMMON_H
#define SOC_MEDIATEK_DSI_COMMON_H

#include <commonlib/helpers.h>
#include <edid.h>
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
	MIPI_DSI_MODE_LPM = BIT(11)
};

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
	u8 reserved6[108];
	u32 dsi_cmdq[128];
};
static struct dsi_regs *const dsi0 = (void *)DSI0_BASE;

check_member(dsi_regs, dsi_phy_lccon, 0x104);
check_member(dsi_regs, dsi_phy_timecon3, 0x11c);
check_member(dsi_regs, dsi_vm_cmd_con, 0x130);
check_member(dsi_regs, dsi_force_commit, 0x190);
check_member(dsi_regs, dsi_cmdq, 0x200);

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

/* MIPI DSI Processor-to-Peripheral transaction types */
enum {
	MIPI_DSI_V_SYNC_START				= 0x01,
	MIPI_DSI_V_SYNC_END				= 0x11,
	MIPI_DSI_H_SYNC_START				= 0x21,
	MIPI_DSI_H_SYNC_END				= 0x31,

	MIPI_DSI_COLOR_MODE_OFF				= 0x02,
	MIPI_DSI_COLOR_MODE_ON				= 0x12,
	MIPI_DSI_SHUTDOWN_PERIPHERAL			= 0x22,
	MIPI_DSI_TURN_ON_PERIPHERAL			= 0x32,

	MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM		= 0x03,
	MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM		= 0x13,
	MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM		= 0x23,

	MIPI_DSI_GENERIC_READ_REQUEST_0_PARAM		= 0x04,
	MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM		= 0x14,
	MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM		= 0x24,

	MIPI_DSI_DCS_SHORT_WRITE			= 0x05,
	MIPI_DSI_DCS_SHORT_WRITE_PARAM			= 0x15,

	MIPI_DSI_DCS_READ				= 0x06,

	MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE		= 0x37,

	MIPI_DSI_END_OF_TRANSMISSION			= 0x08,

	MIPI_DSI_NULL_PACKET				= 0x09,
	MIPI_DSI_BLANKING_PACKET			= 0x19,
	MIPI_DSI_GENERIC_LONG_WRITE			= 0x29,
	MIPI_DSI_DCS_LONG_WRITE				= 0x39,

	MIPI_DSI_LOOSELY_PACKED_PIXEL_STREAM_YCBCR20	= 0x0c,
	MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR24		= 0x1c,
	MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR16		= 0x2c,

	MIPI_DSI_PACKED_PIXEL_STREAM_30			= 0x0d,
	MIPI_DSI_PACKED_PIXEL_STREAM_36			= 0x1d,
	MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR12		= 0x3d,

	MIPI_DSI_PACKED_PIXEL_STREAM_16			= 0x0e,
	MIPI_DSI_PACKED_PIXEL_STREAM_18			= 0x1e,
	MIPI_DSI_PIXEL_STREAM_3BYTE_18			= 0x2e,
	MIPI_DSI_PACKED_PIXEL_STREAM_24			= 0x3e,
};

/* MIPI DSI Peripheral-to-Processor transaction types */
enum {
	MIPI_DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT	= 0x02,
	MIPI_DSI_RX_END_OF_TRANSMISSION			= 0x08,
	MIPI_DSI_RX_GENERIC_SHORT_READ_RESPONSE_1BYTE	= 0x11,
	MIPI_DSI_RX_GENERIC_SHORT_READ_RESPONSE_2BYTE	= 0x12,
	MIPI_DSI_RX_GENERIC_LONG_READ_RESPONSE		= 0x1a,
	MIPI_DSI_RX_DCS_LONG_READ_RESPONSE		= 0x1c,
	MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_1BYTE	= 0x21,
	MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_2BYTE	= 0x22,
};

/* MIPI DCS commands */
enum {
	MIPI_DCS_NOP			= 0x00,
	MIPI_DCS_SOFT_RESET		= 0x01,
	MIPI_DCS_GET_DISPLAY_ID		= 0x04,
	MIPI_DCS_GET_RED_CHANNEL	= 0x06,
	MIPI_DCS_GET_GREEN_CHANNEL	= 0x07,
	MIPI_DCS_GET_BLUE_CHANNEL	= 0x08,
	MIPI_DCS_GET_DISPLAY_STATUS	= 0x09,
	MIPI_DCS_GET_POWER_MODE		= 0x0A,
	MIPI_DCS_GET_ADDRESS_MODE	= 0x0B,
	MIPI_DCS_GET_PIXEL_FORMAT	= 0x0C,
	MIPI_DCS_GET_DISPLAY_MODE	= 0x0D,
	MIPI_DCS_GET_SIGNAL_MODE	= 0x0E,
	MIPI_DCS_GET_DIAGNOSTIC_RESULT	= 0x0F,
	MIPI_DCS_ENTER_SLEEP_MODE	= 0x10,
	MIPI_DCS_EXIT_SLEEP_MODE	= 0x11,
	MIPI_DCS_ENTER_PARTIAL_MODE	= 0x12,
	MIPI_DCS_ENTER_NORMAL_MODE	= 0x13,
	MIPI_DCS_EXIT_INVERT_MODE	= 0x20,
	MIPI_DCS_ENTER_INVERT_MODE	= 0x21,
	MIPI_DCS_SET_GAMMA_CURVE	= 0x26,
	MIPI_DCS_SET_DISPLAY_OFF	= 0x28,
	MIPI_DCS_SET_DISPLAY_ON		= 0x29,
	MIPI_DCS_SET_COLUMN_ADDRESS	= 0x2A,
	MIPI_DCS_SET_PAGE_ADDRESS	= 0x2B,
	MIPI_DCS_WRITE_MEMORY_START	= 0x2C,
	MIPI_DCS_WRITE_LUT		= 0x2D,
	MIPI_DCS_READ_MEMORY_START	= 0x2E,
	MIPI_DCS_SET_PARTIAL_AREA	= 0x30,
	MIPI_DCS_SET_SCROLL_AREA	= 0x33,
	MIPI_DCS_SET_TEAR_OFF		= 0x34,
	MIPI_DCS_SET_TEAR_ON		= 0x35,
	MIPI_DCS_SET_ADDRESS_MODE	= 0x36,
	MIPI_DCS_SET_SCROLL_START	= 0x37,
	MIPI_DCS_EXIT_IDLE_MODE		= 0x38,
	MIPI_DCS_ENTER_IDLE_MODE	= 0x39,
	MIPI_DCS_SET_PIXEL_FORMAT	= 0x3A,
	MIPI_DCS_WRITE_MEMORY_CONTINUE	= 0x3C,
	MIPI_DCS_READ_MEMORY_CONTINUE	= 0x3E,
	MIPI_DCS_SET_TEAR_SCANLINE	= 0x44,
	MIPI_DCS_GET_SCANLINE		= 0x45,
	MIPI_DCS_READ_DDB_START		= 0xA1,
	MIPI_DCS_READ_DDB_CONTINUE	= 0xA8,
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

/* Definitions for cmd in lcm_init_command */
#define LCM_END_CMD 0
#define LCM_DELAY_CMD 1
#define LCM_GENERIC_CMD 2
#define LCM_DCS_CMD 3

struct lcm_init_command {
	u8 cmd;
	u8 len;
	u8 data[];
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
