/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci_def.h>
#include "chip.h"

static void write_pci_camera_device(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;

	if (dev->path.type != DEVICE_PATH_PCI) {
		printk(BIOS_ERR, "CIO2/IMGU devices require PCI\n");
		return;
	}

	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_ADR_pci_device(dev);
	acpigen_write_name_string("_DDN", config->device_type == INTEL_ACPI_CAMERA_CIO2 ?
				  "Camera and Imaging Subsystem" : "Imaging Unit");
}

static void write_i2c_camera_device(const struct device *dev, const char *scope)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = I2C_SPEED_FAST,
		.resource = scope,
	};

	if (dev->path.type != DEVICE_PATH_I2C) {
		printk(BIOS_ERR, "Non-CIO2/IMGU devices require I2C\n");
		return;
	}

	acpigen_write_device(acpi_device_name(dev));

	if (config->device_type == INTEL_ACPI_CAMERA_SENSOR)
		acpigen_write_name_integer("_ADR", 0);

	if (config->acpi_hid)
		acpigen_write_name_string("_HID", config->acpi_hid);
	else if (config->device_type == INTEL_ACPI_CAMERA_VCM)
		acpigen_write_name_string("_HID", ACPI_DT_NAMESPACE_HID);
	else if (config->device_type == INTEL_ACPI_CAMERA_NVM)
		acpigen_write_name_string("_HID", "INT3499");

	acpigen_write_name_integer("_UID", config->acpi_uid);
	acpigen_write_name_string("_DDN", config->chip_name);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);

	/*
	 * The optional vcm/nvram devices are presumed to be on the same I2C bus as the camera
	 * sensor.
	 */
	if (config->device_type == INTEL_ACPI_CAMERA_SENSOR &&
	    config->ssdb.vcm_type && config->vcm_address) {
		struct acpi_i2c i2c_vcm = i2c;
		i2c_vcm.address = config->vcm_address;
		acpi_device_write_i2c(&i2c_vcm);
	}

	if (config->device_type == INTEL_ACPI_CAMERA_SENSOR &&
	    config->ssdb.rom_type && config->rom_address) {
		struct acpi_i2c i2c_rom = i2c;
		i2c_rom.address = config->rom_address;
		acpi_device_write_i2c(&i2c_rom);
	}

	acpigen_write_resourcetemplate_footer();
}

static void write_camera_device_common(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;

	/* Mark it as Camera related device */
	if (config->device_type == INTEL_ACPI_CAMERA_CIO2 ||
	    config->device_type == INTEL_ACPI_CAMERA_IMGU ||
	    config->device_type == INTEL_ACPI_CAMERA_SENSOR ||
	    config->device_type == INTEL_ACPI_CAMERA_VCM) {
		acpigen_write_name_integer("CAMD", config->device_type);
	}
}

static void camera_fill_ssdt(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);

	if (!dev->enabled || !scope)
		return;

	/* Device */
	acpigen_write_scope(scope);

	if (config->device_type == INTEL_ACPI_CAMERA_CIO2 ||
	    config->device_type == INTEL_ACPI_CAMERA_IMGU)
		write_pci_camera_device(dev);
	else
		write_i2c_camera_device(dev, scope);

	write_camera_device_common(dev);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	if (dev->path.type == DEVICE_PATH_PCI) {
		printk(BIOS_INFO, "%s: %s PCI address 0%x\n", acpi_device_path(dev),
		       dev->chip_ops->name, dev->path.pci.devfn);
	} else {
		printk(BIOS_INFO, "%s: %s I2C address 0%xh\n", acpi_device_path(dev),
		       dev->chip_ops->name, dev->path.i2c.device);
	}
}

static const char *camera_acpi_name(const struct device *dev)
{
	const char *prefix = NULL;
	static char name[5];
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;

	if (config->acpi_name)
		return config->acpi_name;

	switch (config->device_type) {
	case INTEL_ACPI_CAMERA_CIO2:
		return "CIO2";
	case INTEL_ACPI_CAMERA_IMGU:
		return "IMGU";
	case INTEL_ACPI_CAMERA_PMIC:
		return "PMIC";
	case INTEL_ACPI_CAMERA_SENSOR:
		prefix = "CAM";
		break;
	case INTEL_ACPI_CAMERA_VCM:
		prefix = "VCM";
		break;
	case INTEL_ACPI_CAMERA_NVM:
		prefix = "NVM";
		break;
	default:
		printk(BIOS_ERR, "Invalid device type: %x\n", config->device_type);
		return NULL;
	}

	/*
	 * The camera # knows which link # they use, so that's used as the basis for the
	 * instance #. The VCM and NVM don't have this information, so the best we can go on is
	 * the _UID.
	 */
	snprintf(name, sizeof(name), "%s%1u", prefix,
		 config->device_type == INTEL_ACPI_CAMERA_SENSOR ?
		 config->ssdb.link_used : config->acpi_uid);
	return name;
}

static struct device_operations camera_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= camera_acpi_name,
	.acpi_fill_ssdt		= camera_fill_ssdt,
};

static void camera_enable(struct device *dev)
{
	dev->ops = &camera_ops;
}

struct chip_operations drivers_intel_mipi_camera_ops = {
	CHIP_NAME("Intel MIPI Camera Device")
	.enable_dev = camera_enable
};
