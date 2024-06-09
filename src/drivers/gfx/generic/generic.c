/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "chip.h"

#define ACPI_DSM_PRIVACY_SCREEN_UUID	"C7033113-8720-4CEB-9090-9D52B3E52D73"

#define ACPI_METHOD_EPS_PRESENT		"EPSP"
#define ACPI_METHOD_EPS_STATE		"EPSS"
#define ACPI_METHOD_EPS_ENABLE		"EPSE"
#define ACPI_METHOD_EPS_DISABLE		"EPSD"

static void privacy_screen_detect_cb(void *arg)
{
	struct drivers_gfx_generic_privacy_screen_config *config = arg;

	acpigen_write_store();
	acpigen_emit_namestring(config->detect_function);
	acpigen_emit_byte(LOCAL2_OP);
	acpigen_write_if_lequal_op_int(LOCAL2_OP, 1);
	acpigen_write_return_singleton_buffer(0xF);
	acpigen_pop_len();
}
static void privacy_screen_get_status_cb(void *arg)
{
	struct drivers_gfx_generic_privacy_screen_config *config = arg;

	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(config->status_function);
}
static void privacy_screen_enable_cb(void *arg)
{
	struct drivers_gfx_generic_privacy_screen_config *config = arg;

	acpigen_emit_namestring(config->enable_function);
}
static void privacy_screen_disable_cb(void *arg)
{
	struct drivers_gfx_generic_privacy_screen_config *config = arg;

	acpigen_emit_namestring(config->disable_function);
}

static void (*privacy_screen_callbacks[])(void *) = {
	privacy_screen_detect_cb,
	privacy_screen_get_status_cb,
	privacy_screen_enable_cb,
	privacy_screen_disable_cb,
};

static void privacy_gpio_acpigen(struct acpi_gpio *gpio)
{
	/* EPS Present */
	acpigen_write_method(ACPI_METHOD_EPS_PRESENT, 0);
	acpigen_write_return_byte(1);
	acpigen_pop_len();

	/* EPS State */
	acpigen_write_method(ACPI_METHOD_EPS_STATE, 0);
	acpigen_get_rx_gpio(gpio);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_pop_len();

	/* EPS Enable */
	acpigen_write_method(ACPI_METHOD_EPS_ENABLE, 0);
	acpigen_enable_tx_gpio(gpio);
	acpigen_pop_len();

	/* EPS Disable */
	acpigen_write_method(ACPI_METHOD_EPS_DISABLE, 0);
	acpigen_disable_tx_gpio(gpio);
	acpigen_pop_len();
}

static void gfx_fill_privacy_screen_dsm(
		struct drivers_gfx_generic_privacy_screen_config *privacy)
{
	if (!privacy->enabled)
		return;

	/* Populate ACPI methods, if EPS controlled via gpio */
	if (privacy->gpio.pin_count == 1) {
		privacy_gpio_acpigen(&privacy->gpio);
		privacy->detect_function = ACPI_METHOD_EPS_PRESENT;
		privacy->status_function = ACPI_METHOD_EPS_STATE;
		privacy->enable_function = ACPI_METHOD_EPS_ENABLE;
		privacy->disable_function = ACPI_METHOD_EPS_DISABLE;
	}

	acpigen_write_dsm(ACPI_DSM_PRIVACY_SCREEN_UUID,
		privacy_screen_callbacks,
		ARRAY_SIZE(privacy_screen_callbacks),
		privacy);
}

static void gfx_fill_ssdt_generator(const struct device *dev)
{
	size_t i;
	struct drivers_gfx_generic_config *config = dev->chip_info;

	const char *scope = acpi_device_scope(dev);

	if (!scope)
		return;

	acpigen_write_scope(scope);

	/* Method (_DOD, 0) */
	acpigen_write_method("_DOD", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(config->device_count);
	for (i = 0; i < config->device_count; i++) {
		/* Generate the Device ID if addr = 0 and type != 0 */
		if (!config->device[i].addr && config->device[i].type)
			/* Though not strictly necessary, set the display index and
			   port attachment to the device index, to ensure uniqueness */
			config->device[i].addr = (config->device[i].type << 8) | (i << 4) | (i);
		acpigen_write_dword(DOD_DID_STD | DOD_FW_DETECT | config->device[i].addr);
	}
	acpigen_pop_len(); /* End Package. */
	acpigen_pop_len(); /* End Method. */

	for (i = 0; i < config->device_count; i++) {
		acpigen_write_device(config->device[i].name);
		if (config->device[i].hid)
			acpigen_write_name_string("_HID", config->device[i].hid);
		else
			acpigen_write_name_integer("_ADR", config->device[i].addr);

		acpigen_write_name_integer("_STA", 0xF);
		gfx_fill_privacy_screen_dsm(&config->device[i].privacy);

		if (config->device[i].use_pld)
			acpigen_write_pld(&config->device[i].pld);

		/* Generate ACPI brightness controls for LCD on Intel iGPU  */
		if (CONFIG(INTEL_GMA_ACPI) && strcmp(config->device[i].name, "LCD0") == 0) {
			/*
			  Method (_BCL, 0, NotSerialized)
			  {
				Return (^^XBCL())
			  }
			*/
			acpigen_write_method("_BCL", 0);
			acpigen_emit_byte(RETURN_OP);
			acpigen_emit_namestring("^^XBCL");
			acpigen_pop_len();

			/*
			  Method (_BCM, 1, NotSerialized)
			  {
				^^XBCM(Arg0)
			  }
			*/
			acpigen_write_method("_BCM", 1);
			acpigen_emit_namestring("^^XBCM");
			acpigen_emit_byte(ARG0_OP);
			acpigen_pop_len();

			/*
			  Method (_BQC, 0, NotSerialized)
			  {
				Return (^^XBQC())
			  }
			*/
			acpigen_write_method("_BQC", 0);
			acpigen_emit_byte(RETURN_OP);
			acpigen_emit_namestring("^^XBQC");
			acpigen_pop_len();
		}

		acpigen_pop_len(); /* Device */
	}
	acpigen_pop_len(); /* Scope */
}

static const char *gfx_acpi_name(const struct device *dev)
{
	struct drivers_gfx_generic_config *config = dev->chip_info;

	return config->name ? : "GFX0";
}

static struct device_operations gfx_ops = {
	.acpi_name	= gfx_acpi_name,
	.acpi_fill_ssdt	= gfx_fill_ssdt_generator,
};

static void gfx_enable(struct device *dev)
{
	struct drivers_gfx_generic_config *config = dev->chip_info;

	if (!config || !dev->enabled)
		return;

	dev->ops = &gfx_ops;
}

struct chip_operations drivers_gfx_generic_ops = {
	.name = "Generic Graphics Device",
	.enable_dev = gfx_enable
};
