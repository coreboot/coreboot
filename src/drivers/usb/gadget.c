/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <console/console.h>
#include <string.h>

#include "ehci_debug.h"
#include "usb_ch9.h"
#include "ehci.h"

#define USB_HUB_PORT_CONNECTION		0
#define USB_HUB_PORT_ENABLED		1
#define USB_HUB_PORT_RESET		4
#define USB_HUB_PORT_POWER		8
#define USB_HUB_C_PORT_CONNECTION	16
#define USB_HUB_C_PORT_RESET		20

static int hub_port_status(const char *buf, int feature)
{
	return !!(buf[feature>>3] & (1<<(feature&0x7)));
}

/* After USB port reset, treat device number 0 as an USB hub. Assign it with
 * a device number hub_addr. Then apply enable and reset on downstream port.
 */
static int dbgp_hub_enable(struct ehci_dbg_port *ehci_debug, unsigned char hub_addr,
	unsigned char port)
{
	char status[8];
	int ret, loop;

	/* Assign a devicenumber for the hub. */
	ret = dbgp_control_msg(ehci_debug, 0,
		USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
		USB_REQ_SET_ADDRESS, hub_addr, 0, NULL, 0);
	if (ret < 0)
		goto err;

	/* Enter configured state on hub. */
	ret = dbgp_control_msg(ehci_debug, hub_addr,
		USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
		USB_REQ_SET_CONFIGURATION, 1, 0, NULL, 0);
	if (ret < 0)
		goto err;

	/* Set PORT_POWER, poll for PORT_CONNECTION. */
	ret = dbgp_control_msg(ehci_debug, hub_addr,
		USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_OTHER,
		USB_REQ_SET_FEATURE, USB_HUB_PORT_POWER, port, NULL, 0);
	if (ret < 0)
		goto err;

	loop = 100;
	do {
		dbgp_mdelay(10);
		ret = dbgp_control_msg(ehci_debug, hub_addr,
			USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_OTHER,
			USB_REQ_GET_STATUS, 0, port, status, 4);
		if (ret < 0)
			goto err;
		if (hub_port_status(status, USB_HUB_PORT_CONNECTION))
			break;
	} while (--loop);
	if (! loop)
		goto err;

	ret = dbgp_control_msg(ehci_debug, hub_addr,
		USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_OTHER,
		USB_REQ_CLEAR_FEATURE, USB_HUB_C_PORT_CONNECTION, port, NULL, 0);
	if (ret < 0)
		goto err;

	/* Set PORT_RESET, poll for C_PORT_RESET. */
	ret = dbgp_control_msg(ehci_debug, hub_addr,
		USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_OTHER,
		USB_REQ_SET_FEATURE, USB_HUB_PORT_RESET, port, NULL, 0);
	if (ret < 0)
		goto err;

	loop = 100;
	do {
		dbgp_mdelay(10);
		ret = dbgp_control_msg(ehci_debug, hub_addr,
			USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_OTHER,
			USB_REQ_GET_STATUS, 0, port, status, 4);
		if (ret < 0)
			goto err;
		if (hub_port_status(status, USB_HUB_C_PORT_RESET))
			break;
	} while (--loop);
	if (! loop)
		goto err;

	ret = dbgp_control_msg(ehci_debug, hub_addr,
		USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_OTHER,
		USB_REQ_CLEAR_FEATURE, USB_HUB_C_PORT_RESET, port, NULL, 0);
	if (ret < 0)
		goto err;

	if (hub_port_status(status, USB_HUB_PORT_ENABLED))
		return 0;
err:
	return -1;
}

static void ack_set_configuration(struct dbgp_pipe *pipe, u8 devnum, int timeout)
{
	int i = DBGP_SETUP_EP0;
	while (++i < DBGP_MAX_ENDPOINTS) {
		if (pipe[i].endpoint != 0) {
			pipe[i].devnum = devnum;
			pipe[i].pid = USB_PID_DATA0;
			pipe[i].timeout = timeout;
		}
	}
}

static void activate_endpoints(struct dbgp_pipe *pipe)
{
	int i = DBGP_SETUP_EP0;
	pipe[i].status |= DBGP_EP_ENABLED | DBGP_EP_VALID;
	while (++i < DBGP_MAX_ENDPOINTS) {
		if (pipe[i].endpoint != 0)
			pipe[i].status |= DBGP_EP_ENABLED | DBGP_EP_VALID;
	}
}

static int probe_for_debug_descriptor(struct ehci_dbg_port *ehci_debug, struct dbgp_pipe *pipe)
{
	struct usb_debug_descriptor dbgp_desc;
	int configured = 0, ret;
	u8 devnum = 0;

	/* Find the debug device and make it device number 127 */
debug_dev_retry:
	memset(&dbgp_desc, 0, sizeof(dbgp_desc));
	ret = dbgp_control_msg(ehci_debug, devnum,
		USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
		USB_REQ_GET_DESCRIPTOR, (USB_DT_DEBUG << 8), 0,
		&dbgp_desc, sizeof(dbgp_desc));
	if (ret == sizeof(dbgp_desc)) {
		if (dbgp_desc.bLength == sizeof(dbgp_desc) && dbgp_desc.bDescriptorType == USB_DT_DEBUG)
			goto debug_dev_found;
		else
			printk(BIOS_INFO, "Invalid debug device descriptor.\n");
	}
	if (devnum == 0) {
		devnum = USB_DEBUG_DEVNUM;
		goto debug_dev_retry;
	} else {
		printk(BIOS_INFO, "Could not find attached debug device.\n");
		return -1;
	}
debug_dev_found:

	/* Move the device to 127 if it isn't already there */
	if (devnum != USB_DEBUG_DEVNUM) {
		ret = dbgp_control_msg(ehci_debug, devnum,
			USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
			USB_REQ_SET_ADDRESS, USB_DEBUG_DEVNUM, 0, NULL, 0);
		if (ret < 0) {
			printk(BIOS_INFO, "Could not move attached device to %d.\n",
				USB_DEBUG_DEVNUM);
			return -2;
		}
		devnum = USB_DEBUG_DEVNUM;
		printk(BIOS_INFO, "EHCI debug device renamed to 127.\n");
	}

	/* Enable the debug interface */
	ret = dbgp_control_msg(ehci_debug, USB_DEBUG_DEVNUM,
		USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
		USB_REQ_SET_FEATURE, USB_DEVICE_DEBUG_MODE, 0, NULL, 0);
	if (ret < 0) {
		printk(BIOS_INFO, "Could not enable EHCI debug device.\n");
		return -3;
	}
	printk(BIOS_INFO, "EHCI debug interface enabled.\n");

	pipe[DBGP_CONSOLE_EPOUT].endpoint = dbgp_desc.bDebugOutEndpoint;
	pipe[DBGP_CONSOLE_EPIN].endpoint = dbgp_desc.bDebugInEndpoint;

	ack_set_configuration(pipe, devnum, 1000);

	/* Perform a small write. */
	configured = 0;
small_write:
	ret = dbgp_bulk_write_x(&pipe[DBGP_CONSOLE_EPOUT], "USB\r\n",5);
	if (ret < 0) {
		printk(BIOS_INFO, "dbgp_bulk_write failed: %d\n", ret);
		if (!configured) {
			/* Send Set Configure request to device. This is required for FX2
			   (CY7C68013) to transfer from USB state Addressed to Configured,
			   only then endpoints other than 0 are enabled. */
			if (dbgp_control_msg(ehci_debug, USB_DEBUG_DEVNUM,
				USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
				USB_REQ_SET_CONFIGURATION, 1, 0, NULL, 0) >= 0) {
				configured = 1;
				goto small_write;
			}
		}
		return -4;
	}
	printk(BIOS_INFO, "Test write done\n");
	return 0;
}

/* FTDI FT232H UART programming. */
#define FTDI_SIO_SET_FLOW_CTRL_REQUEST 0x02
#define FTDI_SIO_SET_BAUDRATE_REQUEST  0x03
#define FTDI_SIO_SET_DATA_REQUEST      0x04
#define FTDI_SIO_SET_BITMODE_REQUEST   0x0b

/* Simplified divisor selection for 12MHz base clock only */
static void ft232h_baud(u16 *const value, u16 *const index, u32 baud)
{
	static const u32 fraction_map[8] = { 0, 3, 2, 4, 1, 5, 6, 7 };

	/* divisor must fit into 14 bits */
	if (baud < 733)
		baud = 733;

	/* divisor is given as a fraction of 8 */
	const u32 div8 = ((12 * 1000 * 1000) * 8) / baud;
	/* upper 3 bits fractional part, lower 14 bits integer */
	u32 div = fraction_map[div8 & 7] << 14 | div8 / 8;

	/* special case for 12MHz */
	if (div == 1)
		div = 0;
	/* special case for 8MHz */
	else if (div == (fraction_map[4] << 14 | 1))
		div = 1;

	*value = div;			/* divisor lower 16 bits */
	*index = (div >> 8) & 0x0100;   /* divisor bit 16 */
	*index |= 0x0200;		/* select 120MHz / 10 */
}

static int probe_for_ftdi(struct ehci_dbg_port *ehci_debug, struct dbgp_pipe *pipe)
{
	int ret;
	u8 devnum = 0;
	u8 uart_if = 1; /* FTDI_INTERFACE_A 1 */
	u16 baud_value, baud_index;

	/* Move the device to 127 if it isn't already there */
	ret = dbgp_control_msg(ehci_debug, devnum,
		USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
		USB_REQ_SET_ADDRESS, USB_DEBUG_DEVNUM, 0, NULL, 0);
	if (ret < 0) {
		printk(BIOS_INFO, "Could not move attached device to %d.\n",
			USB_DEBUG_DEVNUM);
			return -2;
	}
	devnum = USB_DEBUG_DEVNUM;
	printk(BIOS_INFO, "EHCI debug device renamed to %d.\n", devnum);

	/* Send Set Configure request to device.  */
	ret = dbgp_control_msg(ehci_debug, devnum,
		USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
		USB_REQ_SET_CONFIGURATION, 1, 0, NULL, 0);
	if (ret < 0) {
		printk(BIOS_INFO, "FTDI set configuration failed.\n");
		return -2;
	}

	ret = dbgp_control_msg(ehci_debug, devnum,
		USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
		FTDI_SIO_SET_BITMODE_REQUEST, 0, uart_if, NULL, 0);
	if (ret < 0) {
		printk(BIOS_INFO, "FTDI SET_BITMODE failed.\n");
		return -3;
	}
	ft232h_baud(&baud_value, &baud_index,
		    CONFIG_USBDEBUG_DONGLE_FTDI_FT232H_BAUD);
	ret = dbgp_control_msg(ehci_debug, devnum,
		USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
		FTDI_SIO_SET_BAUDRATE_REQUEST,
		baud_value, baud_index | uart_if, NULL, 0);
	if (ret < 0) {
		printk(BIOS_INFO, "FTDI SET_BAUDRATE failed.\n");
		return -3;
	}
	ret = dbgp_control_msg(ehci_debug, devnum,
		USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
		FTDI_SIO_SET_DATA_REQUEST,
		0x0008, uart_if, NULL, 0);
	if (ret < 0) {
		printk(BIOS_INFO, "FTDI SET_DATA failed.\n");
		return -3;
	}
	ret = dbgp_control_msg(ehci_debug, devnum,
		USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
		FTDI_SIO_SET_FLOW_CTRL_REQUEST,
		0x0000, uart_if, NULL, 0);
	if (ret < 0) {
		printk(BIOS_INFO, "FTDI SET_FLOW_CTRL failed.\n");
		return -3;
	}

	pipe[DBGP_CONSOLE_EPOUT].endpoint = 0x02;
	pipe[DBGP_CONSOLE_EPIN].endpoint = 0x81;

	ack_set_configuration(pipe, devnum, 1000);

	/* Perform a small write. */
	ret = dbgp_bulk_write_x(&pipe[DBGP_CONSOLE_EPOUT], "USB\r\n", 5);
	if (ret < 0) {
		printk(BIOS_INFO, "dbgp_bulk_write failed: %d\n", ret);
		return -4;
	}
	printk(BIOS_INFO, "Test write done\n");
	return 0;
}

int dbgp_probe_gadget(struct ehci_dbg_port *ehci_debug, struct dbgp_pipe *pipe)
{
	int ret;

	if (CONFIG_USBDEBUG_OPTIONAL_HUB_PORT != 0) {
		ret = dbgp_hub_enable(ehci_debug, USB_DEBUG_DEVNUM-1,
			CONFIG_USBDEBUG_OPTIONAL_HUB_PORT);
		if (ret < 0) {
			printk(BIOS_INFO, "Could not enable USB hub on debug port.\n");
			return ret;
		}
	}

	if (CONFIG(USBDEBUG_DONGLE_FTDI_FT232H)) {
		ret = probe_for_ftdi(ehci_debug, pipe);
	} else {
		ret = probe_for_debug_descriptor(ehci_debug, pipe);
	}
	if (ret < 0) {
		printk(BIOS_INFO, "Could not enable debug dongle.\n");
		return ret;
	}

	activate_endpoints(pipe);
	return 0;
}
