/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl31.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/bl31.h>

#include <arm-trusted-firmware/include/export/plat/mediatek/common/plat_params_exp.h>

void register_reset_to_bl31(int gpio_index, bool active_high)
{
	static struct bl_aux_param_gpio param_reset = {
		.h = { .type = BL_AUX_PARAM_MTK_RESET_GPIO },
	};

	if (active_high)
		param_reset.gpio.polarity = ARM_TF_GPIO_LEVEL_HIGH;
	else
		param_reset.gpio.polarity = ARM_TF_GPIO_LEVEL_LOW;

	param_reset.gpio.index = gpio_index;
	register_bl31_aux_param(&param_reset.h);
}
