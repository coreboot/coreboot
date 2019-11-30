/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 - 2015 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <console/console.h>
#include <device/i2c_simple.h>
#include <string.h>
#include <soc/blsp.h>
#include <soc/qup.h>
#include <soc/gpio.h>

static qup_config_t blsp1_qup0_config = {
	QUP_MINICORE_I2C_MASTER,
	100000,
	19050000,
	QUP_MODE_BLOCK,
	0
};

static qup_config_t blsp1_qup1_config = {
	QUP_MINICORE_I2C_MASTER,
	100000,
	19050000,
	QUP_MODE_BLOCK,
	0
};

static qup_config_t blsp1_qup2_config = {
	QUP_MINICORE_I2C_MASTER,
	100000,
	19050000,
	QUP_MODE_BLOCK,
	0
};

static qup_config_t blsp1_qup3_config = {
	QUP_MINICORE_I2C_MASTER,
	100000,
	19050000,
	QUP_MODE_BLOCK,
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

static int i2c_init(blsp_qup_id_t id)
{
	qup_config_t *qup_config;

	switch (id) {
	case BLSP_QUP_ID_0:
		qup_config = &blsp1_qup0_config;
		break;
	case BLSP_QUP_ID_1:
		qup_config = &blsp1_qup1_config;
		break;
	case BLSP_QUP_ID_2:
		qup_config = &blsp1_qup2_config;
		break;
	case BLSP_QUP_ID_3:
		qup_config = &blsp1_qup3_config;
		break;
	default:
		printk(BIOS_ERR, "QUP configuration not defined for BLSP%d.\n",
		       id);
		return 1;
	}

	if (qup_config->initialized)
		return 0;

	if (blsp_i2c_init(id)) {
		printk(BIOS_ERR, "failed to initialize blsp\n");
		return 1;
	}

	if (qup_init(id, qup_config)) {
		printk(BIOS_ERR, "failed to initialize qup\n");
		return 1;
	}

	if (qup_reset_i2c_master_status(id)) {
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
