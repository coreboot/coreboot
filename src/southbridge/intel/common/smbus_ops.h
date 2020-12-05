/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/smbus.h>
#include <device/smbus_host.h>

extern struct smbus_bus_operations lops_smbus_bus;

void smbus_read_resources(struct device *dev);
