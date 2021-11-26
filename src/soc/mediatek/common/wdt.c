/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <device/mmio.h>
#include <console/console.h>
#include <soc/wdt.h>
#include <vendorcode/google/chromeos/chromeos.h>

__weak void mtk_wdt_clr_status(uint32_t wdt_sta) { /* do nothing */ }

int mtk_wdt_init(void)
{
	uint32_t wdt_sta;

	/* Writing mode register will clear status register */
	wdt_sta = read32(&mtk_wdt->wdt_status);

	mtk_wdt_clr_status(wdt_sta);

	printk(BIOS_INFO, "WDT: Status = %#x\n", wdt_sta);

	printk(BIOS_INFO, "WDT: Last reset was ");
	if (wdt_sta & MTK_WDT_STA_HW_RST) {
		printk(BIOS_INFO, "hardware watchdog\n");
		mark_watchdog_tombstone();

		/*
		 * We trigger secondary reset by triggering WDT hardware to send signal to EC.
		 * We do not use do_board_reset() to send signal to EC
		 * which is controlled by software driver.
		 * Before triggering secondary reset, clean the data cache so the logs in cbmem
		 * console (either in SRAM or DRAM) can be flushed.
		 */
		dcache_clean_all();
		write32(&mtk_wdt->wdt_mode, MTK_WDT_MODE_EXTEN | MTK_WDT_MODE_KEY);
		write32(&mtk_wdt->wdt_swrst, MTK_WDT_SWRST_KEY);
	} else if (wdt_sta & MTK_WDT_STA_SW_RST)
		printk(BIOS_INFO, "normal software reboot\n");
	else if (wdt_sta & MTK_WDT_STA_SPM_RST)
		printk(BIOS_INFO, "SPM reboot\n");
	else if (!wdt_sta)
		printk(BIOS_INFO, "cold boot\n");
	else
		printk(BIOS_INFO, "unexpected reset type: %#.8x\n", wdt_sta);

	/* Config watchdog reboot mode:
	 * Clearing bits:
	 * DUAL_MODE & IRQ: trigger reset instead of irq then reset.
	 * EXT_POL: select watchdog output signal as active low.
	 * ENABLE: disable watchdog on initialization.
	 * Setting bit EXTEN to enable watchdog output.
	 */
	clrsetbits32(&mtk_wdt->wdt_mode,
		     MTK_WDT_MODE_DUAL_MODE | MTK_WDT_MODE_IRQ |
		     MTK_WDT_MODE_EXT_POL | MTK_WDT_MODE_ENABLE,
		     MTK_WDT_MODE_EXTEN | MTK_WDT_MODE_KEY);

	return wdt_sta;
}
