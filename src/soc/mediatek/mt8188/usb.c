/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.5
 */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/infracfg.h>
#include <soc/pll_common.h>
#include <soc/usb.h>

#define TRAP_USB20_P1_USBD1_EN 0x10005600

static void mt_set_trapping_pin(void)
{
	/*
	 * MT8188 supports port0/port1 download. The hardware needs a trapping pin to
	 * select the port to use. When port1 is selected, the phy of port1 will be
	 * switched to port0. That is, port1 connector will be the physical line of
	 * port0. Since port0 phy isn't initialized in coreboot, switch back to port1
	 * phy.
	 */
	setbits32p(TRAP_USB20_P1_USBD1_EN, BIT(9));
}

void mtk_usb_prepare(void)
{
	mt_pll_set_usb_clock();
	mt_set_trapping_pin();
}
