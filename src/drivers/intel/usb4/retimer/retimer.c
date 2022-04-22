/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_pld.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <drivers/usb/acpi/chip.h>
#include <gpio.h>
#include <string.h>
#include "chip.h"
#include "retimer.h"

/* Unique ID for the retimer _DSM. */
#define INTEL_USB4_RETIMER_DSM_UUID	"E0053122-795B-4122-8A5E-57BE1D26ACB3"

static const char *usb4_retimer_scope;
static const char *usb4_retimer_path_arg(const char *arg)
{
	/* \\_SB.PCI0.TDMx.ARG */
	static char name[DEVICE_PATH_MAX];
	snprintf(name, sizeof(name), "%s%c%s", usb4_retimer_scope, '.', arg);
	return name;
}

/* Each polling cycle takes up to 25 ms with a total of 12 of these iterations */
#define USB4_RETIMER_ITERATION_NUM	12
#define USB4_RETIMER_POLL_CYCLE_MS	25
static void usb4_retimer_execute_ec_cmd(uint8_t port, uint8_t cmd, uint8_t expected_value,
					struct acpi_gpio *power_gpio)
{
	const char *RFWU = ec_retimer_fw_update_path();
	const uint8_t data = cmd << USB_RETIMER_FW_UPDATE_OP_SHIFT | port;

	/* Invoke EC Retimer firmware update command execution */
	ec_retimer_fw_update(data);
	/* If RFWU has return value 0xfe, return error -1 */
	acpigen_write_if_lequal_namestr_int(RFWU, USB_RETIMER_FW_UPDATE_ERROR);
	acpigen_disable_tx_gpio(power_gpio);
	acpigen_write_return_integer(-1);
	acpigen_pop_len(); /* If */

	acpigen_write_store_int_to_op(USB4_RETIMER_ITERATION_NUM, LOCAL2_OP);
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL2_OP);
	acpigen_emit_byte(ZERO_OP);
	acpigen_write_if_lequal_namestr_int(RFWU, expected_value);
	acpigen_emit_byte(BREAK_OP);
	acpigen_pop_len(); /* If */

	if (cmd == USB_RETIMER_FW_UPDATE_GET_MUX) {
		acpigen_write_if_lequal_namestr_int(RFWU, USB_RETIMER_FW_UPDATE_INVALID_MUX);
		acpigen_write_sleep(USB4_RETIMER_POLL_CYCLE_MS);
		acpigen_emit_byte(DECREMENT_OP);
		acpigen_emit_byte(LOCAL2_OP);
		acpigen_emit_byte(CONTINUE_OP);
		acpigen_pop_len(); /* If */

		acpigen_emit_byte(AND_OP);
		acpigen_emit_namestring(RFWU);
		acpigen_write_integer(USB_RETIMER_FW_UPDATE_MUX_MASK);
		acpigen_emit_byte(LOCAL3_OP);
		acpigen_write_if();
		acpigen_emit_byte(LNOT_OP);
		acpigen_emit_byte(LEQUAL_OP);
		acpigen_emit_byte(LOCAL3_OP);
		acpigen_emit_byte(0);
		acpigen_disable_tx_gpio(power_gpio);
		acpigen_write_return_integer(-1);
		acpigen_pop_len(); /* If */
	} else if (cmd == USB_RETIMER_FW_UPDATE_SET_TBT) {
		/*
		 * EC return either USB_PD_MUX_USB4_ENABLED or USB_PD_MUX_TBT_COMPAT_ENABLED
		 * to RFWU after the USB_RETIMER_FW_UPDATE_SET_TBT command execution. It is
		 * needed to add additional check for USB_PD_MUX_TBT_COMPAT_ENABLED.
		 */
		acpigen_write_if_lequal_namestr_int(RFWU, USB_PD_MUX_TBT_COMPAT_ENABLED);
		acpigen_emit_byte(BREAK_OP);
		acpigen_pop_len(); /* If */
	}

	acpigen_write_sleep(USB4_RETIMER_POLL_CYCLE_MS);
	acpigen_emit_byte(DECREMENT_OP);
	acpigen_emit_byte(LOCAL2_OP);
	acpigen_pop_len(); /* While */

	/*
	 * Check whether there is timeout error
	 * Return: -1 if timeout error occurring
	 */
	acpigen_write_if_lequal_op_int(LOCAL2_OP, 0);
	acpigen_disable_tx_gpio(power_gpio);
	acpigen_write_return_integer(-1);
	acpigen_pop_len(); /* If */
}

static void enable_retimer_online_state(uint8_t port, struct acpi_gpio *power_gpio)
{
	uint8_t expected_value;

	/*
	 * Enable_retimer_online_state under NDA
	 * 1. Force power on
	 * 2. Check if there is a device connected
	 * 3. Suspend PD
	 * 4. Set Mux to USB mode
	 * 5. Set Mux to Safe mode
	 * 6. Set Mux to TBT mode
	 */

	/* Force power on for the retimer on the port */
	acpigen_enable_tx_gpio(power_gpio);

	/*
	 * Get MUX mode state
	 * Return -1 if there is a device connected on the port.
	 * Otherwise proceed Retimer firmware upgrade operation.
	 */
	expected_value = USB_PD_MUX_NONE;
	usb4_retimer_execute_ec_cmd(port, USB_RETIMER_FW_UPDATE_GET_MUX, expected_value,
					power_gpio);

	/*
	 * Suspend PD
	 * Command: USB_RETIMER_FW_UPDATE_SUSPEND_PD
	 * Expect return value: 0
	 */
	expected_value = 0;
	usb4_retimer_execute_ec_cmd(port, USB_RETIMER_FW_UPDATE_SUSPEND_PD, expected_value,
					power_gpio);

	/*
	 * Set MUX USB Mode
	 * Command: USB_RETIMER_FW_UPDATE_SUSPEND_PD
	 * Expect return value: USB_PD_MUX_USB_ENABLED
	 */
	expected_value = USB_PD_MUX_USB_ENABLED;
	usb4_retimer_execute_ec_cmd(port, USB_RETIMER_FW_UPDATE_SET_USB, expected_value,
					power_gpio);

	/*
	 * Set MUX Safe Mode
	 * Command: USB_RETIMER_FW_UPDATE_SET_SAFE
	 * Expect return value: USB_PD_MUX_SAFE_MODE
	 */
	expected_value = USB_PD_MUX_SAFE_MODE;
	usb4_retimer_execute_ec_cmd(port, USB_RETIMER_FW_UPDATE_SET_SAFE, expected_value,
					power_gpio);

	/*
	 * Set MUX TBT Mode
	 * Command: USB_RETIMER_FW_UPDATE_SET_TBT
	 * Expect return value: USB_PD_MUX_USB4_ENABLED or USB_PD_MUX_TBT_COMPAT_ENABLED
	 */
	expected_value = USB_PD_MUX_USB4_ENABLED;
	usb4_retimer_execute_ec_cmd(port, USB_RETIMER_FW_UPDATE_SET_TBT, expected_value,
					power_gpio);
}

static void disable_retimer_online_state(uint8_t port, struct acpi_gpio *power_gpio)
{
	uint8_t expected_value;

	/*
	 * Disable_retimer_online_state
	 * 1. Set Mux to disconnect mode
	 * 2. Resume PD
	 * 3. Force power off
	 */

	/*
	 * Set MUX Disconnect Mode
	 * Command: USB_RETIMER_FW_UPDATE_DISCONNECT
	 * Expect return value: 0
	 */
	expected_value = 0;
	usb4_retimer_execute_ec_cmd(port, USB_RETIMER_FW_UPDATE_DISCONNECT, expected_value,
					power_gpio);

	/*
	 * Resume PD
	 * Command: USB_RETIMER_FW_UPDATE_RESUME_PD
	 * Expect return value: 1
	 */
	expected_value = 1;
	usb4_retimer_execute_ec_cmd(port, USB_RETIMER_FW_UPDATE_RESUME_PD, expected_value,
					power_gpio);

	/* Force power off */
	acpigen_disable_tx_gpio(power_gpio);
}

/*
 * Arg0: UUID e0053122-795b-4122-8a5e-57be1d26acb3
 * Arg1: Revision ID (set to 1)
 * Arg2: Function Index
 *       0: Query command implemented
 *       1: Get power state
 *       2: Set power state
 * Arg3: A package containing parameters for the function specified
 *       by the UUID, revision ID, function index and port index.
 */
static void usb4_retimer_cb_standard_query(uint8_t port, void *arg)
{
	/*
	 * ToInteger (Arg1, Local1)
	 * If (Local1 == 1) {
	 *     Return(Buffer() {0x7})
	 * }
	 * Return (Buffer() {0x01})
	 */
	acpigen_write_to_integer(ARG1_OP, LOCAL1_OP);

	/* Revision 1 supports 2 Functions beyond the standard query */
	acpigen_write_if_lequal_op_int(LOCAL1_OP, 1);
	acpigen_write_return_singleton_buffer(0x7);
	acpigen_pop_len(); /* If */

	/* Other revisions support no additional functions */
	acpigen_write_return_singleton_buffer(0x1);
}

static void usb4_retimer_cb_get_power_state(uint8_t port, void *arg)
{
	const char *PWR;
	char pwr[DEVICE_PATH_MAX];

	snprintf(pwr, sizeof(pwr), "HR.DFP%1d.PWR", port);
	PWR = usb4_retimer_path_arg(pwr);

	/*
	 * If (PWR > 0) {
	 *      Return (1)
	 * }
	 */
	acpigen_write_if();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_namestring(PWR);
	acpigen_emit_byte(0);
	acpigen_write_return_integer(1);

	/*
	 * Else {
	 *      Return (0)
	 * }
	 */
	acpigen_write_else();
	acpigen_write_return_integer(0);
	acpigen_pop_len();
}

static void usb4_retimer_cb_set_power_state(uint8_t port, void *arg)
{
	struct acpi_gpio *power_gpio = arg;
	const char *PWR;
	char pwr[DEVICE_PATH_MAX];

	snprintf(pwr, sizeof(pwr), "HR.DFP%1d.PWR", port);
	PWR = usb4_retimer_path_arg(pwr);

	/*
	 * Get information to set retimer power state from Arg3[0]
	 * Local1 = DeRefOf (Arg3[0])
	 */
	acpigen_get_package_op_element(ARG3_OP, 0, LOCAL1_OP);

	/*
	 * If ((Local1 == 0) && (PWR > 0)) {
	 *      PWR--
	 *      If (PWR == 0) {
	 *		// Disable retimer online state
	 *      }
	 * }
	 */
	acpigen_write_if();
	acpigen_emit_byte(LAND_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_byte(0);
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_namestring(PWR);
	acpigen_emit_byte(0);
	/* PWR-- */
	acpigen_emit_byte(DECREMENT_OP);
	acpigen_emit_namestring(PWR);
	acpigen_write_if_lequal_namestr_int(PWR, 0); /* If (PWR == 0) */
	disable_retimer_online_state(port, power_gpio);
	acpigen_pop_len(); /* If (PWR == 0) */

	/*
	 * Else If ((Local1 == 1) && (PWR == 0)) {
	 *       // Enable retimer online state
	 *       PWR++
	 * }
	 */
	acpigen_write_else();
	acpigen_write_if();
	acpigen_emit_byte(LAND_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_byte(1);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring(PWR);
	acpigen_emit_byte(0);
	enable_retimer_online_state(port, power_gpio);
	/* PWR++ */
	acpigen_emit_byte(INCREMENT_OP);
	acpigen_emit_namestring(PWR);

	/*
	 * Else {
	 *      Return (0)
	 * }
	 */
	acpigen_write_else();
	acpigen_write_return_integer(0);
	acpigen_pop_len(); /* Else */
	acpigen_pop_len(); /* If */

	/*
	 * If (PWR == 1) {
	 *      Return (1)
	 * }
	 */
	acpigen_write_if_lequal_namestr_int(PWR, 1);
	acpigen_write_return_integer(1);

	/*
	 * Else {
	 *      Return (0)
	 * }
	*/
	acpigen_write_else();
	acpigen_write_return_integer(0);
	acpigen_pop_len();
}

static void (*usb4_retimer_callbacks[3])(uint8_t port, void *) = {
	usb4_retimer_cb_standard_query,		/* Function 0 */
	usb4_retimer_cb_get_power_state,	/* Function 1 */
	usb4_retimer_cb_set_power_state,	/* Function 2 */
};

static void usb4_retimer_write_dsm(uint8_t port, const char *uuid,
			void (**callbacks)(uint8_t port, void *), size_t count, void *arg)
{
	struct usb4_retimer_dsm_uuid id = DSM_UUID(uuid, callbacks, count, arg);
	size_t i;

	acpigen_write_to_integer(ARG2_OP, LOCAL0_OP);

	for (i = 0; i < id.count; i++) {
		/* If (LEqual (Local0, i)) */
		acpigen_write_if_lequal_op_int(LOCAL0_OP, i);

		/* Callback to write if handler. */
		if (id.callbacks[i])
			id.callbacks[i](port, id.arg);

		acpigen_pop_len(); /* If */
	}
}

static void usb4_retimer_fill_ssdt(const struct device *dev)
{
	struct drivers_intel_usb4_retimer_config *config = dev->chip_info;
	const struct device *usb_device;
	static char dfp[DEVICE_PATH_MAX];
	struct acpi_pld pld;
	uint8_t dfp_port, usb_port;
	int ec_port = 0;

	usb4_retimer_scope = acpi_device_scope(dev);
	if (!usb4_retimer_scope || !config)
		return;

	/* Scope */
	acpigen_write_scope(usb4_retimer_scope);

	/* Host router */
	acpigen_write_device("HR");
	acpigen_write_ADR(0);
	acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);

	for (dfp_port = 0; dfp_port < DFP_NUM_MAX; dfp_port++) {

		if (!config->dfp[dfp_port].power_gpio.pin_count) {
			printk(BIOS_WARNING, "%s: No DFP%1d power GPIO for %s\n",
				__func__, dfp_port, dev_path(dev));
			continue;
		}

		usb_device = config->dfp[dfp_port].typec_port;
		usb_port = usb_device->path.usb.port_id;

		ec_port = retimer_get_index_for_typec(usb_port);
		if (ec_port == -1) {
			printk(BIOS_ERR, "%s: No relative EC port found for TC port %d\n",
				__func__, usb_port);
			continue;
		}
		/* DFPx */
		snprintf(dfp, sizeof(dfp), "DFP%1d", ec_port);
		acpigen_write_device(dfp);
		/* _ADR part is for the lane adapter */
		acpigen_write_ADR(dfp_port*2 + 1);

		/* Fill _PLD with the same USB 3.x object on the Type-C connector */
		if (CONFIG(DRIVERS_USB_ACPI)) {
			if (usb_acpi_get_pld(usb_device, &pld))
				acpigen_write_pld(&pld);
			else
				printk(BIOS_ERR, "Error retrieving PLD for USB Type-C %d\n",
					usb_port);
		}

		/* Power online reference counter(_PWR) */
		acpigen_write_name("PWR");
		acpigen_write_zero();

		/* Method (_DSM, 4, Serialized) */
		acpigen_write_method_serialized("_DSM", 0x4);
		/* ToBuffer (Arg0, Local0) */
		acpigen_write_to_buffer(ARG0_OP, LOCAL0_OP);
		acpigen_write_if();  /* If (UUID != INTEL_USB4_RETIMER_DSM_UUID) */
		acpigen_emit_byte(LNOT_OP);
		acpigen_emit_byte(LEQUAL_OP);
		acpigen_emit_byte(LOCAL0_OP);
		acpigen_write_uuid(INTEL_USB4_RETIMER_DSM_UUID);
		/* Return (Buffer (One) { 0x0 }) */
		acpigen_write_return_singleton_buffer(0x0);
		acpigen_pop_len();
		usb4_retimer_write_dsm(ec_port, INTEL_USB4_RETIMER_DSM_UUID,
			usb4_retimer_callbacks, ARRAY_SIZE(usb4_retimer_callbacks),
			(void *)&config->dfp[dfp_port].power_gpio);
		/* Default case: Return (Buffer (One) { 0x0 }) */
		acpigen_write_return_singleton_buffer(0x0);

		acpigen_pop_len(); /* Method _DSM */
		acpigen_pop_len(); /* DFP */
	}
	acpigen_pop_len(); /* Host Router */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s.HR: %s at %s\n", usb4_retimer_scope, dev->chip_ops->name,
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

__weak const char *ec_retimer_fw_update_path(void)
{
	return NULL;
}

__weak void ec_retimer_fw_update(uint8_t data)
{
}

/*
 * This function will convert CPU physical port mapping to abstract
 * EC port mapping.
 * For example, board might have enabled TCSS port 1 and 3 as per physical
 * port mapping. Since only 2 TCSS ports are enabled EC will index it as port 0
 * and port 1. So there will be an issue when coreboot sends command to EC for
 * port 3 (with coreboot index of 2). EC will produce an error due to wrong index.
 *
 * Note: Each SoC code using retimer driver needs to implement this function
 * since SoC will have physical port details.
 */
__weak int retimer_get_index_for_typec(uint8_t typec_port)
{
	/* By default assume that retimer port index = Type C port */
	return (int)typec_port;
}
