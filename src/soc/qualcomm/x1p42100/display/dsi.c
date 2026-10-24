/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * DSI host controller init for purwa (x1p42100).
 *
 * Ported from src/soc/qualcomm/sc7180/display/dsi.c.
 * PHY init updated to call mdss_dsi_phy_270_init() instead of
 * mdss_dsi_phy_10nm_init() - PHY 2.7.0 is used on purwa.
 *
 * DSI clock enable uses x1p42100 clk_mdss enum entries
 * (added via gerrit/95727 cherry-pick).
 */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <edid.h>
#include <mipi/panel.h>
#include <soc/display/dsi_phy.h>
#include <soc/display/mdss_dsc_calc.h>
#include <soc/display/mdssreg.h>
#include <soc/display/mipi_dsi.h>
#include <soc/display/panel_ops.h>
#include <string.h>
#include <symbols.h>
#include <types.h>

/*
 * Mutable pointer used by static DSI host helpers.
 * Set to dsi0 or dsi1 before calling dsi_config_instance().
 */
static struct dsi_regs *active_dsi;

#define DSI_DMA_STREAM1         0x0
#define DSI_DMA_TRIGGER_SEL     0x4
#define DSI_EMBED_MODE1         0x1
#define DSI_POWER_MODE2         0x1
#define DSI_PACK_TYPE1          0x0
#define DSI_VC1                 0x0
#define DSI_DT1                 0x0
#define DSI_WC1                 0x0
#define DSI_EOF_BLLP_PWR        0x9
#define TRAFFIC_MODE            0x1
#define HS_TX_TO                0xEA60
#define TIMER_RESOLUTION        0x4
#define DSI_PAYLOAD_BYTE_BOUND  256
#define DSI_PAYLOAD_SIZE_ALIGN  4
#define DSI_CMD_DMA_TPG_EN      BIT(1)
#define DSI_TPG_DMA_FIFO_MODE   BIT(2)
#define DSI_CMD_DMA_PATTERN_SEL (BIT(16) | BIT(17))
#define DSI_WR32(addr, val) write32(addr, val)

static void mdss_dsi_host_init(int num_of_lanes)
{
	u8 dlnx_en;
	u32 ctrl_mode = BIT(8) | BIT(0); /* DSI_EN + CLKlane_EN */

	switch (num_of_lanes) {
	default:
	case 1: dlnx_en = 0x01; break;
	case 2: dlnx_en = 0x03; break;
	case 3: dlnx_en = 0x07; break;
	case 4: dlnx_en = 0x0F; break;
	}

	/*
	 * Do NOT set VIDEO_EN (BIT(1)) here - panel init commands must be
	 * sent in CMD mode first. VIDEO_EN is set in video_mode_config().
	 */
	mdss_dsi_clock_config();
	/*
	 * TRIG_CTRL: 0x4 for CMD mode DMA trigger (SW trigger, stream 0)
	 * HLOS golden 0xFFEFC000 is the video mode running state, not init value
	 */
	DSI_WR32(&active_dsi->trig_ctrl, DSI_DMA_STREAM1 << 8 | DSI_DMA_TRIGGER_SEL);
	DSI_WR32(&active_dsi->ctrl, dlnx_en << 4 | ctrl_mode);
	/*
	 * LANE_SWAP_CTRL: [12:8]=dlnx_en (active lanes), [4:0]=0 (no swap)
	 * HLOS golden: 0x1F00 for 4 lanes
	 */
	DSI_WR32(&active_dsi->lane_swap_ctrl, (u32)dlnx_en << 8);
	DSI_WR32(&active_dsi->cmd_mode_dma_ctrl,
		DSI_EMBED_MODE1 << 28 | DSI_POWER_MODE2 << 26 |
		DSI_PACK_TYPE1 << 24 | DSI_VC1 << 22 | DSI_DT1 << 16 | DSI_WC1);
	/*
	 * EOT_PACKET_CTRL: TX_EOT_APPEND[0] = !eotp_disabled
	 * UEFI HALdsi_Config.c: TX_EOT_APPEND = (bDisableEotp ? 0 : 1)
	 * Default: enabled (0x1). Panel sets eotp_disabled=1 to suppress.
	 */
	DSI_WR32(&active_dsi->eot_packet_ctrl,
		mdss_get_panel_ops()->eotp_disabled ? 0x0 : 0x1);
}

static void mdss_dsi_reset(void)
{
	DSI_WR32(&active_dsi->ctrl, 0x0);
	DSI_WR32(&active_dsi->soft_reset, 0x1);
	DSI_WR32(&active_dsi->soft_reset, 0x0);
	/* HS_TIMER_CTRL: HLOS golden=0x3FD08 */
	DSI_WR32(&active_dsi->hs_timer_ctrl, 0x3FD08);
	DSI_WR32(&active_dsi->tpg_dma_fifo_reset, 0x1);
	DSI_WR32(&active_dsi->tpg_dma_fifo_reset, 0x0);
}

static void dsi_video_mode_config_instance(struct edid *edid, u32 bpp,
					   u32 dsc_ratio_x1000, u32 dsi_count)
{
	u16 dst_format;
	u8 lane_en = 0x0F; /* 4 lanes */
	/* For 2-2-2: each DSI sees ha/dsi_count pixels */
	u32 ha_per_dsi = edid->mode.ha / dsi_count;

	switch (bpp) {
	case 16: dst_format = DSI_VIDEO_DST_FORMAT_RGB565; break;
	case 18: dst_format = DSI_VIDEO_DST_FORMAT_RGB666; break;
	case 24:
	default: dst_format = DSI_VIDEO_DST_FORMAT_RGB888; break;
	}

	/*
	 * VIDEO_MODE_ACTIVE_H: HSYNC_END[28:16], HSYNC_START[13:0]
	 * For 2-2-2: each DSI sees ha/dsi_count compressed pixels
	 */
	{
		u32 comp_ha = mdss_dsc_compressed_ha(ha_per_dsi, dsc_ratio_x1000);
		u32 h_start = edid->mode.hbl - edid->mode.hso;  /* hbp + hspw */
		u32 h_end   = comp_ha + h_start;
		DSI_WR32(&active_dsi->video_mode_active_h, (h_end << 16) | h_start);
	}
	DSI_WR32(&active_dsi->video_mode_active_v,
		((edid->mode.va + edid->mode.vbl - edid->mode.vso) << 16) |
		 (edid->mode.vbl - edid->mode.vso));
	/*
	 * VIDEO_MODE_TOTAL:
	 * VIDEO_V_TOTAL[31:16] = v_total - 1  (UEFI: iVsyncPeriodLine = vspw+vbp+va+vfp-1)
	 * VIDEO_H_TOTAL[15:0]  = h_total - 1  (UEFI: iHsyncPeriodDclk = hspw+hbp+ha+hfp-1)
	 * Source: UEFI HALdsi_Config.c HAL_DSI_VideoTimingConfig()
	 * Validated: sim 2-2-2 golden=0x0A0D01F7 -> v_total-1=2573
	 */
	DSI_WR32(&active_dsi->video_mode_active_total,
		(((edid->mode.va + edid->mode.vbl) - 1) << 16) |
		(mdss_dsc_compressed_ha(ha_per_dsi, dsc_ratio_x1000) + edid->mode.hbl - 1));
	/* VIDEO_MODE_HSYNC @ 0x30: HLOS golden=0x80000 = hspw<<16 */
	DSI_WR32(&active_dsi->video_mode_active_hsync, edid->mode.hspw << 16);
	DSI_WR32(&active_dsi->video_mode_active_vsync, 0x0);
	/* VIDEO_MODE_VSYNC_VPOS @ 0x38: HLOS golden=0x10000 = vspw<<16 */
	DSI_WR32(&active_dsi->video_mode_active_vsync_vpos, edid->mode.vspw << 16);
	/*
	 * VIDEO_MODE_CTRL: HLOS golden=0x2009130
	 * bit[25]=1 (PULSE_MODE_HACTIVE_WC_EN), TRAFFIC_MODE, EOF_BLLP_PWR, dst_format
	 */
	DSI_WR32(&active_dsi->video_mode_ctrl,
		BIT(25) | DSI_EOF_BLLP_PWR << 12 | dst_format << 4 | TRAFFIC_MODE << 8);
	/* HS_TIMER_CTRL: HLOS golden=0x3FD08 */
	DSI_WR32(&active_dsi->hs_timer_ctrl, 0x3FD08);
	/*
	 * CTRL: VIDEO_EN|DSI_EN|CLK_EN|4lanes = 0x1F3
	 * HLOS golden: 0x1F3 - CMD_MODE_EN(bit2) must be CLEAR in video mode
	 */
	DSI_WR32(&active_dsi->ctrl,
		lane_en << 4 | BIT(1) /* VIDEO_EN */ |
		BIT(0) /* DSI_EN */ | BIT(8) /* CLK_EN */);

	/*
	 * INT_CTRL: enable VIDEO_MODE_DONE + ERROR interrupts for video mode.
	 * Per HW note: mask=1 enables interrupt.
	 * HLOS golden=0x2010000 has ERROR_MASK(bit25) set after video runs.
	 * We set VIDEO_MODE_DONE_MASK + ERROR_MASK now so status bits are readable.
	 */
	DSI_WR32(&active_dsi->int_ctrl, 0x0);
	setbits32(&active_dsi->int_ctrl,
		  DSI_VIDEO_MODE_DONE_MASK | DSI_ERROR_MASK | DSI_CMD_MODE_DMA_DONE_MASK | DSI_BTA_DONE_MASK);
}

static enum cb_err dsi_config_instance(struct edid *edid, u32 num_of_lanes, u32 bpp)
{
	mdss_dsi_reset();

	/* PHY 2.7.0 init (purwa) */
	if (mdss_dsi_phy_270_init(edid, num_of_lanes, bpp) != CB_SUCCESS) {
		printk(BIOS_ERR, "DSI PHY 2.7.0 init failed\n");
		return CB_ERR;
	}

	mdss_dsi_host_init(num_of_lanes);
	return CB_SUCCESS;
}

void mdss_dsi_clock_config(void)
{
	/*
	 * CLK_CTRL: HLOS golden=0x23F
	 * bits[5:0]=0x3F: AHBS_HCLK_ON|AHBM_SCLK_ON|PCLK_ON|DSICLK_ON|BYTECLK_ON|ESCCLK_ON
	 * bit[9]=FORCE_ON_DYN_AHBM_HCLK
	 * Note: HLOS steady-state=0x23F; UEFI init writes more bits (0x15F3F)
	 * Use HLOS golden as it reflects the working state
	 */
	DSI_WR32(&active_dsi->clk_ctrl, 0x23F);
}

static void mdss_dsi_set_intr(void)
{
	DSI_WR32(&active_dsi->int_ctrl, 0x0);
	setbits32(&active_dsi->int_ctrl,
		  DSI_CMD_MODE_DMA_DONE_MASK | DSI_CMD_MODE_MDP_DONE_MASK |
		  DSI_VIDEO_MODE_DONE_MASK | DSI_ERROR_MASK | DSI_BTA_DONE_MASK);
}

/*
 * DSI DMA polling parameters - from UEFI dsiShared_WaitforInterrupt().
 * DSI_POLLING_CYCLE = 100us per poll, DSI_MAX_POLLING_TIME = 15000us total.
 * 15000 / 100 = 150 polls maximum before timeout.
 */
#define DSI_DMA_POLLING_CYCLE_US  100
#define DSI_DMA_MAX_POLLS         150   /* 150 * 100us = 15ms total */

static int mdss_dsi_cmd_dma_trigger(void)
{
	u32 isr_status;
	int poll;

	DSI_WR32(&active_dsi->cmd_mode_dma_sw_trigger, 0x1);
	dsb();

	/* Poll for CMD_DMA_DONE interrupt status */
	for (poll = 0; poll < DSI_DMA_MAX_POLLS; poll++) {
		udelay(DSI_DMA_POLLING_CYCLE_US);
		isr_status = read32(&active_dsi->int_ctrl);
		if (isr_status & DSI_CMD_MODE_DMA_DONE_AK) {
			/* Ack CMD_DMA_DONE - read-modify-write, preserve masks */
			setbits32(&active_dsi->int_ctrl, DSI_CMD_MODE_DMA_DONE_AK);
			return 0;
		}
	}

	return -1;
}

/*
 * mdss_dsi_send_init_cmd() - send one DSI command via DMA VBIF path.
 *
 * Uses DMA VBIF (not TPG FIFO) - matches UEFI DSI_DEVICE_FLAGS_DSI_DCS_DMA_ONLY path.
 * FIFO is limited to ~256 bytes and causes DMA timeout for long panel init sequences.
 * DMA VBIF uses _dma_coherent buffer (physical address written to DMA_CMD_OFFSET),
 * which supports arbitrary length commands.
 *
 * Embedded packet format (from UEFI dsShared_PackData / HAL_DSI_WriteData):
 *   Short packet (len <= 2):
 *     byte[0] = body[0]  (data0 / cmd)
 *     byte[1] = body[1]  (data1, or 0 if len==1)
 *     byte[2] = type     (DT[5:0] | VC[7:6])
 *     byte[3] = flags    (BIT(7)=last_packet, BIT(6)=long/short, BIT(5)=BTA)
 *     total = 4 bytes
 *
 *   Long packet (len > 2):
 *     byte[0] = WC_lo    (payload length low byte)
 *     byte[1] = WC_hi    (payload length high byte)
 *     byte[2] = type     (DT[5:0] | VC[7:6])
 *     byte[3] = flags    (BIT(7)=last_packet, BIT(6)=long/short=1, BIT(5)=BTA)
 *     byte[4..4+len-1] = payload
 *     total = ALIGN_UP(4 + len, 4) bytes
 *
 * Source: UEFI dsiHostShared.c dsShared_PackData() + HAL_DSI_WriteData()
 */
static enum cb_err mdss_dsi_send_init_cmd(enum mipi_dsi_transaction type,
					  const u8 *body, u8 len,
					  void *user_data)
{
	u8 *pload = _dma_coherent;
	u32 size;
	enum cb_err ret = CB_SUCCESS;
	bool is_long = (len > 2);

	/* Build embedded packet in _dma_coherent buffer */
	memset(pload, 0, is_long ? ALIGN_UP(4 + len, DSI_PAYLOAD_SIZE_ALIGN) : 4);

	if (is_long) {
		/* Long packet: WC_lo, WC_hi, DT|VC, flags, payload */
		pload[0] = len & 0xFF;           /* WC low byte */
		pload[1] = (len >> 8) & 0xFF;    /* WC high byte */
		pload[2] = type & 0x3F;          /* DT[5:0], VC=0 */
		pload[3] = BIT(7) | BIT(6);      /* last_packet=1, long=1 */
		memcpy(pload + 4, body, len);
		size = ALIGN_UP(4 + len, DSI_PAYLOAD_SIZE_ALIGN);
	} else {
		/* Short packet: data0, data1, DT|VC, flags */
		pload[0] = (len > 0) ? body[0] : 0;
		pload[1] = (len > 1) ? body[1] : 0;
		pload[2] = type & 0x3F;          /* DT[5:0], VC=0 */
		pload[3] = BIT(7);               /* last_packet=1, short=0 */
		size = 4;
	}

	/*
	 * Clear any stale interrupt status and arm CMD_DMA_DONE mask before trigger.
	 * Must be done before DMA setup - matches UEFI HAL_DSI_SetInterruptsEnabled()
	 * + HAL_DSI_ClrInterruptStatus() sequence in dsiHostShared.c.
	 */
	mdss_dsi_set_intr();

	/* DMA VBIF path: disable TPG FIFO, set DMA_CMD_OFFSET to physical address */
	DSI_WR32(&active_dsi->test_pattern_gen_ctrl, 0x0);

	/* Write physical address of _dma_coherent to DMA_CMD_OFFSET */
	DSI_WR32(&active_dsi->dma_cmd_offset, (u32)(uintptr_t)_dma_coherent);
	DSI_WR32(&active_dsi->dma_cmd_length, size);

	dsb();  /* ensure buffer writes are visible to DMA before trigger */

	if (mdss_dsi_cmd_dma_trigger())
		ret = CB_ERR;

	udelay(80);
	return ret;
}

static void mdss_dsi_clear_intr(void)
{
	DSI_WR32(&active_dsi->int_ctrl, 0x0);
	setbits32(&active_dsi->int_ctrl,
		  DSI_CMD_MODE_DMA_DONE_AK | DSI_CMD_MODE_MDP_DONE_AK |
		  DSI_VIDEO_MODE_DONE_AK | DSI_BTA_DONE_AK | DSI_ERROR_AK);
	/* ERR_INT_MASK0: HLOS golden=0x13F03B1F */
	DSI_WR32(&active_dsi->err_int_mask0, 0x13F03B1F);
}

/*
 * DSI_CMD_CUSTOM_BROADCAST (0xFD) - dual-DSI panel init broadcast marker.
 * The byte following 0xFD is the controller mask:
 *   0x01 = DSI0 only, 0x02 = DSI1 only, 0x03 = both DSI0 and DSI1.
 * Matches UEFI dsiDriver.c DSI_CMD_CUSTOM_BROADCAST handling.
 */
#define DSI_CMD_CUSTOM_BROADCAST  0xFD
#define DSI_BROADCAST_DSI0        BIT(0)
#define DSI_BROADCAST_DSI1        BIT(1)

/*
 * mdss_dsi_send_pps() - send DSC PPS command to panel via DSI.
 *
 * Builds the 128-byte PPS packet from pre-calculated DSC params and sends
 * it using DSI data type 0x0A (MIPI_DSI_PPS_LONG_WRITE / DSI_DATATYPE_PPSPACKET).
 *
 * UEFI reference: SendPanelPPSRawBuffer() in dsiDriver.c
 *   -> DsiPanelSendCommandSequence(MDP_PANEL_COMMAND_PPS)
 *   -> HAL_MDP_DSC_Prepare_PPS() builds the 128-byte buffer
 *   -> SendPanelXMLSequence() sends via DSI DMA
 *
 * Must be called AFTER mdss_dsc_config() and BEFORE mdss_dsi_panel_initialize().
 * Sent to both DSI0 and DSI1 for dual DSI (2-2-2).
 */
enum cb_err mdss_dsi_send_pps(const struct mdss_dsc_config *cfg,
			      const struct edid_mode *mode,
			      const struct mdss_dsc_params *params)
{
	u8 pps_buf[128];
	enum cb_err ret;

	/* Build the 128-byte PPS packet from DSC params */
	mdss_dsc_build_pps(cfg, mode, params, pps_buf);

	/* Send to DSI0 using MIPI_DSI_PPS_LONG_WRITE (0x0A) */
	active_dsi = dsi0;
	ret = mdss_dsi_send_init_cmd(MIPI_DSI_PPS_LONG_WRITE, pps_buf, 128, NULL);
	if (ret != CB_SUCCESS) {
		printk(BIOS_ERR, "DSI0: PPS send failed\n");
		return ret;
	}
	printk(BIOS_INFO, "DSI0: PPS sent OK\n");

	/* For dual DSI (2-2-2): send same PPS to DSI1 */
	if (mdss_get_panel_ops()->config->dsi_count > 1) {
		active_dsi = dsi1;
		ret = mdss_dsi_send_init_cmd(MIPI_DSI_PPS_LONG_WRITE, pps_buf, 128, NULL);
		if (ret != CB_SUCCESS) {
			printk(BIOS_ERR, "DSI1: PPS send failed\n");
			return ret;
		}
		printk(BIOS_INFO, "DSI1: PPS sent OK\n");
		active_dsi = dsi0;
	}

	return CB_SUCCESS;
}

enum cb_err mdss_dsi_panel_initialize(const u8 *init_cmds)
{
	u32 ctrl_mode0, ctrl_mode1;

	assert(init_cmds != NULL);
	ctrl_mode0 = read32(&dsi0->ctrl);
	ctrl_mode1 = read32(&dsi1->ctrl);

	/* Switch both controllers to command mode for panel init */
	DSI_WR32(&dsi0->ctrl, ctrl_mode0 | 0x04);
	DSI_WR32(&dsi1->ctrl, ctrl_mode1 | 0x04);

	/* Send init commands to DSI0 */
	active_dsi = dsi0;
	enum cb_err ret = mipi_panel_parse_commands(init_cmds,
						    mdss_dsi_send_init_cmd,
						    NULL);

	/* For dual DSI (2-2-2): send same init commands to DSI1 */
	if (ret == CB_SUCCESS &&
	    mdss_get_panel_ops()->config->dsi_count > 1) {
		active_dsi = dsi1;
		ret = mipi_panel_parse_commands(init_cmds,
						mdss_dsi_send_init_cmd,
						NULL);
		active_dsi = dsi0;
	}

	DSI_WR32(&dsi0->ctrl, ctrl_mode0);
	DSI_WR32(&dsi1->ctrl, ctrl_mode1);
	mdss_dsi_clear_intr();
	return ret;
}

enum cb_err mdss_dsi_config(struct edid *edid, u32 num_of_lanes, u32 bpp)
{
	active_dsi = dsi0;
	return dsi_config_instance(edid, num_of_lanes, bpp);
}

enum cb_err mdss_dsi1_config(struct edid *edid, u32 num_of_lanes, u32 bpp)
{
	active_dsi = dsi1;

	mdss_dsi_reset();

	/*
	 * Use DSI1 PHY init - this configures DSI1 PHY/PLL registers
	 * AND enables DSI1 clocks (byte1/pclk1/esc1 CBCRs).
	 * mdss_dsi_phy_270_init() would use DSI0 PHY/clocks instead.
	 */
	if (mdss_dsi1_phy_270_init(edid, num_of_lanes, bpp) != CB_SUCCESS) {
		printk(BIOS_ERR, "DSI1 PHY 2.7.0 init failed\n");
		return CB_ERR;
	}

	mdss_dsi_host_init(num_of_lanes);
	return CB_SUCCESS;
}

void mdss_dsi_video_mode_config(struct edid *edid, u32 bpp)
{
	active_dsi = dsi0;
	dsi_video_mode_config_instance(edid, bpp,
		mdss_get_panel_ops()->config->dsc_ratio_x1000,
		mdss_get_panel_ops()->config->dsi_count);
}

void mdss_dsi1_video_mode_config(struct edid *edid, u32 bpp)
{
	active_dsi = dsi1;
	dsi_video_mode_config_instance(edid, bpp,
		mdss_get_panel_ops()->config->dsc_ratio_x1000,
		mdss_get_panel_ops()->config->dsi_count);
}
