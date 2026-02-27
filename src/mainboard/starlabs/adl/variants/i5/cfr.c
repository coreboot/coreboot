/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/i2c_bus.h>
#include <device/i2c_simple.h>
#include <drivers/option/cfr_frontend.h>
#include <option.h>
#include <static.h>

void cfr_card_reader_update(struct sm_object *new_obj)
{
	struct device *mxc_accel = DEV_PTR(mxc6655);

	if (!i2c_dev_detect(i2c_busdev(mxc_accel), mxc_accel->path.i2c.device))
		new_obj->sm_bool.flags |= CFR_OPTFLAG_SUPPRESS;
}

void cfr_touchscreen_update(struct sm_object *new_obj)
{
	if (get_uint_option("accelerometer", 1) == 0)
		new_obj->sm_bool.flags |= CFR_OPTFLAG_SUPPRESS;
}
