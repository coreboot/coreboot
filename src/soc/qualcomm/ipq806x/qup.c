/*
 * This file is part of the depthcharge project.
 *
 * Copyright (C) 2014 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <arch/io.h>
#include <libpayload.h>
#include "ipq806x_qup.h"

//TODO: refactor the following array to iomap driver.
static unsigned gsbi_qup_base[] = {
	0x12460000, /*gsbi 1*/
	0x124A0000, /*gsbi 2*/
	0x16280000, /*gsbi 3*/
	0x16380000, /*gsbi 4*/
	0x1A280000, /*gsbi 5*/
	0x16580000, /*gsbi 6*/
	0x16680000, /*gsbi 7*/
};

#define QUP_ADDR(gsbi_num, reg)	((void *)((gsbi_qup_base[gsbi_num-1]) + (reg)))
#define MAX_DELAY_MS	100

static char *get_error_string(qup_return_t error)
{
	char *msg;
	switch (error) {
	case QUP_ERR_BAD_PARAM:
		msg = "bad parameter";
		break;
	case QUP_ERR_STATE_SET:
		msg = "setting state failed";
		break;
	case QUP_ERR_TIMEOUT:
		msg = "timeout";
		break;
	case QUP_ERR_UNSUPPORTED:
		msg = "unsupported";
		break;
	case QUP_ERR_I2C_INVALID_SLAVE_ADDR:
		msg = "invalid slave address";
		break;
	case QUP_ERR_XFER_FAIL:
		msg = "transfer failed";
		break;
	case QUP_ERR_UNDEFINED:
	default:
		msg = "undefined";
		break;
	}
	return msg;
}

static qup_return_t qup_i2c_master_status(gsbi_id_t gsbi_id)
{
	qup_return_t ret = QUP_SUCCESS;
	uint32_t reg_val = readl(QUP_ADDR(gsbi_id, QUP_I2C_MASTER_STATUS));

	if (readl(QUP_ADDR(gsbi_id, QUP_ERROR_FLAGS)))
		ret = QUP_ERR_XFER_FAIL;
	else if (reg_val & QUP_I2C_INVALID_READ_ADDR)
		ret = QUP_ERR_I2C_INVALID_SLAVE_ADDR;
	else if (reg_val & (QUP_I2C_FAILED_MASK |
			    QUP_I2C_ARB_LOST |
			    QUP_I2C_BUS_ERROR))
		ret = QUP_ERR_XFER_FAIL;

	return ret;
}

static qup_return_t qup_wait_for_state(gsbi_id_t gsbi_id, unsigned wait_for)
{
	qup_return_t ret = QUP_ERR_STATE_SET;
	uint32_t val = 0;
	uint32_t start_ts;
	uint32_t d = MAX_DELAY_MS * timer_hz() / 1000;
	uint8_t final_state = 0;

	start_ts = timer_raw_value();
	do {
		val = readl(QUP_ADDR(gsbi_id, QUP_STATE));
		final_state = ((val & (QUP_STATE_VALID_MASK|QUP_STATE_MASK))
						 == (QUP_STATE_VALID|wait_for));
	} while ((!final_state) && (start_ts > (timer_raw_value() - d)));

	if (final_state)
		ret = QUP_SUCCESS;

	return ret;
}

static qup_return_t qup_i2c_write(gsbi_id_t gsbi_id, uint8_t mode,
				  qup_data_t *p_tx_obj, uint8_t stop_seq)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	uint32_t start_ts;
	uint32_t d = MAX_DELAY_MS * timer_hz() / 1000;

	switch (mode) {
	case QUP_MODE_FIFO: {
		uint8_t addr = p_tx_obj->p.iic.addr;
		uint8_t *data_ptr = p_tx_obj->p.iic.data;
		unsigned data_len = p_tx_obj->p.iic.data_len;
		unsigned idx = 0;

		writel(0x7C, QUP_ADDR(gsbi_id, QUP_ERROR_FLAGS));
		writel(0x7C, QUP_ADDR(gsbi_id, QUP_ERROR_FLAGS_EN));
		qup_reset_i2c_master_status(gsbi_id);
		qup_set_state(gsbi_id, QUP_STATE_RUN);

		writel((QUP_I2C_START_SEQ | QUP_I2C_ADDR(addr)),
			QUP_ADDR(gsbi_id, QUP_OUTPUT_FIFO));

		while (data_len) {
			if (data_len == 1 && stop_seq) {
				writel((QUP_I2C_STOP_SEQ |
					QUP_I2C_DATA(data_ptr[idx])),
					QUP_ADDR(gsbi_id,
						 QUP_OUTPUT_FIFO));
			} else {
				writel((QUP_I2C_DATA_SEQ |
					QUP_I2C_DATA(data_ptr[idx])),
					QUP_ADDR(gsbi_id,
						 QUP_OUTPUT_FIFO));
			}
			data_len--;
			idx++;
			start_ts = timer_raw_value();
			while (data_len && readl(QUP_ADDR(gsbi_id,
						  QUP_OPERATIONAL)) &
						  OUTPUT_FIFO_FULL) {
				ret = qup_i2c_master_status(gsbi_id);
				if (QUP_SUCCESS != ret)
					goto bailout;
				if (start_ts < (timer_raw_value() - d)) {
					ret = QUP_ERR_TIMEOUT;
					goto bailout;
				}
			}
			/* Hardware sets the OUTPUT_SERVICE_FLAG flag to 1 when
			OUTPUT_FIFO_NOT_EMPTY flag in the QUP_OPERATIONAL
			register changes from 1 to 0, indicating that software
			can write more data to the output FIFO. Software should
			set OUTPUT_SERVICE_FLAG to 1 to clear it to 0, which
			means that software knows to return to fill the output
			FIFO with data.
			*/
			if (readl(QUP_ADDR(gsbi_id, QUP_OPERATIONAL)) &
					      OUTPUT_SERVICE_FLAG) {
				writel(OUTPUT_SERVICE_FLAG,
				       QUP_ADDR(gsbi_id,
				       QUP_OPERATIONAL));
			}
		}

		start_ts = timer_raw_value();
		while (((readl(QUP_ADDR(gsbi_id, QUP_OPERATIONAL))) &
					 OUTPUT_FIFO_NOT_EMPTY)) {
			ret = qup_i2c_master_status(gsbi_id);
			if (QUP_SUCCESS != ret)
				goto bailout;
			if (start_ts < (timer_raw_value() - d)) {
				ret = QUP_ERR_TIMEOUT;
				goto bailout;
			}
		}

		qup_set_state(gsbi_id, QUP_STATE_PAUSE);
		ret = QUP_SUCCESS;
	}
	break;

	default:
		ret = QUP_ERR_UNSUPPORTED;
	}

bailout:
	if (QUP_SUCCESS != ret) {
		qup_set_state(gsbi_id, QUP_STATE_RESET);
		printf("%s() returns %s\n", __func__, get_error_string(ret));
	}

	return ret;
}

static qup_return_t qup_i2c_read(gsbi_id_t gsbi_id, uint8_t mode,
				  qup_data_t *p_tx_obj)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	uint32_t start_ts;
	uint32_t d = MAX_DELAY_MS * timer_hz() / 1000;

	switch (mode) {
	case QUP_MODE_FIFO: {
		uint8_t addr = p_tx_obj->p.iic.addr;
		uint8_t *data_ptr = p_tx_obj->p.iic.data;
		unsigned data_len = p_tx_obj->p.iic.data_len;
		unsigned idx = 0;

		writel(0x7C, QUP_ADDR(gsbi_id, QUP_ERROR_FLAGS));
		writel(0x7C, QUP_ADDR(gsbi_id, QUP_ERROR_FLAGS_EN));
		qup_reset_i2c_master_status(gsbi_id);
		qup_set_state(gsbi_id, QUP_STATE_RUN);

		writel((QUP_I2C_START_SEQ |
		       (QUP_I2C_ADDR(addr)|
			QUP_I2C_SLAVE_READ)),
			QUP_ADDR(gsbi_id, QUP_OUTPUT_FIFO));

		writel((QUP_I2C_RECV_SEQ | data_len),
			QUP_ADDR(gsbi_id, QUP_OUTPUT_FIFO));

		start_ts = timer_raw_value();
		while ((readl(QUP_ADDR(gsbi_id, QUP_OPERATIONAL)) &
				 OUTPUT_FIFO_NOT_EMPTY)) {
			ret = qup_i2c_master_status(gsbi_id);
			if (QUP_SUCCESS != ret)
				goto bailout;
			if (start_ts < (timer_raw_value() - d)) {
				ret = QUP_ERR_TIMEOUT;
				goto bailout;
			}
		}

		writel(OUTPUT_SERVICE_FLAG,
		       QUP_ADDR(gsbi_id, QUP_OPERATIONAL));

		while (data_len) {
			unsigned data;
			start_ts = timer_raw_value();
			while ((!((readl(QUP_ADDR(gsbi_id, QUP_OPERATIONAL))) &
				   INPUT_SERVICE_FLAG))) {
				ret = qup_i2c_master_status(gsbi_id);
				if (QUP_SUCCESS != ret)
					goto bailout;
				if (start_ts < (timer_raw_value() - d)) {
					ret = QUP_ERR_TIMEOUT;
					goto bailout;
				}
			}

			data = readl(QUP_ADDR(gsbi_id, QUP_INPUT_FIFO));

			/*Process tag and corresponding data value.
			For I2C master mini-core, data in FIFO is composed of
			16 bits and is divided into an 8-bit tag for the upper
			bits and 8-bit data for the lower bits.
			The 8-bit tag indicates whether the byte is the last
			byte, or if a bus error happened during the receipt of
			the byte.*/
			if ((QUP_I2C_MI_TAG(data)) == QUP_I2C_MIDATA_SEQ) {
				/* Tag: MIDATA = Master input data.*/
				data_ptr[idx] = QUP_I2C_DATA(data);
				idx++;
				data_len--;
				writel(INPUT_SERVICE_FLAG, QUP_ADDR(gsbi_id,
				       QUP_OPERATIONAL));
			} else if (QUP_I2C_MI_TAG(data) ==
							QUP_I2C_MISTOP_SEQ) {
				/* Tag: MISTOP: Last byte of master input. */
				data_ptr[idx] = QUP_I2C_DATA(data);
				idx++;
				data_len--;
				goto recv_done;
			} else {
				/* Tag: MINACK: Invalid master input data.*/
				goto recv_done;
			}
		}
recv_done:
		writel(INPUT_SERVICE_FLAG,
		       QUP_ADDR(gsbi_id, QUP_OPERATIONAL));
		p_tx_obj->p.iic.data_len = idx;
		qup_set_state(gsbi_id, QUP_STATE_PAUSE);
		ret = QUP_SUCCESS;
	}
	break;

	default:
		ret = QUP_ERR_UNSUPPORTED;
	}

bailout:
	if (QUP_SUCCESS != ret) {
		qup_set_state(gsbi_id, QUP_STATE_RESET);
		printf("%s() returns %s\n", __func__, get_error_string(ret));
	}

	return ret;
}

qup_return_t qup_init(gsbi_id_t gsbi_id, qup_config_t *config_ptr)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	uint32_t reg_val;

	/* Reset the QUP core.*/
	writel(0x1, QUP_ADDR(gsbi_id, QUP_SW_RESET));

	/*Wait till the reset takes effect */
	ret = qup_wait_for_state(gsbi_id, QUP_STATE_RESET);

	if (QUP_SUCCESS != ret)
		return ret;

	/*Reset the config*/
	writel(0, QUP_ADDR(gsbi_id, QUP_CONFIG));

	/*Program the config register*/
	/*Set N value*/
	reg_val = 0x0F;
	/*Set protocol*/
	switch (config_ptr->protocol) {
	case QUP_MINICORE_I2C_MASTER: {
		reg_val |= ((config_ptr->protocol &
				QUP_MINI_CORE_PROTO_MASK) <<
				QUP_MINI_CORE_PROTO_SHFT);
		}
		break;
	default: {
		ret = QUP_ERR_UNSUPPORTED;
		goto bailout;
		}
	}
	writel(reg_val, QUP_ADDR(gsbi_id, QUP_CONFIG));

	/*Reset i2c clk cntl register*/
	writel(0, QUP_ADDR(gsbi_id, QUP_I2C_MASTER_CLK_CTL));

	/*Set QUP IO Mode*/
	switch (config_ptr->mode) {
	case QUP_MODE_FIFO: {
		reg_val = QUP_OUTPUT_BIT_SHIFT_EN |
			  ((config_ptr->mode & QUP_MODE_MASK) <<
					QUP_OUTPUT_MODE_SHFT) |
			  ((config_ptr->mode & QUP_MODE_MASK) <<
					QUP_INPUT_MODE_SHFT);
		}
		break;
	default: {
		ret = QUP_ERR_UNSUPPORTED;
		goto bailout;
		}
	}
	writel(reg_val, QUP_ADDR(gsbi_id, QUP_IO_MODES));

	/*Set i2c clk cntl*/
	reg_val = (QUP_DIVIDER_MIN_VAL << QUP_HS_DIVIDER_SHFT);
	reg_val |= ((((config_ptr->src_frequency / config_ptr->clk_frequency)
			/ 2) - QUP_DIVIDER_MIN_VAL) &
				QUP_FS_DIVIDER_MASK);
	writel(reg_val, QUP_ADDR(gsbi_id, QUP_I2C_MASTER_CLK_CTL));

bailout:
	if (QUP_SUCCESS != ret)
		printf("%s() returns %s\n", __func__, get_error_string(ret));

	return ret;
}

qup_return_t qup_set_state(gsbi_id_t gsbi_id, uint32_t state)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	unsigned curr_state = readl(QUP_ADDR(gsbi_id, QUP_STATE));

	if ((state >= QUP_STATE_RESET && state <= QUP_STATE_PAUSE)
		&& (curr_state & QUP_STATE_VALID_MASK)) {
		/*
		* For PAUSE_STATE to RESET_STATE transition,
		* two writes of  10[binary]) are required for the
		* transition to complete.
		*/
		if (QUP_STATE_PAUSE == curr_state &&
		    QUP_STATE_RESET == state) {
			writel(0x2, QUP_ADDR(gsbi_id, QUP_STATE));
			writel(0x2, QUP_ADDR(gsbi_id, QUP_STATE));
		} else {
			writel(state, QUP_ADDR(gsbi_id, QUP_STATE));
		}
		ret = qup_wait_for_state(gsbi_id, state);
	}
	return ret;
}

qup_return_t qup_reset_i2c_master_status(gsbi_id_t gsbi_id)
{
	/*
	 * Writing a one clears the status bits.
	 * Bit31-25, Bit1 and Bit0 are reserved.
	 */
	//TODO: Define each status bit. OR all status bits in a single macro.
	writel(0x3FFFFFC, QUP_ADDR(gsbi_id, QUP_I2C_MASTER_STATUS));
	return QUP_SUCCESS;
}

qup_return_t qup_send_data(gsbi_id_t gsbi_id, qup_data_t *p_tx_obj,
			   uint8_t stop_seq)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;

	if (p_tx_obj->protocol == ((readl(QUP_ADDR(gsbi_id, QUP_CONFIG)) >>
						    QUP_MINI_CORE_PROTO_SHFT) &
						    QUP_MINI_CORE_PROTO_MASK)) {
		switch (p_tx_obj->protocol) {
		case QUP_MINICORE_I2C_MASTER: {
			uint8_t mode = (readl(QUP_ADDR(gsbi_id,
						       QUP_IO_MODES)) >>
					      QUP_OUTPUT_MODE_SHFT) &
					QUP_MODE_MASK;
			ret = qup_i2c_write(gsbi_id, mode, p_tx_obj, stop_seq);
			if (0) {
				int i;
				printf("i2c tx bus %d device %2.2x:",
					gsbi_id, p_tx_obj->p.iic.addr);
				for (i = 0; i < p_tx_obj->p.iic.data_len; i++)
					printf(" %2.2x",
						p_tx_obj->p.iic.data[i]);
				printf("\n");
			}
			break;
		}

		default:
			ret = QUP_ERR_UNSUPPORTED;
		}
	}
	return ret;
}

qup_return_t qup_recv_data(gsbi_id_t gsbi_id, qup_data_t *p_rx_obj)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	if (p_rx_obj->protocol == ((readl(QUP_ADDR(gsbi_id, QUP_CONFIG)) >>
						QUP_MINI_CORE_PROTO_SHFT) &
						QUP_MINI_CORE_PROTO_MASK)) {
		switch (p_rx_obj->protocol) {
		case QUP_MINICORE_I2C_MASTER: {
			uint8_t mode = (readl(QUP_ADDR(gsbi_id,
						       QUP_IO_MODES)) >>
						QUP_INPUT_MODE_SHFT) &
						QUP_MODE_MASK;
			ret = qup_i2c_read(gsbi_id, mode, p_rx_obj);
			if (0) {
				int i;
				printf("i2c rxed on bus %d device %2.2x:",
					gsbi_id, p_rx_obj->p.iic.addr);
				for (i = 0; i < p_rx_obj->p.iic.data_len; i++)
					printf(" %2.2x",
						p_rx_obj->p.iic.data[i]);
				printf("\n");
			}
			break;
		}
		default:
			ret = QUP_ERR_UNSUPPORTED;
		}
	}
	return ret;
}
