/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <device/mmio.h>
#include <soc/usb.h>

#define REG_SPM_POWERON_CONFIG_EN          (void *)(SPM_BASE + 0x000)
#define REG_SPM_SSPM_PWR_CON               (void *)(SPM_BASE + 0x390)

void mtk_usb_prepare(void)
{
	/* power on SSUSB SRAM FIFO */
	setbits32(REG_SPM_POWERON_CONFIG_EN, 0xB160001);
	clrbits32(REG_SPM_SSPM_PWR_CON, 0x000001FF);
}
