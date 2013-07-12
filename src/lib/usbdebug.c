/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Eric Biederman (ebiederm@xmission.com)
 * Copyright (C) 2007 AMD
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stddef.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/pci.h>
#include <arch/byteorder.h>
#include <cpu/x86/car.h>

#include <usb_ch9.h>
#include <ehci.h>
#include <usbdebug.h>


#define DBGP_EP_VALID		(1<<0)
#define DBGP_EP_ENABLED		(1<<1)
#define DBGP_EP_STATMASK	(DBGP_EP_VALID | DBGP_EP_ENABLED)

struct dbgp_pipe
{
	u8 endpoint;
	u8 status;
	u8 bufidx;
	char buf[8];
};

#define DBGP_MAX_ENDPOINTS	4
#define DBGP_CONSOLE_EPOUT	0
#define DBGP_CONSOLE_EPIN	1

struct ehci_debug_info {
	u8 devnum;

	void *ehci_caps;
	void *ehci_regs;
	void *ehci_debug;

	struct dbgp_pipe ep_pipe[DBGP_MAX_ENDPOINTS];
};

/* Set this to 1 to debug the start-up of EHCI debug port hardware. You need
 * to modify console_init() to initialise some other console before usbdebug
 * to receive the printk lines from here.
 * There will be no real usbdebug console output while DBGP_DEBUG is set.
 */
#define DBGP_DEBUG 0
#if DBGP_DEBUG
# define dbgp_printk(fmt_arg...) printk(BIOS_DEBUG, fmt_arg)
#else
#define dbgp_printk(fmt_arg...)   do {} while(0)
#endif


#define USB_DEBUG_DEVNUM 127

#define DBGP_DATA_TOGGLE	0x8800
#define DBGP_PID_UPDATE(x, tok) \
	((((x) ^ DBGP_DATA_TOGGLE) & 0xffff00) | ((tok) & 0xff))

#define DBGP_LEN_UPDATE(x, len) (((x) & ~0x0f) | ((len) & 0x0f))
/*
 * USB Packet IDs (PIDs)
 */

/* token */
#define USB_PID_OUT		0xe1
#define USB_PID_IN		0x69
#define USB_PID_SOF		0xa5
#define USB_PID_SETUP		0x2d
/* handshake */
#define USB_PID_ACK		0xd2
#define USB_PID_NAK		0x5a
#define USB_PID_STALL		0x1e
#define USB_PID_NYET		0x96
/* data */
#define USB_PID_DATA0		0xc3
#define USB_PID_DATA1		0x4b
#define USB_PID_DATA2		0x87
#define USB_PID_MDATA		0x0f
/* Special */
#define USB_PID_PREAMBLE	0x3c
#define USB_PID_ERR		0x3c
#define USB_PID_SPLIT		0x78
#define USB_PID_PING		0xb4
#define USB_PID_UNDEF_0		0xf0

#define USB_PID_DATA_TOGGLE	0x88
#define DBGP_CLAIM (DBGP_OWNER | DBGP_ENABLED | DBGP_INUSE)

#define PCI_CAP_ID_EHCI_DEBUG	0xa

#define HUB_ROOT_RESET_TIME	50	/* times are in msec */
#define HUB_SHORT_RESET_TIME	10
#define HUB_LONG_RESET_TIME	200
#define HUB_RESET_TIMEOUT	500

#define DBGP_MAX_PACKET		8
#define DBGP_LOOPS 1000

static struct ehci_debug_info glob_dbg_info CAR_GLOBAL;
#if !defined(__PRE_RAM__) && !defined(__SMM__)
static struct device_operations *ehci_drv_ops;
static struct device_operations ehci_dbg_ops;
#endif

static inline struct ehci_debug_info *dbgp_ehci_info(void)
{
	return car_get_var_ptr(&glob_dbg_info);
}

static int dbgp_wait_until_complete(struct ehci_dbg_port *ehci_debug)
{
	u32 ctrl;
	int loop = 0x100000;

	do {
		ctrl = read32((unsigned long)&ehci_debug->control);
		/* Stop when the transaction is finished */
		if (ctrl & DBGP_DONE)
			break;
	} while (--loop > 0);

	if (!loop)
		return -1;

	/* Now that we have observed the completed transaction,
	 * clear the done bit.
	 */
	write32((unsigned long)&ehci_debug->control, ctrl | DBGP_DONE);
	return (ctrl & DBGP_ERROR) ? -DBGP_ERRCODE(ctrl) : DBGP_LEN(ctrl);
}

static void dbgp_breath(void)
{
	/* Sleep to give the debug port a chance to breathe */
}

static int dbgp_wait_until_done(struct ehci_dbg_port *ehci_debug, unsigned ctrl, int loop)
{
	u32 pids, lpid;
	int ret;

retry:
	write32((unsigned long)&ehci_debug->control, ctrl | DBGP_GO);
	ret = dbgp_wait_until_complete(ehci_debug);
	pids = read32((unsigned long)&ehci_debug->pids);
	lpid = DBGP_PID_GET(pids);

	if (ret < 0) {
		if (ret == -DBGP_ERR_BAD && --loop > 0)
			goto retry;
		return ret;
	}

	/* If the port is getting full or it has dropped data
	 * start pacing ourselves, not necessary but it's friendly.
	 */
	if ((lpid == USB_PID_NAK) || (lpid == USB_PID_NYET))
		dbgp_breath();

	/* If I get a NACK reissue the transmission */
	if (lpid == USB_PID_NAK) {
		if (--loop > 0)
			goto retry;
	}

	return ret;
}

static void dbgp_set_data(struct ehci_dbg_port *ehci_debug, const void *buf, int size)
{
	const unsigned char *bytes = buf;
	u32 lo, hi;
	int i;

	lo = hi = 0;
	for (i = 0; i < 4 && i < size; i++)
		lo |= bytes[i] << (8*i);
	for (; i < 8 && i < size; i++)
		hi |= bytes[i] << (8*(i - 4));
	write32((unsigned long)&ehci_debug->data03, lo);
	write32((unsigned long)&ehci_debug->data47, hi);
}

static void dbgp_get_data(struct ehci_dbg_port *ehci_debug, void *buf, int size)
{
	unsigned char *bytes = buf;
	u32 lo, hi;
	int i;

	lo = read32((unsigned long)&ehci_debug->data03);
	hi = read32((unsigned long)&ehci_debug->data47);
	for (i = 0; i < 4 && i < size; i++)
		bytes[i] = (lo >> (8*i)) & 0xff;
	for (; i < 8 && i < size; i++)
		bytes[i] = (hi >> (8*(i - 4))) & 0xff;
}

static int dbgp_bulk_write(struct ehci_dbg_port *ehci_debug,
		unsigned devnum, unsigned endpoint, const char *bytes, int size)
{
	u32 pids, addr, ctrl;
	int ret;

	if (size > DBGP_MAX_PACKET)
		return -1;

	addr = DBGP_EPADDR(devnum, endpoint);

	pids = read32((unsigned long)&ehci_debug->pids);
	pids = DBGP_PID_UPDATE(pids, USB_PID_OUT);

	ctrl = read32((unsigned long)&ehci_debug->control);
	ctrl = DBGP_LEN_UPDATE(ctrl, size);
	ctrl |= DBGP_OUT;
	ctrl |= DBGP_GO;

	dbgp_set_data(ehci_debug, bytes, size);
	write32((unsigned long)&ehci_debug->address, addr);
	write32((unsigned long)&ehci_debug->pids, pids);

	ret = dbgp_wait_until_done(ehci_debug, ctrl, DBGP_LOOPS);

	return ret;
}

int dbgp_bulk_write_x(struct dbgp_pipe *pipe, const char *bytes, int size)
{
	struct ehci_debug_info *dbg_info = dbgp_ehci_info();
	return dbgp_bulk_write(dbg_info->ehci_debug, dbg_info->devnum,
			pipe->endpoint, bytes, size);
}

static int dbgp_bulk_read(struct ehci_dbg_port *ehci_debug, unsigned devnum,
		unsigned endpoint, void *data, int size, int loops)
{
	u32 pids, addr, ctrl;
	int ret;

	if (size > DBGP_MAX_PACKET)
		return -1;

	addr = DBGP_EPADDR(devnum, endpoint);

	pids = read32((unsigned long)&ehci_debug->pids);
	pids = DBGP_PID_UPDATE(pids, USB_PID_IN);

	ctrl = read32((unsigned long)&ehci_debug->control);
	ctrl = DBGP_LEN_UPDATE(ctrl, size);
	ctrl &= ~DBGP_OUT;
	ctrl |= DBGP_GO;

	write32((unsigned long)&ehci_debug->address, addr);
	write32((unsigned long)&ehci_debug->pids, pids);
	ret = dbgp_wait_until_done(ehci_debug, ctrl, loops);
	if (ret < 0)
		return ret;

	if (size > ret)
		size = ret;
	dbgp_get_data(ehci_debug, data, size);
	return ret;
}

int dbgp_bulk_read_x(struct dbgp_pipe *pipe, void *data, int size)
{
	struct ehci_debug_info *dbg_info = dbgp_ehci_info();
	return dbgp_bulk_read(dbg_info->ehci_debug, dbg_info->devnum,
			pipe->endpoint, data, size, DBGP_LOOPS);
}

static void dbgp_mdelay(int ms)
{
	int i;

	while (ms--) {
		for (i = 0; i < 1000; i++)
			inb(0x80);
	}
}

static int dbgp_control_msg(struct ehci_dbg_port *ehci_debug, unsigned devnum, int requesttype,
		int request, int value, int index, void *data, int size)
{
	u32 pids, addr, ctrl;
	struct usb_ctrlrequest req;
	int read;
	int ret;

	read = (requesttype & USB_DIR_IN) != 0;
	if (size > (read ? DBGP_MAX_PACKET:0))
		return -1;

	/* Compute the control message */
	req.bRequestType = requesttype;
	req.bRequest = request;
	req.wValue = cpu_to_le16(value);
	req.wIndex = cpu_to_le16(index);
	req.wLength = cpu_to_le16(size);

	pids = DBGP_PID_SET(USB_PID_DATA0, USB_PID_SETUP);
	addr = DBGP_EPADDR(devnum, 0);

	ctrl = read32((unsigned long)&ehci_debug->control);
	ctrl = DBGP_LEN_UPDATE(ctrl, sizeof(req));
	ctrl |= DBGP_OUT;
	ctrl |= DBGP_GO;

	/* Send the setup message */
	dbgp_set_data(ehci_debug, &req, sizeof(req));
	write32((unsigned long)&ehci_debug->address, addr);
	write32((unsigned long)&ehci_debug->pids, pids);
	ret = dbgp_wait_until_done(ehci_debug, ctrl, DBGP_LOOPS);
	if (ret < 0)
		return ret;


	/* Read the result */
	ret = dbgp_bulk_read(ehci_debug, devnum, 0, data, size, DBGP_LOOPS);
	return ret;
}

static int ehci_reset_port(struct ehci_regs *ehci_regs, int port)
{
	u32 portsc;
	u32 delay_time, delay_ms;
	int loop;

	/* Reset the usb debug port */
	portsc = read32((unsigned long)&ehci_regs->port_status[port - 1]);
	portsc &= ~PORT_PE;
	portsc |= PORT_RESET;
	write32((unsigned long)&ehci_regs->port_status[port - 1], portsc);

	delay_ms = HUB_ROOT_RESET_TIME;
	for (delay_time = 0; delay_time < HUB_RESET_TIMEOUT;
	     delay_time += delay_ms) {
		dbgp_mdelay(delay_ms);

		portsc = read32((unsigned long)&ehci_regs->port_status[port - 1]);
		if (portsc & PORT_RESET) {
			/* force reset to complete */
			loop = 2;
			write32((unsigned long)&ehci_regs->port_status[port - 1],
					portsc & ~(PORT_RWC_BITS | PORT_RESET));
			do {
				dbgp_mdelay(delay_ms);
				portsc = read32((unsigned long)&ehci_regs->port_status[port - 1]);
				delay_time += delay_ms;
			} while ((portsc & PORT_RESET) && (--loop > 0));
			if (!loop) {
				printk(BIOS_DEBUG, "ehci_reset_port forced done");
			}
		}

		/* Device went away? */
		if (!(portsc & PORT_CONNECT))
			return -1; //-ENOTCONN;

		/* bomb out completely if something weird happened */
		if ((portsc & PORT_CSC))
			return -2; //-EINVAL;

		/* If we've finished resetting, then break out of the loop */
		if (!(portsc & PORT_RESET) && (portsc & PORT_PE))
			return 0;
	}
	return -3; //-EBUSY;
}

static int ehci_wait_for_port(struct ehci_regs *ehci_regs, int port)
{
	u32 status;
	int ret, reps;

	for (reps = 0; reps < 3; reps++) {
		dbgp_mdelay(100);
		status = read32((unsigned long)&ehci_regs->status);
		if (status & STS_PCD) {
			ret = ehci_reset_port(ehci_regs, port);
			if (ret == 0)
				return 0;
		}
	}
	return -1; //-ENOTCONN;
}

static int usbdebug_init_(unsigned ehci_bar, unsigned offset, struct ehci_debug_info *info)
{
	struct ehci_caps *ehci_caps;
	struct ehci_regs *ehci_regs;
	struct ehci_dbg_port *ehci_debug;

	struct usb_debug_descriptor dbgp_desc;
	u32 cmd, ctrl, status, portsc, hcs_params;
	u32 debug_port, new_debug_port = 0, n_ports;
	u32 devnum;
	int ret, i;
	int loop;
	int port_map_tried;
	int playtimes = 3;

	ehci_caps  = (struct ehci_caps *)ehci_bar;
	ehci_regs  = (struct ehci_regs *)(ehci_bar +
			HC_LENGTH(read32((unsigned long)&ehci_caps->hc_capbase)));
	ehci_debug = (struct ehci_dbg_port *)(ehci_bar + offset);
	info->ehci_debug = (void *)0;

	memset(&info->ep_pipe, 0, sizeof (info->ep_pipe));
try_next_time:
	port_map_tried = 0;

try_next_port:
	hcs_params = read32((unsigned long)&ehci_caps->hcs_params);
	debug_port = HCS_DEBUG_PORT(hcs_params);
	n_ports    = HCS_N_PORTS(hcs_params);

	dbgp_printk("ehci_bar: 0x%x\n", ehci_bar);
	dbgp_printk("debug_port: %d\n", debug_port);
	dbgp_printk("n_ports:    %d\n", n_ports);

        for (i = 1; i <= n_ports; i++) {
                portsc = read32((unsigned long)&ehci_regs->port_status[i-1]);
                dbgp_printk("PORTSC #%d: %08x\n", i, portsc);
        }

	if(port_map_tried && (new_debug_port != debug_port)) {
		if(--playtimes) {
			set_debug_port(debug_port);
			goto try_next_time;
		}
		return -1;
	}

	loop = 100;
	/* Reset the EHCI controller */
	cmd = read32((unsigned long)&ehci_regs->command);
	cmd |= CMD_RESET;
	write32((unsigned long)&ehci_regs->command, cmd);
	do {
		dbgp_mdelay(10);
		cmd = read32((unsigned long)&ehci_regs->command);
	} while ((cmd & CMD_RESET) && (--loop > 0));

	if(!loop) {
		dbgp_printk("Could not reset EHCI controller.\n");
		// on some systems it works without succeeding here.
		// return -2;
	} else {
		dbgp_printk("EHCI controller reset successfully.\n");
	}

	/* Claim ownership, but do not enable yet */
	ctrl = read32((unsigned long)&ehci_debug->control);
	ctrl |= DBGP_OWNER;
	ctrl &= ~(DBGP_ENABLED | DBGP_INUSE);
	write32((unsigned long)&ehci_debug->control, ctrl);

	/* Start EHCI controller */
	cmd = read32((unsigned long)&ehci_regs->command);
	cmd &= ~(CMD_LRESET | CMD_IAAD | CMD_PSE | CMD_ASE | CMD_RESET);
	cmd |= CMD_RUN;
	write32((unsigned long)&ehci_regs->command, cmd);

	/* Ensure everything is routed to the EHCI */
	write32((unsigned long)&ehci_regs->configured_flag, FLAG_CF);

	/* Wait until the controller is no longer halted */
	loop = 10;
	do {
		dbgp_mdelay(10);
		status = read32((unsigned long)&ehci_regs->status);
	} while ((status & STS_HALT) && (--loop > 0));

	if(!loop) {
		dbgp_printk("EHCI could not be started.\n");
		return -3;
	}
	dbgp_printk("EHCI started.\n");

	/* Wait for a device to show up in the debug port */
	ret = ehci_wait_for_port(ehci_regs, debug_port);
	if (ret < 0) {
		dbgp_printk("No device found in debug port %d\n", debug_port);
		goto next_debug_port;
	}
	dbgp_printk("EHCI done waiting for port.\n");

	/* Enable the debug port */
	ctrl = read32((unsigned long)&ehci_debug->control);
	ctrl |= DBGP_CLAIM;
	write32((unsigned long)&ehci_debug->control, ctrl);
	ctrl = read32((unsigned long)&ehci_debug->control);
	if ((ctrl & DBGP_CLAIM) != DBGP_CLAIM) {
		dbgp_printk("No device in EHCI debug port.\n");
		write32((unsigned long)&ehci_debug->control, ctrl & ~DBGP_CLAIM);
		ret = -4;
		goto err;
	}
	dbgp_printk("EHCI debug port enabled.\n");

	/* Completely transfer the debug device to the debug controller */
	portsc = read32((unsigned long)&ehci_regs->port_status[debug_port - 1]);
	portsc &= ~PORT_PE;
	write32((unsigned long)&ehci_regs->port_status[debug_port - 1], portsc);

	dbgp_mdelay(100);

	/* Find the debug device and make it device number 127 */
	for (devnum = 0; devnum <= 127; devnum++) {
		ret = dbgp_control_msg(ehci_debug, devnum,
			USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
			USB_REQ_GET_DESCRIPTOR, (USB_DT_DEBUG << 8), 0,
			&dbgp_desc, sizeof(dbgp_desc));
		if (ret > 0)
			break;
	}
	if (devnum > 127) {
		dbgp_printk("Could not find attached debug device.\n");
		ret = -5;
		goto err;
	}
	if (ret < 0) {
		dbgp_printk("Attached device is not a debug device.\n");
		ret = -6;
		goto err;
	}

	/* Move the device to 127 if it isn't already there */
	if (devnum != USB_DEBUG_DEVNUM) {
		ret = dbgp_control_msg(ehci_debug, devnum,
			USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
			USB_REQ_SET_ADDRESS, USB_DEBUG_DEVNUM, 0, NULL, 0);
		if (ret < 0) {
			dbgp_printk("Could not move attached device to %d.\n",
				USB_DEBUG_DEVNUM);
			ret = -7;
			goto err;
		}
		devnum = USB_DEBUG_DEVNUM;
		dbgp_printk("EHCI debug device renamed to 127.\n");
	}

	/* Enable the debug interface */
	ret = dbgp_control_msg(ehci_debug, USB_DEBUG_DEVNUM,
		USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
		USB_REQ_SET_FEATURE, USB_DEVICE_DEBUG_MODE, 0, NULL, 0);
	if (ret < 0) {
		dbgp_printk("Could not enable EHCI debug device.\n");
		ret = -8;
		goto err;
	}
	dbgp_printk("EHCI debug interface enabled.\n");

	/* Perform a small write to get the even/odd data state in sync */
	ret = dbgp_bulk_write(ehci_debug, USB_DEBUG_DEVNUM, dbgp_desc.bDebugOutEndpoint, "USB\r\n",5);
	if (ret < 0) {
		dbgp_printk("dbgp_bulk_write failed: %d\n", ret);
		ret = -9;
		goto err;
	}
	dbgp_printk("Test write done\n");

	info->ehci_caps = ehci_caps;
	info->ehci_regs = ehci_regs;
	info->ehci_debug = ehci_debug;
	info->devnum = devnum;

	info->ep_pipe[DBGP_CONSOLE_EPOUT].endpoint = dbgp_desc.bDebugOutEndpoint;
	info->ep_pipe[DBGP_CONSOLE_EPIN].endpoint = dbgp_desc.bDebugInEndpoint;
	info->ep_pipe[DBGP_CONSOLE_EPOUT].status |= DBGP_EP_ENABLED | DBGP_EP_VALID;
	info->ep_pipe[DBGP_CONSOLE_EPIN].status |= DBGP_EP_ENABLED | DBGP_EP_VALID;
	return 0;
err:
	/* Things didn't work so remove my claim */
	ctrl = read32((unsigned long)&ehci_debug->control);
	ctrl &= ~(DBGP_CLAIM | DBGP_OUT);
	write32((unsigned long)(unsigned long)&ehci_debug->control, ctrl);
	//return ret;

next_debug_port:
	port_map_tried |= (1 << (debug_port - 1));
	new_debug_port = ((debug_port-1 + 1) % n_ports) + 1;
	if (port_map_tried != ((1 << n_ports) - 1)) {
		set_debug_port(new_debug_port);
		goto try_next_port;
	}
	if (--playtimes) {
		//set_debug_port(new_debug_port);
		set_debug_port(debug_port);
		goto try_next_time;
	}

	return -10;
}

void usbdebug_tx_byte(struct dbgp_pipe *pipe, unsigned char data)
{
	if (dbgp_ep_is_active(pipe)) {
		pipe->buf[pipe->bufidx++] = data;
		if (pipe->bufidx >= 8) {
			dbgp_bulk_write_x(pipe, pipe->buf, pipe->bufidx);
			pipe->bufidx = 0;
		}
	}
}

void usbdebug_tx_flush(struct dbgp_pipe *pipe)
{
	if (dbgp_ep_is_active(pipe) && pipe->bufidx > 0) {
		dbgp_bulk_write_x(pipe, pipe->buf, pipe->bufidx);
		pipe->bufidx = 0;
	}
}

#if !defined(__PRE_RAM__) && !defined(__SMM__)
static void usbdebug_re_enable(unsigned ehci_base)
{
	struct ehci_debug_info *dbg_info = dbgp_ehci_info();
	unsigned diff;
	int i;

	if (!dbg_info->ehci_debug)
		return;

	diff = (unsigned)dbg_info->ehci_caps - ehci_base;
	dbg_info->ehci_regs -= diff;
	dbg_info->ehci_debug -= diff;
	dbg_info->ehci_caps = (void*)ehci_base;

	for (i=0; i<DBGP_MAX_ENDPOINTS; i++)
		dbg_info->ep_pipe[i].status |= DBGP_EP_ENABLED;
}

static void usbdebug_disable(void)
{
	struct ehci_debug_info *dbg_info = dbgp_ehci_info();
	int i;
	for (i=0; i<DBGP_MAX_ENDPOINTS; i++)
		dbg_info->ep_pipe[i].status &= ~DBGP_EP_ENABLED;
}

static void pci_ehci_set_resources(struct device *dev)
{
	struct resource *res;

	printk(BIOS_DEBUG, "%s EHCI Debug Port hook triggered\n", dev_path(dev));
	usbdebug_disable();

	if (ehci_drv_ops->set_resources)
		ehci_drv_ops->set_resources(dev);
	res = find_resource(dev, EHCI_BAR_INDEX);
	if (!res)
		return;

	usbdebug_re_enable((u32)res->base);
	report_resource_stored(dev, res, "");
	printk(BIOS_DEBUG, "%s EHCI Debug Port relocated\n", dev_path(dev));
}

void pci_ehci_read_resources(struct device *dev)
{
	if (!ehci_drv_ops) {
		memcpy(&ehci_dbg_ops, dev->ops, sizeof(ehci_dbg_ops));
		ehci_drv_ops = dev->ops;
		ehci_dbg_ops.set_resources = pci_ehci_set_resources;
		dev->ops = &ehci_dbg_ops;
		printk(BIOS_DEBUG, "%s EHCI BAR hook registered\n", dev_path(dev));
	} else {
		printk(BIOS_DEBUG, "More than one caller of %s from %s\n", __func__, dev_path(dev));
	}

	pci_dev_read_resources(dev);
}
#endif

int dbgp_ep_is_active(struct dbgp_pipe *pipe)
{
	return (pipe->status & DBGP_EP_STATMASK) == (DBGP_EP_VALID | DBGP_EP_ENABLED);
}

struct dbgp_pipe *dbgp_console_output(void)
{
	return &dbgp_ehci_info()->ep_pipe[DBGP_CONSOLE_EPOUT];
}

struct dbgp_pipe *dbgp_console_input(void)
{
	return &dbgp_ehci_info()->ep_pipe[DBGP_CONSOLE_EPIN];
}

int usbdebug_init(void)
{
	struct ehci_debug_info *dbg_info = dbgp_ehci_info();

#if defined(__PRE_RAM__) || !CONFIG_EARLY_CONSOLE
	enable_usbdebug(CONFIG_USBDEBUG_DEFAULT_PORT);
#endif
	return usbdebug_init_(CONFIG_EHCI_BAR, CONFIG_EHCI_DEBUG_OFFSET, dbg_info);
}
