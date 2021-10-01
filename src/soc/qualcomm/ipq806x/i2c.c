/* SPDX-License-Identifier: BSD-3-Clause */

#include <console/console.h>
#include <device/i2c_simple.h>
#include <string.h>
#include <soc/gsbi.h>
#include <soc/qup.h>

static qup_config_t gsbi1_qup_config = {
	QUP_MINICORE_I2C_MASTER,
	100000,
	24000000,
	QUP_MODE_FIFO,
	0
};

static qup_config_t gsbi4_qup_config = {
	QUP_MINICORE_I2C_MASTER,
	100000,
	24000000,
	QUP_MODE_FIFO,
	0
};

static qup_config_t gsbi7_qup_config = {
	QUP_MINICORE_I2C_MASTER,
	100000,
	24000000,
	QUP_MODE_FIFO,
	0
};

static int i2c_read(uint32_t gsbi_id, uint8_t slave,
		    uint8_t *data, int data_len)
{
	qup_data_t obj;
	qup_return_t qup_ret = 0;

	memset(&obj, 0, sizeof(obj));
	obj.protocol = QUP_MINICORE_I2C_MASTER;
	obj.p.iic.addr = slave;
	obj.p.iic.data_len = data_len;
	obj.p.iic.data = data;
	qup_ret = qup_recv_data(gsbi_id, &obj);

	if (QUP_SUCCESS != qup_ret)
		return 1;
	else
		return 0;
}

static int i2c_write(uint32_t gsbi_id, uint8_t slave,
		     uint8_t *data, int data_len, uint8_t stop_seq)
{
	qup_data_t obj;
	qup_return_t qup_ret = 0;

	memset(&obj, 0, sizeof(obj));
	obj.protocol = QUP_MINICORE_I2C_MASTER;
	obj.p.iic.addr = slave;
	obj.p.iic.data_len = data_len;
	obj.p.iic.data = data;
	qup_ret = qup_send_data(gsbi_id, &obj, stop_seq);

	if (QUP_SUCCESS != qup_ret)
		return 1;
	else
		return 0;
}

static int i2c_init(unsigned int bus)
{
	unsigned int gsbi_id = bus;
	qup_config_t *qup_config;

	switch (gsbi_id) {
	case GSBI_ID_1:
		qup_config = &gsbi1_qup_config;
		break;
	case GSBI_ID_4:
		qup_config = &gsbi4_qup_config;
		break;
	case GSBI_ID_7:
		qup_config = &gsbi7_qup_config;
		break;
	default:
		printk(BIOS_ERR, "QUP configuration not defined for GSBI%d.\n",
		       gsbi_id);
		return 1;
	}

	if (qup_config->initialized)
		return 0;

	if (gsbi_init(gsbi_id, GSBI_PROTO_I2C_ONLY)) {
		printk(BIOS_ERR, "failed to initialize gsbi\n");
		return 1;
	}

	if (qup_init(gsbi_id, qup_config)) {
		printk(BIOS_ERR, "failed to initialize qup\n");
		return 1;
	}

	if (qup_reset_i2c_master_status(gsbi_id)) {
		printk(BIOS_ERR, "failed to reset i2c master status\n");
		return 1;
	}

	qup_config->initialized = 1;
	return 0;
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			  int seg_count)
{
	struct i2c_msg *seg = segments;
	int ret = 0;

	if (i2c_init(bus))
		return 1;

	while (!ret && seg_count--) {
		if (seg->flags & I2C_M_RD)
			ret = i2c_read(bus, seg->slave, seg->buf, seg->len);
		else
			ret = i2c_write(bus, seg->slave, seg->buf, seg->len,
					(seg_count ? 0 : 1));
		seg++;
	}

	if (ret) {
		qup_set_state(bus, QUP_STATE_RESET);
		return 1;
	}

	return 0;
}
