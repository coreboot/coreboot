/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <edid.h>
#include <gpio.h>
#include <string.h>

#include "anx7625.h"

#define ANXERROR(format, ...) \
		printk(BIOS_ERR, "ERROR: %s: " format, __func__, ##__VA_ARGS__)
#define ANXINFO(format, ...) \
		printk(BIOS_INFO, "%s: " format, __func__, ##__VA_ARGS__)
#define ANXDEBUG(format, ...) \
		printk(BIOS_DEBUG, "%s: " format, __func__, ##__VA_ARGS__)

/*
 * There is a sync issue while accessing I2C register between AP(CPU) and
 * internal firmware(OCM). To avoid the race condition, AP should access the
 * reserved slave address before slave address changes.
 */
static int i2c_access_workaround(uint8_t bus, uint8_t saddr)
{
	uint8_t offset;
	static uint8_t saddr_backup = 0;
	int ret = 0;

	if (saddr == saddr_backup)
		return ret;

	saddr_backup = saddr;

	switch (saddr) {
	case TCPC_INTERFACE_ADDR:
		offset = RSVD_00_ADDR;
		break;
	case TX_P0_ADDR:
		offset = RSVD_D1_ADDR;
		break;
	case TX_P1_ADDR:
		offset = RSVD_60_ADDR;
		break;
	case RX_P0_ADDR:
		offset = RSVD_39_ADDR;
		break;
	case RX_P1_ADDR:
		offset = RSVD_7F_ADDR;
		break;
	default:
		offset = RSVD_00_ADDR;
		break;
	}

	ret = i2c_writeb(bus, saddr, offset, 0x00);
	if (ret < 0)
		ANXERROR("Failed to access %#x:%#x\n", saddr, offset);
	return ret;
}

static int anx7625_reg_read(uint8_t bus, uint8_t saddr, uint8_t offset,
			    uint8_t *val)
{
	int ret;

	i2c_access_workaround(bus, saddr);
	ret = i2c_readb(bus, saddr, offset, val);
	if (ret < 0) {
		ANXERROR("Failed to read i2c reg=%#x:%#x\n", saddr, offset);
		return ret;
	}
	return *val;
}

static int anx7625_reg_block_read(uint8_t bus, uint8_t saddr, uint8_t reg_addr,
				  uint8_t len, uint8_t *buf)
{
	int ret;

	i2c_access_workaround(bus, saddr);
	ret = i2c_read_bytes(bus, saddr, reg_addr, buf, len);
	if (ret < 0)
		ANXERROR("Failed to read i2c block=%#x:%#x[len=%#x]\n", saddr,
			 reg_addr, len);
	return ret;
}

static int anx7625_reg_write(uint8_t bus, uint8_t saddr, uint8_t reg_addr,
			     uint8_t reg_val)
{
	int ret;

	i2c_access_workaround(bus, saddr);
	ret = i2c_writeb(bus, saddr, reg_addr, reg_val);
	if (ret < 0)
		ANXERROR("Failed to write i2c id=%#x:%#x\n", saddr, reg_addr);

	return ret;
}

static int anx7625_write_or(uint8_t bus, uint8_t saddr, uint8_t offset,
			    uint8_t mask)
{
	uint8_t val;
	int ret;

	ret = anx7625_reg_read(bus, saddr, offset, &val);
	if (ret < 0)
		return ret;

	return anx7625_reg_write(bus, saddr, offset, val | mask);
}

static int anx7625_write_and(uint8_t bus, uint8_t saddr, uint8_t offset,
			     uint8_t mask)
{
	int ret;
	uint8_t val;

	ret = anx7625_reg_read(bus, saddr, offset, &val);
	if (ret < 0)
		return ret;

	return anx7625_reg_write(bus, saddr, offset, val & mask);
}

static int wait_aux_op_finish(uint8_t bus)
{
	uint8_t val;
	int ret = -1;
	int loop;

	for (loop = 0; loop < 150; loop++) {
		mdelay(2);
		anx7625_reg_read(bus, RX_P0_ADDR, AP_AUX_CTRL_STATUS, &val);
		if (!(val & AP_AUX_CTRL_OP_EN)) {
			ret = 0;
			break;
		}
	}

	if (ret != 0) {
		ANXERROR("Timed out waiting aux operation.\n");
		return ret;
	}

	ret = anx7625_reg_read(bus, RX_P0_ADDR, AP_AUX_CTRL_STATUS, &val);
	if (ret < 0 || val & 0x0F) {
		ANXDEBUG("aux status %02x\n", val);
		ret = -1;
	}

	return ret;
}

static unsigned long gcd(unsigned long a, unsigned long b)
{
	if (a == 0)
		return b;

	while (b != 0) {
		if (a > b)
			a = a - b;
		else
			b = b - a;
	}

	return a;
}

/* Reduce fraction a/b */
static void anx7625_reduction_of_a_fraction(unsigned long *_a,
					    unsigned long *_b)
{
	unsigned long gcd_num;
	unsigned long a = *_a, b = *_b, old_a, old_b;
	u32 denom = 1;

	gcd_num = gcd(a, b);
	a /= gcd_num;
	b /= gcd_num;

	old_a = a;
	old_b = b;

	while (a > MAX_UNSIGNED_24BIT || b > MAX_UNSIGNED_24BIT) {
		denom++;
		a = old_a / denom;
		b = old_b / denom;
	}

	/* Increase a, b to have higher ODFC PLL output frequency accuracy. */
	while ((a << 1) < MAX_UNSIGNED_24BIT && (b << 1) < MAX_UNSIGNED_24BIT) {
		a <<= 1;
		b <<= 1;
	}

	*_a = a;
	*_b = b;
}

static int anx7625_calculate_m_n(u32 pixelclock,
				 unsigned long *m, unsigned long *n,
				 uint8_t *pd)
{
	uint8_t post_divider = *pd;
	if (pixelclock > PLL_OUT_FREQ_ABS_MAX / POST_DIVIDER_MIN) {
		/* pixel clock frequency is too high */
		ANXERROR("pixelclock %u higher than %lu, "
			 "output may be unstable\n",
			 pixelclock, PLL_OUT_FREQ_ABS_MAX / POST_DIVIDER_MIN);
		return 1;
	}

	if (pixelclock < PLL_OUT_FREQ_ABS_MIN / POST_DIVIDER_MAX) {
		/* pixel clock frequency is too low */
		ANXERROR("pixelclock %u lower than %lu, "
			 "output may be unstable\n",
			 pixelclock, PLL_OUT_FREQ_ABS_MIN / POST_DIVIDER_MAX);
		return 1;
	}

	post_divider = 1;

	for (post_divider = 1;
	     pixelclock < PLL_OUT_FREQ_MIN / post_divider;
	     post_divider++)
		;

	if (post_divider > POST_DIVIDER_MAX) {
		for (post_divider = 1;
		     pixelclock < PLL_OUT_FREQ_ABS_MIN / post_divider;
		     post_divider++)
			;

		if (post_divider > POST_DIVIDER_MAX) {
			ANXERROR("cannot find property post_divider(%d)\n",
				 post_divider);
			return 1;
		}
	}

	/* Patch to improve the accuracy */
	if (post_divider == 7) {
		/* 27,000,000 is not divisible by 7 */
		post_divider = 8;
	} else if (post_divider == 11) {
		/* 27,000,000 is not divisible by 11 */
		post_divider = 12;
	} else if (post_divider == 13 || post_divider == 14) {
		/*27,000,000 is not divisible by 13 or 14*/
		post_divider = 15;
	}

	if (pixelclock * post_divider > PLL_OUT_FREQ_ABS_MAX) {
		ANXINFO("act clock(%u) large than maximum(%lu)\n",
			pixelclock * post_divider, PLL_OUT_FREQ_ABS_MAX);
		return 1;
	}

	*m = pixelclock;
	*n = XTAL_FRQ / post_divider;
	*pd = post_divider;

	anx7625_reduction_of_a_fraction(m, n);

	return 0;
}

static int anx7625_odfc_config(uint8_t bus, uint8_t post_divider)
{
	int ret;

	/* config input reference clock frequency 27MHz/19.2MHz */
	ret = anx7625_write_and(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_16,
			~(REF_CLK_27000kHz << MIPI_FREF_D_IND));
	ret |= anx7625_write_or(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_16,
			(REF_CLK_27000kHz << MIPI_FREF_D_IND));
	/* post divider */
	ret |= anx7625_write_and(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_8, 0x0f);
	ret |= anx7625_write_or(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_8,
			post_divider << 4);

	/* add patch for MIS2-125 (5pcs ANX7625 fail ATE MBIST test) */
	ret |= anx7625_write_and(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_7,
			~MIPI_PLL_VCO_TUNE_REG_VAL);

	/* reset ODFC PLL */
	ret |= anx7625_write_and(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_7,
			~MIPI_PLL_RESET_N);
	ret |= anx7625_write_or(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_7,
			MIPI_PLL_RESET_N);

	if (ret < 0)
		ANXERROR("IO error.\n");

	return ret;
}

static int anx7625_dsi_video_config(uint8_t bus, struct display_timing *dt)
{
	unsigned long m, n;
	u16 htotal;
	int ret;
	uint8_t post_divider = 0;

	ret = anx7625_calculate_m_n(dt->pixelclock * 1000, &m, &n,
				    &post_divider);

	if (ret != 0) {
		ANXERROR("cannot get property m n value.\n");
		return -1;
	}

	ANXINFO("compute M(%lu), N(%lu), divider(%d).\n", m, n, post_divider);

	/* configure pixel clock */
	ret = anx7625_reg_write(bus, RX_P0_ADDR, PIXEL_CLOCK_L,
		(dt->pixelclock / 1000) & 0xFF);
	ret |= anx7625_reg_write(bus, RX_P0_ADDR, PIXEL_CLOCK_H,
		(dt->pixelclock / 1000) >> 8);
	/* lane count */
	ret |= anx7625_write_and(bus, RX_P1_ADDR, MIPI_LANE_CTRL_0, 0xfc);

	ret |= anx7625_write_or(bus, RX_P1_ADDR, MIPI_LANE_CTRL_0, 3);

	/* Htotal */
	htotal = dt->hactive + dt->hfront_porch +
		 dt->hback_porch + dt->hsync_len;
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_TOTAL_PIXELS_L, htotal & 0xFF);
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_TOTAL_PIXELS_H, htotal >> 8);
	/* Hactive */
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_ACTIVE_PIXELS_L, dt->hactive & 0xFF);
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_ACTIVE_PIXELS_H, dt->hactive >> 8);
	/* HFP */
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_FRONT_PORCH_L, dt->hfront_porch);
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_FRONT_PORCH_H,
				 dt->hfront_porch >> 8);
	/* HWS */
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_SYNC_WIDTH_L, dt->hsync_len);
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_SYNC_WIDTH_H, dt->hsync_len >> 8);
	/* HBP */
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_BACK_PORCH_L, dt->hback_porch);
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 HORIZONTAL_BACK_PORCH_H, dt->hback_porch >> 8);
	/* Vactive */
	ret |= anx7625_reg_write(bus, RX_P2_ADDR, ACTIVE_LINES_L, dt->vactive);
	ret |= anx7625_reg_write(bus, RX_P2_ADDR, ACTIVE_LINES_H,
				 dt->vactive >> 8);
	/* VFP */
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 VERTICAL_FRONT_PORCH, dt->vfront_porch);
	/* VWS */
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 VERTICAL_SYNC_WIDTH, dt->vsync_len);
	/* VBP */
	ret |= anx7625_reg_write(bus, RX_P2_ADDR,
				 VERTICAL_BACK_PORCH, dt->vback_porch);
	/* M value */
	ret |= anx7625_reg_write(bus, RX_P1_ADDR,
				 MIPI_PLL_M_NUM_23_16, (m >> 16) & 0xff);
	ret |= anx7625_reg_write(bus, RX_P1_ADDR,
				 MIPI_PLL_M_NUM_15_8, (m >> 8) & 0xff);
	ret |= anx7625_reg_write(bus, RX_P1_ADDR,
				 MIPI_PLL_M_NUM_7_0, (m & 0xff));
	/* N value */
	ret |= anx7625_reg_write(bus, RX_P1_ADDR,
				 MIPI_PLL_N_NUM_23_16, (n >> 16) & 0xff);
	ret |= anx7625_reg_write(bus, RX_P1_ADDR,
				 MIPI_PLL_N_NUM_15_8, (n >> 8) & 0xff);
	ret |= anx7625_reg_write(bus, RX_P1_ADDR, MIPI_PLL_N_NUM_7_0,
				 (n & 0xff));
	/* diff */
	ret |= anx7625_reg_write(bus, RX_P1_ADDR, MIPI_DIGITAL_ADJ_1, 0x3d);

	ret |= anx7625_odfc_config(bus, post_divider - 1);

	if (ret < 0)
		ANXERROR("mipi dsi setup IO error.\n");

	return ret;
}

static int anx7625_swap_dsi_lane3(uint8_t bus)
{
	int ret;
	uint8_t val;

	/* swap MIPI-DSI data lane 3 P and N */
	ret = anx7625_reg_read(bus, RX_P1_ADDR, MIPI_SWAP, &val);
	if (ret < 0) {
		ANXERROR("IO error: access MIPI_SWAP.\n");
		return -1;
	}

	val |= (1 << MIPI_SWAP_CH3);
	return anx7625_reg_write(bus, RX_P1_ADDR, MIPI_SWAP, val);
}

static int anx7625_api_dsi_config(uint8_t bus, struct display_timing *dt)

{
	int val, ret;

	/* swap MIPI-DSI data lane 3 P and N */
	ret = anx7625_swap_dsi_lane3(bus);
	if (ret < 0) {
		ANXERROR("IO error: swap dsi lane 3 failed.\n");
		return ret;
	}

	/* DSI clock settings */
	val = (0 << MIPI_HS_PWD_CLK)		|
		(0 << MIPI_HS_RT_CLK)		|
		(0 << MIPI_PD_CLK)		|
		(1 << MIPI_CLK_RT_MANUAL_PD_EN)	|
		(1 << MIPI_CLK_HS_MANUAL_PD_EN)	|
		(0 << MIPI_CLK_DET_DET_BYPASS)	|
		(0 << MIPI_CLK_MISS_CTRL)	|
		(0 << MIPI_PD_LPTX_CH_MANUAL_PD_EN);
	ret = anx7625_reg_write(bus, RX_P1_ADDR, MIPI_PHY_CONTROL_3, val);

	/*
	 * Decreased HS prepare tg delay from 160ns to 80ns work with
	 *     a) Dragon board 810 series (Qualcomm AP)
	 *     b) Moving Pixel DSI source (PG3A pattern generator +
	 *        P332 D-PHY Probe) default D-PHY tg 5ns/step
	 */
	ret |= anx7625_reg_write(bus, RX_P1_ADDR, MIPI_TIME_HS_PRPR, 0x10);

	/* enable DSI mode */
	ret |= anx7625_write_or(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_18,
				SELECT_DSI << MIPI_DPI_SELECT);

	ret |= anx7625_dsi_video_config(bus, dt);
	if (ret < 0) {
		ANXERROR("dsi video tg config failed\n");
		return ret;
	}

	/* toggle m, n ready */
	ret = anx7625_write_and(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_6,
				~(MIPI_M_NUM_READY | MIPI_N_NUM_READY));
	mdelay(1);
	ret |= anx7625_write_or(bus, RX_P1_ADDR, MIPI_DIGITAL_PLL_6,
				MIPI_M_NUM_READY | MIPI_N_NUM_READY);

	/* configure integer stable register */
	ret |= anx7625_reg_write(bus, RX_P1_ADDR, MIPI_VIDEO_STABLE_CNT, 0x02);
	/* power on MIPI RX */
	ret |= anx7625_reg_write(bus, RX_P1_ADDR, MIPI_LANE_CTRL_10, 0x00);
	ret |= anx7625_reg_write(bus, RX_P1_ADDR, MIPI_LANE_CTRL_10, 0x80);

	if (ret < 0)
		ANXERROR("IO error: mipi dsi enable init failed.\n");

	return ret;
}

static int anx7625_dsi_config(uint8_t bus, struct display_timing *dt)
{
	int ret;

	ANXINFO("config dsi.\n");

	/* DSC disable */
	ret = anx7625_write_and(bus, RX_P0_ADDR, R_DSC_CTRL_0, ~DSC_EN);
	ret |= anx7625_api_dsi_config(bus, dt);

	if (ret < 0) {
		ANXERROR("IO error: api dsi config error.\n");
		return ret;
	}

	/* set MIPI RX EN */
	ret = anx7625_write_or(bus, RX_P0_ADDR, AP_AV_STATUS, AP_MIPI_RX_EN);
	/* clear mute flag */
	ret |= anx7625_write_and(bus, RX_P0_ADDR, AP_AV_STATUS, ~AP_MIPI_MUTE);

	if (ret < 0)
		ANXERROR("IO error: enable mipi rx failed.\n");
	else
		ANXINFO("success to config DSI\n");

	return ret;
}

static int sp_tx_rst_aux(uint8_t bus)
{
	int ret;

	ret = anx7625_write_or(bus, TX_P2_ADDR, RST_CTRL2, AUX_RST);
	ret |= anx7625_write_and(bus, TX_P2_ADDR, RST_CTRL2, ~AUX_RST);
	return ret;
}

static int sp_tx_aux_wr(uint8_t bus, uint8_t offset)
{
	int ret;

	ret = anx7625_reg_write(bus, RX_P0_ADDR, AP_AUX_BUFF_START, offset);
	ret |= anx7625_reg_write(bus, RX_P0_ADDR, AP_AUX_COMMAND, 0x04);
	ret |= anx7625_write_or(bus, RX_P0_ADDR,
				AP_AUX_CTRL_STATUS, AP_AUX_CTRL_OP_EN);
	return ret | wait_aux_op_finish(bus);
}

static int sp_tx_aux_rd(uint8_t bus, uint8_t len_cmd)
{
	int ret;

	ret = anx7625_reg_write(bus, RX_P0_ADDR, AP_AUX_COMMAND, len_cmd);
	ret |= anx7625_write_or(bus, RX_P0_ADDR,
				AP_AUX_CTRL_STATUS, AP_AUX_CTRL_OP_EN);
	return ret | wait_aux_op_finish(bus);
}

static int sp_tx_get_edid_block(uint8_t bus)
{
	int ret;
	uint8_t val = 0;

	sp_tx_aux_wr(bus, 0x7e);
	sp_tx_aux_rd(bus, 0x01);
	ret = anx7625_reg_read(bus, RX_P0_ADDR, AP_AUX_BUFF_START, &val);

	if (ret < 0) {
		ANXERROR("IO error: access AUX BUFF.\n");
		return -1;
	}

	ANXINFO("EDID Block = %d\n", val + 1);

	if (val > 3)
		val = 1;

	return val;
}

static int edid_read(uint8_t bus, uint8_t offset, uint8_t *pblock_buf)
{
	uint8_t c, cnt = 0;

	c = 0;
	for (cnt = 0; cnt < 3; cnt++) {
		sp_tx_aux_wr(bus, offset);
		/* set I2C read com 0x01 mot = 0 and read 16 bytes */
		c = sp_tx_aux_rd(bus, 0xf1);

		if (c == 1) {
			sp_tx_rst_aux(bus);
			ANXERROR("edid read failed, reset!\n");
			cnt++;
		} else {
			anx7625_reg_block_read(bus, RX_P0_ADDR,
					AP_AUX_BUFF_START,
					MAX_DPCD_BUFFER_SIZE, pblock_buf);
			return 0;
		}
	}

	return 1;
}

static int segments_edid_read(uint8_t bus, uint8_t segment, uint8_t *buf,
			      uint8_t offset)
{
	uint8_t c, cnt = 0;
	int ret;

	/* write address only */
	ret = anx7625_reg_write(bus, RX_P0_ADDR, AP_AUX_ADDR_7_0, 0x30);
	ret |= anx7625_reg_write(bus, RX_P0_ADDR, AP_AUX_COMMAND, 0x04);
	ret |= anx7625_reg_write(bus, RX_P0_ADDR, AP_AUX_CTRL_STATUS,
				 AP_AUX_CTRL_ADDRONLY | AP_AUX_CTRL_OP_EN);

	ret |= wait_aux_op_finish(bus);
	/* write segment address */
	ret |= sp_tx_aux_wr(bus, segment);
	/* data read */
	ret |= anx7625_reg_write(bus, RX_P0_ADDR, AP_AUX_ADDR_7_0, 0x50);

	if (ret < 0) {
		ANXERROR("IO error: aux initial failed.\n");
		return ret;
	}

	for (cnt = 0; cnt < 3; cnt++) {
		sp_tx_aux_wr(bus, offset);
		/* set I2C read com 0x01 mot = 0 and read 16 bytes */
		c = sp_tx_aux_rd(bus, 0xf1);

		if (c == 1) {
			ret = sp_tx_rst_aux(bus);
			ANXERROR("segment read failed, reset!\n");
			cnt++;
		} else {
			ret = anx7625_reg_block_read(bus, RX_P0_ADDR,
					AP_AUX_BUFF_START,
					MAX_DPCD_BUFFER_SIZE, buf);
			return ret;
		}
	}

	return ret;
}

static int sp_tx_edid_read(uint8_t bus, uint8_t *pedid_blocks_buf,
			   uint32_t size)
{
	uint8_t offset, edid_pos;
	int count, blocks_num;
	uint8_t pblock_buf[MAX_DPCD_BUFFER_SIZE];
	uint8_t i;
	uint8_t g_edid_break = 0;
	int ret;

	/* address initial */
	ret = anx7625_reg_write(bus, RX_P0_ADDR, AP_AUX_ADDR_7_0, 0x50);
	ret |= anx7625_reg_write(bus, RX_P0_ADDR, AP_AUX_ADDR_15_8, 0);
	ret |= anx7625_write_and(bus, RX_P0_ADDR, AP_AUX_ADDR_19_16, 0xf0);

	if (ret < 0) {
		ANXERROR("access aux channel IO error.\n");
		return -1;
	}

	blocks_num = sp_tx_get_edid_block(bus);
	if (blocks_num < 0)
		return blocks_num;

	count = 0;

	do {
		switch (count) {
		case 0:
		case 1:
			for (i = 0; i < 8; i++) {
				offset = (i + count * 8) * MAX_DPCD_BUFFER_SIZE;
				g_edid_break = edid_read(bus, offset,
						pblock_buf);

				if (g_edid_break == 1)
					break;

				if (offset <= size - MAX_DPCD_BUFFER_SIZE)
					memcpy(&pedid_blocks_buf[offset],
					       pblock_buf,
					       MAX_DPCD_BUFFER_SIZE);
			}

			break;
		case 2:
		case 3:
			offset = (count == 2) ? 0x00 : 0x80;

			for (i = 0; i < 8; i++) {
				edid_pos = (i + count * 8) *
					MAX_DPCD_BUFFER_SIZE;

				if (g_edid_break == 1)
					break;

				segments_edid_read(bus, count / 2,
						pblock_buf, offset);
				if (edid_pos <= size - MAX_DPCD_BUFFER_SIZE)
					memcpy(&pedid_blocks_buf[edid_pos],
					       pblock_buf,
					       MAX_DPCD_BUFFER_SIZE);
				offset = offset + 0x10;
			}

			break;
		default:
			die("%s: count should be <= 3", __func__);
			break;
		}

		count++;

	} while (blocks_num >= count);

	/* reset aux channel */
	sp_tx_rst_aux(bus);

	return blocks_num;
}

static void anx7625_disable_pd_protocol(uint8_t bus)
{
	int ret;

	/* reset main ocm */
	ret = anx7625_reg_write(bus, RX_P0_ADDR, 0x88, 0x40);
	/* Disable PD */
	ret |= anx7625_reg_write(bus, RX_P0_ADDR, AP_AV_STATUS, AP_DISABLE_PD);
	/* release main ocm */
	ret |= anx7625_reg_write(bus, RX_P0_ADDR, 0x88, 0x00);

	if (ret < 0)
		ANXERROR("Failed to disable PD feature.\n");
	else
		ANXINFO("Disabled PD feature.\n");
}

#define FLASH_LOAD_STA 0x05
#define FLASH_LOAD_STA_CHK	(1 << 7)

static int anx7625_power_on_init(uint8_t bus)
{
	int i, ret;
	uint8_t val, version, revision;

	anx7625_reg_write(bus, RX_P0_ADDR, XTAL_FRQ_SEL, XTAL_FRQ_27M);

	for (i = 0; i < OCM_LOADING_TIME; i++) {
		/* check interface */
		ret = anx7625_reg_read(bus, RX_P0_ADDR, FLASH_LOAD_STA, &val);
		if (ret < 0) {
			ANXERROR("Failed to load flash\n");
			return ret;
		}

		if ((val & FLASH_LOAD_STA_CHK) != FLASH_LOAD_STA_CHK) {
			mdelay(1);
			continue;
		}
		ANXINFO("Init interface.\n");

		anx7625_disable_pd_protocol(bus);
		anx7625_reg_read(bus, RX_P0_ADDR, OCM_FW_VERSION, &version);
		anx7625_reg_read(bus, RX_P0_ADDR, OCM_FW_REVERSION, &revision);
		ANXINFO("Firmware: ver %#02x, rev %#02x.\n", version, revision);
		return 0;
	}
	return -1;
}

static void anx7625_start_dp_work(uint8_t bus)
{
	int ret;
	uint8_t val;

	/* not support HDCP */
	ret = anx7625_write_and(bus, RX_P1_ADDR, 0xee, 0x9f);

	/* try auth flag */
	ret |= anx7625_write_or(bus, RX_P1_ADDR, 0xec, 0x10);
	/* interrupt for DRM */
	ret |= anx7625_write_or(bus, RX_P1_ADDR, 0xff, 0x01);
	if (ret < 0)
		return;

	ret = anx7625_reg_read(bus, RX_P1_ADDR, 0x86, &val);
	if (ret < 0)
		return;

	ANXINFO("Secure OCM version=%02x\n", val);
}

static int anx7625_hpd_change_detect(uint8_t bus)
{
	int ret;
	uint8_t status;

	ret = anx7625_reg_read(bus, RX_P0_ADDR, SYSTEM_STSTUS, &status);
	if (ret < 0) {
		ANXERROR("IO error: Failed to clear interrupt status.\n");
		return ret;
	}

	if (status & HPD_STATUS) {
		anx7625_start_dp_work(bus);
		ANXINFO("HPD received 0x7e:0x45=%#x\n", status);
		return 1;
	}
	return 0;
}

static void anx7625_parse_edid(const struct edid *edid,
			       struct display_timing *dt)
{
	dt->pixelclock = edid->mode.pixel_clock;

	dt->hactive = edid->mode.ha;
	dt->hsync_len = edid->mode.hspw;
	dt->hback_porch = (edid->mode.hbl - edid->mode.hso -
			   edid->mode.hborder - edid->mode.hspw);
	dt->hfront_porch = edid->mode.hso - edid->mode.hborder;

	dt->vactive = edid->mode.va;
	dt->vsync_len = edid->mode.vspw;
	dt->vfront_porch = edid->mode.vso - edid->mode.vborder;
	dt->vback_porch = (edid->mode.vbl - edid->mode.vso -
			   edid->mode.vspw - edid->mode.vborder);

	ANXINFO("pixelclock(%d).\n"
		" hactive(%d), hsync(%d), hfp(%d), hbp(%d)\n"
		" vactive(%d), vsync(%d), vfp(%d), vbp(%d)\n",
		dt->pixelclock,
		dt->hactive, dt->hsync_len, dt->hfront_porch, dt->hback_porch,
		dt->vactive, dt->vsync_len, dt->vfront_porch, dt->vback_porch);
}

int anx7625_dp_start(uint8_t bus, const struct edid *edid)
{
	int ret;
	struct display_timing dt;

	anx7625_parse_edid(edid, &dt);

	ret = anx7625_dsi_config(bus, &dt);
	if (ret < 0)
		ANXERROR("MIPI phy setup error.\n");
	else
		ANXINFO("MIPI phy setup OK.\n");

	return ret;
}

int anx7625_dp_get_edid(uint8_t bus, struct edid *out)
{
	int block_num;
	int ret;
	u8 edid[FOUR_BLOCK_SIZE];

	block_num = sp_tx_edid_read(bus, edid, FOUR_BLOCK_SIZE);
	if (block_num < 0) {
		ANXERROR("Failed to get eDP EDID.\n");
		return -1;
	}

	ret = decode_edid(edid, (block_num + 1) * ONE_BLOCK_SIZE, out);
	if (ret != EDID_CONFORMANT) {
		ANXERROR("Failed to decode EDID.\n");
		return -1;
	}

	return 0;
}

int anx7625_init(uint8_t bus)
{
	int retry_hpd_change = 50;
	int retry_power_on = 3;

	while (--retry_power_on) {
		if (anx7625_power_on_init(bus) == 0)
			break;
	}
	if (!retry_power_on) {
		ANXERROR("Failed to power on.\n");
		return -1;
	}

	while (--retry_hpd_change) {
		mdelay(10);
		int detected = anx7625_hpd_change_detect(bus);
		if (detected < 0)
			return -1;
		if (detected > 0)
			return 0;
	}

	ANXERROR("Timed out to detect HPD change on bus %d.\n", bus);
	return -1;
}
