/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __QUP_H__
#define __QUP_H__

/* QUP block registers */
#define QUP_CONFIG			0x000
#define QUP_STATE			0x004
#define QUP_IO_MODES			0x008
#define QUP_SW_RESET			0x00C
#define QUP_TRANSFER_CANCEL		0x014
#define QUP_OPERATIONAL			0x018
#define QUP_ERROR_FLAGS			0x01C
#define QUP_ERROR_FLAGS_EN		0x020
#define QUP_TEST_CTRL			0x024
#define QUP_OPERATIONAL_MASK		0x028
#define QUP_HW_VERSION			0x030
#define QUP_MX_OUTPUT_COUNT		0x100
#define QUP_MX_OUTPUT_CNT_CURRENT	0x104
#define QUP_OUTPUT_DEBUG		0x108
#define QUP_OUTPUT_FIFO_WORD_CNT	0x10C
#define QUP_OUTPUT_FIFO			0x110
#define QUP_OUTPUT_FIFO_SIZE		64	/* bytes */
#define QUP_MX_WRITE_COUNT		0x150
#define QUP_MX_WRITE_CNT_CURRENT	0x154
#define QUP_MX_INPUT_COUNT		0x200
#define QUP_MX_INPUT_CNT_CURRENT	0x204
#define QUP_MX_READ_COUNT		0x208
#define QUP_MX_READ_CNT_CURRENT		0x20C
#define QUP_INPUT_DEBUG			0x210
#define QUP_INPUT_FIFO_WORD_CNT		0x214
#define QUP_INPUT_FIFO			0x218
#define QUP_INPUT_FIFO_SIZE		64	/* bytes */
#define QUP_I2C_MASTER_CLK_CTL		0x400
#define QUP_I2C_MASTER_STATUS		0x404
#define QUP_I2C_MASTER_CONFIG		0x408
#define QUP_I2C_MASTER_BUS_CLEAR	0x40C
#define QUP_I2C_MASTER_LOCAL_ID		0x410
#define QUP_I2C_MASTER_COMMAND		0x414

#define OUTPUT_FIFO_FULL		(1<<6)
#define INPUT_FIFO_NOT_EMPTY		(1<<5)
#define OUTPUT_FIFO_NOT_EMPTY		(1<<4)
#define MAX_OUTPUT_DONE_FLAG		(1<<10)
#define MAX_INPUT_DONE_FLAG		(1<<11)
#define INPUT_SERVICE_FLAG		(1<<9)
#define OUTPUT_SERVICE_FLAG		(1<<8)
#define QUP_UNPACK_EN			(1<<14)
#define QUP_PACK_EN			(1<<15)
#define QUP_OUTPUT_BIT_SHIFT_EN		(1<<16)

#define QUP_MODE_MASK			(0x03)
#define QUP_OUTPUT_MODE_SHFT		(10)
#define QUP_INPUT_MODE_SHFT		(12)

#define QUP_FS_DIVIDER_MASK		(0xFF)

#define QUP_APP_CLK_ON_EN		(1 << 12)
#define QUP_CORE_CLK_ON_EN		(1 << 13)
#define QUP_MINI_CORE_PROTO_SHFT	(8)
#define QUP_MINI_CORE_PROTO_MASK	(0x0F)

/* Mini-core states */
#define QUP_STATE_RESET			0x0
#define QUP_STATE_RUN			0x1
#define QUP_STATE_PAUSE			0x3
#define QUP_STATE_VALID			(1<<2)
#define QUP_STATE_MASK			0x3
#define QUP_STATE_VALID_MASK		(1<<2)

/* Tags for output FIFO */
#define QUP_I2C_1CLK_NOOP_SEQ	0x1	/*MSB 8-bit NOP, LSB 8-bits 1 clk.*/
#define QUP_I2C_START_SEQ		(0x1 << 8)
#define QUP_I2C_DATA_SEQ		(0x2 << 8)
#define QUP_I2C_STOP_SEQ		(0x3 << 8)
#define QUP_I2C_RECV_SEQ		(0x4 << 8)

/* Tags for input FIFO */
#define QUP_I2C_MIDATA_SEQ		(0x5 << 8)
#define QUP_I2C_MISTOP_SEQ		(0x6 << 8)
#define QUP_I2C_MINACK_SEQ		(0x7 << 8)

#define QUP_I2C_ADDR(x)			((x & 0xFF) << 1)
#define QUP_I2C_DATA(x)			(x & 0xFF)
#define QUP_I2C_MI_TAG(x)		(x & 0xFF00)
#define QUP_I2C_SLAVE_READ		(0x1)

/*Bit vals for I2C_MASTER_CLK_CTL register */
#define QUP_HS_DIVIDER_SHFT		(8)
#define QUP_DIVIDER_MIN_VAL		(0x3)

/* Bit masks for I2C_MASTER_STATUS register */
#define QUP_I2C_INVALID_READ_SEQ	(1 << 25)
#define QUP_I2C_INVALID_READ_ADDR	(1 << 24)
#define QUP_I2C_INVALID_TAG		(1 << 23)
#define QUP_I2C_FAILED_MASK		(0x3 << 6)
#define QUP_I2C_INVALID_WRITE		(1 << 5)
#define QUP_I2C_ARB_LOST		(1 << 4)
#define QUP_I2C_PACKET_NACK		(1 << 3)
#define QUP_I2C_BUS_ERROR		(1 << 2)

typedef enum {
	QUP_SUCCESS = 0,
	QUP_ERR_BAD_PARAM,
	QUP_ERR_STATE_SET,
	QUP_ERR_TIMEOUT,
	QUP_ERR_UNSUPPORTED,
	QUP_ERR_I2C_FAILED,
	QUP_ERR_I2C_ARB_LOST,
	QUP_ERR_I2C_BUS_ERROR,
	QUP_ERR_I2C_INVALID_SLAVE_ADDR,
	QUP_ERR_XFER_FAIL,
	QUP_ERR_I2C_NACK,
	QUP_ERR_I2C_INVALID_WRITE,
	QUP_ERR_I2C_INVALID_TAG,
	QUP_ERR_UNDEFINED,
} qup_return_t;

typedef enum {
	QUP_MINICORE_SPI = 1,
	QUP_MINICORE_I2C_MASTER,
	QUP_MINICORE_I2C_SLAVE
} qup_protocol_t;

typedef enum {
	QUP_MODE_FIFO = 0,
	QUP_MODE_BLOCK,
	QUP_MODE_DATAMOVER,
} qup_mode_t;

typedef struct {
	qup_protocol_t protocol;
	unsigned int clk_frequency;
	unsigned int src_frequency;
	qup_mode_t mode;
	unsigned int initialized;
} qup_config_t;

typedef struct {
	qup_protocol_t protocol;
	union {
		struct {
			uint8_t addr;
			uint8_t *data;
			unsigned int data_len;
		} iic;
		struct {
			void *in;
			void *out;
			unsigned int size;
		} spi;
	} p;
} qup_data_t;

/*
 * Initialize BLSP QUP block for FIFO I2C transfers.
 * id[IN]: BLSP for which QUP is to be initialized.
 * config_ptr[IN]: configurations parameters for the QUP.
 *
 * return: QUP_SUCCESS, if initialization succeeds.
 */
qup_return_t qup_init(blsp_qup_id_t id, const qup_config_t *config_ptr);

/*
 * Set QUP state to run, pause, reset.
 * id[IN]: BLSP block for which QUP state is to be set.
 * state[IN]: New state to transition to.
 *
 * return: QUP_SUCCESS, if state transition succeeds.
 */
qup_return_t qup_set_state(blsp_qup_id_t id, uint32_t state);

/*
 * Reset the status bits set during an i2c transfer.
 * id[IN]: BLSP block for which i2c status bits are to be cleared.
 *
 * return: QUP_SUCCESS, if status bits are cleared successfully.
 */
qup_return_t qup_reset_i2c_master_status(blsp_qup_id_t id);

/*
 * Send data to the peripheral on the bus.
 * id[IN]: BLSP block for which data is to be sent.
 * p_tx_obj[IN]: Data to be sent to the slave on the bus.
 * stop_seq[IN]: When set to non-zero QUP engine sends i2c stop sequnce.
 *
 * return: QUP_SUCCESS, when data is sent successfully to the peripheral.
 */
qup_return_t qup_send_data(blsp_qup_id_t id, qup_data_t *p_tx_obj,
			   uint8_t stop_seq);

/*
 * Receive data from peripheral on the bus.
 * id[IN]: BLSP block from which data is to be received.
 * p_tx_obj[IN]: length of data to be received, slave address.
 *      [OUT]: buffer filled with data from slave.
 *
 * return: QUP_SUCCESS, when data is received successfully.
 */
qup_return_t qup_recv_data(blsp_qup_id_t id, qup_data_t *p_tx_obj);

#endif //__QUP_H__
