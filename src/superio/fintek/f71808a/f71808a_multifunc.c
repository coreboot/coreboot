/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include "chip.h"
#include "fintek_internal.h"

#define MULTI_FUNC_SEL_REG0 0x28
#define MULTI_FUNC_SEL_REG1 0x29
#define MULTI_FUNC_SEL_REG2 0x2A
#define MULTI_FUNC_SEL_REG3 0x2B
#define MULTI_FUNC_SEL_REG4 0x2C

void f71808a_multifunc_init(struct device *dev)
{
	const struct superio_fintek_f71808a_config *conf = dev->chip_info;

	pnp_enter_conf_mode(dev);

	/* multi-func select reg0 */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG0,
			conf->multi_function_register_0);

	/* multi-func select reg1 */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG1,
			conf->multi_function_register_1);

	/* multi-func select reg2 */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG2,
			conf->multi_function_register_2);

	/* multi-func select reg3 */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG3,
			conf->multi_function_register_3);

	/* multi-func select reg4 */
	pnp_write_config(dev, MULTI_FUNC_SEL_REG4,
			conf->multi_function_register_4);

	pnp_exit_conf_mode(dev);
}
