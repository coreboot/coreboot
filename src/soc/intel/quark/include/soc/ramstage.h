/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <chip.h>
#include <device/device.h>
#if IS_ENABLED(CONFIG_PLATFORM_USES_FSP1_1)
#include <fsp/ramstage.h>
#endif
#include <soc/QuarkNcSocId.h>

void mainboard_gpio_i2c_init(struct device *dev);
#if IS_ENABLED(CONFIG_PLATFORM_USES_FSP1_1)
void fsp_silicon_init(bool s3wake);
#endif
asmlinkage void chipset_teardown_car(void);

#endif /* _SOC_RAMSTAGE_H_ */
