/* SPDX-License-Identifier: BSD-3-Clause */

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
