/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_XHCI_H
#define SOC_INTEL_COMMON_BLOCK_XHCI_H

#include <device/device.h>

/*
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

/*
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

/*
 * soc_get_xhci_usb_info() - Get the information about USB2 & USB3 ports.
 *
 * This function is used to get USB ports and status register offset information
 * within a XHCI controller.
 *
 * Return: USB ports and status register offset info for the SoC.
 */
const struct xhci_usb_info *soc_get_xhci_usb_info(void);

/*
 * usb_xhci_disable_unused() - Disable unused USB devices
 * @ext_usb_xhci_en_cb: Callback function to be invoked, supplied by mainboard,
 *			to identify the status of externally visible USB ports.
 *			(Return true if port is present, false if port is absent)
 *
 * This function is used to disable unused USB devices/ports that are configured
 * in the device tree. For the internal USB ports, the connect status of the port
 * is probed from the XHCI controller block and the port is disabled if it is not
 * connected. For the external USB ports, the mainboard provides the connect status
 * of the concerned port depending on the variants and their SKUs. If the mainboard
 * supplied callback function is NULL, then all the externally visible USB devices
 * in the device tree are enabled.
 */
void usb_xhci_disable_unused(bool (*ext_usb_xhci_en_cb)(unsigned int port_type,
							unsigned int port_id));

#endif	/* SOC_INTEL_COMMON_BLOCK_XHCI_H */
