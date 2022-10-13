/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_DP_DP_INTF_H
#define SOC_MEDIATEK_COMMON_DP_DP_INTF_H

#include <edid.h>
#include <types.h>

#define DPINTF_EN			0x00
#define EN				BIT(0)

#define DPINTF_RET			0x04
#define RST				BIT(0)
#define RST_SEL				BIT(16)

#define DPINTF_INTEN			0x08
#define INT_VSYNC_EN			BIT(0)
#define INT_VDE_EN			BIT(1)
#define INT_UNDERFLOW_EN		BIT(2)
#define INT_TARGET_LINE_EN		BIT(3)

#define DPINTF_INTSTA			0x0C
#define INT_VSYNC_STA			BIT(0)
#define INT_VDE_STA			BIT(1)
#define INT_UNDERFLOW_STA		BIT(2)
#define INT_TARGET_LINE_STA		BIT(3)

#define DPINTF_CON			0x10
#define BG_ENABLE			BIT(0)
#define INTL_EN				BIT(2)
#define TDFP_EN				BIT(3)
#define VS_LODD_EN			BIT(16)
#define VS_LEVEN_EN			BIT(17)
#define VS_RODD_EN			BIT(18)
#define VS_REVEN			BIT(19)
#define FAKE_DE_LODD			BIT(20)
#define FAKE_DE_LEVEN			BIT(21)
#define FAKE_DE_RODD			BIT(22)
#define FAKE_DE_REVEN			BIT(23)
#define YUV422_EN			BIT(24)
#define CLPF_EN				BIT(25)
#define MATRIX_EN			BIT(26)
#define INTERNAL_CG_EN			BIT(27)
#define LOWPOWER_EN			BIT(28)
#define INPUT_2P_EN			BIT(29)
#define EXT_VSYNC_EN			BIT(30)

#define DPINTF_OUTPUT_SETTING		0x14
#define PIXEL_SWAP			BIT(0)
#define CH_SWAP				BIT(1)
#define CH_SWAP_MASK			(0x7 << 1)
#define SWAP_RGB			(0x00 << 1)
#define SWAP_GBR			(0x01 << 1)
#define SWAP_BRG			(0x02 << 1)
#define SWAP_RBG			(0x03 << 1)
#define SWAP_GRB			(0x04 << 1)
#define SWAP_BGR			(0x05 << 1)
#define B_MASK				BIT(4)
#define G_MASK				BIT(5)
#define R_MASK				BIT(6)
#define DE_MASK				BIT(8)
#define HS_MASK				BIT(9)
#define VS_MASK				BIT(10)
#define HSYNC_POL			BIT(13)
#define VSYNC_POL			BIT(14)
#define OUT_BIT				BIT(16)
#define OUT_BIT_MASK			(0x3 << 18)
#define OUT_BIT_8			(0x00 << 18)
#define OUT_BIT_10			(0x01 << 18)
#define OUT_BIT_12			(0x02 << 18)
#define OUT_BIT_16			(0x03 << 18)

#define DPINTF_SIZE			0x18
#define HSIZE				0
#define HSIZE_MASK			(0xffff << 0)
#define VSIZE				16
#define VSIZE_MASK			(0xffff << 16)

#define DPINTF_TGEN_HWIDTH		0x20
#define HPW				0
#define HPW_MASK			(0xffff << 0)

#define DPINTF_TGEN_HPORCH		0x24
#define HBP				0
#define HBP_MASK			(0xffff << 0)
#define HFP				16
#define HFP_MASK			(0xffff << 16)

#define DPINTF_TGEN_VWIDTH		0x28
#define VSYNC_WIDTH_SHIFT		0
#define VSYNC_WIDTH_MASK		(0xffff << 0)
#define VSYNC_HALF_LINE_SHIFT		16
#define VSYNC_HALF_LINE_MASK		BIT(16)


#define DPINTF_TGEN_VPORCH		0x2C
#define VSYNC_BACK_PORCH_SHIFT		0
#define VSYNC_BACK_PORCH_MASK		(0xffff << 0)
#define VSYNC_FRONT_PORCH_SHIFT		16
#define VSYNC_FRONT_PORCH_MASK		(0xffff << 16)

#define DPINTF_BG_HCNTL			0x30
#define BG_RIGHT			(0xffff << 0)
#define BG_LEFT				(0xffff << 16)

#define DPINTF_BG_VCNTL			0x34
#define BG_BOT				(0xffff << 0)
#define BG_TOP				(0xffff << 16)

#define DPINTF_BG_COLOR			0x38
#define BG_B				(0x3ff << 0)
#define BG_G				(0x3ff << 10)
#define BG_R				(0x3ff << 20)

#define DPINTF_FIFO_CTL			0x3C
#define FIFO_VALID_SET			(0x1F << 0)
#define FIFO_RST_SEL			BIT(8)
#define FIFO_RD_MASK			BIT(12)

#define DPINTF_STATUS			0x40
#define VCOUNTER			(0x3ffff << 0)
#define DPINTF_BUSY			BIT(24)
#define FIELD				BIT(28)
#define TDLR				BIT(29)

#define DPINTF_TGEN_VWIDTH_LEVEN	0x68
#define DPINTF_TGEN_VPORCH_LEVEN	0x6C
#define DPINTF_TGEN_VWIDTH_RODD		0x70
#define DPINTF_TGEN_VPORCH_RODD		0x74
#define DPINTF_TGEN_VWIDTH_REVEN	0x78
#define DPINTF_TGEN_VPORCH_REVEN	0x7C

#define DPINTF_CLPF_SETTING		0x94
#define CLPF_TYPE			(0x3 << 0)
#define ROUND_EN			BIT(4)

#define DPINTF_Y_LIMIT			0x98
#define Y_LIMINT_BOT			0
#define Y_LIMINT_BOT_MASK		(0xFFF << 0)
#define Y_LIMINT_TOP			16
#define Y_LIMINT_TOP_MASK		(0xFFF << 16)

#define DPINTF_C_LIMIT			0x9C
#define C_LIMIT_BOT			0
#define C_LIMIT_BOT_MASK		(0xFFF << 0)
#define C_LIMIT_TOP			16
#define C_LIMIT_TOP_MASK		(0xFFF << 16)

#define DPINTF_YUV422_SETTING		0xA0
#define UV_SWAP				BIT(0)
#define CR_DELSEL			BIT(4)
#define CB_DELSEL			BIT(5)
#define Y_DELSEL			BIT(6)
#define DE_DELSEL			BIT(7)

#define DPINTF_MATRIX_SET		0xB4
#define INT_MATRIX_SEL_MASK		0x1f
#define RGB_TO_JPEG			0x00
#define RGB_TO_FULL709			0x01
#define RGB_TO_BT601			0x02
#define RGB_TO_BT709			0x03
#define JPEG_TO_RGB			0x04
#define FULL709_TO_RGB			0x05
#define BT601_TO_RGB			0x06
#define BT709_TO_RGB			0x07
#define JPEG_TO_BT601			0x08
#define JPEG_TO_BT709			0x09
#define BT601_TO_JPEG			0xA
#define BT709_TO_JPEG			0xB
#define BT709_TO_BT601			0xC
#define BT601_TO_BT709			0xD
#define JPEG_TO_CERGB			0x14
#define FULL709_TO_CERGB		0x15
#define BT601_TO_CERGB			0x16
#define BT709_TO_CERGB			0x17
#define RGB_TO_CERGB			0x1C

#define MATRIX_BIT_MASK			(0x3 << 8)
#define EXT_MATRIX_EN			BIT(12)

enum mtk_dpintf_out_bit_num {
	MTK_DPINTF_OUT_BIT_NUM_8BITS,
	MTK_DPINTF_OUT_BIT_NUM_10BITS,
	MTK_DPINTF_OUT_BIT_NUM_12BITS,
	MTK_DPINTF_OUT_BIT_NUM_16BITS,
};

enum mtk_dpintf_out_yc_map {
	MTK_DPINTF_OUT_YC_MAP_RGB,
	MTK_DPINTF_OUT_YC_MAP_CYCY,
	MTK_DPINTF_OUT_YC_MAP_YCYC,
	MTK_DPINTF_OUT_YC_MAP_CY,
	MTK_DPINTF_OUT_YC_MAP_YC,
};

enum mtk_dpintf_out_channel_swap {
	MTK_DPINTF_OUT_CHANNEL_SWAP_RGB,
	MTK_DPINTF_OUT_CHANNEL_SWAP_GBR,
	MTK_DPINTF_OUT_CHANNEL_SWAP_BRG,
	MTK_DPINTF_OUT_CHANNEL_SWAP_RBG,
	MTK_DPINTF_OUT_CHANNEL_SWAP_GRB,
	MTK_DPINTF_OUT_CHANNEL_SWAP_BGR,
};

enum mtk_dpintf_out_color_format {
	MTK_DPINTF_COLOR_FORMAT_RGB,
	MTK_DPINTF_COLOR_FORMAT_RGB_FULL,
	MTK_DPINTF_COLOR_FORMAT_YCBCR_444,
	MTK_DPINTF_COLOR_FORMAT_YCBCR_422,
	MTK_DPINTF_COLOR_FORMAT_XV_YCC,
	MTK_DPINTF_COLOR_FORMAT_YCBCR_444_FULL,
	MTK_DPINTF_COLOR_FORMAT_YCBCR_422_FULL,
};

enum mtk_dpintf_input_mode {
	MTK_DPINTF_INPUT_MODE_1P = 0,
	MTK_DPINTF_INPUT_MODE_2P = INPUT_2P_EN,
};

enum TVDPLL_CLK {
	TVDPLL_PLL = 0,
	TVDPLL_D2 = 1,
	TVDPLL_D4 = 3,
	TVDPLL_D8 = 5,
	TVDPLL_D16 = 7,
};

struct mtk_dpintf {
	void *regs;
	enum mtk_dpintf_out_color_format color_format;
	enum mtk_dpintf_out_yc_map yc_map;
	enum mtk_dpintf_out_bit_num bit_num;
	enum mtk_dpintf_out_channel_swap channel_swap;
	enum mtk_dpintf_input_mode input_mode;
};

enum mtk_dpintf_polarity {
	MTK_DPINTF_POLARITY_RISING,
	MTK_DPINTF_POLARITY_FALLING,
};

struct mtk_dpintf_polarities {
	enum mtk_dpintf_polarity de_pol;
	enum mtk_dpintf_polarity ck_pol;
	enum mtk_dpintf_polarity hsync_pol;
	enum mtk_dpintf_polarity vsync_pol;
};

struct mtk_dpintf_sync_param {
	u32 sync_width;
	u32 front_porch;
	u32 back_porch;
	bool shift_half_line;
};

struct mtk_dpintf_yc_limit {
	u16 y_top;
	u16 y_bottom;
	u16 c_top;
	u16 c_bottom;
};

extern const struct mtk_dpintf dpintf_data;

void dp_intf_config(const struct edid *edid);

#endif /* SOC_MEDIATEK_COMMON_DP_DP_INTF_H */
