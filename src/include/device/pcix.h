/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_PCIX_H
#define DEVICE_PCIX_H
/* (c) 2005 Linux Networx GPL see COPYING for details */

#include <device/device.h>
#include <stdint.h>

void pcix_scan_bridge(struct device *dev);

const char *pcix_speed(u16 sstatus);

extern struct device_operations default_pcix_ops_bus;

#endif /* DEVICE_PCIX_H */
