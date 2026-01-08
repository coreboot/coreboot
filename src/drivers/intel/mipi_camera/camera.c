/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <stdio.h>

#include "chip.h"

#define CSI2_DATA_STREAM_INTERFACE_GUID \
	GUID_INIT(0x8A395669, 0x11F7, 0x4EA9, \
	0x9C, 0x7D, 0x20, 0xEE, 0x0A, 0xB5, 0xCA, 0x40)

#define UUID_DSM_SENSOR		"822ace8f-2814-4174-a56b-5f029fe079ee"
#define UUID_DSM_I2C		"26257549-9271-4ca4-bb43-c4899d5a4881"
#define UUID_DSM_I2C_V2		"5815c5c8-c47d-477b-9a8d-76173176414b"
#define UUID_DSM_CVF		"02f55f0c-2e63-4f05-84f3-bf1980f9af79"
#define DEFAULT_ENDPOINT	0
#define DEFAULT_REMOTE_NAME	"\\_SB.PCI0.CIO2"
#define CIO2_PCI_DEV		0x14
#define CIO2_PCI_FN		0x3
#define POWER_RESOURCE_NAME	"PRIC"
#define GUARD_VARIABLE_FORMAT	"RES%1d"
#define ENABLE_METHOD_FORMAT	"ENB%1d"
#define DISABLE_METHOD_FORMAT	"DSB%1d"
#define UNKNOWN_METHOD_FORMAT	"UNK%1d"
#define CLK_ENABLE_METHOD	"MCON"
#define CLK_DISABLE_METHOD	"MCOF"

static struct camera_resource_manager res_mgr;

static void resource_set_action_type(struct resource_config *res_config,
				     enum action_type action)
{
	if (res_config)
		res_config->action = action;
}

static enum action_type resource_get_action_type(const struct resource_config *res_config)
{
	return res_config ? res_config->action : UNKNOWN_ACTION;
}

static enum ctrl_type resource_get_ctrl_type(const struct resource_config *res_config)
{
	return res_config ? res_config->type : UNKNOWN_CTRL;
}

static void resource_set_clk_config(struct resource_config *res_config,
				    const struct clk_config *clk_conf)
{
	if (res_config) {
		res_config->type = IMGCLK;
		res_config->clk_conf = clk_conf;
	}
}

static const struct clk_config *resource_clk_config(const struct resource_config *res_config)
{
	return res_config ? res_config->clk_conf : NULL;
}

static void resource_set_gpio_config(struct resource_config *res_config,
				     const struct gpio_config *gpio_conf)
{
	if (res_config) {
		res_config->type = GPIO;
		res_config->gpio_conf = gpio_conf;
	}
}

static const struct gpio_config *resource_gpio_config(const struct resource_config *res_config)
{
	return res_config ? res_config->gpio_conf : NULL;
}

/*
 * This implementation assumes there is only 1 endpoint at each end of every data port. It also
 * assumes there are no clock lanes.  It also assumes that any VCM or NVM for a CAM is on the
 * same I2C bus as the CAM.
 */

/*
 * Adds settings for a CIO2 device (typically at "\_SB.PCI0.CIO2").  A _DSD is added that
 * specifies a child table for each port (e.g., PRT0 for "port0" and PRT1 for "port1").  Each
 * PRTx table specifies a table for each endpoint (though only 1 endpoint is supported by this
 * implementation so the table only has an "endpoint0" that points to a EPx0 table). The EPx0
 * table primarily describes the # of lanes in "data-lines" and specifies the name of the other
 * side in "remote-endpoint" (the name is usually of the form "\_SB.PCI0.I2Cx.CAM0" for the
 * first port/cam and "\_SB.PCI0.I2Cx.CAM1" if there's a second port/cam).
 */
static void camera_fill_cio2(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	struct acpi_dp *dsd = acpi_dp_new_table("_DSD");
	char *ep_table_name[MAX_PORT_ENTRIES] = {NULL};
	char *port_table_name[MAX_PORT_ENTRIES] = {NULL};
	char *port_name[MAX_PORT_ENTRIES] = {NULL};
	unsigned int i, j;
	char name[BUS_PATH_MAX];
	struct acpi_dp *ep_table = NULL;
	struct acpi_dp *port_table = NULL;
	struct acpi_dp *remote = NULL;
	struct acpi_dp *lanes = NULL;

	for (i = 0; i < config->cio2_num_ports && i < MAX_PORT_ENTRIES; ++i) {
		snprintf(name, sizeof(name), "EP%u0", i);
		ep_table_name[i] = strdup(name);
		ep_table = acpi_dp_new_table(ep_table_name[i]);
		acpi_dp_add_integer(ep_table, "endpoint", 0);
		acpi_dp_add_integer(ep_table, "clock-lanes", 0);

		if (config->cio2_lanes_used[i] > 0) {
			lanes = acpi_dp_new_table("data-lanes");

			for (j = 1; j <= config->cio2_lanes_used[i] &&
			     j <= MAX_PORT_ENTRIES; ++j)
				acpi_dp_add_integer(lanes, NULL, j);
			acpi_dp_add_array(ep_table, lanes);
		}

		if (config->cio2_lane_endpoint[i]) {
			remote = acpi_dp_new_table("remote-endpoint");
			acpi_dp_add_reference(remote, NULL, config->cio2_lane_endpoint[i]);
			acpi_dp_add_integer(remote, NULL, 0);
			acpi_dp_add_integer(remote, NULL, 0);
			acpi_dp_add_array(ep_table, remote);
		}

		snprintf(name, sizeof(name), "PRT%u", i);
		port_table_name[i] = strdup(name);
		port_table = acpi_dp_new_table(port_table_name[i]);
		acpi_dp_add_integer(port_table, "port", config->cio2_prt[i]);
		acpi_dp_add_child(port_table, "endpoint0", ep_table);

		snprintf(name, sizeof(name), "port%u", i);
		port_name[i] = strdup(name);
		acpi_dp_add_integer(dsd, "is_es",
			CONFIG(DRIVERS_INTEL_MIPI_SUPPORTS_PRE_PRODUCTION_SOC));
		acpi_dp_add_child(dsd, port_name[i], port_table);
	}

	acpi_dp_write(dsd);

	for (i = 0; i < config->cio2_num_ports; ++i) {
		free(ep_table_name[i]);
		free(port_table_name[i]);
		free(port_name[i]);
	}
}

static void apply_pld_defaults(struct drivers_intel_mipi_camera_config *config)
{
	if (!config->pld.ignore_color)
		config->pld.ignore_color = 1;

	if (!config->pld.visible)
		config->pld.visible = 1;

	if (!config->pld.vertical_offset)
		config->pld.vertical_offset = 0xffff;

	if (!config->pld.horizontal_offset)
		config->pld.horizontal_offset = 0xffff;

	/*
	 * PLD_PANEL_TOP has a value of zero, so the following will change any instance of
	 * PLD_PANEL_TOP to PLD_PANEL_FRONT.
	 */
	if (!config->pld.panel)
		config->pld.panel = PLD_PANEL_FRONT;

	/*
	 * PLD_HORIZONTAL_POSITION_LEFT has a value of zero, so the following will change any
	 * instance of that value to PLD_HORIZONTAL_POSITION_CENTER.
	 */
	if (!config->pld.horizontal_position)
		config->pld.horizontal_position = PLD_HORIZONTAL_POSITION_CENTER;

	/*
	 * The desired default for |vertical_position| is PLD_VERTICAL_POSITION_UPPER, which
	 * has a value of zero so no work is needed to set a default. The same applies for
	 * setting |shape| to PLD_SHAPE_ROUND.
	 */
}

static void camera_generate_pld(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;

	apply_pld_defaults(config);

	acpigen_write_pld(&config->pld);
}

static uint32_t address_for_dev_type(const struct device *dev, uint8_t dev_type)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	uint16_t i2c_bus = dev->upstream ? dev->upstream->secondary : 0xFFFF;
	uint16_t i2c_addr;

	switch (dev_type) {
	case DEV_TYPE_SENSOR:
		i2c_addr = dev->path.i2c.device;
		break;
	case DEV_TYPE_VCM:
		i2c_addr = config->vcm_address;
		break;
	case DEV_TYPE_ROM:
		i2c_addr = config->rom_address;
		break;
	default:
		return 0;
	}

	return (((uint32_t)i2c_bus) << 24 | ((uint32_t)i2c_addr) << 8 | dev_type);
}

/*
 * Generate ASL DSM code for Sensor Device
 *
 * Generated ASL:
 * If (LEqual (Local0, ToUUID ("822ace8f-2814-4174-a56b-5f029fe079ee"))) {
 *     If (LEqual (Arg2, Zero)) {
 *         If (LEqual (Arg1, Zero)) {
 *             Return (Buffer (One) { 0x3 })
 *         } Else {
 *             Return (Buffer (One) { 0x1 })
 *         }
 *     }
 *     If (LEqual (Arg2, One)) {
 *         If (sensor_name exists)
 *             Return ("sensor_name")
 *         Else
 *             Return ("UNKNOWN")
 *     }
 * }
 */
static void camera_generate_dsm_sensor(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;

	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(UUID_DSM_SENSOR);

	acpigen_write_if_lequal_op_int(ARG2_OP, 0);
	acpigen_write_if_lequal_op_int(ARG1_OP, 0);
	acpigen_write_return_singleton_buffer(0x3);
	acpigen_write_else();
	acpigen_write_return_singleton_buffer(0x1);
	acpigen_pop_len();	/* If Arg1=0 */

	acpigen_pop_len();	/* If Arg2=0 */

	acpigen_write_if_lequal_op_int(ARG2_OP, 1);
	acpigen_write_return_string(config && config->sensor_name ? config->sensor_name : "UNKNOWN");
	acpigen_pop_len();	/* If Arg2=1 */

	acpigen_pop_len();	/* If uuid */
}

/*
 * Generate ASL DSM code for I2C device count and addresses
 *
 * Generated ASL:
 * If (LEqual (Local0, ToUUID ("26257549-9271-4ca4-bb43-c4899d5a4881"))) {
 *     ToInteger (Arg2, Local1)
 *     If (LEqual (Local1, 1)) {
 *         Return (i2c_dev_count)
 *     }
 *     If (LEqual (Local1, 2)) {
 *         Return (sensor_address)
 *     }
 *     If (LEqual (Local1, 3)) {
 *         Return (vcm_address)  // if vcm_type exists
 *     }
 *     If (LEqual (Local1, 3 or 4)) {
 *         Return (rom_address)  // if rom_type exists (index depends on vcm)
 *     }
 * }
 */
static void camera_generate_dsm_i2c(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	int i2c_dev_count = 1 + (config->ssdb.vcm_type ? 1 : 0) + (config->ssdb.rom_type ? 1 : 0);
	int i2c_dev_idx = 1;

	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(UUID_DSM_I2C);
	acpigen_write_to_integer(ARG2_OP, LOCAL1_OP);

	acpigen_write_if_lequal_op_int(LOCAL1_OP, i2c_dev_idx++);
	acpigen_write_return_integer(i2c_dev_count);
	acpigen_pop_len();	/* If Arg2=1 */

	acpigen_write_if_lequal_op_int(LOCAL1_OP, i2c_dev_idx++);
	acpigen_write_return_integer(address_for_dev_type(dev, DEV_TYPE_SENSOR));
	acpigen_pop_len();	/* If Arg2=2 */

	if (config->ssdb.vcm_type) {
		acpigen_write_if_lequal_op_int(LOCAL1_OP, i2c_dev_idx++);
		acpigen_write_return_integer(address_for_dev_type(dev, DEV_TYPE_VCM));
		acpigen_pop_len();      /* If Arg2=3 */
	}

	if (config->ssdb.rom_type) {
		acpigen_write_if_lequal_op_int(LOCAL1_OP, i2c_dev_idx);
		acpigen_write_return_integer(address_for_dev_type(dev, DEV_TYPE_ROM));
		acpigen_pop_len();      /* If Arg2=3 or 4 */
	}

	acpigen_pop_len();      /* If uuid */
}

/*
 * Generate ASL DSM code for I2C device count and addresses (V2)
 *
 * Generated ASL:
 * If (LEqual (Local0, ToUUID ("5815c5c8-c47d-477b-9a8d-76173176414b"))) {
 *     If (LEqual (Arg2, Zero)) {
 *         If (LEqual (Arg1, Zero)) {
 *             Return (Buffer (One) { 0x3 })
 *         } Else {
 *             Return (Buffer (One) { 0x1 })
 *         }
 *     }
 *     If (LEqual (Arg2, One)) {
 *         Return (Buffer (52) {
 *             i2c_count, sensor_addr, [vcm_addr], [rom_addr], 0, 0, ...
 *         })
 *         // Buffer is 13 * 4 = 52 bytes: count + up to 12 device addresses
 *     }
 * }
 */
static void camera_generate_dsm_i2c_v2(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	if (!config)
		return;

	int i2c_count = 1 + (config->ssdb.vcm_type ? 1 : 0) + (config->ssdb.rom_type ? 1 : 0);
	int i2c_idx = 1;

	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(UUID_DSM_I2C_V2);

	acpigen_write_if_lequal_op_int(ARG2_OP, 0);

	acpigen_write_if_lequal_op_int(ARG1_OP, 0);
	acpigen_write_return_singleton_buffer(0x3);
	acpigen_write_else();
	acpigen_write_return_singleton_buffer(0x1);
	acpigen_pop_len();	/* If Arg1=0 */

	acpigen_pop_len();	/* If Arg2=0 */

	acpigen_write_if_lequal_op_int(ARG2_OP, 1);

	/* Buffer is 13 * 4 = 52 bytes: count + up to 12 device addresses */
	uint32_t i2c_buffer[13] = {0};
	_Static_assert(sizeof(i2c_buffer) == 52, "i2c_buffer size must be 52 bytes");

	i2c_buffer[0] = i2c_count;
	i2c_buffer[i2c_idx++] = address_for_dev_type(dev, DEV_TYPE_SENSOR);

	if (config->ssdb.vcm_type) {
		i2c_buffer[i2c_idx++] = address_for_dev_type(dev, DEV_TYPE_VCM);
	}

	if (config->ssdb.rom_type) {
		i2c_buffer[i2c_idx] = address_for_dev_type(dev, DEV_TYPE_ROM);
	}

	acpigen_write_return_byte_buffer((uint8_t *)i2c_buffer, sizeof(i2c_buffer));

	acpigen_pop_len();	/* If Arg2=1 */

	acpigen_pop_len();	/* If uuid */
}

/*
 * Generate ASL DSM code for Computer Vision Framework (CVF)
 *
 * Generated ASL:
 * If (LEqual (Local0, ToUUID ("02f55f0c-2e63-4f05-84f3-bf1980f9af79"))) {
 *     If (LEqual (Arg2, Zero)) {
 *         Return (Buffer (One) { 0x3 })
 *     }
 *     If (LEqual (Arg2, One)) {
 *         Return (Zero)
 *     }
 * }
 */
static void camera_generate_dsm_cvf(const struct device *dev)
{
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(UUID_DSM_CVF);

	acpigen_write_if_lequal_op_int(ARG2_OP, 0);
	acpigen_write_return_singleton_buffer(0x3);
	acpigen_pop_len();

	acpigen_write_if_lequal_op_int(ARG2_OP, 1);
	acpigen_write_return_integer(0);
	acpigen_pop_len();

	acpigen_pop_len();	/* If uuid */
}

static void camera_generate_dsm(const struct device *dev)
{
	/* Method (_DSM, 4, NotSerialized) */
	acpigen_write_method("_DSM", 4);

	/* ToBuffer (Arg0, Local0) */
	acpigen_write_to_buffer(ARG0_OP, LOCAL0_OP);

	camera_generate_dsm_sensor(dev);
	camera_generate_dsm_i2c(dev);
	camera_generate_dsm_i2c_v2(dev);
	camera_generate_dsm_cvf(dev);

	/* Return (Buffer (One) { 0x0 }) */
	acpigen_write_return_singleton_buffer(0x0);

	acpigen_pop_len();      /* Method _DSM */
}

static void camera_fill_ssdb_defaults(struct drivers_intel_mipi_camera_config *config)
{
	config->ssdb.version = 1;

	if (!config->ssdb.sensor_card_sku.card_type)
		config->ssdb.sensor_card_sku.card_type = SKU_CRD_D;

	guidcpy(&config->ssdb.csi2_data_stream_interface, &CSI2_DATA_STREAM_INTERFACE_GUID);

	if (!config->ssdb.bdf_value)
		config->ssdb.bdf_value = PCI_DEVFN(CIO2_PCI_DEV, CIO2_PCI_FN);

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
 * Adds settings for a camera sensor device (typically at "\_SB.PCI0.I2Cx.CAMy").
 *
 * Single ACPI device mode: The drivers for Windows and Linux want the sensor and any associated
 * VCM or NVM devices to be grouped together in the camera sensor ACPI device. The OS driver
 * uses the "_DSM" method to disambiguate the I2C resources in the camera sensor ACPI device.
 * Drivers typically query "SSDB" for configuration information (represented as a binary blob
 * dump of struct).
 *
 * Multi ACPI device mode: The drivers for ChromeOS expect the camera sensor device and any
 * related nvram / vcm devices to be separate ACPI devices.
 *
 * The tree of tables in "_DSD" is analogous to what's used for the "CIO2" device.  The _DSD
 * specifies a child table for the sensor's port (e.g., PRT0 for "port0"--this implementation
 * assumes a camera only has 1 port). The PRT0 table specifies a table for each endpoint
 * (though only 1 endpoint is supported by this implementation so the table only has an
 * "endpoint0" that points to a EP00 table). The EP00 table primarily describes the # of lanes
 * in "data-lanes", a list of frequencies in "list-frequencies", and specifies the name of the
 * other side in "remote-endpoint" (typically "\_SB.PCI0.CIO2").
 */
static void camera_fill_sensor(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;

	camera_generate_pld(dev);

	camera_fill_ssdb_defaults(config);

	/* _DSM */
	camera_generate_dsm(dev);

	if (CONFIG(MIPI_ACPI_TYPE_WINDOWS_LINUX)) {
		acpigen_write_method_serialized("SSDB", 0);
		acpigen_write_return_byte_buffer((uint8_t *)&config->ssdb, sizeof(config->ssdb));
		acpigen_pop_len(); /* Method */
		return;
	}

	/* Multi-device mode: add _DSD with endpoint information */
	struct acpi_dp *ep00 = NULL;
	struct acpi_dp *prt0 = NULL;
	struct acpi_dp *dsd = NULL;
	struct acpi_dp *remote = NULL;
	const char *vcm_name = NULL;
	struct acpi_dp *lens_focus = NULL;
	const char *remote_name;
	struct device *cio2 = pcidev_on_root(CIO2_PCI_DEV, CIO2_PCI_FN);

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

	if (config->remote_name) {
		remote_name = config->remote_name;
	} else {
		if (cio2)
			remote_name = acpi_device_path(cio2);
		else
			remote_name = DEFAULT_REMOTE_NAME;
	}

	acpi_dp_add_reference(remote, NULL, remote_name);
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
			struct device *vcm_dev = find_dev_path(dev->upstream, &path);
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

	if (config->low_power_probe)
		acpi_dp_add_integer(dsd, "i2c-allow-low-power-probe", 0x01);

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
	struct acpi_dp *dsd;

	if (!config->nvm_compat)
		return;

	dsd = acpi_dp_new_table("_DSD");

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

	acpi_dp_add_string(dsd, "compatible", config->nvm_compat);

	if (config->low_power_probe)
		acpi_dp_add_integer(dsd, "i2c-allow-low-power-probe", 0x01);

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

	if (config->low_power_probe)
		acpi_dp_add_integer(dsd, "i2c-allow-low-power-probe", 0x01);

	acpi_dp_write(dsd);
}

static int get_resource_index(const struct resource_config *res_config)
{
	enum ctrl_type type = resource_get_ctrl_type(res_config);
	const struct clk_config *clk_config;
	const struct gpio_config *gpio_config;
	unsigned int i;
	uint8_t res_id;

	switch (type) {
	case IMGCLK:
		clk_config = resource_clk_config(res_config);
		res_id = clk_config->clknum;
		break;
	case GPIO:
		gpio_config = resource_gpio_config(res_config);
		res_id = gpio_config->gpio_num;
		break;
	default:
		printk(BIOS_ERR, "Unsupported power operation: %x\n"
				 "OS camera driver will likely not work", type);
		return -1;
	}

	for (i = 0; i < res_mgr.cnt; i++)
		if (res_mgr.resource[i].type == type && res_mgr.resource[i].id == res_id)
			return i;

	return -1;
}

static void add_guarded_method_namestring(struct resource_config *res_config, int res_index)
{
	char method_name[ACPI_NAME_BUFFER_SIZE];
	enum action_type action = resource_get_action_type(res_config);

	switch (action) {
	case ENABLE:
		snprintf(method_name, sizeof(method_name), ENABLE_METHOD_FORMAT, res_index);
		break;
	case DISABLE:
		snprintf(method_name, sizeof(method_name), DISABLE_METHOD_FORMAT, res_index);
		break;
	default:
		snprintf(method_name, sizeof(method_name), UNKNOWN_METHOD_FORMAT, res_index);
		printk(BIOS_ERR, "Unsupported resource action: %x\n", action);
	}

	acpigen_emit_namestring(method_name);
}

static void call_guarded_method(struct resource_config *res_config)
{
	int res_index;

	if (res_config == NULL)
		return;

	res_index = get_resource_index(res_config);

	if (res_index != -1)
		add_guarded_method_namestring(res_config, res_index);
}

static void add_clk_op(const struct clk_config *clk_config, enum action_type action)
{
	if (clk_config == NULL)
		return;

	switch (action) {
	case ENABLE:
		acpigen_write_if();
		acpigen_emit_ext_op(COND_REFOF_OP);
		acpigen_emit_string(CLK_ENABLE_METHOD);
		acpigen_emit_namestring(CLK_ENABLE_METHOD);
		acpigen_write_integer(clk_config->clknum);
		acpigen_write_integer(clk_config->freq);
		acpigen_pop_len(); /* CondRefOf */
		break;
	case DISABLE:
		acpigen_write_if();
		acpigen_emit_ext_op(COND_REFOF_OP);
		acpigen_emit_string(CLK_DISABLE_METHOD);
		acpigen_emit_namestring(CLK_DISABLE_METHOD);
		acpigen_write_integer(clk_config->clknum);
		acpigen_pop_len(); /* CondRefOf */
		break;
	default:
		acpigen_write_debug_string("Unsupported clock action");
		printk(BIOS_ERR, "Unsupported clock action: %x\n"
				 "OS camera driver will likely not work", action);
	}
}

static void add_gpio_op(const struct gpio_config *gpio_config, enum action_type action)
{
	if (gpio_config == NULL)
		return;

	switch (action) {
	case ENABLE:
		acpigen_soc_set_tx_gpio(gpio_config->gpio_num);
		break;
	case DISABLE:
		acpigen_soc_clear_tx_gpio(gpio_config->gpio_num);
		break;
	default:
		acpigen_write_debug_string("Unsupported GPIO action");
		printk(BIOS_ERR, "Unsupported GPIO action: %x\n"
				 "OS camera driver will likely not work\n", action);
	}
}

static void add_power_operation(const struct resource_config *res_config)
{
	const struct clk_config *clk_config;
	const struct gpio_config *gpio_config;
	enum ctrl_type type = resource_get_ctrl_type(res_config);
	enum action_type action = resource_get_action_type(res_config);

	if (res_config == NULL)
		return;

	switch (type) {
	case IMGCLK:
		clk_config = resource_clk_config(res_config);
		add_clk_op(clk_config, action);
		break;
	case GPIO:
		gpio_config = resource_gpio_config(res_config);
		add_gpio_op(gpio_config, action);
		break;
	default:
		printk(BIOS_ERR, "Unsupported power operation: %x\n"
				 "OS camera driver will likely not work\n", type);
		break;
	}
}

static void write_guard_variable(uint8_t res_index)
{
	char varname[ACPI_NAME_BUFFER_SIZE];

	snprintf(varname, sizeof(varname), GUARD_VARIABLE_FORMAT, res_index);
	acpigen_write_name_integer(varname, 0);
}

static void write_enable_method(struct resource_config *res_config, uint8_t res_index)
{
	char method_name[ACPI_NAME_BUFFER_SIZE];
	char varname[ACPI_NAME_BUFFER_SIZE];

	snprintf(varname, sizeof(varname), GUARD_VARIABLE_FORMAT, res_index);

	snprintf(method_name, sizeof(method_name), ENABLE_METHOD_FORMAT, res_index);

	acpigen_write_method_serialized(method_name, 0);
	acpigen_write_if_lequal_namestr_int(varname, 0);
	resource_set_action_type(res_config, ENABLE);
	add_power_operation(res_config);
	acpigen_pop_len(); /* if */

	acpigen_emit_byte(INCREMENT_OP);
	acpigen_emit_namestring(varname);
	acpigen_pop_len(); /* method_name */
}

static void write_disable_method(struct resource_config *res_config, uint8_t res_index)
{
	char method_name[ACPI_NAME_BUFFER_SIZE];
	char varname[ACPI_NAME_BUFFER_SIZE];

	snprintf(varname, sizeof(varname), GUARD_VARIABLE_FORMAT, res_index);

	snprintf(method_name, sizeof(method_name), DISABLE_METHOD_FORMAT, res_index);

	acpigen_write_method_serialized(method_name, 0);
	acpigen_write_if();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_namestring(varname);
	acpigen_write_integer(0x0);
	acpigen_emit_byte(DECREMENT_OP);
	acpigen_emit_namestring(varname);
	acpigen_pop_len(); /* if */

	acpigen_write_if_lequal_namestr_int(varname, 0);
	resource_set_action_type(res_config, DISABLE);
	add_power_operation(res_config);
	acpigen_pop_len(); /* if */
	acpigen_pop_len(); /* method_name */
}

static void add_guarded_operations(const struct drivers_intel_mipi_camera_config *config,
				   const struct operation_seq *seq)
{
	unsigned int i;
	uint8_t index;
	uint8_t res_id;
	struct resource_config res_config;
	int res_index;

	for (i = 0; i < seq->ops_cnt && i < MAX_PWR_OPS; i++) {
		index = seq->ops[i].index;
		switch (seq->ops[i].type) {
		case IMGCLK:
			res_id = config->clk_panel.clks[index].clknum;
			resource_set_clk_config(&res_config, &config->clk_panel.clks[index]);
			break;
		case GPIO:
			res_id = config->gpio_panel.gpio[index].gpio_num;
			resource_set_gpio_config(&res_config, &config->gpio_panel.gpio[index]);
			break;
		default:
			printk(BIOS_ERR, "Unsupported power operation: %x\n"
					 "OS camera driver will likely not work\n",
					 seq->ops[i].type);
			return;
		}

		res_index = get_resource_index(&res_config);

		if (res_index == -1) {
			if (res_mgr.cnt >= MAX_GUARDED_RESOURCES) {
				printk(BIOS_ERR, "Unable to add guarded camera resource\n"
						 "OS camera driver will likely not work\n");
				return;
			}

			res_mgr.resource[res_mgr.cnt].id = res_id;
			res_mgr.resource[res_mgr.cnt].type = seq->ops[i].type;

			write_guard_variable(res_mgr.cnt);
			write_enable_method(&res_config, res_mgr.cnt);
			write_disable_method(&res_config, res_mgr.cnt);

			res_mgr.cnt++;
		}
	}
}

static void fill_power_res_sequence(struct drivers_intel_mipi_camera_config *config,
				    struct operation_seq *seq)
{
	struct resource_config res_config;
	unsigned int i;
	uint8_t index;

	for (i = 0; i < seq->ops_cnt && i < MAX_PWR_OPS; i++) {
		index = seq->ops[i].index;

		switch (seq->ops[i].type) {
		case IMGCLK:
			resource_set_clk_config(&res_config, &config->clk_panel.clks[index]);
			break;
		case GPIO:
			resource_set_gpio_config(&res_config, &config->gpio_panel.gpio[index]);
			break;
		default:
			printk(BIOS_ERR, "Unsupported power operation: %x\n"
					 "OS camera driver will likely not work\n",
					 seq->ops[i].type);
			return;
		}

		resource_set_action_type(&res_config, seq->ops[i].action);
		call_guarded_method(&res_config);
		if (seq->ops[i].delay_ms)
			acpigen_write_sleep(seq->ops[i].delay_ms);
	}
}

static void write_pci_camera_device(const struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PCI) {
		printk(BIOS_ERR, "CIO2/IMGU devices require PCI\n");
		return;
	}

	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_ADR_pci_device(dev);
	acpigen_write_name_string("_DDN", "Camera and Imaging Subsystem");
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

	acpigen_write_device(acpi_device_name(dev));

	/* add power resource */
	if (config->has_power_resource) {
		acpigen_write_power_res(POWER_RESOURCE_NAME, 0, 0, NULL, 0);
		acpigen_write_name_integer("STA", 0);
		acpigen_write_STA_ext("STA");

		acpigen_write_method_serialized("_ON", 0);
		acpigen_write_if();
		acpigen_emit_byte(LEQUAL_OP);
		acpigen_emit_namestring("STA");
		acpigen_write_integer(0);

		fill_power_res_sequence(config, &config->on_seq);

		acpigen_write_store_op_to_namestr(1, "STA");
		acpigen_pop_len(); /* if */
		acpigen_pop_len(); /* _ON */

		/* _OFF operations */
		acpigen_write_method_serialized("_OFF", 0);
		acpigen_write_if();
		acpigen_emit_byte(LEQUAL_OP);
		acpigen_emit_namestring("STA");
		acpigen_write_integer(1);

		fill_power_res_sequence(config, &config->off_seq);

		acpigen_write_store_op_to_namestr(0, "STA");
		acpigen_pop_len(); /* if */
		acpigen_pop_len(); /* _ON */

		acpigen_pop_len(); /* Power Resource */
	}

	if (config->acpi_hid)
		acpigen_write_name_string("_HID", config->acpi_hid);
	else if (config->device_type == INTEL_ACPI_CAMERA_VCM ||
		 config->device_type == INTEL_ACPI_CAMERA_NVM)
		acpigen_write_name_string("_HID", ACPI_DT_NAMESPACE_HID);
	else if (config->device_type == INTEL_ACPI_CAMERA_SENSOR)
		acpigen_write_name_integer("_ADR", 0);

	acpigen_write_name_integer("_UID", config->acpi_uid);
	if (CONFIG(MIPI_ACPI_TYPE_WINDOWS_LINUX))
		acpigen_write_name_string("_DDN", config->sensor_name);
	else
		acpigen_write_name_string("_DDN", config->chip_name);
	acpigen_write_STA(acpi_device_status(dev));
	acpigen_write_method("_DSC", 0);
	acpigen_write_return_integer(config->max_dstate_for_probe);
	acpigen_pop_len(); /* Method _DSC */

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

	/* Mark it as Camera related device (multi-device mode only) */
	if (CONFIG(MIPI_ACPI_TYPE_CHROMEOS) &&
	    (config->device_type == INTEL_ACPI_CAMERA_CIO2 ||
	     config->device_type == INTEL_ACPI_CAMERA_IMGU ||
	     config->device_type == INTEL_ACPI_CAMERA_SENSOR ||
	     config->device_type == INTEL_ACPI_CAMERA_VCM)) {
		acpigen_write_name_integer("CAMD", config->device_type);
	}

	if (config->pr0 || config->has_power_resource) {
		acpigen_write_name("_PR0");
		acpigen_write_package(1);
		if (config->pr0)
			acpigen_emit_namestring(config->pr0); /* External power resource */
		else
			acpigen_emit_namestring(POWER_RESOURCE_NAME);

		acpigen_pop_len(); /* _PR0 */
	}

	switch (config->device_type) {
	case INTEL_ACPI_CAMERA_CIO2:
		camera_fill_cio2(dev);
		break;
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
	const char *scope = NULL;
	const struct device *pdev = dev->upstream->dev;

	if (CONFIG(MIPI_ACPI_TYPE_WINDOWS_LINUX)) {
		/* Only generate SSDT for an i2c-attached sensor device */
		if (dev->path.type != DEVICE_PATH_I2C || config->device_type != INTEL_ACPI_CAMERA_SENSOR)
			return;

		scope = acpi_device_scope(dev);
		if (!scope) {
			printk(BIOS_ERR, "Failed to get scope for device %s\n", dev_path(dev));
			return;
		}

		acpigen_write_scope(scope);

		if (config->has_power_resource && pdev && pdev->enabled) {
			add_guarded_operations(config, &config->on_seq);
			add_guarded_operations(config, &config->off_seq);
		}

		write_i2c_camera_device(dev, scope);
		write_camera_device_common(dev);

		acpigen_pop_len(); /* Device */
		acpigen_pop_len(); /* Scope */

		printk(BIOS_INFO, "%s: %s at I2C 0x%02x\n", acpi_device_path(dev),
		       dev->chip_ops->name, dev->path.i2c.device);
		return;
	}

	/* Multi-device mode */
	if (config->has_power_resource) {
		if (!pdev || !pdev->enabled)
			return;

		scope = acpi_device_scope(pdev);
		if (!scope)
			return;

		acpigen_write_scope(scope);
		add_guarded_operations(config, &config->on_seq);
		add_guarded_operations(config, &config->off_seq);
		acpigen_pop_len(); /* Guarded power resource operations scope */
	}

	switch (dev->path.type) {
	case DEVICE_PATH_I2C:
		scope = acpi_device_scope(dev);
		if (!scope)
			return;

		acpigen_write_scope(scope);
		write_i2c_camera_device(dev, scope);
		break;
	case DEVICE_PATH_GENERIC:
		scope = acpi_device_scope(pdev);
		if (!scope)
			return;

		acpigen_write_scope(scope);
		write_pci_camera_device(pdev);
		break;
	default:
		printk(BIOS_ERR, "Unsupported device type: %x\n"
				 "OS camera driver will likely not work\n", dev->path.type);
		return;
	}

	write_camera_device_common(dev);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	if (dev->path.type == DEVICE_PATH_GENERIC) {
		printk(BIOS_INFO, "%s: %s at PCI %02x.%01x\n", acpi_device_path(pdev),
		       dev->chip_ops->name, PCI_SLOT(pdev->path.pci.devfn),
		       PCI_FUNC(pdev->path.pci.devfn));
	} else {
		printk(BIOS_INFO, "%s: %s at I2C 0x%02x\n", acpi_device_path(dev),
		       dev->chip_ops->name, dev->path.i2c.device);
	}
}

static const char *camera_acpi_name(const struct device *dev)
{
	const char *prefix = NULL;
	static char name[ACPI_NAME_BUFFER_SIZE];
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;

	if (config->acpi_name)
		return config->acpi_name;

	if (CONFIG(MIPI_ACPI_TYPE_WINDOWS_LINUX)) {
		snprintf(name, sizeof(name), "CAM%1u", config->acpi_uid);
		return name;
	}

	/* Multi-device mode */
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
	/* Validate Camera Parameters */
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	bool params_error = false;

	/*
	 * Non-sensor devices (like an aggregator) don't need
	 * SSDB validation, just assign ops and return.
	 */
	if (config->device_type != INTEL_ACPI_CAMERA_SENSOR) {
		dev->ops = &camera_ops;
		return;
	}

	if (!config->ssdb.lanes_used) {
		printk(BIOS_ERR, "MIPI camera: SSDB lanes_used not set\n");
		params_error = true;
	}

	if (!config->ssdb.platform) {
		printk(BIOS_ERR, "MIPI camera: SSDB platform not set\n");
		params_error = true;
	}

	if (config->ssdb.rom_type && !config->rom_address) {
		printk(BIOS_ERR, "MIPI camera: ROM address not set\n");
		params_error = true;
	}

	if (config->ssdb.vcm_type && !config->vcm_address) {
		printk(BIOS_ERR, "MIPI camera: VCM address not set\n");
		params_error = true;
	}

	if (params_error) {
		printk(BIOS_ERR, "MIPI camera: Parameters missing, ACPI device(s) will not be created.\n");
		printk(BIOS_ERR, "MIPI camera: Please fix your devicetree configuration.\n");
		return;
	}

	dev->ops = &camera_ops;
}

struct chip_operations drivers_intel_mipi_camera_ops = {
	.name = "Intel MIPI Camera Device",
	.enable_dev = camera_enable
};
