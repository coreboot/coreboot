/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __BLSP_H_
#define __BLSP_H_

typedef enum {
	BLSP_QUP_ID_0,
	BLSP_QUP_ID_1,
	BLSP_QUP_ID_2,
	BLSP_QUP_ID_3,
} blsp_qup_id_t;

typedef enum {
	BLSP_SUCCESS = 0,
	BLSP_ID_ERROR,
	BLSP_ERROR,
	BLSP_UNSUPPORTED
} blsp_return_t;

typedef enum {
	BLSP_PROTO_I2C_UIM = 1,
	BLSP_PROTO_I2C_ONLY,
	BLSP_PROTO_SPI_ONLY,
	BLSP_PROTO_UART_FLOW_CTL,
	BLSP_PROTO_UIM,
	BLSP_PROTO_I2C_UART,
} blsp_protocol_t;

blsp_return_t blsp_i2c_init(blsp_qup_id_t id);
int blsp_i2c_init_board(blsp_qup_id_t id);

#endif
