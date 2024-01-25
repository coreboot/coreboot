/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 13.1,13.2
 */

#include <soc/addressmap.h>
#include <device/mmio.h>
#include <soc/usb.h>

#define CTRL_U3_PORT_SS_SUP_SPEED      (3U << 9)

void mtk_usb_prepare(void)
{
	/* SW sets this register to change USB3.2 speed to Gen1-5Gbps */
	struct ssusb_ippc_regs *ippc_regs = (void *)(SSUSB_IPPC_BASE);

	clrsetbits32(&ippc_regs->u3_ctrl_p[0], CTRL_U3_PORT_SS_SUP_SPEED, 0);
}
