/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_RAMSTAGE_H_
#define _SOC_RAMSTAGE_H_

#include <arch/cpu.h>
#include <device/device.h>
#include <soc/QuarkNcSocId.h>

#include "../../chip.h"

void mainboard_gpio_i2c_init(struct device *dev);
asmlinkage void chipset_teardown_car(void);

#endif /* _SOC_RAMSTAGE_H_ */
