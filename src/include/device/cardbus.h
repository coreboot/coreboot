/* SPDX-License-Identifier: GPL-2.0-only */

/* (c) 2005 Linux Networx GPL see COPYING for details */

#ifndef DEVICE_CARDBUS_H
#define DEVICE_CARDBUS_H

#include <device/device.h>

void cardbus_read_resources(struct device *dev);
void cardbus_enable_resources(struct device *dev);

extern struct device_operations default_cardbus_ops_bus;

#endif /* DEVICE_CARDBUS_H */
