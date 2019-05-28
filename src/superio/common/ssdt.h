/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SUPERIO_COMMON_SSDT_H__
#define __SUPERIO_COMMON_SSDT_H__

#include <device/device.h>

const char *superio_common_ldn_acpi_name(const struct device *dev);
void superio_common_fill_ssdt_generator(struct device *dev);

#endif /* __SUPERIO_COMMON_SSDT_H__ */
