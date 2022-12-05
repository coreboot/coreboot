/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/xhci.h>
#include <string.h>

union xhci_ext_caps_header {
	uint32_t val;
	struct {
		uint32_t cap_id : 8;
		uint32_t next_ptr : 8;
		uint32_t reserved : 16;
	};
};

enum cb_err xhci_for_each_ext_cap(const struct device *device, void *context,
				  void (*callback)(void *context,
						   const struct xhci_ext_cap *cap))
{
	struct resource *res;
	uint32_t *ext_cap_ptr;
	uint32_t ext_caps_word_offset;
	union xhci_ext_caps_header header;
	struct xhci_ext_cap cap;

	if (!device || !callback)
		return CB_ERR_ARG;

	res = probe_resource(device, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_ERR, "%s: Unable to find BAR resource for %s\n", __func__,
		       dev_path(device));
		return CB_ERR;
	}

	if (!(res->flags & IORESOURCE_ASSIGNED)) {
		printk(BIOS_ERR, "%s: BAR is not assigned\n", __func__);
		return CB_ERR;
	}

	if (res->limit > 0xFFFFFFFF) {
		printk(BIOS_ERR, "%s: 64-bit BAR is not supported\n", __func__);
		return CB_ERR;
	}

	ext_caps_word_offset = read16(res2mmio(res, XHCI_HCCPARAMS1_XECP, 0));

	if (!ext_caps_word_offset) {
		printk(BIOS_ERR, "%s: No extended capabilities defined\n", __func__);
		return CB_ERR;
	}

	ext_cap_ptr = res2mmio(res, ext_caps_word_offset << 2, 0);

	while ((uintptr_t)ext_cap_ptr < (uintptr_t)res->limit) {
		header.val = read32(ext_cap_ptr);

		cap.cap_id = header.cap_id;

		if (header.cap_id == XHCI_ECP_CAP_ID_SUPP) {
			cap.supported_protocol.reg0 = header.val;
			cap.supported_protocol.reg1 = read32(ext_cap_ptr + 1);
			cap.supported_protocol.reg2 = read32(ext_cap_ptr + 2);
		}

		callback(context, &cap);

		if (!header.next_ptr)
			break;

		ext_cap_ptr += header.next_ptr;
	}

	return CB_SUCCESS;
}

struct supported_usb_cap_context {
	void *context;
	void (*callback)(void *context, const struct xhci_supported_protocol *data);
};

static void xhci_supported_usb_cap_handler(void *context, const struct xhci_ext_cap *cap)
{
	const struct xhci_supported_protocol *data;
	struct supported_usb_cap_context *internal_context = context;

	if (cap->cap_id != XHCI_ECP_CAP_ID_SUPP)
		return;

	data = &cap->supported_protocol;

	if (memcmp(data->name, "USB ", 4)) {
		printk(BIOS_DEBUG, "%s: Unknown Protocol: %.*s\n", __func__,
		       (int)sizeof(data->name), data->name);
		return;
	}

	internal_context->callback(internal_context->context, data);
}

enum cb_err xhci_for_each_supported_usb_cap(
	const struct device *device, void *context,
	void (*callback)(void *context, const struct xhci_supported_protocol *data))
{
	struct supported_usb_cap_context internal_context = {
		.context = context,
		.callback = callback,
	};

	return xhci_for_each_ext_cap(device, &internal_context, xhci_supported_usb_cap_handler);
}

void xhci_print_supported_protocol(const struct xhci_supported_protocol *supported_protocol)
{
	printk(BIOS_DEBUG, "xHCI Supported Protocol:\n");
	printk(BIOS_DEBUG, "  Major: %#x, Minor: %#x, Protocol: '%.*s'\n",
	       supported_protocol->major_rev, supported_protocol->minor_rev,
	       (int)sizeof(supported_protocol->name), supported_protocol->name);
	printk(BIOS_DEBUG, "  Port Offset: %d, Port Count: %d\n",
	       supported_protocol->port_offset, supported_protocol->port_count);
}
