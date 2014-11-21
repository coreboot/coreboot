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

#include <assert.h>
#include <libpayload.h>

#include "base/container_of.h"
#include "drivers/bus/i2c/i2c.h"
#include "drivers/bus/i2c/ipq806x_qup.h"
#include "drivers/bus/i2c/ipq806x_gsbi.h"
#include "drivers/bus/i2c/ipq806x.h"

static int i2c_init(unsigned gsbi_id)
{
	gsbi_return_t gsbi_ret = 0;
	qup_return_t qup_ret = 0;
	qup_config_t gsbi4_qup_config = {
		QUP_MINICORE_I2C_MASTER,
		100000,
		24000000,
		QUP_MODE_FIFO
	};

	gsbi_ret = gsbi_init(gsbi_id, GSBI_PROTO_I2C_ONLY);
	if (GSBI_SUCCESS != gsbi_ret)
		return 1;

	qup_ret = qup_init(gsbi_id, &gsbi4_qup_config);
	if (QUP_SUCCESS != qup_ret)
		return 1;

	qup_ret = qup_reset_i2c_master_status(gsbi_id);
	if (QUP_SUCCESS != qup_ret)
		return 1;

	return 0;
}

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

static int i2c_transfer(struct I2cOps *me, I2cSeg *segments, int seg_count)
{
	Ipq806xI2c *bus = container_of(me, Ipq806xI2c, ops);
	I2cSeg *seg = segments;
	int ret = 0;

	if (!bus->initialized)
		if (0 != i2c_init(bus->gsbi_id))
			return 1;

	while (seg_count--) {
		if (seg->read)
			ret = i2c_read(bus->gsbi_id, seg->chip,
				       seg->buf, seg->len);
		else
			ret  = i2c_write(bus->gsbi_id, seg->chip,
					 seg->buf, seg->len,
					 (seg_count ? 0 : 1));
		seg++;
	}

	if (QUP_SUCCESS != ret) {
		qup_set_state(bus->gsbi_id, QUP_STATE_RESET);
		return 1;
	}

	return 0;
}

Ipq806xI2c *new_ipq806x_i2c(unsigned gsbi_id)
{
	Ipq806xI2c *bus = 0;

	if (!i2c_init(gsbi_id)) {
		bus = xzalloc(sizeof(*bus));
		bus->gsbi_id = gsbi_id;
		bus->initialized = 1;
		bus->ops.transfer = &i2c_transfer;
	}
	return bus;
}
