/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <stdio.h>

#include "chip.h"

#define CROS_EC_AUDIO_CODEC_HID		"GOOG0013"
#define CROS_EC_AUDIO_CODEC_DDN		"Cros EC audio codec"

static void crosec_audio_codec_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	struct ec_google_chromeec_audio_codec_config *cfg = dev->chip_info;

	if (!scope || !cfg)
		return;

	acpigen_write_scope(scope);

	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", CROS_EC_AUDIO_CODEC_HID);
	acpigen_write_name_integer("_UID", cfg->uid);
	acpigen_write_name_string("_DDN", CROS_EC_AUDIO_CODEC_DDN);
	acpigen_write_STA(acpi_device_status(dev));

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev), CROS_EC_AUDIO_CODEC_DDN,
						dev_path(dev));
}

static const char *crosec_audio_codec_acpi_name(const struct device *dev)
{
	struct ec_google_chromeec_audio_codec_config *cfg = dev->chip_info;
	static char name[5];

	if (cfg->name)
		return cfg->name;

	snprintf(name, sizeof(name), "ECA%X", dev->path.generic.id);
	return name;
}

static struct device_operations crosec_audio_codec_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= crosec_audio_codec_acpi_name,
	.acpi_fill_ssdt		= crosec_audio_codec_fill_ssdt,
	.scan_bus		= scan_static_bus,
};

static void crosec_audio_codec_enable(struct device *dev)
{
	dev->ops = &crosec_audio_codec_ops;
}

struct chip_operations ec_google_chromeec_audio_codec_ops = {
	.name = "CrosEC Audio Codec Device",
	.enable_dev = crosec_audio_codec_enable
};
