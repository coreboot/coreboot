/* SPDX-License-Identifier: BSD-3-Clause */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <timer.h>
#include <soc/iomap.h>
#include <soc/qup.h>

#define TIMEOUT_CNT	100000

//TODO: refactor the following array to iomap driver.
static unsigned int gsbi_qup_base[] = {
	(unsigned int)GSBI_QUP1_BASE,
	(unsigned int)GSBI_QUP2_BASE,
	(unsigned int)GSBI_QUP3_BASE,
	(unsigned int)GSBI_QUP4_BASE,
	(unsigned int)GSBI_QUP5_BASE,
	(unsigned int)GSBI_QUP6_BASE,
	(unsigned int)GSBI_QUP7_BASE,
};

#define QUP_ADDR(gsbi_num, reg)	((void *)((gsbi_qup_base[gsbi_num-1]) + (reg)))

static qup_return_t qup_i2c_master_status(gsbi_id_t gsbi_id)
{
	uint32_t reg_val = read32(QUP_ADDR(gsbi_id, QUP_I2C_MASTER_STATUS));

	if (read32(QUP_ADDR(gsbi_id, QUP_ERROR_FLAGS)))
		return QUP_ERR_XFER_FAIL;
	if (reg_val & QUP_I2C_INVALID_READ_ADDR)
		return QUP_ERR_I2C_INVALID_SLAVE_ADDR;
	if (reg_val & QUP_I2C_FAILED_MASK)
		return QUP_ERR_I2C_FAILED;
	if (reg_val & QUP_I2C_ARB_LOST)
		return QUP_ERR_I2C_ARB_LOST;
	if (reg_val & QUP_I2C_BUS_ERROR)
		return QUP_ERR_I2C_BUS_ERROR;
	if (reg_val & QUP_I2C_INVALID_WRITE)
		return QUP_ERR_I2C_INVALID_WRITE;
	if (reg_val & QUP_I2C_PACKET_NACK)
		return QUP_ERR_I2C_NACK;
	if (reg_val & QUP_I2C_INVALID_TAG)
		return QUP_ERR_I2C_INVALID_TAG;

	return QUP_SUCCESS;
}

static int check_bit_state(uint32_t *reg, int wait_for)
{
	unsigned int count = TIMEOUT_CNT;

	while ((read32(reg) & (QUP_STATE_VALID_MASK | QUP_STATE_MASK)) !=
			(QUP_STATE_VALID | wait_for)) {
		if (count == 0)
			return QUP_ERR_TIMEOUT;
		count--;
		udelay(1);
	}

	return QUP_SUCCESS;
}

/*
 * Check whether GSBIn_QUP State is valid
 */
static qup_return_t qup_wait_for_state(gsbi_id_t gsbi_id, unsigned int wait_for)
{
	return check_bit_state(QUP_ADDR(gsbi_id, QUP_STATE), wait_for);
}

qup_return_t qup_reset_i2c_master_status(gsbi_id_t gsbi_id)
{
	/*
	 * Writing a one clears the status bits.
	 * Bit31-25, Bit1 and Bit0 are reserved.
	 */
	//TODO: Define each status bit. OR all status bits in a single macro.
	write32(QUP_ADDR(gsbi_id, QUP_I2C_MASTER_STATUS), 0x3FFFFFC);
	return QUP_SUCCESS;
}

static qup_return_t qup_reset_master_status(gsbi_id_t gsbi_id)
{
	write32(QUP_ADDR(gsbi_id, QUP_ERROR_FLAGS), 0x7C);
	write32(QUP_ADDR(gsbi_id, QUP_ERROR_FLAGS_EN), 0x7C);
	qup_reset_i2c_master_status(gsbi_id);
	return QUP_SUCCESS;
}

static qup_return_t qup_fifo_wait_for(gsbi_id_t gsbi_id, uint32_t status,
				      struct stopwatch *timeout)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;

	while (!(read32(QUP_ADDR(gsbi_id, QUP_OPERATIONAL)) & status)) {
		ret = qup_i2c_master_status(gsbi_id);
		if (ret)
			return ret;
		if (stopwatch_expired(timeout))
			return QUP_ERR_TIMEOUT;
	}

	return QUP_SUCCESS;
}

static qup_return_t qup_fifo_wait_while(gsbi_id_t gsbi_id, uint32_t status,
					struct stopwatch *timeout)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;

	while (read32(QUP_ADDR(gsbi_id, QUP_OPERATIONAL)) & status) {
		ret = qup_i2c_master_status(gsbi_id);
		if (ret)
			return ret;
		if (stopwatch_expired(timeout))
			return QUP_ERR_TIMEOUT;
	}

	return QUP_SUCCESS;
}

static qup_return_t qup_i2c_write_fifo(gsbi_id_t gsbi_id, qup_data_t *p_tx_obj,
				       uint8_t stop_seq)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	uint8_t addr = p_tx_obj->p.iic.addr;
	uint8_t *data_ptr = p_tx_obj->p.iic.data;
	unsigned int data_len = p_tx_obj->p.iic.data_len;
	unsigned int idx = 0;
	struct stopwatch timeout;

	qup_reset_master_status(gsbi_id);
	qup_set_state(gsbi_id, QUP_STATE_RUN);

	write32(QUP_ADDR(gsbi_id, QUP_OUTPUT_FIFO),
		(QUP_I2C_START_SEQ | QUP_I2C_ADDR(addr)));

	stopwatch_init_usecs_expire(&timeout, CONFIG_I2C_TRANSFER_TIMEOUT_US);
	while (data_len) {
		if (data_len == 1 && stop_seq) {
			write32(QUP_ADDR(gsbi_id, QUP_OUTPUT_FIFO),
				QUP_I2C_STOP_SEQ | QUP_I2C_DATA(data_ptr[idx]));
		} else {
			write32(QUP_ADDR(gsbi_id, QUP_OUTPUT_FIFO),
				QUP_I2C_DATA_SEQ | QUP_I2C_DATA(data_ptr[idx]));
		}
		data_len--;
		idx++;
		if (data_len) {
			ret = qup_fifo_wait_while(gsbi_id, OUTPUT_FIFO_FULL,
						  &timeout);
			if (ret)
				return ret;
		}
		/* Hardware sets the OUTPUT_SERVICE_FLAG flag to 1 when
			OUTPUT_FIFO_NOT_EMPTY flag in the QUP_OPERATIONAL
			register changes from 1 to 0, indicating that software
			can write more data to the output FIFO. Software should
			set OUTPUT_SERVICE_FLAG to 1 to clear it to 0, which
			means that software knows to return to fill the output
			FIFO with data.
		 */
		if (read32(QUP_ADDR(gsbi_id, QUP_OPERATIONAL)) &
				OUTPUT_SERVICE_FLAG) {
			write32(QUP_ADDR(gsbi_id, QUP_OPERATIONAL),
				OUTPUT_SERVICE_FLAG);
		}
	}

	ret = qup_fifo_wait_while(gsbi_id, OUTPUT_FIFO_NOT_EMPTY, &timeout);
	if (ret)
		return ret;

	qup_set_state(gsbi_id, QUP_STATE_PAUSE);
	return qup_i2c_master_status(gsbi_id);
}

static qup_return_t qup_i2c_write(gsbi_id_t gsbi_id, uint8_t mode,
				  qup_data_t *p_tx_obj, uint8_t stop_seq)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;

	switch (mode) {
	case QUP_MODE_FIFO:
		ret = qup_i2c_write_fifo(gsbi_id, p_tx_obj, stop_seq);
		break;
	default:
		ret = QUP_ERR_UNSUPPORTED;
	}

	if (ret) {
		qup_set_state(gsbi_id, QUP_STATE_RESET);
		printk(BIOS_ERR, "%s() failed (%d)\n", __func__, ret);
	}

	return ret;
}

static qup_return_t qup_i2c_read_fifo(gsbi_id_t gsbi_id, qup_data_t *p_tx_obj)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	uint8_t addr = p_tx_obj->p.iic.addr;
	uint8_t *data_ptr = p_tx_obj->p.iic.data;
	unsigned int data_len = p_tx_obj->p.iic.data_len;
	unsigned int idx = 0;
	struct stopwatch timeout;

	qup_reset_master_status(gsbi_id);
	qup_set_state(gsbi_id, QUP_STATE_RUN);

	write32(QUP_ADDR(gsbi_id, QUP_OUTPUT_FIFO),
		QUP_I2C_START_SEQ | (QUP_I2C_ADDR(addr) | QUP_I2C_SLAVE_READ));

	write32(QUP_ADDR(gsbi_id, QUP_OUTPUT_FIFO),
		QUP_I2C_RECV_SEQ | data_len);

	stopwatch_init_usecs_expire(&timeout, CONFIG_I2C_TRANSFER_TIMEOUT_US);
	ret = qup_fifo_wait_while(gsbi_id, OUTPUT_FIFO_NOT_EMPTY, &timeout);
	if (ret)
		return ret;

	write32(QUP_ADDR(gsbi_id, QUP_OPERATIONAL), OUTPUT_SERVICE_FLAG);

	while (data_len) {
		uint32_t data;

		ret = qup_fifo_wait_for(gsbi_id, INPUT_SERVICE_FLAG, &timeout);
		if (ret)
			return ret;

		data = read32(QUP_ADDR(gsbi_id, QUP_INPUT_FIFO));

		/*
		 * Process tag and corresponding data value. For I2C master
		 * mini-core, data in FIFO is composed of 16 bits and is divided
		 * into an 8-bit tag for the upper bits and 8-bit data for the
		 * lower bits. The 8-bit tag indicates whether the byte is the
		 * last byte, or if a bus error happened during the receipt of
		 * the byte.
		 */
		if ((QUP_I2C_MI_TAG(data)) == QUP_I2C_MIDATA_SEQ) {
			/* Tag: MIDATA = Master input data.*/
			data_ptr[idx] = QUP_I2C_DATA(data);
			idx++;
			data_len--;
			write32(QUP_ADDR(gsbi_id, QUP_OPERATIONAL),
				INPUT_SERVICE_FLAG);
		} else if (QUP_I2C_MI_TAG(data) == QUP_I2C_MISTOP_SEQ) {
			/* Tag: MISTOP: Last byte of master input. */
			data_ptr[idx] = QUP_I2C_DATA(data);
			idx++;
			data_len--;
			break;
		} else {
			/* Tag: MINACK: Invalid master input data.*/
			break;
		}
	}

	write32(QUP_ADDR(gsbi_id, QUP_OPERATIONAL), INPUT_SERVICE_FLAG);
	p_tx_obj->p.iic.data_len = idx;
	qup_set_state(gsbi_id, QUP_STATE_PAUSE);

	return QUP_SUCCESS;
}

static qup_return_t qup_i2c_read(gsbi_id_t gsbi_id, uint8_t mode,
				 qup_data_t *p_tx_obj)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;

	switch (mode) {
	case QUP_MODE_FIFO:
		ret = qup_i2c_read_fifo(gsbi_id, p_tx_obj);
		break;
	default:
		ret = QUP_ERR_UNSUPPORTED;
	}

	if (ret) {
		qup_set_state(gsbi_id, QUP_STATE_RESET);
		printk(BIOS_ERR, "%s() failed (%d)\n", __func__, ret);
	}

	return ret;
}

qup_return_t qup_init(gsbi_id_t gsbi_id, const qup_config_t *config_ptr)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	uint32_t reg_val;

	/* Reset the QUP core.*/
	write32(QUP_ADDR(gsbi_id, QUP_SW_RESET), 0x1);

	/*Wait till the reset takes effect */
	ret = qup_wait_for_state(gsbi_id, QUP_STATE_RESET);
	if (ret)
		goto bailout;

	/* Reset the config */
	write32(QUP_ADDR(gsbi_id, QUP_CONFIG), 0);

	/*Program the config register*/
	/*Set N value*/
	reg_val = 0x0F;
	/*Set protocol*/
	switch (config_ptr->protocol) {
	case QUP_MINICORE_I2C_MASTER:
		reg_val |= ((config_ptr->protocol &
				QUP_MINI_CORE_PROTO_MASK) <<
				QUP_MINI_CORE_PROTO_SHFT);
		break;
	default:
		ret = QUP_ERR_UNSUPPORTED;
		goto bailout;
	}
	write32(QUP_ADDR(gsbi_id, QUP_CONFIG), reg_val);

	/*Reset i2c clk cntl register*/
	write32(QUP_ADDR(gsbi_id, QUP_I2C_MASTER_CLK_CTL), 0);

	/*Set QUP IO Mode*/
	switch (config_ptr->mode) {
	case QUP_MODE_FIFO:
		reg_val = QUP_OUTPUT_BIT_SHIFT_EN |
			  ((config_ptr->mode & QUP_MODE_MASK) <<
					QUP_OUTPUT_MODE_SHFT) |
			  ((config_ptr->mode & QUP_MODE_MASK) <<
					QUP_INPUT_MODE_SHFT);
		break;
	default:
		ret = QUP_ERR_UNSUPPORTED;
		goto bailout;
	}
	write32(QUP_ADDR(gsbi_id, QUP_IO_MODES), reg_val);

	/*Set i2c clk cntl*/
	reg_val = (QUP_DIVIDER_MIN_VAL << QUP_HS_DIVIDER_SHFT);
	reg_val |= ((((config_ptr->src_frequency / config_ptr->clk_frequency)
			/ 2) - QUP_DIVIDER_MIN_VAL) &
				QUP_FS_DIVIDER_MASK);
	write32(QUP_ADDR(gsbi_id, QUP_I2C_MASTER_CLK_CTL), reg_val);

bailout:
	if (ret)
		printk(BIOS_ERR, "failed to init qup (%d)\n", ret);

	return ret;
}

qup_return_t qup_set_state(gsbi_id_t gsbi_id, uint32_t state)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	unsigned int curr_state = read32(QUP_ADDR(gsbi_id, QUP_STATE));

	if (state <= QUP_STATE_PAUSE && (curr_state & QUP_STATE_VALID_MASK)) {
		/*
		* For PAUSE_STATE to RESET_STATE transition,
		* two writes of  10[binary]) are required for the
		* transition to complete.
		*/
		if (QUP_STATE_PAUSE == curr_state && QUP_STATE_RESET == state) {
			write32(QUP_ADDR(gsbi_id, QUP_STATE), 0x2);
			write32(QUP_ADDR(gsbi_id, QUP_STATE), 0x2);
		} else {
			write32(QUP_ADDR(gsbi_id, QUP_STATE), state);
		}
		ret = qup_wait_for_state(gsbi_id, state);
	}

	return ret;
}

static qup_return_t qup_i2c_send_data(gsbi_id_t gsbi_id, qup_data_t *p_tx_obj,
				      uint8_t stop_seq)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	uint8_t mode = (read32(QUP_ADDR(gsbi_id, QUP_IO_MODES)) >>
			QUP_OUTPUT_MODE_SHFT) & QUP_MODE_MASK;

	ret = qup_i2c_write(gsbi_id, mode, p_tx_obj, stop_seq);
	if (0) {
		int i;
		printk(BIOS_DEBUG, "i2c tx bus %d device %2.2x:",
		       gsbi_id, p_tx_obj->p.iic.addr);
		for (i = 0; i < p_tx_obj->p.iic.data_len; i++)
			printk(BIOS_DEBUG, " %2.2x", p_tx_obj->p.iic.data[i]);
		printk(BIOS_DEBUG, "\n");
	}

	return ret;
}

qup_return_t qup_send_data(gsbi_id_t gsbi_id, qup_data_t *p_tx_obj,
			   uint8_t stop_seq)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;

	if (p_tx_obj->protocol == ((read32(QUP_ADDR(gsbi_id, QUP_CONFIG)) >>
			QUP_MINI_CORE_PROTO_SHFT) & QUP_MINI_CORE_PROTO_MASK)) {
		switch (p_tx_obj->protocol) {
		case QUP_MINICORE_I2C_MASTER:
			ret = qup_i2c_send_data(gsbi_id, p_tx_obj, stop_seq);
			break;
		default:
			ret = QUP_ERR_UNSUPPORTED;
		}
	}

	return ret;
}

static qup_return_t qup_i2c_recv_data(gsbi_id_t gsbi_id, qup_data_t *p_rx_obj)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;
	uint8_t mode = (read32(QUP_ADDR(gsbi_id, QUP_IO_MODES)) >>
			QUP_INPUT_MODE_SHFT) & QUP_MODE_MASK;

	ret = qup_i2c_read(gsbi_id, mode, p_rx_obj);
	if (0) {
		int i;
		printk(BIOS_DEBUG, "i2c rxed on bus %d device %2.2x:",
		       gsbi_id, p_rx_obj->p.iic.addr);
		for (i = 0; i < p_rx_obj->p.iic.data_len; i++)
			printk(BIOS_DEBUG, " %2.2x", p_rx_obj->p.iic.data[i]);
		printk(BIOS_DEBUG, "\n");
	}

	return ret;
}

qup_return_t qup_recv_data(gsbi_id_t gsbi_id, qup_data_t *p_rx_obj)
{
	qup_return_t ret = QUP_ERR_UNDEFINED;

	if (p_rx_obj->protocol == ((read32(QUP_ADDR(gsbi_id, QUP_CONFIG)) >>
			QUP_MINI_CORE_PROTO_SHFT) & QUP_MINI_CORE_PROTO_MASK)) {
		switch (p_rx_obj->protocol) {
		case QUP_MINICORE_I2C_MASTER:
			ret = qup_i2c_recv_data(gsbi_id, p_rx_obj);
			break;
		default:
			ret = QUP_ERR_UNSUPPORTED;
		}
	}

	return ret;
}
