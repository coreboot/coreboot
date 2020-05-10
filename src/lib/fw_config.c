/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * fw_config_get() - Provide firmware configuration value.
 *
 * Return 32bit firmware configuration value determined for the system.
 */
static uint32_t fw_config_get(void)
{
	static uint32_t fw_config_value;
	static bool fw_config_value_initialized;

	/* Nothing to prepare if setup is already done. */
	if (fw_config_value_initialized)
		return fw_config_value;
	fw_config_value_initialized = true;

	/* Look in CBFS to allow override of value. */
	if (CONFIG(FW_CONFIG_SOURCE_CBFS)) {
		if (cbfs_boot_load_file(CONFIG_CBFS_PREFIX "/fw_config",
					&fw_config_value, sizeof(fw_config_value),
					CBFS_TYPE_RAW) != sizeof(fw_config_value)) {
			printk(BIOS_WARNING, "%s: Could not get fw_config from CBFS\n",
			       __func__);
			fw_config_value = 0;
		} else {
			printk(BIOS_INFO, "FW_CONFIG value from CBFS is 0x%08x\n",
			       fw_config_value);
			return fw_config_value;
		}
	}

	/* Read the value from EC CBI. */
	if (CONFIG(FW_CONFIG_SOURCE_CHROMEEC_CBI)) {
		if (google_chromeec_cbi_get_fw_config(&fw_config_value))
			printk(BIOS_WARNING, "%s: Could not get fw_config from EC\n", __func__);
	}

	printk(BIOS_INFO, "FW_CONFIG value is 0x%08x\n", fw_config_value);
	return fw_config_value;
}

bool fw_config_probe(const struct fw_config *match)
{
	/* Compare to system value. */
	if ((fw_config_get() & match->mask) == match->value) {
		if (match->field_name && match->option_name)
			printk(BIOS_INFO, "fw_config match found: %s=%s\n", match->field_name,
			       match->option_name);
		else
			printk(BIOS_INFO, "fw_config match found: mask=0x%08x value=0x%08x\n",
			       match->mask, match->value);
		return true;
	}

	return false;
}

#if ENV_RAMSTAGE
static void fw_config_init(void *unused)
{
	struct device *dev;

	for (dev = all_devices; dev; dev = dev->next) {
		const struct fw_config *probe;
		bool match = false;

		if (!dev->probe_list)
			continue;

		for (probe = dev->probe_list; probe && probe->mask != 0; probe++) {
			if (fw_config_probe(probe)) {
				match = true;
				break;
			}
		}

		if (!match) {
			printk(BIOS_INFO, "%s disabled by fw_config\n", dev_path(dev));
			dev->enabled = 0;
		}
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_ENTRY, fw_config_init, NULL);
#endif
