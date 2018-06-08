/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_SD_H
#define SOC_INTEL_COMMON_BLOCK_SD_H

#include <arch/acpi_device.h>

/*
 * Fill the GPIO Interrupt or I/O information that will be used for the
 * GPIO Connection Descriptor.
 */
int sd_fill_soc_gpio_info(struct acpi_gpio* gpio, struct device *dev);

#endif /* SOC_INTEL_COMMON_BLOCK_SD_H */
