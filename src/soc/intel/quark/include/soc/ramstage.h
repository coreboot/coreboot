/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_RAMSTAGE_H_
#define _SOC_RAMSTAGE_H_

#include <cpu/cpu.h>
#include <device/device.h>
#include <soc/QuarkNcSocId.h>

#include "../../chip.h"

void mainboard_gpio_i2c_init(struct device *dev);
asmlinkage void chipset_teardown_car(void);

#endif /* _SOC_RAMSTAGE_H_ */
