/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <gpio.h>
#include <string.h>
#include "chip.h"

/* Unique ID for the retimer _DSM. */
#define INTEL_USB4_RETIMER_DSM_UUID	"61788900-C470-42BB-80F0-23A313864593"

/*
 * Arg0: UUID
 * Arg1: Revision ID (set to 1)
 * Arg2: Function Index
 *       0: Query command implemented
 *       1: Query force power enable state
 *       2: Set force power state
 * Arg3: A package containing parameters for the function specified
 *       by the UUID, revision ID and function index.
 */

static void usb4_retimer_cb_standard_query(void *arg)
{
	/*
	 * ToInteger (Arg1, Local2)
	 * If (Local2 == 1) {
	 *     Return(Buffer() {0x07})
	 * }
	 * Return (Buffer() {0x01})
	 */
	acpigen_write_to_integer(ARG1_OP, LOCAL2_OP);

	/* Revision 1 supports 2 Functions beyond the standard query */
	acpigen_write_if_lequal_op_int(LOCAL2_OP, 1);
	acpigen_write_return_singleton_buffer(0x07);
	acpigen_pop_len(); /* If */

	/* Other revisions support no additional functions */
	acpigen_write_return_singleton_buffer(0);
}

static void usb4_retimer_cb_get_power_state(void *arg)
{
	struct acpi_gpio *power_gpio = arg;

	/*
	 * // Read power gpio into Local0
	 * Store (\_SB.PCI0.GTXS (power_gpio), Local0)
	 * Return (Local0)
	 */
	acpigen_get_tx_gpio(power_gpio);
	acpigen_write_return_op(LOCAL0_OP);
}

static void usb4_retimer_cb_set_power_state(void *arg)
{
	struct acpi_gpio *power_gpio = arg;

	/*
	 * // Get argument for on/off from Arg3[0]
	 * Local0 = DeRefOf (Arg3[0])
	 */
	acpigen_get_package_op_element(ARG3_OP, 0, LOCAL0_OP);

	/*
	 * If (Local0 == 0) {
	 *     // Turn power off
	 *     \_SB.PCI0.CTXS (power_gpio)
	 * }
	 */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 0);
	acpigen_disable_tx_gpio(power_gpio);
	acpigen_pop_len(); /* If */

	/*
	 * Else {
	 *     // Turn power on
	 *     \_SB.PCI0.STXS (power_gpio)
	 * }
	 */
	acpigen_write_else();
	acpigen_enable_tx_gpio(power_gpio);
	acpigen_pop_len();

	/* Return (Zero) */
	acpigen_write_return_integer(0);
}

static void (*usb4_retimer_callbacks[3])(void *) = {
	usb4_retimer_cb_standard_query,		/* Function 0 */
	usb4_retimer_cb_get_power_state,	/* Function 1 */
	usb4_retimer_cb_set_power_state,	/* Function 2 */
};

static void usb4_retimer_fill_ssdt(const struct device *dev)
{
	const struct drivers_intel_usb4_retimer_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);

	if (!scope || !config)
		return;

	if (!config->power_gpio.pin_count) {
		printk(BIOS_ERR, "%s: Power GPIO required for %s\n", __func__, dev_path(dev));
		return;
	}

	/* Write the _DSM that toggles power with provided GPIO. */
	acpigen_write_scope(scope);
	acpigen_write_dsm(INTEL_USB4_RETIMER_DSM_UUID, usb4_retimer_callbacks,
			  ARRAY_SIZE(usb4_retimer_callbacks), (void *)&config->power_gpio);
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev), dev->chip_ops->name,
	       dev_path(dev));
}

static struct device_operations usb4_retimer_dev_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.acpi_fill_ssdt	= usb4_retimer_fill_ssdt,
};

static void usb4_retimer_enable(struct device *dev)
{
	dev->ops = &usb4_retimer_dev_ops;
}

struct chip_operations drivers_intel_usb4_retimer_ops = {
	CHIP_NAME("Intel USB4 Retimer")
	.enable_dev = usb4_retimer_enable
};
