/*
 *
 * Copyright (C) 2013 secunet Security Networks AG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <inttypes.h>
#include <arch/virtual.h>
#include "xhci_private.h"

trb_t *
xhci_next_command_trb(xhci_t *const xhci)
{
	xhci_clear_trb(xhci->cr.cur, xhci->cr.pcs);
	return xhci->cr.cur;
}

void
xhci_post_command(xhci_t *const xhci)
{
	xhci_debug("Command %d (@%p)\n",
		   TRB_GET(TT, xhci->cr.cur), xhci->cr.cur);

	TRB_SET(C, xhci->cr.cur, xhci->cr.pcs);
	++xhci->cr.cur;

	/* pass command trb to hardware */
	wmb();
	/* Ring the doorbell */
	xhci->dbreg[0] = 0;

	while (TRB_GET(TT, xhci->cr.cur) == TRB_LINK) {
		xhci_debug("Handling LINK pointer (@%p)\n", xhci->cr.cur);
		const int tc = TRB_GET(TC, xhci->cr.cur);
		TRB_SET(C, xhci->cr.cur, xhci->cr.pcs);
		xhci->cr.cur = phys_to_virt(xhci->cr.cur->ptr_low);
		if (tc)
			xhci->cr.pcs ^= 1;
	}
}

static int
xhci_wait_for_command(xhci_t *const xhci,
		      const trb_t *const cmd_trb,
		      const int clear_event)
{
	int cc;

	cc = xhci_wait_for_command_done(xhci, cmd_trb, clear_event);
	if (cc != TIMEOUT)
		return cc;

	/* Abort command on timeout */
	xhci_debug("Aborting command (@%p), CRCR: 0x%"PRIx32"\n",
		   cmd_trb, xhci->opreg->crcr_lo);
	/*
	 * Ref. xHCI Specification Revision 1.2, May 2019.
	 * Section 5.4.5, Table 5-24.
	 *
	 * Abort the command and stop the ring.
	 */
	xhci->opreg->crcr_lo |= CRCR_CA;
	xhci->opreg->crcr_hi = 0;
	cc = xhci_wait_for_command_aborted(xhci, cmd_trb);

	if (xhci->opreg->crcr_lo & CRCR_CRR)
		fatal("xhci_wait_for_command: Command ring still running\n");

	return cc;
}

/*
 * xhci_cmd_* return >= 0: xhci completion code (cc)
 *		      < 0: driver error code
 */

int
xhci_cmd_enable_slot(xhci_t *const xhci, int *const slot_id)
{
	trb_t *const cmd = xhci_next_command_trb(xhci);
	TRB_SET(TT, cmd, TRB_CMD_ENABLE_SLOT);
	xhci_post_command(xhci);

	int cc = xhci_wait_for_command(xhci, cmd, 0);
	if (cc >= 0) {
		if (cc == CC_SUCCESS) {
			*slot_id = TRB_GET(ID, xhci->er.cur);
			if (*slot_id > xhci->max_slots_en)
				cc = CONTROLLER_ERROR;
		}
		xhci_advance_event_ring(xhci);
		xhci_handle_events(xhci);
	}
	return cc;
}

int
xhci_cmd_disable_slot(xhci_t *const xhci, const int slot_id)
{
	trb_t *const cmd = xhci_next_command_trb(xhci);
	TRB_SET(TT, cmd, TRB_CMD_DISABLE_SLOT);
	TRB_SET(ID, cmd, slot_id);
	xhci_post_command(xhci);

	return xhci_wait_for_command(xhci, cmd, 1);
}

int
xhci_cmd_address_device(xhci_t *const xhci,
			const int slot_id,
			inputctx_t *const ic)
{
	trb_t *const cmd = xhci_next_command_trb(xhci);
	TRB_SET(TT, cmd, TRB_CMD_ADDRESS_DEV);
	TRB_SET(ID, cmd, slot_id);
	cmd->ptr_low = virt_to_phys(ic->raw);
	xhci_post_command(xhci);

	return xhci_wait_for_command(xhci, cmd, 1);
}

int
xhci_cmd_configure_endpoint(xhci_t *const xhci,
			    const int slot_id,
			    const int config_id,
			    inputctx_t *const ic)
{
	trb_t *const cmd = xhci_next_command_trb(xhci);
	TRB_SET(TT, cmd, TRB_CMD_CONFIGURE_EP);
	TRB_SET(ID, cmd, slot_id);
	cmd->ptr_low = virt_to_phys(ic->raw);
	if (config_id == 0)
		TRB_SET(DC, cmd, 1);
	xhci_post_command(xhci);

	return xhci_wait_for_command(xhci, cmd, 1);
}

int
xhci_cmd_evaluate_context(xhci_t *const xhci,
			  const int slot_id,
			  inputctx_t *const ic)
{
	trb_t *const cmd = xhci_next_command_trb(xhci);
	TRB_SET(TT, cmd, TRB_CMD_EVAL_CTX);
	TRB_SET(ID, cmd, slot_id);
	cmd->ptr_low = virt_to_phys(ic->raw);
	xhci_post_command(xhci);

	return xhci_wait_for_command(xhci, cmd, 1);
}

int
xhci_cmd_reset_endpoint(xhci_t *const xhci, const int slot_id, const int ep)
{
	trb_t *const cmd = xhci_next_command_trb(xhci);
	TRB_SET(TT, cmd, TRB_CMD_RESET_EP);
	TRB_SET(ID, cmd, slot_id);
	TRB_SET(EP, cmd, ep);
	xhci_post_command(xhci);

	return xhci_wait_for_command(xhci, cmd, 1);
}

int
xhci_cmd_stop_endpoint(xhci_t *const xhci, const int slot_id, const int ep)
{
	trb_t *const cmd = xhci_next_command_trb(xhci);
	TRB_SET(TT, cmd, TRB_CMD_STOP_EP);
	TRB_SET(ID, cmd, slot_id);
	TRB_SET(EP, cmd, ep);
	xhci_post_command(xhci);

	return xhci_wait_for_command(xhci, cmd, 1);
}

int
xhci_cmd_set_tr_dq(xhci_t *const xhci, const int slot_id, const int ep,
		   trb_t *const dq_trb, const int dcs)
{
	trb_t *const cmd = xhci_next_command_trb(xhci);
	TRB_SET(TT, cmd, TRB_CMD_SET_TR_DQ);
	TRB_SET(ID, cmd, slot_id);
	TRB_SET(EP, cmd, ep);
	cmd->ptr_low = virt_to_phys(dq_trb) | dcs;
	xhci_post_command(xhci);

	return xhci_wait_for_command(xhci, cmd, 1);
}
