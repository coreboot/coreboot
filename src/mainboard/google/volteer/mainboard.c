/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <drivers/tpm/cr50.h>
#include <ec/ec.h>
#include <fw_config.h>
#include <gpio.h>
#include <intelblocks/gpio.h>
#include <security/tpm/tss.h>
#include <intelblocks/tcss.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <variant/gpio.h>
#include <vb2_api.h>

#include "drivers/intel/pmc_mux/conn/chip.h"

WEAK_DEV_PTR(conn1);

static void typec_orientation_fixup(void)
{
	const struct device *conn = DEV_PTR(conn1);

	if (!is_dev_enabled(conn))
		return;

	if (fw_config_probe(FW_CONFIG(DB_USB, USB4_GEN2))
	    || fw_config_probe(FW_CONFIG(DB_USB, USB3_ACTIVE))
	    || fw_config_probe(FW_CONFIG(DB_USB, USB4_GEN3))
	    || fw_config_probe(FW_CONFIG(DB_USB, USB3_NO_A))) {
		struct drivers_intel_pmc_mux_conn_config *config = conn->chip_info;

		if (config) {
			printk(BIOS_INFO,
			       "Configure Right Type-C port orientation for retimer\n");
			config->sbu_orientation = TYPEC_ORIENTATION_NORMAL;
		}
	}
}

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
	typec_orientation_fixup();
	variant_devtree_update();
}

void __weak variant_devtree_update(void)
{
}

void __weak variant_ramstage_init(void)
{
	/* Default weak implementation */
}

static void add_fw_config_oem_string(const struct fw_config *config, void *arg)
{
	struct smbios_type11 *t;
	char buffer[64];

	t = (struct smbios_type11 *)arg;

	snprintf(buffer, sizeof(buffer), "%s-%s", config->field_name, config->option_name);
	t->count = smbios_add_string(t->eos, buffer);
}

static void mainboard_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	fw_config_for_each_found(add_fw_config_oem_string, t);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_strings = mainboard_smbios_strings;

	variant_ramstage_init();
}

void mainboard_update_soc_chip_config(struct soc_intel_tigerlake_config *cfg)
{
	int ret;
	if (!CONFIG(MAINBOARD_HAS_SPI_TPM_CR50)) {
		/*
		 * Negotiation of long interrupt pulses is only supported via SPI.  I2C is only
		 * used on reworked prototypes on which the TPM is replaced with Dauntless under
		 * development, it will use long pulses by default, or use the interrupt line in
		 * a different way altogether.
		 */
		return;
	}

	ret = tlcl_lib_init();
	if (ret != VB2_SUCCESS) {
		printk(BIOS_ERR, "tlcl_lib_init() failed: 0x%x\n", ret);
		return;
	}

	if (cr50_is_long_interrupt_pulse_enabled()) {
		printk(BIOS_INFO, "Enabling S0i3.4\n");
	} else {
		/*
		 * Disable S0i3.4, preventing the GPIO block from switching to
		 * slow clock.
		 */
		printk(BIOS_INFO, "Not enabling S0i3.4\n");
		cfg->LpmStateDisableMask |= LPM_S0i3_4;
		cfg->gpio_override_pm = 1;
		memset(cfg->gpio_pm, 0, sizeof(cfg->gpio_pm));
	}
}

static void mainboard_chip_init(void *chip_info)
{
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	size_t base_num, override_num;

	base_pads = variant_base_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);

	gpio_configure_pads_with_override(base_pads, base_num, override_pads, override_num);

	/*
	 * Check SATAXPCIE1 (GPP_A12) RX status to determine if SSD is NVMe or SATA and set
	 * the IOSSTATE RX field to drive 0 or 1 back to the internal controller to ensure
	 * the attached device is not mis-detected on resume from S0ix.
	 */
	if (gpio_get(GPP_A12)) {
		const struct pad_config gpio_pedet_nvme[] = {
			PAD_CFG_NF_IOSSTATE(GPP_A12, NONE, DEEP, NF1, HIZCRx1),
		};
		gpio_configure_pads(gpio_pedet_nvme, ARRAY_SIZE(gpio_pedet_nvme));
		printk(BIOS_INFO, "SATAXPCIE1 indicates PCIe NVMe is present\n");
	} else {
		const struct pad_config gpio_pedet_sata[] = {
			PAD_CFG_NF_IOSSTATE(GPP_A12, NONE, DEEP, NF1, HIZCRx0),
		};
		gpio_configure_pads(gpio_pedet_sata, ARRAY_SIZE(gpio_pedet_sata));
		printk(BIOS_INFO, "SATAXPCIE1 indicates SATA SSD is present\n");
	}
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
};
