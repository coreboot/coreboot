/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corporation.
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

#ifndef SOC_INTEL_COMMON_BLOCK_XHCI_H
#define SOC_INTEL_COMMON_BLOCK_XHCI_H

#include <device/device.h>

/**
 * struct xhci_usb_info - Data containing number of USB ports & offset.
 * @usb2_port_status_reg: Offset to USB2 port status register.
 * @num_usb2_ports: Number of USB2 ports.
 * @usb3_port_status_reg: Offset to USB3 port status register.
 * @num_usb3_ports: Number of USB3 ports.
 */
struct xhci_usb_info {
	uint32_t usb2_port_status_reg;
	uint32_t num_usb2_ports;
	uint32_t usb3_port_status_reg;
	uint32_t num_usb3_ports;
};

/**
 * pch_xhci_update_wake_event() - Identify and log XHCI wake events.
 * @info: Information about number of USB ports and their status reg offset.
 *
 * This function goes through individual USB port status registers within the
 * XHCI block and identifies if any of those USB ports triggered a wake-up and
 * log information about those ports to the event log.
 *
 * Return: True if any port is identified as a wake source, false if none.
 */
bool pch_xhci_update_wake_event(const struct xhci_usb_info *info);

void soc_xhci_init(struct device *dev);

#endif	/* SOC_INTEL_COMMON_BLOCK_XHCI_H */
