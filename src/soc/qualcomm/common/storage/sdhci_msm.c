/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <commonlib/sd_mmc_ctrlr.h>
#include <commonlib/sdhci.h>
#include <commonlib/storage/sdhci.h>
#include <delay.h>
#include <soc/sdhci_msm.h>

/*
 * msm specific SDHC initialization.  This is ported from depthcharge's
 * sdhci_msm_init().
 */
static int sdhci_msm_init(struct sdhci_ctrlr *host)
{
	uint32_t vendor_caps = 0;

	printk(BIOS_INFO, "Initializing SDHCI MSM host controller!\n");

	/* Read host controller capabilities */
	vendor_caps = sdhci_readl(host, SDHCI_CAPABILITIES);

	/*
	 * Explicitly enable the capabilities which are not advertised
	 * by default
	 */
	vendor_caps |= SDHCI_CAN_VDD_180 | SDHCI_CAN_DO_8BIT;

	/*
	 * Update internal capabilities register so that these updated values
	 * will get reflected in SDHCI_CAPABILITEIS register.
	 */
	sdhci_writel(host, vendor_caps, SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0);

	/*
	 * Reset the vendor spec register to power on reset state.
	 * This is to ensure that this register is set to right value
	 * incase if this register get updated by bootrom when using SDHCI boot.
	 */
	sdhci_writel(host, VENDOR_SPEC_FUN3_POR_VAL,
		     SDCC_HC_VENDOR_SPECIFIC_FUNC3);

	/*
	 * Set SD power off, otherwise reset will result in pwr irq.
	 * And without setting bus off status, reset would fail.
	 */
	sdhci_writeb(host, 0x0, SDHCI_POWER_CONTROL);
	udelay(10);

	return 0;
}


/*
 * This function is a wrapper around new_mem_sdhci_controller().  It initializes
 * the pre_init callback function to sdhci_msm_init(), which takes care of any
 * msm initialization before the actual sdhci initialization is executed.
 */
struct sd_mmc_ctrlr *new_sdhci_msm_host(void *ioaddr)
{
	struct sd_mmc_ctrlr *host;

	host = new_mem_sdhci_controller(ioaddr, sdhci_msm_init);
	if (host == NULL)
		printk(BIOS_ERR, "Error initializing SDHCI MSM host controller!\n");

	return host;
}
