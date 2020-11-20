/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MTK_COMMON_I2C_H
#define MTK_COMMON_I2C_H

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
	I2C_SET_SPEED_FAIL_OVER_SPEED = 0xA001,
	I2C_TRANSFER_INVALID_LENGTH = 0xA002,
	I2C_TRANSFER_FAIL_HS_NACKERR = 0xA003,
	I2C_TRANSFER_FAIL_ACKERR = 0xA004,
	I2C_TRANSFER_FAIL_TIMEOUT = 0xA005,
	I2C_TRANSFER_INVALID_ARGUMENT = 0xA006
};

struct mtk_i2c {
	struct mt_i2c_regs *i2c_regs;
	struct mt_i2c_dma_regs *i2c_dma_regs;
	uint32_t mt_i2c_flag;
};

extern struct mtk_i2c mtk_i2c_bus_controller[];
#endif
