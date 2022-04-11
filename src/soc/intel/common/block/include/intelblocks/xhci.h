/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_XHCI_H
#define SOC_INTEL_COMMON_BLOCK_XHCI_H

#include <device/device.h>
#include <elog.h>
#include <stdint.h>

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
 * struct xhci_wake_info - Relates an XHCI device to registers and wake types
 * @xhci_dev: devfn of the XHCI device
 * @elog_wake_type_host: the wake type for the controller device
 */
struct xhci_wake_info {
	pci_devfn_t xhci_dev;
	uint8_t elog_wake_type_host;
};

/*
 * xhci_update_wake_event() - Identify and log XHCI wake events.
 * @wake_info: A mapping of XHCI devfn to elog wake types
 * @wake_info_count: Count of items in wake_info
 * @info: Information about number of USB ports and their status reg offset.
 *
 * This function goes through individual USB port status registers within the
 * XHCI block and identifies if any of those USB ports triggered a wake-up and
 * log information about those ports to the event log.
 *
 * Return: True if any port is identified as a wake source, false if none.
 */
bool xhci_update_wake_event(const struct xhci_wake_info *wake_info,
			    size_t wake_info_count);

/* xhci_host_reset() - Function to reset the host controller */
void xhci_host_reset(void);
void soc_xhci_init(struct device *dev);

/*
 * soc_get_xhci_usb_info() - Get the information about USB2 & USB3 ports.
 *
 * This function is used to get USB ports and status register offset information
 * within a XHCI controller.
 *
 * Return: USB ports and status register offset info for the SoC.
 */
const struct xhci_usb_info *soc_get_xhci_usb_info(pci_devfn_t xhci_dev);

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
