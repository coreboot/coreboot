/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/intel/cpu_ids.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci_def.h>
#include "chip.h"

#define SENSOR_NAME_UUID	"822ace8f-2814-4174-a56b-5f029fe079ee"
#define SENSOR_TYPE_UUID	"26257549-9271-4ca4-bb43-c4899d5a4881"
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
		if (CONFIG(ACPI_ADL_IPU_ES_SUPPORT)) {
			u32 cpu_id = cpu_get_cpuid();
			if (cpu_id == CPUID_ALDERLAKE_J0 || cpu_id == CPUID_ALDERLAKE_Q0 ||
				cpu_id == CPUID_ALDERLAKE_N_A0 || cpu_id == CPUID_RAPTORLAKE_P_J0)
				acpi_dp_add_integer(dsd, "is_es", 1);
			else
				acpi_dp_add_integer(dsd, "is_es", 0);
		}

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
	 * PLD_PANEL_TOP to PLD_PANEL_FRONT unless disable_pld_defaults is set.
	 */
	if (!config->pld.panel)
		config->pld.panel = PLD_PANEL_FRONT;

	/*
	 * PLD_HORIZONTAL_POSITION_LEFT has a value of zero, so the following will change any
	 * instance of that value to PLD_HORIZONTAL_POSITION_CENTER unless disable_pld_defaults
	 * is set.
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

	if (config->use_pld) {
		if (!config->disable_pld_defaults)
			apply_pld_defaults(config);

		acpigen_write_pld(&config->pld);
	}
}

static uint32_t address_for_dev_type(const struct device *dev, uint8_t dev_type)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	uint16_t i2c_bus = dev->bus ? dev->bus->secondary : 0xFFFF;
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

static void camera_generate_dsm(const struct device *dev)
{
	struct drivers_intel_mipi_camera_config *config = dev->chip_info;
	int local1_ret = 1 + (config->ssdb.vcm_type ? 1 : 0) + (config->ssdb.rom_type ? 1 : 0);
	int next_local1 = 1;
	/* Method (_DSM, 4, NotSerialized) */
	acpigen_write_method("_DSM", 4);

	/* ToBuffer (Arg0, Local0) */
	acpigen_write_to_buffer(ARG0_OP, LOCAL0_OP);

	/* If (LEqual (Local0, ToUUID(uuid))) */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(SENSOR_NAME_UUID);
	acpigen_write_return_string(config->sensor_name ? config->sensor_name : "UNKNOWN");
	acpigen_pop_len();	/* If */

	/* If (LEqual (Local0, ToUUID(uuid))) */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(SENSOR_TYPE_UUID);
	/* ToInteger (Arg2, Local1) */
	acpigen_write_to_integer(ARG2_OP, LOCAL1_OP);

	/* If (LEqual (Local1, 1)) */
	acpigen_write_if_lequal_op_int(LOCAL1_OP, next_local1++);
	acpigen_write_return_integer(local1_ret);
	acpigen_pop_len();	/* If Arg2=1 */

	/* If (LEqual (Local1, 2)) */
	acpigen_write_if_lequal_op_int(LOCAL1_OP, next_local1++);
	acpigen_write_return_integer(address_for_dev_type(dev, DEV_TYPE_SENSOR));
	acpigen_pop_len();	/* If Arg2=2 */

	if (config->ssdb.vcm_type) {
		/* If (LEqual (Local1, 3)) */
		acpigen_write_if_lequal_op_int(LOCAL1_OP, next_local1++);
		acpigen_write_return_integer(address_for_dev_type(dev, DEV_TYPE_VCM));
		acpigen_pop_len();      /* If Arg2=3 */
	}

	if (config->ssdb.rom_type) {
		/* If (LEqual (Local1, 3 or 4)) */
		acpigen_write_if_lequal_op_int(LOCAL1_OP, next_local1);
		acpigen_write_return_integer(address_for_dev_type(dev, DEV_TYPE_ROM));
		acpigen_pop_len();      /* If Arg2=3 or 4 */
	}

	acpigen_pop_len();      /* If uuid */

	/* Return (Buffer (One) { 0x0 }) */
	acpigen_write_return_singleton_buffer(0x0);

	acpigen_pop_len();      /* Method _DSM */
}

static void camera_fill_ssdb_defaults(struct drivers_intel_mipi_camera_config *config)
{
	struct device *cio2 = pcidev_on_root(CIO2_PCI_DEV, CIO2_PCI_FN);
	struct drivers_intel_mipi_camera_config *cio2_config;

	if (config->disable_ssdb_defaults)
		return;

	if (!config->ssdb.bdf_value)
		config->ssdb.bdf_value = PCI_DEVFN(CIO2_PCI_DEV, CIO2_PCI_FN);

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

	if (!config->ssdb.lanes_used) {
		cio2_config = cio2 ? cio2->chip_info : NULL;

		if (!cio2_config) {
			printk(BIOS_ERR, "Failed to get CIO2 config\n");
		} else if (cio2_config->device_type != INTEL_ACPI_CAMERA_CIO2) {
			printk(BIOS_ERR, "Device type isn't CIO2: %u\n",
			       (u32)cio2_config->device_type);
		} else if (config->ssdb.link_used >= cio2_config->cio2_num_ports) {
			printk(BIOS_ERR, "%u exceeds CIO2's %u links\n",
			       (u32)config->ssdb.link_used,
			       (u32)cio2_config->cio2_num_ports);
		} else {
			config->ssdb.lanes_used =
				cio2_config->cio2_lanes_used[config->ssdb.link_used];
		}
	}
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
	const char *remote_name;
	struct device *cio2 = pcidev_on_root(CIO2_PCI_DEV, CIO2_PCI_FN);

	camera_generate_pld(dev);

	camera_fill_ssdb_defaults(config);

	/* _DSM */
	camera_generate_dsm(dev);

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

	if (config->device_type == INTEL_ACPI_CAMERA_SENSOR)
		acpigen_write_name_integer("_ADR", 0);

	if (config->acpi_hid)
		acpigen_write_name_string("_HID", config->acpi_hid);
	else if (config->device_type == INTEL_ACPI_CAMERA_VCM ||
		 config->device_type == INTEL_ACPI_CAMERA_NVM)
		acpigen_write_name_string("_HID", ACPI_DT_NAMESPACE_HID);

	acpigen_write_name_integer("_UID", config->acpi_uid);
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

	/* Mark it as Camera related device */
	if (config->device_type == INTEL_ACPI_CAMERA_CIO2 ||
	    config->device_type == INTEL_ACPI_CAMERA_IMGU ||
	    config->device_type == INTEL_ACPI_CAMERA_SENSOR ||
	    config->device_type == INTEL_ACPI_CAMERA_VCM) {
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
	const struct device *pdev;

	if (config->has_power_resource) {
		pdev = dev->bus->dev;
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
		pdev = dev->bus->dev;
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
	static char name[ACPI_NAME_BUFFER_SIZE];
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
