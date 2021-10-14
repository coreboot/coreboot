/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <bootblock_common.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <timer.h>

#define FC350_PCIE_INIT_DELAY_US (20 * USECS_PER_MSEC)
struct stopwatch pcie_init_timeout_sw;

void mb_set_up_early_espi(void)
{
	/*
	 * We don't need to initialize all of the GPIOs that are done
	 * in bootblock_mainboard_early_init(), but we need to release
	 * the EC eSPI reset and do the rest of the configuration.
	 *
	 * This will not be present in the normal boot flow.
	 */
	bootblock_mainboard_early_init();
}

void bootblock_mainboard_early_init(void)
{
	uint32_t dword;
	size_t base_num_gpios, override_num_gpios;
	const struct soc_amd_gpio *base_gpios, *override_gpios;

	/* Beware that the bit definitions for LPC_LDRQ0_PU_EN and LPC_LDRQ0_PD_EN are swapped
	   on Picasso and older compared to Renoir/Cezanne and newer */
	dword = pci_read_config32(SOC_LPC_DEV, LPC_MISC_CONTROL_BITS);
	dword &= ~(LPC_LDRQ0_PU_EN | LPC_LDRQ1_EN | LPC_LDRQ0_EN);
	dword |= LPC_LDRQ0_PD_EN;
	pci_write_config32(SOC_LPC_DEV, LPC_MISC_CONTROL_BITS, dword);

	/*
	 * All LPC decodes need to be cleared before we can configure the LPC pads as secondary
	 * eSPI interface that gets used for the EC communication. This is already done by
	 * lpc_disable_decodes that gets called before this function.
	 */

	if (CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK))
		return;

	base_gpios = variant_early_gpio_table(&base_num_gpios);
	override_gpios = variant_early_override_gpio_table(&override_num_gpios);

	gpio_configure_pads_with_override(base_gpios, base_num_gpios,
			override_gpios, override_num_gpios);

	/* Set a timer to make sure there's enough delay for
	 * the Fibocom 350 PCIe init
	 */
	stopwatch_init_usecs_expire(&pcie_init_timeout_sw, FC350_PCIE_INIT_DELAY_US);

	/* Early eSPI interface configuration */

	dword = pm_read32(PM_SPI_PAD_PU_PD);
	dword |= PM_ESPI_CS_USE_DATA2;
	pm_write32(PM_SPI_PAD_PU_PD, dword);

	/* Switch the pads that can be used as either LPC or secondary eSPI to 1.8V mode */
	dword = pm_read32(PM_ACPI_CONF);
	dword |= PM_ACPI_S5_LPC_PIN_MODE | PM_ACPI_S5_LPC_PIN_MODE_SEL;
	pm_write32(PM_ACPI_CONF, dword);
}

void bootblock_mainboard_init(void)
{
	size_t base_num_gpios, override_num_gpios;
	const struct soc_amd_gpio *base_gpios, *override_gpios;
	int i = 0;

	/* Make sure that at least 20ms has elapsed since enabling WWAN power
	 * in bootblock_mainboard_early_init.
	 * This is only applicable if verstage is not in the PSP and the board
	 * is using the fibocom 350 WLAN card, so this typically will not be hit.
	 */
	if (!CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK) && variant_has_pcie_wwan()) {
		while (!stopwatch_expired(&pcie_init_timeout_sw)) {
			mdelay(1);
			i++;
		};
		if (i)
			printk(BIOS_DEBUG, "Delayed %d ms for PCIe\n", i);
	}

	base_gpios = variant_bootblock_gpio_table(&base_num_gpios);
	override_gpios = variant_bootblock_override_gpio_table(&override_num_gpios);

	gpio_configure_pads_with_override(base_gpios, base_num_gpios, override_gpios,
					  override_num_gpios);

	/* FPMCU check needs to happen after EC initialization for FW_CONFIG bits */
	if (variant_has_fpmcu())
		variant_fpmcu_reset();
}
