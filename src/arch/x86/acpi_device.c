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

#include <string.h>
#include <arch/acpi.h>
#include <arch/acpi_device.h>
#include <arch/acpigen.h>
#include <device/device.h>
#include <device/path.h>

/* Locate and return the ACPI name for this device */
const char *acpi_device_name(struct device *dev)
{
	if (!dev)
		return NULL;

	/* Check for device specific handler */
	if (dev->ops->acpi_name)
		return dev->ops->acpi_name(dev);

	/* Check parent device in case it has a global handler */
	if (dev->bus && dev->bus->dev->ops->acpi_name)
		return dev->bus->dev->ops->acpi_name(dev);

	return NULL;
}

/* Recursive function to find the root device and print a path from there */
static size_t acpi_device_path_fill(struct device *dev, char *buf,
				    size_t buf_len, size_t cur)
{
	const char *name = acpi_device_name(dev);
	size_t next = 0;

	/*
	 * Make sure this name segment will fit, including the path segment
	 * separator and possible NUL terminator if this is the last segment.
	 */
	if (!dev || !name || (cur + strlen(name) + 2) > buf_len)
		return cur;

	/* Walk up the tree to the root device */
	if (dev->path.type != DEVICE_PATH_ROOT && dev->bus && dev->bus->dev)
		next = acpi_device_path_fill(dev->bus->dev, buf, buf_len, cur);

	/* Fill in the path from the root device */
	next += snprintf(buf + next, buf_len - next, "%s%s",
			 dev->path.type == DEVICE_PATH_ROOT ? "" : ".", name);

	return next;
}

/*
 * Warning: just as with dev_path() this uses a static buffer
 * so should not be called mulitple times in one statement
 */
const char *acpi_device_path(struct device *dev)
{
	static char buf[DEVICE_PATH_MAX] = {};

	if (!dev)
		return NULL;

	if (acpi_device_path_fill(dev, buf, sizeof(buf), 0) <= 0)
		return NULL;

	return buf;
}

/* Return the path of the parent device as the ACPI Scope for this device */
const char *acpi_device_scope(struct device *dev)
{
	if (!dev || !dev->bus || !dev->bus->dev)
		return NULL;

	return acpi_device_path(dev->bus->dev);
}

/* Concatentate the device path and provided name suffix */
const char *acpi_device_path_join(struct device *dev, const char *name)
{
	static char buf[DEVICE_PATH_MAX] = {};
	size_t len;

	if (!dev)
		return NULL;

	/* Build the path of this device */
	len = acpi_device_path_fill(dev, buf, sizeof(buf), 0);
	if (len <= 0)
		return NULL;

	/* Ensure there is room for the added name, separator, and NUL */
	if ((len + strlen(name) + 2) > sizeof(buf))
		return NULL;
	snprintf(buf + len, sizeof(buf) - len, ".%s", name);

	return buf;
}
