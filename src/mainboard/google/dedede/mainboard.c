/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <drivers/tpm/cr50.h>
#include <ec/ec.h>
#include <security/tpm/tss.h>
#include <soc/soc_chip.h>
#include <static.h>
#include <timer.h>
#include <vb2_api.h>

static void mainboard_update_soc_chip_config(void)
{
	struct soc_intel_jasperlake_config *cfg = config_of_soc();
	tpm_result_t rc;

	rc = tlcl_lib_init();
	if (rc != TPM_SUCCESS) {
		printk(BIOS_ERR, "tlcl_lib_init() failed: %#x\n", rc);
		return;
	}

	if (!cr50_is_long_interrupt_pulse_enabled()) {
		/* Disable GPIO PM to allow for shorter IRQ pulses */
		printk(BIOS_INFO, "Override GPIO PM\n");
		cfg->gpio_override_pm = 1;
		memset(cfg->gpio_pm, 0, sizeof(cfg->gpio_pm));
	}
}

static bool any_hpd_ready(const gpio_t *gpios, size_t num_gpios)
{
	for (size_t i = 0; i < num_gpios; i++) {
		if (gpio_get(gpios[i]))
			return true;
	}

	return false;
}

static void mainboard_wait_for_hpd(void)
{
	static const long display_timeout_ms = 3000;
	struct stopwatch sw;
	size_t num_gpios;
	const gpio_t *hpd_gpios = variant_hpd_gpios(&num_gpios);

	if (num_gpios == 0) {
		printk(BIOS_WARNING, "No HPD GPIOs, skip waiting\n");
		return;
	}

	printk(BIOS_INFO, "Waiting for HPD\n");

	/* Pins will be configured back by gpio_configure_pads. */
	for (size_t i = 0; i < num_gpios; i++) {
		gpio_input(hpd_gpios[i]);
	}

	stopwatch_init_msecs_expire(&sw, display_timeout_ms);
	while (!any_hpd_ready(hpd_gpios, num_gpios)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING,
			       "HPD not ready after %ld ms. Abort.\n",
			       display_timeout_ms);
			return;
		}
		mdelay(200);
	}
	printk(BIOS_INFO, "HPD ready after %lld ms\n",
	       stopwatch_duration_msecs(&sw));
}

static void mainboard_init(void *chip_info)
{
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	size_t base_num, override_num;

	/*
	 * For chromeboxes, wait for DP HPD to be asserted before
	 * entering FSP-S, otherwise display init may fail.
	 */
	if (!CONFIG(SYSTEM_TYPE_LAPTOP) && display_init_required())
		mainboard_wait_for_hpd();

	base_pads = baseboard_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);

	gpio_configure_pads_with_override(base_pads, base_num,
		override_pads, override_num);

	variant_devtree_update();

	if (CONFIG(BOARD_GOOGLE_BASEBOARD_DEDEDE_CR50))
		mainboard_update_soc_chip_config();
}

void __weak variant_devtree_update(void)
{
	/* Override dev tree settings per board */
}

static void mainboard_dev_init(struct device *dev)
{
	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(
		const struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
{
	return current;
}

static void mainboard_generate_s0ix_hook(void)
{
	acpigen_write_if_lequal_op_int(ARG0_OP, 1);
	variant_generate_s0ix_hook(S0IX_ENTRY);
	acpigen_write_else();
	variant_generate_s0ix_hook(S0IX_EXIT);
	acpigen_write_if_end();
}

static void mainboard_fill_ssdt(const struct device *dev)
{
	acpigen_write_scope("\\_SB");
	acpigen_write_method_serialized("MS0X", 1);
	mainboard_generate_s0ix_hook();
	acpigen_write_method_end(); /* Method */
	acpigen_write_scope_end(); /* Scope */
}

void __weak variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	/* Add board-specific MS0X entries */
	/*
	if (s0ix_entry == S0IX_ENTRY) {
		implement variant operations here
	}
	if (s0ix_entry == S0IX_EXIT) {
		implement variant operations here
	}
	*/
}


static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_dev_init;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
