/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EHCI_DEBUG_H_
#define _EHCI_DEBUG_H_

#include <types.h>

void usbdebug_re_enable(uintptr_t ehci_base);
void usbdebug_disable(void);

/* Returns 0 on success and sets MMIO base and dbg_offset if EHCI debug
 * capability was found and enabled. Returns non-zero on error.
 */
int ehci_debug_hw_enable(unsigned int *base, unsigned int *dbg_offset);
void ehci_debug_select_port(unsigned int port);

#define DBGP_EP_VALID		(1<<0)
#define DBGP_EP_ENABLED		(1<<1)
#define DBGP_EP_BUSY		(1<<2)
#define DBGP_EP_NOT_PRESENT	(1<<3)
#define DBGP_EP_STATMASK	(DBGP_EP_VALID | DBGP_EP_ENABLED)

#define DBGP_MAX_ENDPOINTS	4
#define DBGP_SETUP_EP0		0	/* Compulsory endpoint 0. */
#define DBGP_CONSOLE_EPOUT	1
#define DBGP_CONSOLE_EPIN	2

struct ehci_dbg_port;

struct dbgp_pipe
{
	u8 devnum;
	u8 endpoint;
	u8 pid;
	u8 status;
	u16 timeout;

	u8 bufidx;
	u8 buflen;
	char buf[8];
} __packed;

void dbgp_put(struct dbgp_pipe *pipe);
int dbgp_try_get(struct dbgp_pipe *pipe);

struct dbgp_pipe *dbgp_console_output(void);
struct dbgp_pipe *dbgp_console_input(void);
int dbgp_ep_is_active(struct dbgp_pipe *pipe);
int dbgp_bulk_write_x(struct dbgp_pipe *pipe, const char *bytes, int size);
int dbgp_bulk_read_x(struct dbgp_pipe *pipe, void *data, int size);

int dbgp_control_msg(struct ehci_dbg_port *ehci_debug, unsigned int devnum,
	int requesttype, int request, int value, int index, void *data, int size);
void dbgp_mdelay(int ms);

int dbgp_probe_gadget(struct ehci_dbg_port *ehci_debug, struct dbgp_pipe *pipe);

#endif /* _EHCI_DEBUG_H_ */
