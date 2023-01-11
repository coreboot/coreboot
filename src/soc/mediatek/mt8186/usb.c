/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.5
 */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/pll_common.h>
#include <soc/usb.h>

#define PERI_USB_WAKEUP_DEC_CON1	0x404
#define PERI_U3_WAKE_CTRL0		0x420

void mtk_usb_prepare(void)
{
	mt_pll_set_usb_clock();

	gpio_output(GPIO(USB_DRVVBUS_P1), 1);

	/* disable IP0 debounce */
	write32p(PERICFG_BASE + PERI_U3_WAKE_CTRL0, 0);
	/* disable IP1 debounce */
	write32p(PERICFG_BASE + PERI_USB_WAKEUP_DEC_CON1, 0);
}
