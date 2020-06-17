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

#define SENSOR_NAME_UUID	"822ace8f-2814-4174-a56b-5f029fe079ee"
#define SENSOR_TYPE_UUID	"26257549-9271-4ca4-bb43-c4899d5a4881"
#define DEFAULT_ENDPOINT	0

static void camera_fill_sensor_defaults(struct drivers_intel_mipi_camera_config *config)
{
	if (config->disable_ssdb_defaults)
		return;

	if (!config->ssdb.platform)
		config->ssdb.platform = PLATFORM_SKC;

	if (!config->ssdb.flash_support)
		config->ssdb.flash_support = FLASH_DISABLE;

	if (!config->ssdb.privacy_led)
		config->ssdb.privacy_led = PRIVACY_LED_A_16mA;

	if (!config->ssdb.mipi_define)
		config->ssdb.mipi_define = MIPI_INFO_ACPI_DEFINED;

	if (!config->ssdb.mclk_speed)
		config->ssdb.mclk_speed = CLK_FREQ_19_2MHZ;
}

/*
 * Adds settings for a camera sensor device (typically at "\_SB.PCI0.I2Cx.CAMy"). The drivers
 * for Linux tends to expect the camera sensor device and any related nvram / vcm devices to be
 * separate ACPI devices, while the drivers for Windows want all of these to be grouped
 * together in the camera sensor ACPI device. This implementation tries to satisfy both,
 * though the unfortunate tradeoff is that the same I2C address for nvram and vcm is advertised
 * by multiple devices in ACPI (via "_CRS"). The Windows driver can use the "_DSM" method to
 * disambiguate the I2C resources in the camera sensor ACPI device.  Drivers for Windows
 * typically query "SSDB" for configuration information (represented as a binary blob dump of
 * struct), while Linux drivers typically consult individual parameters in "_DSD".
 *
 * The tree of tables in "_DSD" is analogous to what's used for the "CIO2" device.  The _DSD
 * specifies a child table for the sensor's port (e.g., PRT0 for "port0"--this implementation
 * assumes a camera only has 1 port). The PRT0 table specifies a table for each endpoint
 * (though only 1 endpoint is supported by this implementation so the table only has an
 * "endpoint0" that points to a EP00 table). The EP00 table primarily describes the # of lanes
 * in "data-lines", a list of frequencies in "list-frequencies", and specifies the name of the
 * other side in "remote-endpoint" (typically "\_SB.PCI0.CIO2").
 */
static void camera_fill_sensor(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	struct acpi_dp *ep00 = NULL;
	struct acpi_dp *prt0 = NULL;
	struct acpi_dp *dsd = NULL;
	struct acpi_dp *remote = NULL;
	const char *vcm_name = NULL;
	struct acpi_dp *lens_focus = NULL;

	camera_fill_sensor_defaults(config);

	ep00 = acpi_dp_new_table("EP00");
	acpi_dp_add_integer(ep00, "endpoint", DEFAULT_ENDPOINT);
	acpi_dp_add_integer(ep00, "clock-lanes", 0);

	if (config->ssdb.lanes_used > 0) {
		struct acpi_dp *lanes = acpi_dp_new_table("data-lanes");
		uint32_t i;
		for (i = 1; i <= config->ssdb.lanes_used; ++i)
			acpi_dp_add_integer(lanes, NULL, i);
		acpi_dp_add_array(ep00, lanes);
	}

	if (config->num_freq_entries) {
		struct acpi_dp *freq = acpi_dp_new_table("link-frequencies");
		uint32_t i;
		for (i = 0; i < config->num_freq_entries && i < MAX_LINK_FREQ_ENTRIES; ++i)
			acpi_dp_add_integer(freq, NULL, config->link_freq[i]);
		acpi_dp_add_array(ep00, freq);
	}

	remote = acpi_dp_new_table("remote-endpoint");

	acpi_dp_add_reference(remote, NULL, config->remote_name);
	acpi_dp_add_integer(remote, NULL, config->ssdb.link_used);
	acpi_dp_add_integer(remote, NULL, DEFAULT_ENDPOINT);
	acpi_dp_add_array(ep00, remote);

	prt0 = acpi_dp_new_table("PRT0");

	acpi_dp_add_integer(prt0, "port", 0);
	acpi_dp_add_child(prt0, "endpoint0", ep00);
	dsd = acpi_dp_new_table("_DSD");

	acpi_dp_add_integer(dsd, "clock-frequency", config->ssdb.mclk_speed);

	if (config->ssdb.degree)
		acpi_dp_add_integer(dsd, "rotation", 180);

	if (config->ssdb.vcm_type) {
		if (config->vcm_name) {
			vcm_name = config->vcm_name;
		} else {
			const struct device_path path = {
				.type = DEVICE_PATH_I2C,
				.i2c.device = config->vcm_address,
			};
			struct device *vcm_dev = find_dev_path(dev->bus, &path);
			struct drivers_intel_mipi_camera_config *vcm_config;
			vcm_config = vcm_dev ? vcm_dev->chip_info : NULL;

			if (!vcm_config)
				printk(BIOS_ERR, "Failed to get VCM\n");
			else if (vcm_config->device_type != INTEL_ACPI_CAMERA_VCM)
				printk(BIOS_ERR, "Device isn't VCM\n");
			else
				vcm_name = acpi_device_path(vcm_dev);
		}
	}

	if (vcm_name) {
		lens_focus = acpi_dp_new_table("lens-focus");
		acpi_dp_add_reference(lens_focus, NULL, vcm_name);
		acpi_dp_add_array(dsd, lens_focus);
	}

	acpi_dp_add_child(dsd, "port0", prt0);
	acpi_dp_write(dsd);

	acpigen_write_method_serialized("SSDB", 0);
	acpigen_write_return_byte_buffer((uint8_t *)&config->ssdb, sizeof(config->ssdb));
	acpigen_pop_len(); /* Method */

	/* Fill Power Sequencing Data */
	if (config->num_pwdb_entries > 0) {
		acpigen_write_method_serialized("PWDB", 0);
		acpigen_write_return_byte_buffer((uint8_t *)&config->pwdb,
						 sizeof(struct intel_pwdb) *
						 config->num_pwdb_entries);
		acpigen_pop_len(); /* Method */
	}
}

static void camera_fill_nvm(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	struct acpi_dp *dsd = acpi_dp_new_table("_DSD");

	/* It might be possible to default size or width based on type. */
	if (!config->disable_nvm_defaults && !config->nvm_pagesize)
		config->nvm_pagesize = 1;

	if (!config->disable_nvm_defaults && !config->nvm_readonly)
		config->nvm_readonly = 1;

	if (config->nvm_size)
		acpi_dp_add_integer(dsd, "size", config->nvm_size);

	if (config->nvm_pagesize)
		acpi_dp_add_integer(dsd, "pagesize", config->nvm_pagesize);

	if (config->nvm_readonly)
		acpi_dp_add_integer(dsd, "read-only", config->nvm_readonly);

	if (config->nvm_width)
		acpi_dp_add_integer(dsd, "address-width", config->nvm_width);

	acpi_dp_write(dsd);
}

static void camera_fill_vcm(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	struct acpi_dp *dsd;

	if (!config->vcm_compat)
		return;

	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_string(dsd, "compatible", config->vcm_compat);
	acpi_dp_write(dsd);
}

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

	switch (config->device_type) {
	case INTEL_ACPI_CAMERA_SENSOR:
		camera_fill_sensor(dev);
		break;
	case INTEL_ACPI_CAMERA_VCM:
		camera_fill_vcm(dev);
		break;
	case INTEL_ACPI_CAMERA_NVM:
		camera_fill_nvm(dev);
		break;
	default:
		break;
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
