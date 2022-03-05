/* SPDX-License-Identifier: GPL-2.0-only */

#include <mipi/dsi.h>
#include <mipi/panel.h>
#include <device/mmio.h>
#include <console/console.h>
#include <assert.h>
#include <edid.h>
#include <delay.h>
#include <symbols.h>
#include <types.h>
#include <string.h>
#include <soc/display/mipi_dsi.h>
#include <soc/display/mdssreg.h>
#include <soc/display/dsi_phy.h>

#define DSI_DMA_STREAM1 0x0
#define DSI_EMBED_MODE1 0x1
#define DSI_POWER_MODE2 0x1
#define DSI_PACK_TYPE1 0x0
#define DSI_VC1 0x0
#define DSI_DT1 0x0
#define DSI_WC1 0x0
#define DSI_EOF_BLLP_PWR 0x9
#define DSI_DMA_TRIGGER_SEL 0x4
#define TRAFFIC_MODE 0x1

#define DSI_EN 0x1
#define DSI_CLKLN_EN 0x1
#define DSI_VIDEO_EN 0x1

#define HS_TX_TO 0xEA60
#define TIMER_RESOLUTION 0x4
#define DSI_PAYLOAD_BYTE_BOUND 256
#define DSI_PAYLOAD_SIZE_ALIGN 4
#define DSI_CMD_DMA_TPG_EN  BIT(1)
#define DSI_TPG_DMA_FIFO_MODE BIT(2)
#define DSI_CMD_DMA_PATTERN_SEL (BIT(16) | BIT(17))

static void mdss_dsi_host_init(int num_of_lanes)
{
	uint8_t dlnx_en;
	uint32_t ctrl_mode = BIT(8) | BIT(0); /* Enable DSI and CLKlane. */

	switch (num_of_lanes) {
	default:
	case 1:
		dlnx_en = 1;
		break;

	case 2:
		dlnx_en = 3;
		break;

	case 3:
		dlnx_en = 7;
		break;

	case 4:
		dlnx_en = 0x0F;
		break;
	}

	/*
	 * Need to send pixel data before sending the ON commands
	 * so need to configure controller to VIDEO MODE.
	 */
	ctrl_mode |= BIT(1);

	mdss_dsi_clock_config();
	write32(&dsi0->trig_ctrl, DSI_DMA_STREAM1 << 8 | DSI_DMA_TRIGGER_SEL);
	write32(&dsi0->ctrl, dlnx_en << 4 | ctrl_mode);
	write32(&dsi0->cmd_mode_dma_ctrl,
		DSI_EMBED_MODE1 << 28 | DSI_POWER_MODE2 << 26 |
		DSI_PACK_TYPE1 << 24 | DSI_VC1 << 22 | DSI_DT1 << 16 | DSI_WC1);
	write32(&dsi0->eot_packet_ctrl, 0x1);
}

static void mdss_dsi_reset(void)
{
	/*
	 * Disable DSI Controller, DSI lane states,
	 * DSI command-mode and DSI video-mode engines
	 */
	write32(&dsi0->ctrl, 0x0);

	/* DSI soft reset */
	write32(&dsi0->soft_reset, 0x1);
	write32(&dsi0->soft_reset, 0x0);

	/* set hs timer count speed */
	write32(&dsi0->hs_timer_ctrl, HS_TX_TO | TIMER_RESOLUTION << 16);

	/* dma fifo reset */
	write32(&dsi0->tpg_dma_fifo_reset, 0x1);
	write32(&dsi0->tpg_dma_fifo_reset, 0x0);
}

void mdss_dsi_video_mode_config(struct edid *edid, uint32_t bpp)
{
	uint16_t dst_format;
	uint8_t lane_en = 15; /* Enable 4 lanes by default */

	switch (bpp) {
	case 16:
		dst_format = DSI_VIDEO_DST_FORMAT_RGB565;
		break;
	case 18:
		dst_format = DSI_VIDEO_DST_FORMAT_RGB666;
		break;
	case 24:
	default:
		dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
		break;
	}

	write32(&dsi0->video_mode_active_h,
		((edid->mode.ha + edid->mode.hbl - edid->mode.hso) << 16) |
		 (edid->mode.hbl - edid->mode.hso));

	write32(&dsi0->video_mode_active_v,
		((edid->mode.va + edid->mode.vbl - edid->mode.vso) << 16) |
		 (edid->mode.vbl - edid->mode.vso));

	write32(&dsi0->video_mode_active_total,
		((edid->mode.va + edid->mode.vbl - 1) << 16) |
		 (edid->mode.ha + edid->mode.hbl - 1));

	write32(&dsi0->video_mode_active_hsync, (edid->mode.hspw << 16));
	write32(&dsi0->video_mode_active_vsync, 0x0);
	write32(&dsi0->video_mode_active_vsync_vpos, edid->mode.vspw << 16);

	write32(&dsi0->video_mode_ctrl,
			DSI_EOF_BLLP_PWR << 12 | dst_format << 4 | TRAFFIC_MODE << 8);

	write32(&dsi0->hs_timer_ctrl, HS_TX_TO | TIMER_RESOLUTION << 16);

	write32(&dsi0->ctrl, lane_en << 4 | DSI_VIDEO_EN << 1 | DSI_EN | DSI_CLKLN_EN << 8);
}

enum cb_err mdss_dsi_config(struct edid *edid, uint32_t num_of_lanes, uint32_t bpp)
{
	mdss_dsi_reset();
	if ((mdss_dsi_phy_10nm_init(edid, num_of_lanes, bpp)) != 0) {
		printk(BIOS_ERR, "dsi phy setup returned error\n");
		return CB_ERR;
	}

	mdss_dsi_host_init(num_of_lanes);

	return CB_SUCCESS;
}

void mdss_dsi_clock_config(void)
{
	/* Clock for AHI Bus Master, for DMA out from memory */
	write32(&dsi0->clk_ctrl, 0);
	setbits32(&dsi0->clk_ctrl, DSI_AHBM_SCLK_ON | DSI_FORCE_ON_DYN_AHBM_HCLK);

	/* Clock for MDP/DSI, for DMA out from MDP */
	setbits32(&dsi0->clk_ctrl, DSI_PCLK_ON);

	/* Clock for rest of DSI */
	setbits32(&dsi0->clk_ctrl, DSI_AHBS_HCLK_ON | DSI_DSICLK_ON |
		  DSI_BYTECLK_ON | DSI_ESCCLK_ON);
}

static void mdss_dsi_set_intr(void)
{
	write32(&dsi0->int_ctrl, 0x0);

	/* Enable all HW interrupts. */
	setbits32(&dsi0->int_ctrl, DSI_CMD_MODE_DMA_DONE_MASK | DSI_CMD_MODE_MDP_DONE_MASK |
			DSI_VIDEO_MODE_DONE_MASK | DSI_ERROR_MASK | DSI_BTA_DONE_MASK);
}

static int mdss_dsi_cmd_dma_trigger_for_panel(void)
{
	uint32_t read_value;
	uint32_t count = 0;
	int status = 0;

	mdss_dsi_set_intr();
	write32(&dsi0->cmd_mode_dma_sw_trigger, 0x1);
	dsb();

	read_value = read32(&dsi0->int_ctrl) & 0x1;

	while (read_value != 0x1) {
		read_value = read32(&dsi0->int_ctrl) & 0x1;
		count++;
		if (count > 0xffff) {
			status = -1;
			printk(BIOS_ERR,
				"Panel CMD: count :%d command mode dma test failed\n", count);
			printk(BIOS_ERR,
			       "Panel CMD: read value = %x, addr=%p\n",
			       read_value, (&dsi0->int_ctrl));
			return status;
		}
	}

	write32(&dsi0->int_ctrl, (read32(&dsi0->int_ctrl) | 0x01000001));
	return status;
}

static enum cb_err mdss_dsi_send_init_cmd(enum mipi_dsi_transaction type, const u8 *body,
					  u8 len)
{
	uint8_t *pload = _dma_coherent;
	uint32_t size;
	enum cb_err ret = CB_SUCCESS;
	int data = 0;
	uint32_t *bp = NULL;

	if (len > 2) {
		pload[0] = len;
		pload[1] = 0;
		pload[2] = type;
		pload[3] = BIT(7) | BIT(6);

		/* The payload size has to be a multiple of 4 */
		memcpy(pload + 4, body, len);
		size = ALIGN_UP(len + 4, DSI_PAYLOAD_SIZE_ALIGN);
		memset(pload + 4 + len, 0, size - 4 - len);
		assert(size < DSI_PAYLOAD_BYTE_BOUND);
	} else {
		pload[0] = body[0];
		pload[1] = len > 1 ? body[1] : 0;
		pload[2] = type;
		pload[3] = BIT(7);
		size = 4;
	}

	bp = (uint32_t *)pload;

	/* Enable custom pattern stored in TPG DMA FIFO */
	data = DSI_CMD_DMA_PATTERN_SEL;

	/* select CMD_DMA_FIFO_MODE to 1 */
	data |= DSI_TPG_DMA_FIFO_MODE;
	data |= DSI_CMD_DMA_TPG_EN;

	write32(&dsi0->test_pattern_gen_ctrl, data);
	for (int j = 0; j < size; j += 4) {
		write32(&dsi0->test_pattern_gen_cmd_dma_init_val, *bp);
		bp++;
	}

	if ((size % 8) != 0)
		write32(&dsi0->test_pattern_gen_cmd_dma_init_val, 0x0);

	write32(&dsi0->dma_cmd_length, size);
	write32(&dsi0->cmd_mode_dma_sw_trigger, 0x1);
	if (mdss_dsi_cmd_dma_trigger_for_panel())
		ret = CB_ERR;

	/* Reset the DMA TPG FIFO */
	write32(&dsi0->tpg_dma_fifo_reset, 0x1);
	write32(&dsi0->tpg_dma_fifo_reset, 0x0);

	/* Disable CMD_DMA_TPG */
	write32(&dsi0->test_pattern_gen_ctrl, 0x0);

	udelay(80);

	return ret;
}

static void mdss_dsi_clear_intr(void)
{
	write32(&dsi0->int_ctrl, 0x0);

	/* Clear all the hardware interrupts */
	setbits32(&dsi0->int_ctrl, DSI_CMD_MODE_DMA_DONE_AK | DSI_CMD_MODE_MDP_DONE_AK |
			DSI_VIDEO_MODE_DONE_AK |  DSI_BTA_DONE_AK | DSI_ERROR_AK);
	write32(&dsi0->err_int_mask0, 0x13FF3BFF);
}

enum cb_err mdss_dsi_panel_initialize(const u8 *init_cmds)
{
	uint32_t ctrl_mode = 0;

	assert(init_cmds != NULL);
	ctrl_mode = read32(&dsi0->ctrl);

	/* Enable command mode before sending the commands */
	write32(&dsi0->ctrl, ctrl_mode | 0x04);

	enum cb_err ret = mipi_panel_parse_init_commands(init_cmds, mdss_dsi_send_init_cmd);
	write32(&dsi0->ctrl, ctrl_mode);
	mdss_dsi_clear_intr();

	return ret;
}
