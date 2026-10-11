/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/lib_helpers.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <stdint.h>

/*
 * TF-A arms the non-secure SBSA generic watchdog before it hands over to BL33,
 * with a timeout of NV_SI_WDT_BIOS_EXP_MINS minutes (default 5). BL33 has to
 * disable it once it booted successfully, otherwise the system is reset. See
 * edk2-platforms AmpereAltraPkg Drivers/FailSafeDxe.
 */

#define GWDT_WCS		0x00
#define  GWDT_WCS_EN		(1 << 0)
#define GWDT_WOR		0x08
#define GWDT_WCV		0x10

static void ac01_disable_watchdog(void *unused)
{
	void *const base = (void *)AC01_GWDT_CONTROL_BASE;
	const uint32_t wcs = read32(base + GWDT_WCS);

	if (!(wcs & GWDT_WCS_EN))
		return;

	const uint64_t wcv = read64(base + GWDT_WCV);
	const uint64_t now = raw_read_cntpct_el0();
	const uint64_t freq = raw_read_cntfrq_el0();

	printk(BIOS_DEBUG, "Generic watchdog: WCS 0x%x, timeout %llu s, WS0 in %lld s\n", wcs,
	       read32(base + GWDT_WOR) / freq, ((int64_t)(wcv - now)) / (int64_t)freq);

	write32(base + GWDT_WCS, wcs & ~GWDT_WCS_EN);

	printk(BIOS_INFO, "Generic watchdog disabled\n");
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, ac01_disable_watchdog, NULL);
