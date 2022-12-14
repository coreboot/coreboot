/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bootstate.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <inttypes.h>
#include <lib.h>
#include <stdbool.h>
#include <stdint.h>
#include <drivers/vpd/vpd.h>

uint64_t fw_config_get(void)
{
	static uint64_t fw_config_value;
	static bool fw_config_value_initialized;

	/* Nothing to prepare if setup is already done. */
	if (fw_config_value_initialized)
		return fw_config_value;
	fw_config_value_initialized = true;
	fw_config_value = UNDEFINED_FW_CONFIG;

	/* Read the value from EC CBI. */
	if (CONFIG(FW_CONFIG_SOURCE_CHROMEEC_CBI)) {
		if (google_chromeec_cbi_get_fw_config(&fw_config_value))
			printk(BIOS_WARNING, "%s: Could not get fw_config from CBI\n",
				__func__);
		else
			printk(BIOS_INFO, "FW_CONFIG value from CBI is 0x%" PRIx64 "\n",
				fw_config_value);
	}

	/* Look in CBFS to allow override of value. */
	if (CONFIG(FW_CONFIG_SOURCE_CBFS) && fw_config_value == UNDEFINED_FW_CONFIG) {
		if (cbfs_load(CONFIG_CBFS_PREFIX "/fw_config", &fw_config_value,
			      sizeof(fw_config_value)) != sizeof(fw_config_value))
			printk(BIOS_WARNING, "%s: Could not get fw_config from CBFS\n",
				__func__);
		else
			printk(BIOS_INFO, "FW_CONFIG value from CBFS is 0x%" PRIx64 "\n",
				fw_config_value);
	}

	if (CONFIG(FW_CONFIG_SOURCE_VPD) && fw_config_value == UNDEFINED_FW_CONFIG) {
		int vpd_value;
		if (vpd_get_int("fw_config", VPD_RW_THEN_RO, &vpd_value)) {
			fw_config_value = vpd_value;
			printk(BIOS_INFO, "FW_CONFIG value from VPD is 0x%" PRIx64 "\n",
				fw_config_value);
		} else
			printk(BIOS_WARNING, "%s: Could not get fw_config from vpd\n",
				__func__);
	}

	return fw_config_value;
}

uint64_t fw_config_get_field(const struct fw_config_field *field)
{
	/* If fw_config is not provisioned, then there is nothing to get. */
	if (!fw_config_is_provisioned())
		return UNDEFINED_FW_CONFIG;

	int shift = __ffs64(field->mask);
	const uint64_t value = (fw_config_get() & field->mask) >> shift;

	printk(BIOS_INFO, "fw_config get field name=%s, mask=0x%" PRIx64 ", shift=%d, value=0x%"
		PRIx64 "\n", field->field_name, field->mask, shift, value);

	return value;
}

bool fw_config_probe(const struct fw_config *match)
{
	/* If fw_config is not provisioned, then there is nothing to match. */
	if (!fw_config_is_provisioned())
		return false;

	/* Compare to system value. */
	if ((fw_config_get() & match->mask) == match->value) {
		if (match->field_name && match->option_name)
			printk(BIOS_INFO, "fw_config match found: %s=%s\n", match->field_name,
			       match->option_name);
		else
			printk(BIOS_INFO, "fw_config match found: mask=0x%" PRIx64 " value=0x%"
			       PRIx64 "\n",
			       match->mask, match->value);
		return true;
	}

	return false;
}

bool fw_config_is_provisioned(void)
{
	return fw_config_get() != UNDEFINED_FW_CONFIG;
}

bool fw_config_probe_dev(const struct device *dev, const struct fw_config **matching_probe)
{
	const struct fw_config *probe;

	if (matching_probe)
		*matching_probe = NULL;

	/* If the device does not have a probe list, then probing is not required. */
	if (!dev->probe_list)
		return true;

	for (probe = dev->probe_list; probe && probe->mask != 0; probe++) {
		if (!fw_config_probe(probe))
			continue;

		if (matching_probe)
			*matching_probe = probe;
		return true;
	}

	return false;
}

#if ENV_RAMSTAGE

/*
 * The maximum number of fw_config fields is limited by the 64-bit mask that is used to
 * represent them.
 */
#define MAX_CACHE_ELEMENTS	(8 * sizeof(uint64_t))

static const struct fw_config *cached_configs[MAX_CACHE_ELEMENTS];

static size_t probe_index(uint64_t mask)
{
	assert(mask);
	return __ffs64(mask);
}

const struct fw_config *fw_config_get_found(uint64_t field_mask)
{
	const struct fw_config *config;
	config = cached_configs[probe_index(field_mask)];
	if (config && config->mask == field_mask)
		return config;

	return NULL;
}

void fw_config_for_each_found(void (*cb)(const struct fw_config *config, void *arg), void *arg)
{
	size_t i;

	for (i = 0; i < MAX_CACHE_ELEMENTS; ++i)
		if (cached_configs[i])
			cb(cached_configs[i], arg);
}

static void fw_config_init(void *unused)
{
	struct device *dev;

	for (dev = all_devices; dev; dev = dev->next) {
		const struct fw_config *probe;

		if (!fw_config_probe_dev(dev, &probe)) {
			printk(BIOS_INFO, "%s disabled by fw_config\n", dev_path(dev));
			dev->enabled = 0;
			continue;
		}

		if (probe)
			cached_configs[probe_index(probe->mask)] = probe;
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_ENTRY, fw_config_init, NULL);
#endif
