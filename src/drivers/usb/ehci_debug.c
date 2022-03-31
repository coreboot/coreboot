/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <console/console.h>
#include <console/usb.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <arch/symbols.h>
#include <string.h>
#include <cbmem.h>

#include "ehci_debug.h"
#include "usb_ch9.h"
#include "ehci.h"

struct ehci_debug_info {
	u64 ehci_base;
	u64 ehci_debug;

	struct dbgp_pipe ep_pipe[DBGP_MAX_ENDPOINTS];
} __packed;

/* With CONFIG(DEBUG_CONSOLE_INIT), you can debug the connection of
 * usbdebug dongle. EHCI port register bits and USB packets are dumped
 * on console, assuming some other console already works.
 */
#define dprintk(LEVEL, args...)						\
	do {								\
		if (CONFIG(DEBUG_CONSOLE_INIT) && !dbgp_enabled())	\
			printk(LEVEL, ##args);				\
	} while (0)

#define DBGP_LEN_UPDATE(x, len) (((x) & ~0x0f) | ((len) & 0x0f))

#define DBGP_CLAIM (DBGP_OWNER | DBGP_ENABLED | DBGP_INUSE)

#define HUB_ROOT_RESET_TIME	50	/* times are in msec */
#define HUB_SHORT_RESET_TIME	10
#define HUB_LONG_RESET_TIME	200
#define HUB_RESET_TIMEOUT	500

#define DBGP_MICROFRAME_TIMEOUT_LOOPS	1000
#define DBGP_MICROFRAME_RETRIES		10
#define DBGP_MAX_PACKET		8

static int dbgp_enabled(void);
static void dbgp_print_data(struct ehci_dbg_port *ehci_debug);

static struct ehci_debug_info glob_dbg_info;
static struct ehci_debug_info *glob_dbg_info_p;

static inline struct ehci_debug_info *dbgp_ehci_info(void)
{
	if (glob_dbg_info_p == NULL) {
		struct ehci_debug_info *info;
		if (ENV_BOOTBLOCK || ENV_SEPARATE_VERSTAGE || ENV_ROMSTAGE) {
			/* The message likely does not show if we hit this. */
			if (sizeof(*info) > _car_ehci_dbg_info_size)
				die("BUG: Increase ehci_dbg_info reserve in CAR");
			info = (void *)_car_ehci_dbg_info;
		} else {
			info = &glob_dbg_info;
		}
		glob_dbg_info_p = info;
	}
	return glob_dbg_info_p;
}

static int dbgp_wait_until_complete(struct ehci_dbg_port *ehci_debug)
{
	u32 ctrl;
	int loop = 0;

	do {
		ctrl = read32(&ehci_debug->control);
		/* Stop when the transaction is finished */
		if (ctrl & DBGP_DONE)
			break;
	} while (++loop < DBGP_MICROFRAME_TIMEOUT_LOOPS);

	if (! (ctrl & DBGP_DONE)) {
		dprintk(BIOS_ERR, "%s: retry timeout.\n", __func__);
		return -DBGP_ERR_SIGNAL;
	}

	/* Now that we have observed the completed transaction,
	 * clear the done bit.
	 */
	write32(&ehci_debug->control, ctrl | DBGP_DONE);
	return (ctrl & DBGP_ERROR) ? -DBGP_ERRCODE(ctrl) : DBGP_LEN(ctrl);
}

static void dbgp_breath(void)
{
	/* Sleep to give the debug port a chance to breathe */
}

static int dbgp_wait_until_done(struct ehci_dbg_port *ehci_debug, struct dbgp_pipe *pipe,
	unsigned int ctrl, const int timeout)
{
	u32 rd_ctrl, rd_pids;
	u32 ctrl_prev = 0, pids_prev = 0;
	u8 lpid;
	int ret, host_retries;
	int loop;

	loop = 0;
device_retry:
	host_retries = 0;
	if (loop++ >= timeout)
		return -DBGP_ERR_BAD;

host_retry:
	if (host_retries++ >= DBGP_MICROFRAME_RETRIES)
		return -DBGP_ERR_BAD;
	if (loop == 1 || host_retries > 1)
		dprintk(BIOS_SPEW, "dbgp:  start (@ %3d,%d) ctrl=%08x\n",
			loop, host_retries, ctrl | DBGP_GO);
	write32(&ehci_debug->control, ctrl | DBGP_GO);
	ret = dbgp_wait_until_complete(ehci_debug);
	rd_ctrl = read32(&ehci_debug->control);
	rd_pids = read32(&ehci_debug->pids);

	if (rd_ctrl != ctrl_prev || rd_pids != pids_prev || (ret<0)) {
		ctrl_prev = rd_ctrl;
		pids_prev = rd_pids;
		dprintk(BIOS_SPEW, "dbgp: status (@ %3d,%d) ctrl=%08x pids=%08x ret=%d\n",
			loop, host_retries, rd_ctrl, rd_pids, ret);
	}

	/* Controller hardware failure. */
	if (ret == -DBGP_ERR_SIGNAL) {
		return ret;

	/* Bus failure (corrupted microframe). */
	} else if (ret == -DBGP_ERR_BAD) {
		goto host_retry;
	}

	lpid = DBGP_PID_GET(rd_pids);

	/* If I get an ACK or in-sync DATA PID, we are done. */
	if ((lpid == USB_PID_ACK) || (lpid == pipe->pid)) {
		pipe->pid ^= USB_PID_DATA_TOGGLE;
	}

	/* If the port is getting full or it has dropped data
	 * start pacing ourselves, not necessary but it's friendly.
	 */
	else if (lpid == USB_PID_NYET) {
		dbgp_breath();
		goto device_retry;
	}

	/* If I get a NACK or out-of-sync DATA PID, reissue the transmission. */
	else if ((lpid == USB_PID_NAK) || (lpid == (pipe->pid ^ USB_PID_DATA_TOGGLE))) {
		goto device_retry;
	}

	/* Abort on STALL handshake for endpoint 0.*/
	else if ((lpid == USB_PID_STALL) && (pipe->endpoint == 0x0)) {
		ret = -DBGP_ERR_BAD;
	}

	dbgp_print_data(ehci_debug);

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
	write32(&ehci_debug->data03, lo);
	write32(&ehci_debug->data47, hi);
}

static void dbgp_get_data(struct ehci_dbg_port *ehci_debug, void *buf, int size)
{
	unsigned char *bytes = buf;
	u32 lo, hi;
	int i;

	lo = read32(&ehci_debug->data03);
	hi = read32(&ehci_debug->data47);
	for (i = 0; i < 4 && i < size; i++)
		bytes[i] = (lo >> (8*i)) & 0xff;
	for (; i < 8 && i < size; i++)
		bytes[i] = (hi >> (8*(i - 4))) & 0xff;
}

static void dbgp_print_data(struct ehci_dbg_port *ehci_debug)
{
	int len;
	u32 ctrl, lo, hi;

	if (!CONFIG(DEBUG_CONSOLE_INIT) || dbgp_enabled())
		return;

	ctrl = read32(&ehci_debug->control);
	lo = read32(&ehci_debug->data03);
	hi = read32(&ehci_debug->data47);

	len = DBGP_LEN(ctrl);
	if (len) {
		int i;
		dprintk(BIOS_SPEW, "dbgp:    buf:");
		for (i = 0; i < 4 && i < len; i++)
			dprintk(BIOS_SPEW, " %02x", (lo >> (8*i)) & 0xff);
		for (; i < 8 && i < len; i++)
			dprintk(BIOS_SPEW, " %02x", (hi >> (8*(i - 4))) & 0xff);
		dprintk(BIOS_SPEW, "\n");
	}
}

static int dbgp_bulk_write(struct ehci_dbg_port *ehci_debug, struct dbgp_pipe *pipe,
	const char *bytes, int size)
{
	u32 pids, addr, ctrl;
	int ret;

	if (size > DBGP_MAX_PACKET)
		return -1;

	addr = DBGP_EPADDR(pipe->devnum, pipe->endpoint);
	pids = DBGP_PID_SET(pipe->pid, USB_PID_OUT);

	ctrl = read32(&ehci_debug->control);
	ctrl = DBGP_LEN_UPDATE(ctrl, size);
	ctrl |= DBGP_OUT;

	dbgp_set_data(ehci_debug, bytes, size);
	write32(&ehci_debug->address, addr);
	write32(&ehci_debug->pids, pids);

	ret = dbgp_wait_until_done(ehci_debug, pipe, ctrl, pipe->timeout);

	return ret;
}

int dbgp_bulk_write_x(struct dbgp_pipe *pipe, const char *bytes, int size)
{
	struct ehci_debug_info *dbg_info = dbgp_ehci_info();
	struct ehci_dbg_port *port;
	port = (void *)(uintptr_t)dbg_info->ehci_debug;
	return dbgp_bulk_write(port, pipe, bytes, size);
}

static int dbgp_bulk_read(struct ehci_dbg_port *ehci_debug, struct dbgp_pipe *pipe,
	void *data, int size)
{
	u32 pids, addr, ctrl;
	int ret;

	if (size > DBGP_MAX_PACKET)
		return -1;

	addr = DBGP_EPADDR(pipe->devnum, pipe->endpoint);
	pids = DBGP_PID_SET(pipe->pid, USB_PID_IN);

	ctrl = read32(&ehci_debug->control);
	ctrl = DBGP_LEN_UPDATE(ctrl, size);
	ctrl &= ~DBGP_OUT;

	write32(&ehci_debug->address, addr);
	write32(&ehci_debug->pids, pids);
	ret = dbgp_wait_until_done(ehci_debug, pipe, ctrl, pipe->timeout);
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
	struct ehci_dbg_port *port;
	port = (void *)(uintptr_t)dbg_info->ehci_debug;
	return dbgp_bulk_read(port, pipe, data, size);
}

void dbgp_mdelay(int ms)
{
	int i;

	while (ms--) {
		for (i = 0; i < 1000; i++)
			inb(0x80);
	}
}

int dbgp_control_msg(struct ehci_dbg_port *ehci_debug, unsigned int devnum, int requesttype,
		int request, int value, int index, void *data, int size)
{
	struct ehci_debug_info *info = dbgp_ehci_info();
	struct dbgp_pipe *pipe = &info->ep_pipe[DBGP_SETUP_EP0];
	u32 pids, addr, ctrl;
	struct usb_ctrlrequest req;
	int read;
	int ret, ret2;

	read = (requesttype & USB_DIR_IN) != 0;
	if (size > DBGP_MAX_PACKET)
		return -1;

	/* Compute the control message */
	req.bRequestType = requesttype;
	req.bRequest = request;
	req.wValue = cpu_to_le16(value);
	req.wIndex = cpu_to_le16(index);
	req.wLength = cpu_to_le16(size);

	pipe->devnum = devnum;
	pipe->endpoint = 0;
	pipe->pid = USB_PID_DATA0;
	pipe->timeout = 1000;
	addr = DBGP_EPADDR(pipe->devnum, pipe->endpoint);
	pids = DBGP_PID_SET(pipe->pid, USB_PID_SETUP);

	ctrl = read32(&ehci_debug->control);
	ctrl = DBGP_LEN_UPDATE(ctrl, sizeof(req));
	ctrl |= DBGP_OUT;

	/* Setup stage */
	dbgp_set_data(ehci_debug, &req, sizeof(req));
	write32(&ehci_debug->address, addr);
	write32(&ehci_debug->pids, pids);
	ret = dbgp_wait_until_done(ehci_debug, pipe, ctrl, 1);
	if (ret < 0)
		return ret;

	/* Data stage (optional) */
	if (read && size)
		ret = dbgp_bulk_read(ehci_debug, pipe, data, size);
	else if (!read && size)
		ret = dbgp_bulk_write(ehci_debug, pipe, data, size);

	/* Status stage in opposite direction */
	pipe->pid = USB_PID_DATA1;
	ctrl = read32(&ehci_debug->control);
	ctrl = DBGP_LEN_UPDATE(ctrl, 0);
	if (read) {
		pids = DBGP_PID_SET(pipe->pid, USB_PID_OUT);
		ctrl |= DBGP_OUT;
	} else {
		pids = DBGP_PID_SET(pipe->pid, USB_PID_IN);
		ctrl &= ~DBGP_OUT;
	}

	write32(&ehci_debug->pids, pids);
	ret2 = dbgp_wait_until_done(ehci_debug, pipe, ctrl, pipe->timeout);
	if (ret2 < 0)
		return ret2;

	return ret;
}

static int ehci_reset_port(struct ehci_regs *ehci_regs, int port)
{
	u32 portsc;
	int loop;

	/* Reset the USB debug port */
	portsc = read32(&ehci_regs->port_status[port - 1]);
	portsc &= ~PORT_PE;
	portsc |= PORT_RESET;
	write32(&ehci_regs->port_status[port - 1], portsc);

	dbgp_mdelay(HUB_ROOT_RESET_TIME);

	portsc = read32(&ehci_regs->port_status[port - 1]);
	write32(&ehci_regs->port_status[port - 1],
			portsc & ~(PORT_RWC_BITS | PORT_RESET));

	loop = 100;
	do {
		dbgp_mdelay(1);
		portsc = read32(&ehci_regs->port_status[port - 1]);
	} while ((portsc & PORT_RESET) && (--loop > 0));

	/* Device went away? */
	if (!(portsc & PORT_CONNECT))
		return -1; //-ENOTCONN;

	/* bomb out completely if something weird happened */
	if ((portsc & PORT_CSC))
		return -2; //-EINVAL;

	/* If we've finished resetting, then break out of the loop */
	if (!(portsc & PORT_RESET) && (portsc & PORT_PE))
		return 0;

	return -3; //-EBUSY;
}

static int ehci_wait_for_port(struct ehci_regs *ehci_regs, int port)
{
	u32 status;
	int ret, reps;

	for (reps = 0; reps < 3; reps++) {
		dbgp_mdelay(100);
		status = read32(&ehci_regs->status);
		if (status & STS_PCD) {
			ret = ehci_reset_port(ehci_regs, port);
			if (ret == 0)
				return 0;
		}
	}
	return -1; //-ENOTCONN;
}

static int usbdebug_init_(uintptr_t ehci_bar, unsigned int offset, struct ehci_debug_info *info)
{
	struct ehci_caps *ehci_caps;
	struct ehci_regs *ehci_regs;

	u32 cmd, ctrl, status, portsc, hcs_params;
	u32 debug_port, new_debug_port = 0, n_ports;
	int ret, i;
	int loop;
	int port_map_tried;
	int playtimes = 3;

	/* Keep all endpoints disabled before any printk() call. */
	memset(info, 0, sizeof(*info));
	info->ehci_base = ehci_bar;
	info->ehci_debug = ehci_bar + offset;
	info->ep_pipe[0].status	|= DBGP_EP_NOT_PRESENT;

	dprintk(BIOS_INFO, "ehci_bar: 0x%lx debug_offset 0x%x\n", ehci_bar, offset);

	ehci_caps  = (struct ehci_caps *)ehci_bar;
	ehci_regs  = (struct ehci_regs *)(ehci_bar +
			HC_LENGTH(read32(&ehci_caps->hc_capbase)));

	struct ehci_dbg_port *ehci_debug = (void *)(uintptr_t)info->ehci_debug;

	if (CONFIG_USBDEBUG_DEFAULT_PORT > 0)
		ehci_debug_select_port(CONFIG_USBDEBUG_DEFAULT_PORT);
	else
		ehci_debug_select_port(1);

try_next_time:
	port_map_tried = 0;

try_next_port:
	hcs_params = read32(&ehci_caps->hcs_params);
	debug_port = HCS_DEBUG_PORT(hcs_params);
	n_ports    = HCS_N_PORTS(hcs_params);

	dprintk(BIOS_INFO, "debug_port: %d\n", debug_port);
	dprintk(BIOS_INFO, "n_ports:    %d\n", n_ports);

	for (i = 1; i <= n_ports; i++) {
		portsc = read32(&ehci_regs->port_status[i-1]);
		dprintk(BIOS_INFO, "PORTSC #%d: %08x\n", i, portsc);
	}

	if (port_map_tried && (new_debug_port != debug_port)) {
		if (--playtimes) {
			ehci_debug_select_port(debug_port);
			goto try_next_time;
		}
		return -1;
	}

	/* Wait until the controller is halted */
	status = read32(&ehci_regs->status);
	if (!(status & STS_HALT)) {
		cmd = read32(&ehci_regs->command);
		cmd &= ~CMD_RUN;
		write32(&ehci_regs->command, cmd);
		loop = 100;
		do {
			dbgp_mdelay(10);
			status = read32(&ehci_regs->status);
		} while (!(status & STS_HALT) && (--loop > 0));
		if (status & STS_HALT)
			dprintk(BIOS_INFO, "EHCI controller halted successfully.\n");
		else
			dprintk(BIOS_INFO, "EHCI controller is not halted. Reset may fail.\n");
	}

	loop = 100;
	/* Reset the EHCI controller */
	cmd = read32(&ehci_regs->command);
	cmd |= CMD_RESET;
	write32(&ehci_regs->command, cmd);
	do {
		dbgp_mdelay(10);
		cmd = read32(&ehci_regs->command);
	} while ((cmd & CMD_RESET) && (--loop > 0));

	if (!loop) {
		dprintk(BIOS_INFO, "Could not reset EHCI controller.\n");
		// on some systems it works without succeeding here.
		// return -2;
	} else {
		dprintk(BIOS_INFO, "EHCI controller reset successfully.\n");
	}

	/* Claim ownership, but do not enable yet */
	ctrl = read32(&ehci_debug->control);
	ctrl |= DBGP_OWNER;
	ctrl &= ~(DBGP_ENABLED | DBGP_INUSE);
	write32(&ehci_debug->control, ctrl);

	/* Start EHCI controller */
	cmd = read32(&ehci_regs->command);
	cmd &= ~(CMD_LRESET | CMD_IAAD | CMD_PSE | CMD_ASE | CMD_RESET);
	cmd |= CMD_RUN;
	write32(&ehci_regs->command, cmd);

	/* Ensure everything is routed to the EHCI */
	write32(&ehci_regs->configured_flag, FLAG_CF);

	/* Wait until the controller is no longer halted */
	loop = 10;
	do {
		dbgp_mdelay(10);
		status = read32(&ehci_regs->status);
	} while ((status & STS_HALT) && (--loop > 0));

	if (!loop) {
		dprintk(BIOS_INFO, "EHCI could not be started.\n");
		return -3;
	}
	dprintk(BIOS_INFO, "EHCI started.\n");

	/* Wait for a device to show up in the debug port */
	ret = ehci_wait_for_port(ehci_regs, debug_port);
	if (ret < 0) {
		dprintk(BIOS_INFO, "No device found in debug port %d\n", debug_port);
		goto next_debug_port;
	}
	dprintk(BIOS_INFO, "EHCI done waiting for port.\n");

	/* Enable the debug port */
	ctrl = read32(&ehci_debug->control);
	ctrl |= DBGP_CLAIM;
	write32(&ehci_debug->control, ctrl);
	ctrl = read32(&ehci_debug->control);
	if ((ctrl & DBGP_CLAIM) != DBGP_CLAIM) {
		dprintk(BIOS_INFO, "No device in EHCI debug port.\n");
		write32(&ehci_debug->control, ctrl & ~DBGP_CLAIM);
		ret = -4;
		goto err;
	}
	dprintk(BIOS_INFO, "EHCI debug port enabled.\n");

	dbgp_mdelay(100);

	struct ehci_dbg_port *port = (void *)(uintptr_t)info->ehci_debug;
	ret = dbgp_probe_gadget(port, &info->ep_pipe[0]);
	if (ret < 0) {
		dprintk(BIOS_INFO, "Could not probe gadget on debug port.\n");
		ret = -6;
		goto err;
	}

	info->ep_pipe[0].status	&= ~DBGP_EP_NOT_PRESENT;

	return 0;
err:
	/* Things didn't work so remove my claim */
	ctrl = read32(&ehci_debug->control);
	ctrl &= ~(DBGP_CLAIM | DBGP_OUT);
	write32(&ehci_debug->control, ctrl);
	//return ret;

next_debug_port:
	if (CONFIG_USBDEBUG_DEFAULT_PORT == 0) {
		port_map_tried |= (1 << (debug_port - 1));
		new_debug_port = ((debug_port-1 + 1) % n_ports) + 1;
		if (port_map_tried != ((1 << n_ports) - 1)) {
			ehci_debug_select_port(new_debug_port);
			goto try_next_port;
		}
		if (--playtimes) {
			ehci_debug_select_port(new_debug_port);
			goto try_next_time;
		}
	} else {
		if (--playtimes)
			goto try_next_time;
	}

	return ret;
}

static int dbgp_enabled(void)
{
	struct dbgp_pipe *globals = &dbgp_ehci_info()->ep_pipe[DBGP_SETUP_EP0];
	return (globals->status & DBGP_EP_ENABLED);
}

static int dbgp_not_present(void)
{
	struct dbgp_pipe *globals = &dbgp_ehci_info()->ep_pipe[DBGP_SETUP_EP0];
	return (globals->status & DBGP_EP_NOT_PRESENT);
}

int dbgp_try_get(struct dbgp_pipe *pipe)
{
	struct dbgp_pipe *globals = &dbgp_ehci_info()->ep_pipe[DBGP_SETUP_EP0];
	if (!dbgp_ep_is_active(pipe) || (globals->status & DBGP_EP_BUSY))
		return 0;
	globals->status |= DBGP_EP_BUSY;
	pipe->status |= DBGP_EP_BUSY;
	return 1;
}

void dbgp_put(struct dbgp_pipe *pipe)
{
	struct dbgp_pipe *globals = &dbgp_ehci_info()->ep_pipe[DBGP_SETUP_EP0];
	globals->status &= ~DBGP_EP_BUSY;
	pipe->status &= ~DBGP_EP_BUSY;
}

#if ENV_RAMSTAGE
void usbdebug_re_enable(uintptr_t ehci_base)
{
	struct ehci_debug_info *dbg_info = dbgp_ehci_info();
	u64 diff;
	int i;

	diff = dbg_info->ehci_base - ehci_base;
	dbg_info->ehci_debug -= diff;
	dbg_info->ehci_base = ehci_base;

	for (i=0; i<DBGP_MAX_ENDPOINTS; i++)
		if (dbg_info->ep_pipe[i].status & DBGP_EP_VALID)
			dbg_info->ep_pipe[i].status |= DBGP_EP_ENABLED;
}

void usbdebug_disable(void)
{
	struct ehci_debug_info *dbg_info = dbgp_ehci_info();
	int i;
	for (i=0; i<DBGP_MAX_ENDPOINTS; i++)
		dbg_info->ep_pipe[i].status &= ~DBGP_EP_ENABLED;
}

#endif

int usbdebug_hw_init(bool force)
{
	struct ehci_debug_info *dbg_info = dbgp_ehci_info();
	u32 ehci_base, dbg_offset;

	if (dbgp_enabled() && !force)
		return 0;

	if (dbgp_not_present() && !force)
		return -1;

	/* Do not attempt slow gadget init in postcar. */
	if (ENV_POSTCAR)
		return -1;

	/* Do full init if state claims we are still not enabled. */
	if (ehci_debug_hw_enable(&ehci_base, &dbg_offset))
		return -1;
	return usbdebug_init_(ehci_base, dbg_offset, dbg_info);
}

static void migrate_ehci_debug(int is_recovery)
{
	struct ehci_debug_info *dbg_info_cbmem;
	int rv;

	if (ENV_CREATES_CBMEM) {
		/* Move state from CAR to CBMEM. */
		struct ehci_debug_info *dbg_info = dbgp_ehci_info();
		dbg_info_cbmem = cbmem_add(CBMEM_ID_EHCI_DEBUG,
					sizeof(*dbg_info));
		if (dbg_info_cbmem == NULL)
			return;
		memcpy(dbg_info_cbmem, dbg_info, sizeof(*dbg_info));
		glob_dbg_info_p = dbg_info_cbmem;
		return;
	}

	if (CONFIG(USBDEBUG_IN_PRE_RAM)) {
		/* Use state in CBMEM. */
		dbg_info_cbmem = cbmem_find(CBMEM_ID_EHCI_DEBUG);
		if (dbg_info_cbmem)
			glob_dbg_info_p = dbg_info_cbmem;
	}

	rv = usbdebug_hw_init(false);
	if (rv < 0)
		printk(BIOS_DEBUG, "usbdebug: Failed hardware init\n");
	else
		printk(BIOS_DEBUG, "usbdebug: " ENV_STRING " starting...\n");
}

CBMEM_READY_HOOK(migrate_ehci_debug);

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

void usbdebug_init(void)
{
	/* USB console init is done early in romstage, yet delayed to
	 * CBMEM_READY_HOOKs for postcar and ramstage as we recover state
	 * from CBMEM.
	 */
	if (CONFIG(USBDEBUG_IN_PRE_RAM)
	    && (ENV_ROMSTAGE || ENV_BOOTBLOCK))
		usbdebug_hw_init(false);

	/* USB console init is done early in ramstage if it was
	 * not done in romstage, this does not require CBMEM.
	 */
	if (!CONFIG(USBDEBUG_IN_PRE_RAM) && ENV_RAMSTAGE)
		usbdebug_hw_init(false);
}
