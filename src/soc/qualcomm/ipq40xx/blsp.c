/* SPDX-License-Identifier: BSD-3-Clause */

#include <device/mmio.h>
#include <soc/blsp.h>
#include <soc/clock.h>

blsp_return_t blsp_i2c_init(blsp_qup_id_t id)
{
	void *base = blsp_qup_base(id);

	if (!base)
		return BLSP_ID_ERROR;

	if (blsp_i2c_clock_config(id) != 0)
		return BLSP_ID_ERROR;

	if (blsp_i2c_init_board(id))
		return BLSP_UNSUPPORTED;

	/* Configure Mini core to I2C core */
	clrsetbits32(base, BLSP_MINI_CORE_MASK, BLSP_MINI_CORE_I2C);

	return BLSP_SUCCESS;
}
