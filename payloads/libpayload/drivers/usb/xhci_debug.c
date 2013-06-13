/*
 * This file is part of the libpayload project.
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
#include "xhci_private.h"

#ifdef XHCI_DUMPS

void
xhci_dump_slotctx(const slotctx_t *const sc)
{
	xhci_debug("Slot Context (@%p):\n", sc);
	usb_debug(" FIELD1\t0x%08"PRIx32"\n", sc->f1);
	usb_debug(" FIELD2\t0x%08"PRIx32"\n", sc->f2);
	usb_debug(" FIELD3\t0x%08"PRIx32"\n", sc->f3);
	usb_debug(" FIELD4\t0x%08"PRIx32"\n", sc->f4);
	SC_DUMP(ROUTE,  *sc);
	SC_DUMP(SPEED,  *sc);
	SC_DUMP(MTT,    *sc);
	SC_DUMP(HUB,    *sc);
	SC_DUMP(CTXENT, *sc);
	SC_DUMP(RHPORT, *sc);
	SC_DUMP(NPORTS, *sc);
	SC_DUMP(TTID,   *sc);
	SC_DUMP(TTPORT, *sc);
	SC_DUMP(TTT,    *sc);
	SC_DUMP(UADDR,  *sc);
	SC_DUMP(STATE,  *sc);
}

void
xhci_dump_epctx(const epctx_t *const ec)
{
	xhci_debug("Endpoint Context (@%p):\n", ec);
	usb_debug(" FIELD1\t0x%08"PRIx32"\n", ec->f1);
	usb_debug(" FIELD2\t0x%08"PRIx32"\n", ec->f2);
	usb_debug(" TRDQ_L\t0x%08"PRIx32"\n", ec->tr_dq_low);
	usb_debug(" TRDQ_H\t0x%08"PRIx32"\n", ec->tr_dq_high);
	usb_debug(" FIELD5\t0x%08"PRIx32"\n", ec->f5);
	EC_DUMP(STATE,  *ec);
	EC_DUMP(INTVAL, *ec);
	EC_DUMP(CERR,   *ec);
	EC_DUMP(TYPE,   *ec);
	EC_DUMP(MBS,    *ec);
	EC_DUMP(MPS,    *ec);
	EC_DUMP(DCS,    *ec);
	EC_DUMP(AVRTRB, *ec);
	EC_DUMP(MXESIT, *ec);
}

void
xhci_dump_devctx(const devctx_t *const dc, const u32 ctx_mask)
{
	int i;
	if (ctx_mask & 1)
		xhci_dump_slotctx(&dc->slot);
	for (i = 0; i < SC_GET(CTXENT, dc->slot); ++i) {
		if (ctx_mask & (2 << i))
			xhci_dump_epctx(&dc->all_eps[i]);
	}
}

void
xhci_dump_inputctx(const inputctx_t *const ic)
{
	xhci_debug("Input Control  add: 0x%08"PRIx32"\n", ic->control.add);
	xhci_debug("Input Control drop: 0x%08"PRIx32"\n", ic->control.drop);
	xhci_dump_devctx(&ic->dev, ic->control.add);
}

void
xhci_dump_transfer_trb(const trb_t *const cur)
{
	xhci_debug("Transfer TRB (@%p):\n", cur);
	usb_debug(" PTR_L\t0x%08"PRIx32"\n", cur->ptr_low);
	usb_debug(" PTR_H\t0x%08"PRIx32"\n", cur->ptr_high);
	usb_debug(" STATUS\t0x%08"PRIx32"\n", cur->status);
	usb_debug(" CNTRL\t0x%08"PRIx32"\n", cur->control);
	TRB_DUMP(TL,	cur);
	TRB_DUMP(TDS,	cur);
	TRB_DUMP(C,	cur);
	TRB_DUMP(ISP,	cur);
	TRB_DUMP(CH,	cur);
	TRB_DUMP(IOC,	cur);
	TRB_DUMP(IDT,	cur);
	TRB_DUMP(TT,	cur);
	TRB_DUMP(DIR,	cur);
}

static const trb_t *
xhci_next_trb(const trb_t *const cur)
{
	if (TRB_GET(TT, cur) == TRB_LINK)
		return (!cur->ptr_low) ? NULL : phys_to_virt(cur->ptr_low);
	else
		return cur + 1;
}

void
xhci_dump_transfer_trbs(const trb_t *const first, const trb_t *const last)
{
	const trb_t *cur;
	for (cur = first; cur; cur = xhci_next_trb(cur)) {
		xhci_dump_transfer_trb(cur);
		if (cur == last)
			break;
	}
}

#endif
