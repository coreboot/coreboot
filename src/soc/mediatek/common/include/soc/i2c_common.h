/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MTK_COMMON_I2C_H
#define MTK_COMMON_I2C_H

#include <commonlib/bsd/helpers.h>
#include <device/i2c.h>

/* I2C DMA Registers */
struct mt_i2c_dma_regs {
	uint32_t dma_int_flag;
	uint32_t dma_int_en;
	uint32_t dma_en;
	uint32_t dma_rst;
	uint32_t reserved1;
	uint32_t dma_flush;
	uint32_t dma_con;
	uint32_t dma_tx_mem_addr;
	uint32_t dma_rx_mem_addr;
	uint32_t dma_tx_len;
	uint32_t dma_rx_len;
};

check_member(mt_i2c_dma_regs, dma_tx_len, 0x24);

/* I2C Configuration */
enum {
	I2C_HS_DEFAULT_VALUE	 = 0x0102,
};

enum i2c_modes {
	I2C_WRITE_MODE		= 0,
	I2C_READ_MODE		= 1,
	I2C_WRITE_READ_MODE	= 2,
};

enum {
	I2C_DMA_CON_TX          = 0x0,
	I2C_DMA_CON_RX          = 0x1,
	I2C_DMA_START_EN        = 0x1,
	I2C_DMA_INT_FLAG_NONE   = 0x0,
	I2C_DMA_CLR_FLAG        = 0x0,
	I2C_DMA_FLUSH_FLAG      = 0x1,
	I2C_DMA_ASYNC_MODE      = 0x0004,
	I2C_DMA_SKIP_CONFIG     = 0x0010,
	I2C_DMA_DIR_CHANGE      = 0x0200,
	I2C_DMA_WARM_RST        = 0x1,
	I2C_DMA_HARD_RST        = 0x2,
	I2C_DMA_HANDSHAKE_RST   = 0x4,
};

enum {
	I2C_TRANS_LEN_MASK = (0xff),
	I2C_TRANS_AUX_LEN_MASK = (0x1f << 8),
	I2C_CONTROL_MASK = (0x3f << 1)
};

enum {
	I2C_APDMA_NOASYNC       = 0,
	I2C_APDMA_ASYNC         = 1,
};

/* Register mask */
enum {
	I2C_HS_NACKERR = (1 << 2),
	I2C_ACKERR = (1 << 1),
	I2C_TRANSAC_COMP = (1 << 0),
};

/* reset bits */
enum {
	I2C_CLR_FLAG            = 0x0,
	I2C_SOFT_RST            = 0x1,
	I2C_HANDSHAKE_RST       = 0x20,
};

/* i2c control bits */
enum {
	ASYNC_MODE = (1 << 9),
	DMAACK_EN = (1 << 8),
	ACK_ERR_DET_EN = (1 << 5),
	DIR_CHG = (1 << 4),
	CLK_EXT = (1 << 3),
	DMA_EN = (1 << 2),
	REPEATED_START_FLAG = (1 << 1),
	STOP_FLAG = (0 << 1)
};

/* I2C Status Code */
enum {
	I2C_OK = 0x0000,
	I2C_SET_SPEED_FAIL_OVER_SPEED = -0xA001,
	I2C_TRANSFER_INVALID_LENGTH = -0xA002,
	I2C_TRANSFER_FAIL_HS_NACKERR = -0xA003,
	I2C_TRANSFER_FAIL_ACKERR = -0xA004,
	I2C_TRANSFER_FAIL_TIMEOUT = -0xA005,
	I2C_TRANSFER_INVALID_ARGUMENT = -0xA006,
};

struct mtk_i2c_ac_timing {
	u16 htiming;
	u16 ltiming;
	u16 hs;
	u16 ext;
	u16 inter_clk_div;
	u16 scl_hl_ratio;
	u16 hs_scl_hl_ratio;
	u16 sta_stop;
	u16 hs_sta_stop;
	u16 sda_timing;
};

struct mtk_i2c {
	struct mt_i2c_regs *i2c_regs;
	struct mt_i2c_dma_regs *i2c_dma_regs;
	struct mtk_i2c_ac_timing ac_timing;
	uint32_t mt_i2c_flag;
};

#define I2C_TIME_CLR_VALUE		0x0000
#define MAX_SAMPLE_CNT_DIV		8
#define MAX_STEP_CNT_DIV		64
#define MAX_HS_STEP_CNT_DIV		8
#define I2C_TIME_DEFAULT_VALUE		0x0083
#define I2C_STANDARD_MODE_BUFFER	(1000 / 3)
#define I2C_FAST_MODE_BUFFER		(300 / 3)
#define I2C_FAST_MODE_PLUS_BUFFER	(20 / 3)

/*
 * struct i2c_spec_values:
 * @min_low_ns: min LOW period of the SCL clock
 * @min_su_sta_ns: min set-up time for a repeated START condition
 * @max_hd_dat_ns: max data hold time
 * @min_su_dat_ns: min data set-up time
 */
struct i2c_spec_values {
	uint32_t min_low_ns;
	uint32_t min_su_sta_ns;
	uint32_t max_hd_dat_ns;
	uint32_t min_su_dat_ns;
};

extern struct mtk_i2c mtk_i2c_bus_controller[];
const struct i2c_spec_values *mtk_i2c_get_spec(uint32_t speed);
void mtk_i2c_dump_more_info(struct mt_i2c_regs *regs);

int mtk_i2c_check_ac_timing(uint8_t bus, uint32_t clk_src,
			    uint32_t check_speed,
			    uint32_t step_cnt,
			    uint32_t sample_cnt);
int mtk_i2c_calculate_speed(uint8_t bus, uint32_t clk_src,
			    uint32_t target_speed,
			    uint32_t *timing_step_cnt,
			    uint32_t *timing_sample_cnt);
void mtk_i2c_speed_init(uint8_t bus, uint32_t speed);
void mtk_i2c_config_timing(struct mt_i2c_regs *regs, struct mtk_i2c *bus_ctrl);
void mtk_i2c_set_gpio_pinmux(uint8_t bus);
void mtk_i2c_bus_init(uint8_t bus, uint32_t speed);

#endif
