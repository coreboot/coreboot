/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#ifndef __ACPI_DEVICE_H
#define __ACPI_DEVICE_H

struct device;
const char *acpi_device_name(struct device *dev);
const char *acpi_device_path(struct device *dev);
const char *acpi_device_scope(struct device *dev);
const char *acpi_device_path_join(struct device *dev, const char *name);

#endif
