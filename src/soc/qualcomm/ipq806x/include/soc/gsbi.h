/* SPDX-License-Identifier: ISC */

#ifndef __GSBI_H_
#define __GSBI_H_

/* GSBI Registers */
#define GSBI_CTRL_REG(base)        ((base) + 0x0)

#define GSBI_CTRL_REG_PROTOCOL_CODE_S   4
#define GSBI_PROTOCOL_CODE_I2C          0x2
#define GSBI_PROTOCOL_CODE_SPI          0x3
#define GSBI_PROTOCOL_CODE_UART_FLOW    0x4
#define GSBI_PROTOCOL_CODE_I2C_UART     0x6

#define GSBI_HCLK_CTL_S                 4
#define GSBI_HCLK_CTL_CLK_ENA           0x1

typedef enum {
	GSBI_ID_1 = 1,
	GSBI_ID_2,
	GSBI_ID_3,
	GSBI_ID_4,
	GSBI_ID_5,
	GSBI_ID_6,
	GSBI_ID_7,
} gsbi_id_t;

typedef enum {
	GSBI_SUCCESS = 0,
	GSBI_ID_ERROR,
	GSBI_ERROR,
	GSBI_UNSUPPORTED
} gsbi_return_t;

typedef enum {
	GSBI_PROTO_I2C_UIM = 1,
	GSBI_PROTO_I2C_ONLY,
	GSBI_PROTO_SPI_ONLY,
	GSBI_PROTO_UART_FLOW_CTL,
	GSBI_PROTO_UIM,
	GSBI_PROTO_I2C_UART,
} gsbi_protocol_t;

gsbi_return_t gsbi_init(gsbi_id_t gsbi_id, gsbi_protocol_t protocol);
int gsbi_init_board(gsbi_id_t gsbi_id);

#endif
