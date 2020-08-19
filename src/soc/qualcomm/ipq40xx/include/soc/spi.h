/* Register definitions for the IPQ BLSP SPI Controller */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _IPQ40XX_SPI_H_
#define _IPQ40XX_SPI_H_

#include <soc/iomap.h>
#include <soc/qup.h>
#include <spi-generic.h>

#define BLSP0_QUP_REG_BASE		((void *)0x78b5000u)
#define BLSP1_QUP_REG_BASE		((void *)0x78b6000u)

#define BLSP0_SPI_CONFIG_REG		(BLSP0_QUP_REG_BASE + 0x00000300)
#define BLSP1_SPI_CONFIG_REG		(BLSP1_QUP_REG_BASE + 0x00000300)

#define BLSP0_SPI_IO_CONTROL_REG	(BLSP0_QUP_REG_BASE + 0x00000304)
#define BLSP1_SPI_IO_CONTROL_REG	(BLSP1_QUP_REG_BASE + 0x00000304)

#define BLSP0_SPI_ERROR_FLAGS_REG	(BLSP0_QUP_REG_BASE + 0x00000308)
#define BLSP1_SPI_ERROR_FLAGS_REG	(BLSP1_QUP_REG_BASE + 0x00000308)

#define BLSP0_SPI_DEASSERT_WAIT_REG	(BLSP0_QUP_REG_BASE + 0x00000310)
#define BLSP1_SPI_DEASSERT_WAIT_REG	(BLSP1_QUP_REG_BASE + 0x00000310)
#define BLSP0_SPI_ERROR_FLAGS_EN_REG	(BLSP0_QUP_REG_BASE + 0x0000030c)
#define BLSP1_SPI_ERROR_FLAGS_EN_REG	(BLSP1_QUP_REG_BASE + 0x0000030c)

#define BLSP0_QUP_CONFIG_REG		(BLSP0_QUP_REG_BASE + 0x00000000)
#define BLSP1_QUP_CONFIG_REG		(BLSP1_QUP_REG_BASE + 0x00000000)

#define BLSP0_QUP_ERROR_FLAGS_REG	(BLSP0_QUP_REG_BASE + 0x0000001c)
#define BLSP1_QUP_ERROR_FLAGS_REG	(BLSP1_QUP_REG_BASE + 0x0000001c)

#define BLSP0_QUP_ERROR_FLAGS_EN_REG	(BLSP0_QUP_REG_BASE + 0x00000020)
#define BLSP1_QUP_ERROR_FLAGS_EN_REG	(BLSP1_QUP_REG_BASE + 0x00000020)

#define BLSP0_QUP_OPERATIONAL_MASK	(BLSP0_QUP_REG_BASE + 0x00000028)
#define BLSP1_QUP_OPERATIONAL_MASK	(BLSP1_QUP_REG_BASE + 0x00000028)

#define BLSP0_QUP_OPERATIONAL_REG	(BLSP0_QUP_REG_BASE + 0x00000018)
#define BLSP1_QUP_OPERATIONAL_REG	(BLSP1_QUP_REG_BASE + 0x00000018)

#define BLSP0_QUP_IO_MODES_REG		(BLSP0_QUP_REG_BASE + 0x00000008)
#define BLSP1_QUP_IO_MODES_REG		(BLSP1_QUP_REG_BASE + 0x00000008)

#define BLSP0_QUP_STATE_REG		(BLSP0_QUP_REG_BASE + 0x00000004)
#define BLSP1_QUP_STATE_REG		(BLSP1_QUP_REG_BASE + 0x00000004)

#define BLSP0_QUP_INPUT_FIFOc_REG(c) \
		(BLSP0_QUP_REG_BASE + 0x00000218 + 4 * (c))
#define BLSP1_QUP_INPUT_FIFOc_REG(c) \
		(BLSP1_QUP_REG_BASE + 0x00000218 + 4 * (c))

#define BLSP0_QUP_OUTPUT_FIFOc_REG(c) \
		(BLSP0_QUP_REG_BASE + 0x00000110 + 4 * (c))
#define BLSP1_QUP_OUTPUT_FIFOc_REG(c) \
		(BLSP1_QUP_REG_BASE + 0x00000110 + 4 * (c))

#define BLSP0_QUP_MX_INPUT_COUNT_REG	(BLSP0_QUP_REG_BASE + 0x00000200)
#define BLSP1_QUP_MX_INPUT_COUNT_REG	(BLSP1_QUP_REG_BASE + 0x00000200)

#define BLSP0_QUP_MX_OUTPUT_COUNT_REG	(BLSP0_QUP_REG_BASE + 0x00000100)
#define BLSP1_QUP_MX_OUTPUT_COUNT_REG	(BLSP1_QUP_REG_BASE + 0x00000100)

#define BLSP0_QUP_SW_RESET_REG		(BLSP0_QUP_REG_BASE + 0x0000000c)
#define BLSP1_QUP_SW_RESET_REG		(BLSP1_QUP_REG_BASE + 0x0000000c)

#define QUP_CONFIG_MINI_CORE_MSK		(0x0F << 8)
#define QUP_CONFIG_MINI_CORE_SPI		(1 << 8)
#define QUP_CONF_INPUT_MSK			(1 << 7)
#define QUP_CONF_INPUT_ENA			(0 << 7)
#define QUP_CONF_NO_INPUT			(1 << 7)
#define QUP_CONF_OUTPUT_MSK			(1 << 6)
#define QUP_CONF_OUTPUT_ENA			(0 << 6)
#define QUP_CONF_NO_OUTPUT			(1 << 6)
#define QUP_CONF_N_MASK				0x1F
#define QUP_CONF_N_SPI_8_BIT_WORD		0x07

#define SPI_CONFIG_INPUT_FIRST			(1 << 9)
#define SPI_CONFIG_INPUT_FIRST_BACK		(0 << 9)
#define SPI_CONFIG_LOOP_BACK_MSK		(1 << 8)
#define SPI_CONFIG_NO_LOOP_BACK			(0 << 8)
#define SPI_CONFIG_NO_SLAVE_OPER_MSK		(1 << 5)
#define SPI_CONFIG_NO_SLAVE_OPER		(0 << 5)

#define SPI_IO_CTRL_CLK_ALWAYS_ON		(0 << 9)
#define SPI_IO_CTRL_MX_CS_MODE			(1 << 8)
#define SPI_IO_CTRL_NO_TRI_STATE		(1 << 0)
#define SPI_IO_CTRL_FORCE_CS_MSK		(1 << 11)
#define SPI_IO_CTRL_FORCE_CS_EN			(1 << 11)
#define SPI_IO_CTRL_FORCE_CS_DIS		(0 << 11)
#define SPI_IO_CTRL_CLOCK_IDLE_HIGH		(1 << 10)

#define QUP_IO_MODES_OUTPUT_BIT_SHIFT_MSK	(1 << 16)
#define QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN	(1 << 16)
#define QUP_IO_MODES_INPUT_MODE_MSK		(0x03 << 12)
#define QUP_IO_MODES_INPUT_BLOCK_MODE		(0x01 << 12)
#define QUP_IO_MODES_OUTPUT_MODE_MSK		(0x03 << 10)
#define QUP_IO_MODES_OUTPUT_BLOCK_MODE		(0x01 << 10)

#define SPI_INPUT_BLOCK_SIZE			4
#define SPI_OUTPUT_BLOCK_SIZE			4

#define MAX_COUNT_SIZE				0xffff

#define SPI_CORE_RESET				0
#define SPI_CORE_RUNNING			1
#define SPI_MODE0				0
#define SPI_MODE1				1
#define SPI_MODE2				2
#define SPI_MODE3				3
#define BLSP0_SPI				0
#define BLSP1_SPI				1

struct blsp_spi {
	void *spi_config;
	void *io_control;
	void *error_flags;
	void *error_flags_en;
	void *qup_config;
	void *qup_error_flags;
	void *qup_error_flags_en;
	void *qup_operational;
	void *qup_io_modes;
	void *qup_state;
	void *qup_input_fifo;
	void *qup_output_fifo;
	void *qup_mx_input_count;
	void *qup_mx_output_count;
	void *qup_sw_reset;
	void *qup_ns_reg;
	void *qup_md_reg;
	void *qup_op_mask;
	void *qup_deassert_wait;
};

#define SUCCESS		0

#define DUMMY_DATA_VAL		0
#define TIMEOUT_CNT		100

#define ETIMEDOUT -10
#define EINVAL -11
#define EIO -12

/* MX_INPUT_COUNT and MX_OUTPUT_COUNT are 16-bits. Zero has a special meaning
 * (count function disabled) and does not hold significance in the count. */
#define MAX_PACKET_COUNT	((64 * KiB) - 1)

struct ipq_spi_slave {
	struct spi_slave slave;
	const struct blsp_spi *regs;
	unsigned int mode;
	unsigned int initialized;
	unsigned long freq;
	int allocated;
};

#endif /* _IPQ40XX_SPI_H_ */
