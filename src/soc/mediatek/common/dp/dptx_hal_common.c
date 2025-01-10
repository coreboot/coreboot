/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <edid.h>
#include <soc/dptx.h>
#include <soc/dptx_hal.h>
#include <soc/dptx_reg.h>
#include <types.h>

#define REG_OFFSET_LIMIT 0x8000

u32 mtk_dp_read(struct mtk_dp *mtk_dp, u32 offset)
{
	void *addr = mtk_dp->regs + offset;

	assert(mtk_dp->regs);
	assert(offset % 4 == 0 && offset <= REG_OFFSET_LIMIT);

	return read32(addr);
}
void mtk_dp_write(struct mtk_dp *mtk_dp, u32 offset, u32 val)
{
	void *addr = mtk_dp->regs + offset;

	assert(mtk_dp->regs);
	assert(offset % 4 == 0 && offset <= REG_OFFSET_LIMIT);

	write32(addr, val);
}

void mtk_dp_mask(struct mtk_dp *mtk_dp, u32 offset, u32 val, u32 mask)
{
	void *addr = mtk_dp->regs + offset;

	assert(mtk_dp->regs);
	assert(offset % 4 == 0 && offset <= REG_OFFSET_LIMIT);
	assert((val & mask) == val);

	clrsetbits32(addr, mask, val);
}

void mtk_dp_write_byte(struct mtk_dp *mtk_dp, u32 addr, u8 val, u8 mask)
{
	if (addr % 2) {
		mtk_dp_write(mtk_dp, DP_TX_TOP_APB_WSTRB, 0x12);
		mtk_dp_mask(mtk_dp, addr - 1, val << 8, mask << 8);
	} else {
		mtk_dp_write(mtk_dp, DP_TX_TOP_APB_WSTRB, 0x11);
		mtk_dp_mask(mtk_dp, addr, val, mask);
	}

	mtk_dp_write(mtk_dp, DP_TX_TOP_APB_WSTRB, 0x0);
}

u32 mtk_dp_phy_read(struct mtk_dp *mtk_dp, u32 offset)
{
	void *addr = mtk_dp->phy_regs + offset;

	assert(mtk_dp->phy_regs);
	assert(offset % 4 == 0 && offset <= REG_OFFSET_LIMIT);

	return read32(addr);
}

void mtk_dp_phy_write(struct mtk_dp *mtk_dp, u32 offset, u32 val)
{
	void *addr = mtk_dp->phy_regs + offset;

	assert(mtk_dp->phy_regs);
	assert(offset % 4 == 0 && offset <= REG_OFFSET_LIMIT);

	write32(addr, val);
}

void mtk_dp_phy_mask(struct mtk_dp *mtk_dp, u32 offset, u32 val, u32 mask)
{
	void *addr = mtk_dp->phy_regs + offset;

	assert(mtk_dp->phy_regs);
	assert(offset % 4 == 0 && offset <= REG_OFFSET_LIMIT);
	assert((val & mask) == val);

	clrsetbits32(addr, mask, val);
}

void dptx_hal_verify_clock(struct mtk_dp *mtk_dp)
{
	u32 m, n, ls_clk, pix_clk;

	m = mtk_dp_read(mtk_dp, REG_33C8_DP_ENCODER1_P0);
	n = 0x8000;
	ls_clk = mtk_dp->train_info.linkrate;
	ls_clk *= 27;

	pix_clk = m * ls_clk / n;
	printk(BIOS_DEBUG, "DPTX calc pixel clock = %d MHz, dp_intf clock = %dMHz\n", pix_clk,
	       pix_clk / 4);
}

void dptx_hal_bypassmsa_en(struct mtk_dp *mtk_dp, bool enable)
{
	mtk_dp_mask(mtk_dp, REG_3030_DP_ENCODER0_P0, enable ? 0 : 0x3ff, 0x3ff);
}

void dptx_hal_set_msa(struct mtk_dp *mtk_dp)
{
	u32 va, vsync, vbp, vfp, vtotal, ha, hsync, hbp, hfp, htotal;
	struct edid *edid = mtk_dp->edid;

	va = edid->mode.va;
	vsync = edid->mode.vspw;
	vbp = edid->mode.vbl - edid->mode.vso -
	      edid->mode.vspw - edid->mode.vborder;
	vfp = edid->mode.vso - edid->mode.vborder;

	ha = edid->mode.ha;
	hsync = edid->mode.hspw;
	hbp = edid->mode.hbl - edid->mode.hso -
	      edid->mode.hspw - edid->mode.hborder;
	hfp = edid->mode.hso - edid->mode.hborder;

	htotal = ha + hsync + hbp + hfp;
	vtotal = va + vsync + vbp + vfp;

	/* horizontal */
	DP_WRITE2BYTE(mtk_dp, REG_3010_DP_ENCODER0_P0, htotal);
	DP_WRITE2BYTE(mtk_dp, REG_3018_DP_ENCODER0_P0, hsync + hbp);
	mtk_dp_mask(mtk_dp, REG_3028_DP_ENCODER0_P0,
		    hsync << HSW_SW_DP_ENCODER0_P0_FLDMASK_POS,
		    HSW_SW_DP_ENCODER0_P0_FLDMASK);
	mtk_dp_mask(mtk_dp, REG_3028_DP_ENCODER0_P0,
		    0 << HSP_SW_DP_ENCODER0_P0_FLDMASK_POS,
		    HSP_SW_DP_ENCODER0_P0_FLDMASK);
	DP_WRITE2BYTE(mtk_dp, REG_3020_DP_ENCODER0_P0, ha);
	/* vertical */
	DP_WRITE2BYTE(mtk_dp, REG_3014_DP_ENCODER0_P0, vtotal);
	DP_WRITE2BYTE(mtk_dp, REG_301C_DP_ENCODER0_P0, vsync + vbp);
	mtk_dp_mask(mtk_dp, REG_302C_DP_ENCODER0_P0,
		    vsync << VSW_SW_DP_ENCODER0_P0_FLDMASK_POS,
		    VSW_SW_DP_ENCODER0_P0_FLDMASK);
	mtk_dp_mask(mtk_dp, REG_302C_DP_ENCODER0_P0,
		    0 << VSP_SW_DP_ENCODER0_P0_FLDMASK_POS,
		    VSP_SW_DP_ENCODER0_P0_FLDMASK);
	DP_WRITE2BYTE(mtk_dp, REG_3024_DP_ENCODER0_P0, va);
	/* horizontal */
	DP_WRITE2BYTE(mtk_dp, REG_3064_DP_ENCODER0_P0, ha);
	DP_WRITE2BYTE(mtk_dp, REG_3154_DP_ENCODER0_P0, htotal);
	DP_WRITE2BYTE(mtk_dp, REG_3158_DP_ENCODER0_P0, hfp);
	DP_WRITE2BYTE(mtk_dp, REG_315C_DP_ENCODER0_P0, hsync);
	DP_WRITE2BYTE(mtk_dp, REG_3160_DP_ENCODER0_P0, hsync + hbp);
	DP_WRITE2BYTE(mtk_dp, REG_3164_DP_ENCODER0_P0, ha);
	/* vertical */
	DP_WRITE2BYTE(mtk_dp, REG_3168_DP_ENCODER0_P0, vtotal);
	DP_WRITE2BYTE(mtk_dp, REG_316C_DP_ENCODER0_P0, vfp);
	DP_WRITE2BYTE(mtk_dp, REG_3170_DP_ENCODER0_P0, vsync);
	DP_WRITE2BYTE(mtk_dp, REG_3174_DP_ENCODER0_P0, vsync + vbp);
	DP_WRITE2BYTE(mtk_dp, REG_3178_DP_ENCODER0_P0, va);

	printk(BIOS_INFO, "MSA:Htt(%d), Vtt(%d), Hact(%d), Vact(%d), FPS(%d)\n",
	       htotal, vtotal, ha, va,
	       edid->mode.pixel_clock * 1000 / htotal / vtotal);
}

void dptx_hal_set_color_depth(struct mtk_dp *mtk_dp, u8 color_depth)
{
	u8 val;

	mtk_dp_write_byte(mtk_dp, REG_3034_DP_ENCODER0_P0, color_depth << 0x5, 0xe0);

	switch (color_depth) {
	case DP_COLOR_DEPTH_6BIT:
		val = 4;
		break;
	case DP_COLOR_DEPTH_8BIT:
		val = 3;
		break;
	case DP_COLOR_DEPTH_10BIT:
		val = 2;
		break;
	case DP_COLOR_DEPTH_12BIT:
		val = 1;
		break;
	case DP_COLOR_DEPTH_16BIT:
		val = 0;
		break;
	default:
		return;
	}
	mtk_dp_write_byte(mtk_dp, REG_303C_DP_ENCODER0_P0 + 1, val, 0x7);
}

void dptx_hal_setmisc(struct mtk_dp *mtk_dp, u8 cmisc[2])
{
	mtk_dp_write_byte(mtk_dp, REG_3034_DP_ENCODER0_P0, cmisc[0], 0xfe);
	mtk_dp_write_byte(mtk_dp, REG_3034_DP_ENCODER0_P0 + 1, cmisc[1], 0xff);
}

void dptx_hal_overwrite_mn(struct mtk_dp *mtk_dp, bool enable, u32 video_m, u32 video_n)
{
	if (enable) {
		/* Turn on overwrite MN */
		DP_WRITE2BYTE(mtk_dp, REG_3008_DP_ENCODER0_P0, video_m & 0xffff);
		DP_WRITE1BYTE(mtk_dp, REG_300C_DP_ENCODER0_P0, (video_m >> 16) & 0xff);
		DP_WRITE2BYTE(mtk_dp, REG_3044_DP_ENCODER0_P0, video_n & 0xffff);
		DP_WRITE1BYTE(mtk_dp, REG_3048_DP_ENCODER0_P0, (video_n >> 16) & 0xff);
		DP_WRITE2BYTE(mtk_dp, REG_3050_DP_ENCODER0_P0, video_n & 0xffff);

		/* Add legerII. */
		DP_WRITE1BYTE(mtk_dp, REG_3054_DP_ENCODER0_P0, (video_n >> 16) & 0xff);
		mtk_dp_write_byte(mtk_dp, REG_3004_DP_ENCODER0_P0 + 1, BIT(0), BIT(0));
	} else {
		/* Turn off overwrite MN */
		mtk_dp_write_byte(mtk_dp, REG_3004_DP_ENCODER0_P0 + 1, 0, BIT(0));
	}
}

u8 dptx_hal_get_colorbpp(struct mtk_dp *mtk_dp)
{
	u8 color_bpp;
	u8 color_depth = mtk_dp->info.depth;
	u8 color_format = mtk_dp->info.format;

	switch (color_depth) {
	case DP_COLOR_DEPTH_6BIT:
		if (color_format == DP_COLOR_FORMAT_YUV_422)
			color_bpp = 16;
		else if (color_format == DP_COLOR_FORMAT_YUV_420)
			color_bpp = 12;
		else
			color_bpp = 18;
		break;
	case DP_COLOR_DEPTH_8BIT:
		if (color_format == DP_COLOR_FORMAT_YUV_422)
			color_bpp = 16;
		else if (color_format == DP_COLOR_FORMAT_YUV_420)
			color_bpp = 12;
		else
			color_bpp = 24;
		break;
	case DP_COLOR_DEPTH_10BIT:
		if (color_format == DP_COLOR_FORMAT_YUV_422)
			color_bpp = 20;
		else if (color_format == DP_COLOR_FORMAT_YUV_420)
			color_bpp = 15;
		else
			color_bpp = 30;
		break;
	case DP_COLOR_DEPTH_12BIT:
		if (color_format == DP_COLOR_FORMAT_YUV_422)
			color_bpp = 24;
		else if (color_format == DP_COLOR_FORMAT_YUV_420)
			color_bpp = 18;
		else
			color_bpp = 36;
		break;
	case DP_COLOR_DEPTH_16BIT:
		if (color_format == DP_COLOR_FORMAT_YUV_422)
			color_bpp = 32;
		else if (color_format == DP_COLOR_FORMAT_YUV_420)
			color_bpp = 24;
		else
			color_bpp = 48;
		break;
	default:
		color_bpp = 24;
		printk(BIOS_ERR, "Set wrong bpp = %d\n", color_bpp);
		break;
	}

	return color_bpp;
}

void dptx_hal_settu_sramrd_start(struct mtk_dp *mtk_dp, u16 value)
{
	/*
	 * [5:0] video sram start address
	 * modify in 480P case only, default=0x1f
	 */
	mtk_dp_write_byte(mtk_dp, REG_303C_DP_ENCODER0_P0, (u8)value, 0x3f);
}

void dptx_hal_setsdp_downcnt_init_inhblanking(struct mtk_dp *mtk_dp, u16 value)
{
	DP_WRITE2BYTE(mtk_dp, REG_3364_DP_ENCODER1_P0, value);
}

void dptx_hal_setsdp_downcnt_init(struct mtk_dp *mtk_dp, u16 value)
{
	DP_WRITE2BYTE(mtk_dp, REG_3040_DP_ENCODER0_P0, value);
}

bool dptx_hal_auxread_bytes(struct mtk_dp *mtk_dp, u8 cmd, u32 dpcd_addr, size_t length,
			    u8 *rx_buf)
{
	bool valid_cmd = false;
	u8 reply_cmd, aux_irq_status;
	int rd_count;
	u32 wait_reply_count = AUX_WAITREPLY_LPNUM;

	DP_WRITE1BYTE(mtk_dp, REG_3640_AUX_TX_P0, 0x7f);
	mdelay(1);

	if (length > 16 || (cmd == AUX_CMD_NATIVE_R && length == 0x0))
		return false;

	DP_WRITE1BYTE(mtk_dp, REG_3650_AUX_TX_P0 + 1, 0x1);
	DP_WRITE1BYTE(mtk_dp, REG_3644_AUX_TX_P0, cmd);
	DP_WRITE2BYTE(mtk_dp, REG_3648_AUX_TX_P0, dpcd_addr & 0xffff);
	DP_WRITE1BYTE(mtk_dp, REG_364C_AUX_TX_P0, (dpcd_addr >> 16) & 0xf);

	if (length > 0) {
		mtk_dp_mask(mtk_dp, REG_3650_AUX_TX_P0,
			    (length - 1) << MCU_REQ_DATA_NUM_AUX_TX_P0_FLDMASK_POS,
			    MCU_REQUEST_DATA_NUM_AUX_TX_P0_FLDMASK);
		DP_WRITE1BYTE(mtk_dp, REG_362C_AUX_TX_P0, 0x0);
	}

	if (cmd == AUX_CMD_I2C_R || cmd == AUX_CMD_I2C_R_MOT0)
		if (length == 0x0)
			mtk_dp_mask(mtk_dp, REG_362C_AUX_TX_P0,
				    0x1 << AUX_NO_LENGTH_AUX_TX_P0_FLDMASK_POS,
				    AUX_NO_LENGTH_AUX_TX_P0_FLDMASK);

	mtk_dp_mask(mtk_dp, REG_3630_AUX_TX_P0,
		    0x1 << AUX_TX_REQUEST_READY_AUX_TX_P0_FLDMASK_POS,
		    AUX_TX_REQUEST_READY_AUX_TX_P0_FLDMASK);

	while (--wait_reply_count) {
		if (mtk_dp_read(mtk_dp, REG_3618_AUX_TX_P0) &
		    AUX_RX_FIFO_WRITE_POINTER_AUX_TX_P0_FLDMASK) {
			valid_cmd = true;
			break;
		}

		if (mtk_dp_read(mtk_dp, REG_3618_AUX_TX_P0) &
		    AUX_RX_FIFO_FULL_AUX_TX_P0_FLDMASK) {
			valid_cmd = true;
			break;
		}

		aux_irq_status = mtk_dp_read(mtk_dp, REG_3640_AUX_TX_P0) & 0xff;

		if (aux_irq_status & AUX_RX_RECV_COMPLETE_IRQ_TX_P0_FLDMASK) {
			valid_cmd = true;
			break;
		}

		if (aux_irq_status & AUX_400US_TIMEOUT_IRQ_AUX_TX_P0_FLDMASK) {
			printk(BIOS_ERR, "(AUX Read)HW Timeout 400us irq\n");
			break;
		}
	}

	reply_cmd = mtk_dp_read(mtk_dp, REG_3624_AUX_TX_P0) & 0xf;
	if (reply_cmd)
		printk(BIOS_ERR, "reply_cmd(%#x), NACK or Defer\n", reply_cmd);

	if (wait_reply_count == 0x0 || reply_cmd) {
		u8 phy_status = 0x0;

		phy_status = mtk_dp_read(mtk_dp, REG_3628_AUX_TX_P0);
		if (phy_status != 0x1)
			printk(BIOS_ERR, "Aux read: aux hang, need sw reset\n");

		mtk_dp_mask(mtk_dp, REG_3650_AUX_TX_P0,
			    0x1 << MCU_ACK_TRAN_COMPLETE_AUX_TX_P0_FLDMASK_POS,
			    MCU_ACK_TRANSACTION_COMPLETE_AUX_TX_P0_FLDMASK);
		DP_WRITE1BYTE(mtk_dp, REG_3640_AUX_TX_P0, 0x7f);

		mdelay(1);
		printk(BIOS_ERR, "wait_reply_count(%#x), TimeOut\n", wait_reply_count);
		return false;
	}

	if (length == 0) {
		DP_WRITE1BYTE(mtk_dp, REG_362C_AUX_TX_P0, 0x0);
	} else {
		if (valid_cmd) {
			mtk_dp_mask(mtk_dp, REG_3620_AUX_TX_P0,
				    0x0 << AUX_RD_MODE_AUX_TX_P0_FLDMASK_POS,
				    AUX_RD_MODE_AUX_TX_P0_FLDMASK);

			for (rd_count = 0; rd_count < length; rd_count++) {
				mtk_dp_mask(mtk_dp, REG_3620_AUX_TX_P0,
					    0x1 << AUX_RX_FIFO_R_PULSE_TX_P0_FLDMASK_POS,
					    AUX_RX_FIFO_READ_PULSE_TX_P0_FLDMASK);
				mdelay(1);
				*(rx_buf + rd_count) = mtk_dp_read(mtk_dp, REG_3620_AUX_TX_P0);
			}
		} else {
			printk(BIOS_INFO, "Read TimeOut %#x\n", dpcd_addr);
		}
	}

	mtk_dp_mask(mtk_dp, REG_3650_AUX_TX_P0,
		    0x1 << MCU_ACK_TRAN_COMPLETE_AUX_TX_P0_FLDMASK_POS,
		    MCU_ACK_TRANSACTION_COMPLETE_AUX_TX_P0_FLDMASK);
	DP_WRITE1BYTE(mtk_dp, REG_3640_AUX_TX_P0, 0x7f);

	mdelay(1);
	return valid_cmd;
}

bool dptx_hal_auxwrite_bytes(struct mtk_dp *mtk_dp, u8 cmd, u32 dpcd_addr, size_t length,
			     u8 *data)
{
	bool valid_cmd = false;
	u8 reply_cmd;
	int i;
	u16 wait_reply_count = AUX_WAITREPLY_LPNUM;
	int reg_idx;

	mtk_dp_write_byte(mtk_dp, REG_3704_AUX_TX_P0,
			  1 << AUX_TX_FIFO_NEW_MODE_EN_AUX_TX_P0_FLDMASK_POS,
			  AUX_TX_FIFO_NEW_MODE_EN_AUX_TX_P0_FLDMASK);
	DP_WRITE1BYTE(mtk_dp, REG_3650_AUX_TX_P0 + 1, 0x1);
	DP_WRITE1BYTE(mtk_dp, REG_3640_AUX_TX_P0, 0x7f);
	mdelay(1);

	DP_WRITE1BYTE(mtk_dp, REG_3650_AUX_TX_P0 + 1, 0x1);
	DP_WRITE1BYTE(mtk_dp, REG_3644_AUX_TX_P0, cmd);
	DP_WRITE1BYTE(mtk_dp, REG_3648_AUX_TX_P0, dpcd_addr & 0xff);
	DP_WRITE1BYTE(mtk_dp, REG_3648_AUX_TX_P0 + 1, (dpcd_addr >> 8) & 0xff);
	DP_WRITE1BYTE(mtk_dp, REG_364C_AUX_TX_P0, (dpcd_addr >> 16) & 0xf);

	if (length > 0) {
		DP_WRITE1BYTE(mtk_dp, REG_362C_AUX_TX_P0, 0x0);
		for (i = 0; i < (length + 1) / 2; i++)
			for (reg_idx = 0; reg_idx < 2; reg_idx++)
				if ((i * 2 + reg_idx) < length)
					DP_WRITE1BYTE(mtk_dp,
						      REG_3708_AUX_TX_P0 + i * 4 + reg_idx,
						      data[i * 2 + reg_idx]);
		DP_WRITE1BYTE(mtk_dp, REG_3650_AUX_TX_P0 + 1, ((length - 1) & 0xf) << 4);
	} else {
		DP_WRITE1BYTE(mtk_dp, REG_362C_AUX_TX_P0, 0x1);
	}

	mtk_dp_write_byte(mtk_dp, REG_3704_AUX_TX_P0,
			  AUX_TX_FIFO_WRITE_DATA_NEW_MODE_TOGGLE_AUX_TX_P0_FLDMASK,
			  AUX_TX_FIFO_WRITE_DATA_NEW_MODE_TOGGLE_AUX_TX_P0_FLDMASK);
	DP_WRITE1BYTE(mtk_dp, REG_3630_AUX_TX_P0, 0x8);

	while (--wait_reply_count) {
		u8 aux_irq_status;

		aux_irq_status = mtk_dp_read(mtk_dp, REG_3640_AUX_TX_P0) & 0xff;
		mdelay(1);
		if (aux_irq_status & AUX_RX_RECV_COMPLETE_IRQ_TX_P0_FLDMASK) {
			valid_cmd = true;
			break;
		}

		if (aux_irq_status & AUX_400US_TIMEOUT_IRQ_AUX_TX_P0_FLDMASK)
			break;
	}

	reply_cmd = mtk_dp_read(mtk_dp, REG_3624_AUX_TX_P0) & 0xf;
	if (reply_cmd)
		printk(BIOS_ERR, "reply_cmd(%#x), NACK or Defer\n", reply_cmd);

	if (wait_reply_count == 0x0 || reply_cmd) {
		u8 phy_status = 0x0;

		phy_status = mtk_dp_read(mtk_dp, REG_3628_AUX_TX_P0);
		if (phy_status != 0x1)
			printk(BIOS_ERR, "Aux write: aux hang, need SW reset!\n");

		DP_WRITE1BYTE(mtk_dp, REG_3650_AUX_TX_P0 + 1, 0x1);
		DP_WRITE1BYTE(mtk_dp, REG_3640_AUX_TX_P0, 0x7f);

		mdelay(1);

		printk(BIOS_INFO, "reply_cmd(%#x), wait_reply_count(%d)\n", reply_cmd,
		       wait_reply_count);
		return false;
	}

	DP_WRITE1BYTE(mtk_dp, REG_3650_AUX_TX_P0 + 1, 0x1);

	if (length == 0)
		DP_WRITE1BYTE(mtk_dp, REG_362C_AUX_TX_P0, 0x0);

	DP_WRITE1BYTE(mtk_dp, REG_3640_AUX_TX_P0, 0x7f);

	mdelay(1);

	return valid_cmd;
}

void dptx_hal_hpd_int_en(struct mtk_dp *mtk_dp, bool enable)
{
	/* [7]:int, [6]:Con, [5]DisCon, [4]No-Use: UnMASK HPD Port */
	mtk_dp_write_byte(mtk_dp, REG_3418_DP_TRANS_P0, enable ? 0 : GENMASK(7, 5),
			  GENMASK(7, 5));
}

void dptx_hal_set_txtrainingpattern(struct mtk_dp *mtk_dp, int value)
{
	/* if Set TPS1. */
	if (value == BIT(4))
		dptx_hal_phy_setidlepattern(mtk_dp, false);

	mtk_dp_write_byte(mtk_dp, REG_3400_DP_TRANS_P0 + 1, value, GENMASK(7, 4));
}

void dptx_hal_phy_setidlepattern(struct mtk_dp *mtk_dp, bool enable)
{
	mtk_dp_write_byte(mtk_dp, REG_3580_DP_TRANS_P0 + 1,
			  enable ? 0xf : 0x0, 0xf);
}

void dptx_hal_set_ef_mode(struct mtk_dp *mtk_dp, bool enable)
{
	/*
	 * [4]: REG_enhanced_frame_mode
	 * [1 : 0]: REG_lane_num
	 */
	if (enable)
		mtk_dp_write_byte(mtk_dp, REG_3000_DP_ENCODER0_P0, BIT(4), BIT(4));
	else
		mtk_dp_write_byte(mtk_dp, REG_3000_DP_ENCODER0_P0, 0, BIT(4));
}

void dptx_hal_setscramble(struct mtk_dp *mtk_dp, bool enable)
{
	/* [0]: dp tx transmitter scramble enable. */
	if (enable)
		mtk_dp_write_byte(mtk_dp, REG_3404_DP_TRANS_P0, BIT(0), BIT(0));
	else
		mtk_dp_write_byte(mtk_dp, REG_3404_DP_TRANS_P0, 0, BIT(0));
}

void dptx_hal_videomute(struct mtk_dp *mtk_dp, bool enable)
{
	if (enable) {
		mtk_dp_write_byte(mtk_dp, REG_3000_DP_ENCODER0_P0, BIT(3) | BIT(2),
				  BIT(3) | BIT(2));
		mtk_dp_write_byte(mtk_dp, DP_TX_SECURE_REG11, BIT(3) | BIT(4), BIT(3) | BIT(4));
	} else {
		mtk_dp_write_byte(mtk_dp, REG_3000_DP_ENCODER0_P0, BIT(3), BIT(3) | BIT(2));
		mtk_dp_write_byte(mtk_dp, DP_TX_SECURE_REG11, BIT(4), BIT(3) | BIT(4));
	}
	printk(BIOS_DEBUG, "mute = %#x\n", read32(mtk_dp->regs + 0x402c));
}

void dptx_hal_analog_power_en(struct mtk_dp *mtk_dp, bool enable)
{
	if (enable) {
		mtk_dp_write_byte(mtk_dp, DP_TX_TOP_RESET_AND_PROBE, 0, BIT(4));
		mdelay(1);
		mtk_dp_write_byte(mtk_dp, DP_TX_TOP_RESET_AND_PROBE, BIT(4), BIT(4));
	} else {
		DP_WRITE2BYTE(mtk_dp, TOP_OFFSET, 0x0);
		mdelay(1);
		DP_WRITE2BYTE(mtk_dp, 0x0034, 0x4aa);
		DP_WRITE2BYTE(mtk_dp, 0x1040, 0x0);
		DP_WRITE2BYTE(mtk_dp, 0x0038, 0x555);
	}
}
