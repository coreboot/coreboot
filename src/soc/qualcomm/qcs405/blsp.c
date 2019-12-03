/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 - 2016, 2019 The Linux Foundation. All rights reserved.
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

#include <device/mmio.h>
#include <soc/blsp.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/clock.h>

blsp_return_t blsp_i2c_init(blsp_qup_id_t id)
{
	void *base;

	switch (id) {
	case BLSP_QUP_ID_1:
		gpio_configure
			(GPIO(24), 2, GPIO_PULL_UP, GPIO_2MA, GPIO_OUTPUT);
		gpio_configure
			(GPIO(25), 2, GPIO_PULL_UP, GPIO_2MA, GPIO_OUTPUT);
		break;
	default:
		return BLSP_ID_ERROR;
	}

	clock_configure_i2c(19200000);
	clock_enable_i2c();

	base = blsp_qup_base(id);

	if (!base)
		return BLSP_ID_ERROR;

	/* Configure Mini core to I2C core */
	clrsetbits32(base, BLSP_MINI_CORE_MASK, BLSP_MINI_CORE_I2C);

	return BLSP_SUCCESS;
}
