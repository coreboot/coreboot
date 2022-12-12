/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_dsm.h>

/* -------------------  I2C HID DSM ---------------------------- */

#define ACPI_DSM_I2C_HID_UUID		"3CDFF6F7-4267-4555-AD05-B30A3D8938DE"

/* I2C HID currently supports revision 1 only, for which, only 1 additional
 * function is supported. Thus, the query function should return 0x3:
 *	bit 0 = additional function supported
 *	bit 1 = function with index 1 supported
 * All other revisions do not support additional functions and hence return 0
*/

static void i2c_hid_func0_cb(void *arg)
{
	/* ToInteger (Arg1, Local2) */
	acpigen_write_to_integer(ARG1_OP, LOCAL2_OP);
	/* If (LEqual (Local2, 0x1)) */
	acpigen_write_if_lequal_op_int(LOCAL2_OP, 0x1);
	/*   Return (Buffer (One) { 0x3 }) */
	acpigen_write_return_singleton_buffer(0x3);
	/* Else */
	acpigen_write_else();
	/*     Return (Buffer (One) { 0x0 }) */
	acpigen_write_return_singleton_buffer(0x0);
	acpigen_pop_len();	/* Pop : Else */
}

static void i2c_hid_func1_cb(void *arg)
{
	struct dsm_i2c_hid_config *config = arg;
	acpigen_write_return_byte(config->hid_desc_reg_offset);
}

static void (*i2c_hid_callbacks[2])(void *) = {
	i2c_hid_func0_cb,
	i2c_hid_func1_cb,
};

void acpigen_write_dsm_i2c_hid(struct dsm_i2c_hid_config *config)
{
	acpigen_write_dsm(ACPI_DSM_I2C_HID_UUID, i2c_hid_callbacks,
			  ARRAY_SIZE(i2c_hid_callbacks), config);
}

/* ------------------- End: I2C HID DSM ------------------------- */

#define USB_DSM_UUID    "CE2EE385-00E6-48CB-9F05-2EDB927C4899"

static void usb_dsm_func5_cb(void *arg)
{
	struct dsm_usb_config *config = arg;
	acpigen_write_return_byte(config->usb_lpm_incapable);
}

static void (*usb_dsm_callbacks[6])(void *) = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	usb_dsm_func5_cb,
};

void acpigen_write_dsm_usb(struct dsm_usb_config *config)
{
	acpigen_write_dsm(USB_DSM_UUID, usb_dsm_callbacks,
			  ARRAY_SIZE(usb_dsm_callbacks), config);
}
