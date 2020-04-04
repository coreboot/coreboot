/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <soc/pll.h>
#include <soc/usb.h>

void mtk_usb_prepare(void)
{
	mt_pll_enable_ssusb_clk();
}
