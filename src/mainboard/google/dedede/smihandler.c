/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/smm.h>
#include <elog.h>
#include <gpio.h>
#include <intelblocks/gpio.h>
#include <intelblocks/smihandler.h>
#include <spi_flash.h>
#include <variant/ec.h>

void mainboard_smi_gpi_handler(const struct gpi_status *sts)
{
	/* TODO: Process SMI events from GPI */
}

void mainboard_smi_sleep(u8 slp_typ)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_sleep_gpio_table(&num);
	gpio_configure_pads(pads, num);

	variant_smi_sleep(slp_typ);

	chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS,
				MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS,
				MAINBOARD_EC_SMI_EVENTS);
	return 0;
}

void elog_gsmi_cb_mainboard_log_wake_source(void)
{
	google_chromeec_log_events(MAINBOARD_EC_LOG_EVENTS | MAINBOARD_EC_S0IX_WAKE_EVENTS);
}

void mainboard_smi_espi_handler(void)
{
	chromeec_smi_process_events();
}

void __weak variant_smi_sleep(u8 slp_typ)
{
}

static void mainboard_config_cbi_wp(void)
{
	int hw_wp = gpio_get(GPIO_PCH_WP);
	const struct spi_flash *spi_flash_dev = boot_device_spi_flash();
	uint8_t sr1;
	int rv;

	/*
	 * The CBI EEPROM WP should mirror our software write protect status if
	 * hardware write protect is set. If software write protect status is
	 * set at all via status register 1, that should be a sufficient signal.
	 * If the hardware WP is not set, or software write protect is not set
	 * while hardware write protect is set, deassert the CBI EEPROM WP.
	 *
	 * HW WP | SW WP | CBI WP
	 * ------|-------|-------
	 *   0   |   X   |   0
	 *   1   |   0   |   0
	 *   1   |   1   |   1
	 */
	if (spi_flash_status(spi_flash_dev, &sr1) < 0) {
		printk(BIOS_ERR, "MB: Failed to read SPI status register 1\n");
		printk(BIOS_ERR, "MB: CBI EEPROM WP cannot change!");
		return;
	}

	/*
	 * Note that we are assuming that the Status Register protect bits
	 * are located at this index and that 1 means hardware protected.  This
	 * should be the case for these boards.
	 */
	const bool is_wp = !!(sr1 & 0x80) && hw_wp;
	printk(BIOS_INFO, "MB: SPI flash is %swrite protected\n",
	       is_wp ? "" : "not ");

	/* Inverted because the signal is active low. */
	gpio_set(GPP_B16, !is_wp);

	/* Lock the configuration down. */
	rv = gpio_lock_pad(GPP_B16, GPIO_LOCK_FULL);
	if (rv)
		printk(BIOS_ERR, "MB: Failed to lock CBI WP (rv=%d)\n",
		       rv);
}

void mainboard_smi_finalize(void)
{
	if (CONFIG(BOARD_GOOGLE_BASEBOARD_DEDEDE_TPM2))
		mainboard_config_cbi_wp();
}
