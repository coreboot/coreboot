/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <intelblocks/oc_wdt.h>
#include <soc/iomap.h>
#include <types.h>

/* OC WDT configuration */
#define PCH_OC_WDT_CTL				(ACPI_BASE_ADDRESS + 0x54)
#define   PCH_OC_WDT_CTL_RLD			BIT(31)
#define   PCH_OC_WDT_CTL_ICCSURV_STS		BIT(25)
#define   PCH_OC_WDT_CTL_NO_ICCSURV_STS		BIT(24)
#define   PCH_OC_WDT_CTL_FORCE_ALL		BIT(15)
#define   PCH_OC_WDT_CTL_EN			BIT(14)
#define   PCH_OC_WDT_CTL_ICCSURV		BIT(13)
#define   PCH_OC_WDT_CTL_LCK			BIT(12)
#define   PCH_OC_WDT_CTL_TOV_MASK		0x3FF

/*
 * Starts and reloads the OC watchdog with given timeout.
 *
 * timeout - Time in seconds before OC watchdog times out. Supported range = 70 - 1024
 */
static void oc_wdt_start(unsigned int timeout)
{
	uint32_t oc_wdt_ctrl;

	if (!CONFIG(SOC_INTEL_COMMON_OC_WDT_ENABLE))
		return;

	if ((timeout < 70) || (timeout > (PCH_OC_WDT_CTL_TOV_MASK + 1))) {
		timeout = CONFIG_SOC_INTEL_COMMON_OC_WDT_TIMEOUT_SECONDS;
		printk(BIOS_WARNING, "OC Watchdog: invalid timeout value,"
				     " using config default: %ds\n", timeout);
	}

	printk(BIOS_SPEW, "OC Watchdog: start and relaod timer (timeout %ds)\n", timeout);

	oc_wdt_ctrl = inl(PCH_OC_WDT_CTL);
	oc_wdt_ctrl |= (PCH_OC_WDT_CTL_EN | PCH_OC_WDT_CTL_FORCE_ALL | PCH_OC_WDT_CTL_ICCSURV);


	oc_wdt_ctrl &= ~PCH_OC_WDT_CTL_TOV_MASK;
	oc_wdt_ctrl |= (timeout - 1);
	oc_wdt_ctrl |= PCH_OC_WDT_CTL_RLD;

	outl(oc_wdt_ctrl, PCH_OC_WDT_CTL);
}

/* Checks if OC WDT is enabled and returns true if so, otherwise false. */
static bool is_oc_wdt_enabled(void)
{
	return (inl(PCH_OC_WDT_CTL) & PCH_OC_WDT_CTL_EN) ? true : false;
}

/* Reloads the OC watchdog (if enabled) preserving the current settings. */
void oc_wdt_reload(void)
{
	uint32_t oc_wdt_ctrl;

	/* Reload only works if OC WDT enable bit is set */
	if (!is_oc_wdt_enabled())
		return;

	oc_wdt_ctrl = inl(PCH_OC_WDT_CTL);
	/* Unset write-1-to-clear bits and preserve other settings */
	oc_wdt_ctrl &= ~(PCH_OC_WDT_CTL_ICCSURV_STS | PCH_OC_WDT_CTL_NO_ICCSURV_STS);
	oc_wdt_ctrl |= PCH_OC_WDT_CTL_RLD;
	outl(oc_wdt_ctrl, PCH_OC_WDT_CTL);
}

/* Disables the OC WDT. */
static void oc_wdt_disable(void)
{
	uint32_t oc_wdt_ctrl;

	printk(BIOS_INFO, "OC Watchdog: disabling watchdog timer\n");

	oc_wdt_ctrl = inl(PCH_OC_WDT_CTL);
	oc_wdt_ctrl &= ~(PCH_OC_WDT_CTL_EN | PCH_OC_WDT_CTL_FORCE_ALL);
	outl(oc_wdt_ctrl, PCH_OC_WDT_CTL);
}

/* Returns currently programmed OC watchdog timeout in seconds */
unsigned int oc_wdt_get_current_timeout(void)
{
	return (inl(PCH_OC_WDT_CTL) & PCH_OC_WDT_CTL_TOV_MASK) + 1;
}

/* Starts and reloads the OC watchdog if enabled in Kconfig */
void setup_oc_wdt(void)
{
	if (CONFIG(SOC_INTEL_COMMON_OC_WDT_ENABLE)) {
		oc_wdt_start(CONFIG_SOC_INTEL_COMMON_OC_WDT_TIMEOUT_SECONDS);
		if (is_oc_wdt_enabled())
			printk(BIOS_DEBUG, "OC Watchdog enabled\n");
		else
			printk(BIOS_ERR, "Failed to enable OC watchdog\n");
	} else {
		oc_wdt_disable();
	}
}
