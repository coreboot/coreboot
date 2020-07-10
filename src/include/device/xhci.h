/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DEVICE_XHCI_H__
#define __DEVICE_XHCI_H__

#include <stdint.h>
#include <device/device.h>
#include <commonlib/bsd/cb_err.h>

#define XHCI_HCCPARAMS1_XECP 0x12

#define XHCI_ECP_CAP_ID_LEGACY 1
#define XHCI_ECP_CAP_ID_SUPP 2

struct xhci_supported_protocol {
	union {
		uint32_t reg0;
		struct {
			uint32_t cap_id : 8;
			uint32_t next_ptr : 8;
			uint32_t minor_rev : 8;
			uint32_t major_rev : 8;
		};
	};
	union {
		uint32_t reg1;
		char name[4];
	};
	union {
		uint32_t reg2;
		struct {
			uint32_t port_offset : 8;
			uint32_t port_count : 8;
			uint32_t reserved : 12;
			uint32_t protocol_speed_id_count : 4;
		};
	};
};

struct xhci_ext_cap {
	uint32_t cap_id;
	/* cap_id is used to select the correct struct in the union. */
	union {
		struct xhci_supported_protocol supported_protocol;
	};
};

/**
 * Iterates over the xHCI Extended Capabilities List.
 */
enum cb_err xhci_for_each_ext_cap(const struct device *device, void *context,
				  void (*callback)(void *context,
						   const struct xhci_ext_cap *cap));

/**
 * Helper method that iterates over only the USB supported capabilities structures in the
 * xHCI Extended Capabilities List.
 */
enum cb_err xhci_for_each_supported_usb_cap(
	const struct device *device, void *context,
	void (*callback)(void *context, const struct xhci_supported_protocol *data));

void xhci_print_supported_protocol(const struct xhci_supported_protocol *supported_protocol);

#endif /* __DEVICE_XHCI_H__ */
