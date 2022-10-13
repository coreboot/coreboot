/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <soc/dp_intf.h>

const struct mtk_dpintf dpintf_data = {
	.regs = (void *)(DP_INTF0_BASE),
	.color_format = MTK_DPINTF_COLOR_FORMAT_RGB,
	.yc_map = MTK_DPINTF_OUT_YC_MAP_RGB,
	.bit_num = MTK_DPINTF_OUT_BIT_NUM_8BITS,
	.channel_swap = MTK_DPINTF_OUT_CHANNEL_SWAP_RGB,
	.input_mode = MTK_DPINTF_INPUT_MODE_2P,
};
