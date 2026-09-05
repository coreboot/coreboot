/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <stdio.h>

#include "chip.h"
#include "../ssdb.h"

#define UUID_DSM_I2C		"26257549-9271-4ca4-bb43-c4899d5a4881"
#define UUID_DSM_I2C_V2		"5815c5c8-c47d-477b-9a8d-76173176414b"
#define UUID_DSM_GPIO		"79234640-9e10-4fea-a5c1-b5aa8b19756f"
#define UUID_DSM_IMGCLK		"82c0d13a-78c5-4244-9bb1-eb8b539a8d11"

#define CLK_ENABLE_METHOD	"\\_SB.PCI0.MCON"
#define CLK_DISABLE_METHOD	"\\_SB.PCI0.MCOF"
/* IMGCLK _DSM frequency args (differ from MCON's 0=24MHz / 1=19.2MHz) */
#define DSM_CLK_FREQ_19_2MHZ	1
#define DSM_CLK_FREQ_24MHZ	3
#define MCON_FREQ_24MHZ		0
#define MCON_FREQ_19_2MHZ	1

static uint32_t address_for_dev_type(const struct device *dev, uint8_t dev_type)
{
	uint16_t i2c_bus = dev->upstream ? dev->upstream->secondary : 0xFFFF;
	uint16_t i2c_addr;

	switch (dev_type) {
	case DEV_TYPE_PMIC:
		i2c_addr = dev->path.i2c.device;
		break;
	default:
		return 0;
	}

	return (((uint32_t)i2c_bus) << 24 | ((uint32_t)i2c_addr) << 8 | dev_type);
}

/*
 * Generate ASL DSM code for I2C device addresses (V2)
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
 *             i2c_count, pmic_addr, 0, 0, ...
 *         })
 *         // Buffer is 13 * 4 = 52 bytes: count + up to 12 device addresses
 *     }
 * }
 */
static void pmic_generate_dsm_i2c_v2(const struct device *dev)
{
	int i2c_count = 1;

	/* If (LEqual (Local0, ToUUID(uuid))) */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(UUID_DSM_I2C_V2);

	/* If (LEqual (Arg2, Zero)) */
	acpigen_write_if_lequal_op_int(ARG2_OP, 0);

		/* If (LEqual (Arg1, Zero)) */
		acpigen_write_if_lequal_op_int(ARG1_OP, 0);
			/* Return (Buffer (One) { 0x3 }) */
			acpigen_write_return_singleton_buffer(0x3);
		/* Else */
		acpigen_write_else();
			/* Return (Buffer (One) { 0x1 }) */
			acpigen_write_return_singleton_buffer(0x1);
		acpigen_pop_len();	/* If Arg1=0 */

	acpigen_pop_len();	/* If Arg2=0 */

	/* If (LEqual (Arg2, One)) */
	acpigen_write_if_lequal_op_int(ARG2_OP, 1);

		/* Buffer is 13 * 4 = 52 bytes: count + up to 12 device addresses */
		uint32_t i2c_buffer[13] = {0};
		_Static_assert(sizeof(i2c_buffer) == 13*4, "i2c_buffer size must be 52 bytes");

		i2c_buffer[0] = i2c_count;
		i2c_buffer[1] = address_for_dev_type(dev, DEV_TYPE_PMIC);

		acpigen_write_return_byte_buffer((uint8_t *)i2c_buffer, sizeof(i2c_buffer));

	acpigen_pop_len();	/* If Arg2=1 */

	acpigen_pop_len();      /* If uuid */
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
 *         Return (pmic_address)
 *     }
 * }
 */
static void pmic_generate_dsm_i2c(const struct device *dev)
{
	int i2c_count = 1;
	int i2c_idx = 1;

	/* If (LEqual (Local0, ToUUID(uuid))) */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(UUID_DSM_I2C);

	/* ToInteger (Arg2, Local1) */
	acpigen_write_to_integer(ARG2_OP, LOCAL1_OP);

	/* If (LEqual (Local1, 1)) */
	acpigen_write_if_lequal_op_int(LOCAL1_OP, i2c_idx++);
	acpigen_write_return_integer(i2c_count);
	acpigen_pop_len();	/* If Arg2=1 */

	/* If (LEqual (Local1, 2)) */
	acpigen_write_if_lequal_op_int(LOCAL1_OP, i2c_idx++);
	acpigen_write_return_integer(address_for_dev_type(dev, DEV_TYPE_PMIC));
	acpigen_pop_len();	/* If Arg2=2 */

	acpigen_pop_len();      /* If uuid */
}

static uint32_t gpio_for_pin_idx(const struct device *dev, uint8_t pin_idx)
{
	struct drivers_intel_mipi_camera_pmic_config *config = dev->chip_info;
	struct mipicam_gpio_ctrl_panel gpio_panel = config->gpio_panel[pin_idx];
	uint8_t gpio_function = gpio_panel.gpio_function;
	bool gpio_active_value = gpio_panel.gpio_active_value;
	bool gpio_initial_value = gpio_panel.gpio_initial_value;
	uint16_t gpio_addr;

	switch (gpio_function) {
	case MIPICAM_GPIO_TYPE_RESET:
	case MIPICAM_GPIO_TYPE_POWER:
	case MIPICAM_GPIO_TYPE_CLOCK:
	case MIPICAM_GPIO_TYPE_PLED:
	case MIPICAM_GPIO_TYPE_STROBE:
	case MIPICAM_GPIO_TYPE_HANDSHAKE:
	case MIPICAM_GPIO_TYPE_HOTPLUG_DETECT:
		gpio_addr = config->gpio_panel[pin_idx].gpio.pins[0];
		break;
	default:
		return 0;
	}

	return (((uint32_t)gpio_active_value) << 24 | ((uint32_t)gpio_initial_value) << 16 | ((uint32_t)gpio_addr) << 8 | gpio_function);
}

static bool pmic_has_clock_gpio(const struct drivers_intel_mipi_camera_pmic_config *config)
{
	for (int i = 0; i < MAX_MIPICAM_GPIO_CONFIGS; i++) {
		if (config->gpio_panel[i].gpio.pin_count == 0)
			break;
		if (config->gpio_panel[i].gpio_function == MIPICAM_GPIO_TYPE_CLOCK)
			return true;
	}

	return false;
}

static void pmic_generate_dsm_gpio(const struct device *dev)
{
	struct drivers_intel_mipi_camera_pmic_config *config = dev->chip_info;
	int gpio_count = 0;
	int gpio_idx = 1;

	for (int i = 0; i < MAX_MIPICAM_GPIO_CONFIGS; i++) {
		if (config->gpio_panel[i].gpio.pin_count == 0) {
			break;
		}
		gpio_count++;
	}

	/* If (LEqual (Local0, ToUUID(uuid))) */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(UUID_DSM_GPIO);

	/* ToInteger (Arg2, Local1) */
	acpigen_write_to_integer(ARG2_OP, LOCAL1_OP);

	/* If (LEqual (Local1, 1)) */
	acpigen_write_if_lequal_op_int(LOCAL1_OP, gpio_idx++);
	acpigen_write_return_integer(gpio_count);
	acpigen_pop_len();	/* If Arg2=1 */

	while (gpio_idx < gpio_count + 2) {
		/* If (LEqual (Local1, 2 -> 7)) for up to MAX_MIPICAM_GPIO_CONFIGS */
		acpigen_write_if_lequal_op_int(LOCAL1_OP, gpio_idx++);
		acpigen_write_return_integer(gpio_for_pin_idx(dev, gpio_idx - 3));
		acpigen_pop_len();      /* If Arg2=n */
	}

	acpigen_pop_len();      /* If uuid */
}

/*
 * Discrete IMGCLK _DSM (82c0d13a-...): used when there is no clock-enable GPIO.
 * Function 1 package: [clock_index, enable, dsm_frequency]
 * Calls \_SB.PCI0.MCON / MCOF when present (TGL+ camera_clock_ctl.asl).
 * DSM freq 3 = 24MHz, anything else (incl. 1) maps to MCON 19.2MHz.
 */
static void pmic_generate_dsm_imgclk(void)
{
	/* If (LEqual (Local0, ToUUID(uuid))) */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_uuid(UUID_DSM_IMGCLK);

	/* If (LEqual (Arg2, Zero)) - query supported functions */
	acpigen_write_if_lequal_op_int(ARG2_OP, 0);
		acpigen_write_if_lequal_op_int(ARG1_OP, 0);
			/* Revision 0: functions 0 and 1 */
			acpigen_write_return_singleton_buffer(0x3);
		acpigen_write_else();
			acpigen_write_return_singleton_buffer(0x1);
		acpigen_pop_len();	/* If Arg1=0 */
	acpigen_pop_len();	/* If Arg2=0 */

	/* If (LEqual (Arg2, One)) - enable/disable clock */
	acpigen_write_if_lequal_op_int(ARG2_OP, 1);

		/* Local1 = clock index, Local2 = enable, Local3 = DSM frequency */
		acpigen_get_package_op_element(ARG3_OP, 0, LOCAL1_OP);
		acpigen_write_to_integer(LOCAL1_OP, LOCAL1_OP);
		acpigen_get_package_op_element(ARG3_OP, 1, LOCAL2_OP);
		acpigen_write_to_integer(LOCAL2_OP, LOCAL2_OP);
		acpigen_get_package_op_element(ARG3_OP, 2, LOCAL3_OP);
		acpigen_write_to_integer(LOCAL3_OP, LOCAL3_OP);

		/* Local4 = MCON frequency (0=24MHz, 1=19.2MHz) */
		acpigen_write_if_lequal_op_int(LOCAL3_OP, DSM_CLK_FREQ_24MHZ);
			acpigen_write_store_int_to_op(MCON_FREQ_24MHZ, LOCAL4_OP);
		acpigen_write_else();
			acpigen_write_store_int_to_op(MCON_FREQ_19_2MHZ, LOCAL4_OP);
		acpigen_pop_len();

		acpigen_write_if_lequal_op_int(LOCAL2_OP, 1);
			acpigen_write_if_cond_ref_of(CLK_ENABLE_METHOD);
				acpigen_emit_namestring(CLK_ENABLE_METHOD);
				acpigen_emit_byte(LOCAL1_OP);
				acpigen_emit_byte(LOCAL4_OP);
			acpigen_write_if_end();
		acpigen_write_else();
			acpigen_write_if_cond_ref_of(CLK_DISABLE_METHOD);
				acpigen_emit_namestring(CLK_DISABLE_METHOD);
				acpigen_emit_byte(LOCAL1_OP);
			acpigen_write_if_end();
		acpigen_pop_len();	/* If enable */

		acpigen_write_return_integer(0);
	acpigen_pop_len();	/* If Arg2=1 */

	acpigen_pop_len();	/* If uuid */
}

static void pmic_generate_dsm(const struct device *dev)
{
	struct drivers_intel_mipi_camera_pmic_config *config = dev->chip_info;
	/* Method (_DSM, 4, NotSerialized) */
	acpigen_write_method("_DSM", 4);

	/* ToBuffer (Arg0, Local0) */
	acpigen_write_to_buffer(ARG0_OP, LOCAL0_OP);

	switch (config->cldb.pmic_type) {
	case CL_TYPE_DISCRETE:
		pmic_generate_dsm_gpio(dev);
		/* Linux INT3472 prefers a clock GPIO when present; emit IMGCLK only otherwise. */
		if (!pmic_has_clock_gpio(config))
			pmic_generate_dsm_imgclk();
		break;
	case CL_TYPE_TPS68470:
		pmic_generate_dsm_i2c_v2(dev);
		pmic_generate_dsm_i2c(dev);
		break;
	default:
		break;
	}

	/* Return (Buffer (One) { 0x0 }) */
	acpigen_write_return_singleton_buffer(0x0);

	acpigen_pop_len();      /* Method _DSM */
}

static void pmic_fill_cldb_defaults(struct drivers_intel_mipi_camera_pmic_config *config)
{
	config->cldb.version = 1;

	if (!config->cldb.sensor_card_sku.card_type)
		config->cldb.sensor_card_sku.card_type = SKU_CRD_D;
}

static bool pmic_cldb_params_ok(const struct drivers_intel_mipi_camera_pmic_config *config)
{
	if (!config->cldb.platform) {
		printk(BIOS_ERR, "MIPI PMIC: CLDB platform not set\n");
		return false;
	}

	return true;
}

static void pmic_write_resources(const struct device *dev, const char *scope)
{
	struct drivers_intel_mipi_camera_pmic_config *config = dev->chip_info;

	if (!config) {
		printk(BIOS_ERR, "pmic config is NULL\n");
		return;
	}

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();

	if (dev->path.type == DEVICE_PATH_I2C) {
		struct acpi_i2c i2c = {
			.address = dev->path.i2c.device,
			.mode_10bit = dev->path.i2c.mode_10bit,
			.speed = I2C_SPEED_FAST,
			.resource = scope,
		};
		acpi_device_write_i2c(&i2c);
	}

	for (int i = 0; i < MAX_MIPICAM_GPIO_CONFIGS; i++) {
		if (config->gpio_panel[i].gpio.pin_count == 0) {
			break;
		}
		acpi_device_write_gpio(&config->gpio_panel[i].gpio);
	}

	acpigen_write_resourcetemplate_footer();
}

static void pmic_fill_ssdt(const struct device *dev)
{
	struct drivers_intel_mipi_camera_pmic_config *config = dev->chip_info;
	const char *scope = NULL;
	const struct device *pdev = NULL;

	if (!config) {
		printk(BIOS_ERR, "pmic config is NULL\n");
		return;
	}

	if (dev->upstream)
		pdev = dev->upstream->dev;

	pmic_fill_cldb_defaults(config);

	if (!pmic_cldb_params_ok(config)) {
		printk(BIOS_ERR, "MIPI PMIC: Parameters missing, ACPI device will not be created.\n");
		printk(BIOS_ERR, "MIPI PMIC: Please fix your devicetree configuration.\n");
		return;
	}

	switch (config->cldb.pmic_type) {
	case CL_TYPE_DISCRETE:
		if (config->cldb.sensor_card_sku.card_type != SKU_CRD_D) {
			printk(BIOS_ERR, "pmic type %x requires sku to be CRD-D\n",
			       config->cldb.pmic_type);
			return;
		}
		break;
	case CL_TYPE_TPS68470:
		if (dev->path.type != DEVICE_PATH_I2C) {
			printk(BIOS_ERR, "pmic type %x requires i2c resource\n",
			       config->cldb.pmic_type);
			return;
		}
		break;
	default:
		printk(BIOS_ERR, "Invalid pmic type: %x\n", config->cldb.pmic_type);
		return;
	}

	scope = acpi_device_scope(dev);
	if (!scope)
		return;

	acpigen_write_scope(scope);

	acpigen_write_device(acpi_device_name(dev));

	if (config->acpi_hid)
		acpigen_write_name_string("_HID", config->acpi_hid);
	else
		acpigen_write_name_string("_HID", "INT3472");
	acpigen_write_name_integer("_UID", config->acpi_uid);

	if (config->acpi_sta)
		acpigen_write_STA_ext(config->acpi_sta);
	else
		acpigen_write_STA(acpi_device_status(dev));

	pmic_write_resources(dev, scope);

	/* _DSM */
	pmic_generate_dsm(dev);

	acpigen_write_method_serialized("CLDB", 0);
	acpigen_write_return_byte_buffer((uint8_t *)&config->cldb, sizeof(config->cldb));
	acpigen_pop_len(); /* Method */

	/* Power Resource (named PR## via acpi_device_add_power_res) */
	if (config->has_power_resource) {
		const struct acpi_power_res_params power_res_params = {
			&config->reset_gpio,
			config->reset_delay_ms,
			config->reset_off_delay_ms,
			&config->enable_gpio,
			config->enable_delay_ms,
			config->enable_off_delay_ms,
			&config->stop_gpio,
			config->stop_delay_ms,
			config->stop_off_delay_ms,
			config->use_gpio_for_status
		};
		acpi_device_add_power_res(&power_res_params);
	}

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	if (dev->path.type == DEVICE_PATH_GENERIC && pdev &&
	    pdev->path.type == DEVICE_PATH_PCI) {
		printk(BIOS_INFO, "%s: %s at PCI %02x.%01x\n", acpi_device_path(pdev),
		       dev->chip_ops->name, PCI_SLOT(pdev->path.pci.devfn),
		       PCI_FUNC(pdev->path.pci.devfn));
	} else if (dev->path.type == DEVICE_PATH_I2C) {
		printk(BIOS_INFO, "%s: %s at I2C 0x%02x\n", acpi_device_path(dev),
		       dev->chip_ops->name, dev->path.i2c.device);
	} else {
		printk(BIOS_INFO, "%s: %s\n", acpi_device_path(dev),
		       dev->chip_ops->name);
	}
}

static const char *pmic_acpi_name(const struct device *dev)
{
	const char *prefix = NULL;
	static char name[ACPI_NAME_BUFFER_SIZE];
	struct drivers_intel_mipi_camera_pmic_config *config = dev->chip_info;

	if (!config) {
		printk(BIOS_ERR, "pmic config is NULL\n");
		return NULL;
	}

	if (config->acpi_name)
		return config->acpi_name;

	switch (config->cldb.pmic_type) {
	case CL_TYPE_DISCRETE:
		prefix = "DSC";
		break;
	case CL_TYPE_TPS68470:
		prefix = "CLP";
		break;
	default:
		printk(BIOS_ERR, "Invalid pmic type: %x\n", config->cldb.pmic_type);
		return NULL;
	}

	/*
	 * The pmic # knows which uid # it is, so that's used as the basis for the
	 * instance #.
	 */
	snprintf(name, sizeof(name), "%s%1u", prefix,
		 config->acpi_uid);
	return name;
}

static struct device_operations pmic_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= pmic_acpi_name,
	.acpi_fill_ssdt		= pmic_fill_ssdt,
};

static void pmic_enable(struct device *dev)
{
	/* Only enable for Windows/Linux ACPI type */
	if (!CONFIG(MIPI_ACPI_TYPE_WINDOWS_LINUX))
		return;

	dev->ops = &pmic_ops;
}

struct chip_operations drivers_intel_mipi_camera_pmic_ops = {
	.name = "Intel MIPI Camera PMIC Device",
	.enable_dev = pmic_enable
};
