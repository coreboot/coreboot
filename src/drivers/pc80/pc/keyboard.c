/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <pc80/keyboard.h>
#include <device/device.h>
#include <arch/io.h>
#include <delay.h>
#include <types.h>
#include <acpi/acpi.h>

#define KBD_DATA	0x60
#define KBD_COMMAND	0x64
#define KBD_STATUS	0x64
#define KBD_IBF	(1 << 1)	// 1: input buffer full (data ready for ec)
#define KBD_OBF	(1 << 0)	// 1: output buffer full (data ready for host)

// Keyboard Controller Commands
#define KBC_CMD_READ_COMMAND	0x20	// Read command byte
#define KBC_CMD_WRITE_COMMAND	0x60	// Write command byte
#define KBC_CMD_AUX_ENABLE	0xA8	// Auxiliary Interface enable
#define KBC_CMD_AUX_TEST	0xA9	// Auxiliary Interface test
#define KBC_CMD_SELF_TEST	0xAA	// Controller self-test
#define KBC_CMD_KBD_TEST	0xAB	// Keyboard Interface test

// Keyboard Controller Replies
#define KBC_REPLY_SELFTEST_OK	0x55	// controller self-test succeeded

//
// Keyboard Replies
//
#define KBD_REPLY_POR		0xAA	// Power on reset
#define KBD_REPLY_ACK		0xFA	// Command ACK
#define KBD_REPLY_RESEND	0xFE	// Command NACK, send command again

/* Wait 400ms for keyboard controller answers */
#define KBC_TIMEOUT_IN_MS 400

static int kbc_input_buffer_empty(void)
{
	u32 timeout;

	for (timeout = KBC_TIMEOUT_IN_MS;
	     timeout && (inb(KBD_STATUS) & KBD_IBF); timeout--)
		mdelay(1);

	if (!timeout)
		printk(BIOS_WARNING,
		       "Unexpected Keyboard controller input buffer full\n");
	return !!timeout;
}

static int kbc_output_buffer_full(void)
{
	u32 timeout;

	for (timeout = KBC_TIMEOUT_IN_MS;
	     timeout && ((inb(KBD_STATUS) & KBD_OBF) == 0); timeout--)
		mdelay(1);

	if (!timeout)
		printk(BIOS_INFO,
		       "Keyboard controller output buffer result timeout\n");
	return !!timeout;
}

static int kbc_cleanup_buffers(void)
{
	u32 timeout;

	for (timeout = KBC_TIMEOUT_IN_MS;
	     timeout && (inb(KBD_STATUS) & (KBD_OBF | KBD_IBF)); timeout--) {
		mdelay(1);
		inb(KBD_DATA);
	}

	if (!timeout) {
		printk(BIOS_ERR,
		       "Couldn't cleanup the keyboard controller buffers\n");
		printk(BIOS_ERR, "Status (0x%x): 0x%x, Buffer (0x%x): 0x%x\n",
		       KBD_STATUS, inb(KBD_STATUS), KBD_DATA, inb(KBD_DATA));
	}

	return !!timeout;
}

static enum cb_err kbc_self_test(uint8_t probe_aux, uint8_t *aux_probe_result)
{
	uint8_t self_test;
	uint8_t byte;

	/* Set initial aux probe output value */
	if (aux_probe_result)
		*aux_probe_result = 0;

	/* Clean up any junk that might have been in the KBC.
	 * Both input and output buffers must be empty.
	 */
	if (!kbc_cleanup_buffers())
		return CB_KBD_CONTROLLER_FAILURE;

	/* reset/self test 8042 - send cmd 0xAA */
	outb(KBC_CMD_SELF_TEST, KBD_COMMAND);

	if (!kbc_output_buffer_full()) {
		/* There probably is no keyboard controller. */
		printk(BIOS_ERR, "Could not reset keyboard controller.\n");
		return CB_KBD_CONTROLLER_FAILURE;
	}

	/* read self-test result, 0x55 is returned in the output buffer */
	self_test = inb(KBD_DATA);

	if (self_test != 0x55) {
		printk(BIOS_ERR, "Keyboard Controller self-test failed: 0x%x\n",
		       self_test);
		return CB_KBD_CONTROLLER_FAILURE;
	}

	/* ensure the buffers are empty */
	kbc_cleanup_buffers();

	/* keyboard interface test */
	outb(KBC_CMD_KBD_TEST, KBD_COMMAND);

	if (!kbc_output_buffer_full()) {
		printk(BIOS_ERR, "Keyboard Interface test timed out.\n");
		return CB_KBD_CONTROLLER_FAILURE;
	}

	/* read test result, 0x00 should be returned in case of no failures */
	self_test = inb(KBD_DATA);

	if (self_test != 0x00) {
		printk(BIOS_ERR, "Keyboard Interface test failed: 0x%x\n",
		       self_test);
		return CB_KBD_INTERFACE_FAILURE;
	}

	if (probe_aux) {
		/* aux interface detect */
		outb(KBC_CMD_AUX_ENABLE, KBD_COMMAND);
		if (!kbc_input_buffer_empty()) {
			printk(BIOS_ERR, "Timeout waiting for controller during aux enable.\n");
			return CB_KBD_CONTROLLER_FAILURE;
		}
		outb(KBC_CMD_READ_COMMAND, KBD_COMMAND);
		if (!kbc_output_buffer_full()) {
			printk(BIOS_ERR, "Timeout waiting for controller during aux probe.\n");
			return CB_KBD_CONTROLLER_FAILURE;
		}

		byte = inb(KBD_DATA);
		if (!(byte & (0x1 << 5))) {
			printk(BIOS_DEBUG, "PS/2 auxiliary channel detected...\n");

			/* auxiliary interface test */
			outb(KBC_CMD_AUX_TEST, KBD_COMMAND);

			if (!kbc_output_buffer_full()) {
				printk(BIOS_ERR, "Auxiliary channel probe timed out.\n");
				goto aux_failure;
			}

			/* read test result, 0x00 should be returned in case of no failures */
			self_test = inb(KBD_DATA);

			if (self_test != 0x00) {
				printk(BIOS_ERR, "No device detected on auxiliary channel: 0x%x\n",
				self_test);
				goto aux_failure;
			}

			printk(BIOS_DEBUG, "PS/2 device detected on auxiliary channel\n");
			if (aux_probe_result)
				*aux_probe_result = 1;
		}
	}

aux_failure:

	return CB_SUCCESS;
}

static u8 send_keyboard(u8 command)
{
	u8 regval = 0;
	u8 resend = 10;

	do {
		if (!kbc_input_buffer_empty())
			return 0;
		outb(command, KBD_DATA);
		/* the reset command takes much longer then normal commands and
		 * even worse, some keyboards do send the ACK _after_ doing the
		 * reset */
		if (command == 0xFF) {
			u8 retries;

			for (retries = 9; retries && !kbc_output_buffer_full();
			     retries--)
				;
		}
		if (!kbc_output_buffer_full()) {
			printk(BIOS_ERR,
			       "Could not send keyboard command %02x\n",
			       command);
			return 0;
		}
		regval = inb(KBD_DATA);
		--resend;
	} while (regval == KBD_REPLY_RESEND && resend > 0);

	return regval;
}

uint8_t pc_keyboard_init(uint8_t probe_aux)
{
	u8 retries;
	u8 regval;
	enum cb_err err;
	uint8_t aux_dev_detected;

	if (!CONFIG(DRIVERS_PS2_KEYBOARD))
		return 0;

	if (acpi_is_wakeup_s3())
		return 0;

	printk(BIOS_DEBUG, "Keyboard init...\n");

	/* Run a keyboard controller self-test */
	err = kbc_self_test(probe_aux, &aux_dev_detected);
	/* Ignore interface failure as it's non-fatal.  */
	if (err != CB_SUCCESS && err != CB_KBD_INTERFACE_FAILURE)
		return 0;

	/* Enable keyboard interface - No IRQ */
	if (!kbc_input_buffer_empty())
		return 0;
	outb(0x60, KBD_COMMAND);
	if (!kbc_input_buffer_empty())
		return 0;
	outb(0x20, KBD_DATA);	/* send cmd: enable keyboard */
	if (!kbc_input_buffer_empty()) {
		printk(BIOS_INFO, "Timeout while enabling keyboard\n");
		return 0;
	}

	/* clean up any junk that might have been in the keyboard */
	if (!kbc_cleanup_buffers())
		return 0;

	/* reset keyboard and self test (keyboard side) */
	regval = send_keyboard(0xFF);
	if (regval == KBD_REPLY_RESEND) {
		/* keeps sending RESENDs, probably no keyboard. */
		printk(BIOS_INFO, "No PS/2 keyboard detected.\n");
		return 0;
	}

	if (regval != KBD_REPLY_ACK) {
		printk(BIOS_ERR, "Keyboard reset failed ACK: 0x%x\n", regval);
		return 0;
	}

	/* the reset command takes some time, so wait a little longer */
	for (retries = 9; retries && !kbc_output_buffer_full(); retries--)
		;

	if (!kbc_output_buffer_full()) {
		printk(BIOS_ERR, "Timeout waiting for keyboard after reset.\n");
		return 0;
	}

	regval = inb(KBD_DATA);
	if (regval != 0xAA) {
		printk(BIOS_ERR, "Keyboard reset selftest failed: 0x%x\n",
		       regval);
		return 0;
	}

	/*
	 * The following set scancode stuff is what normal BIOS do. It could be
	 * argued that coreboot shouldn't set the scan code.....
	 */

	/* disable the keyboard */
	regval = send_keyboard(0xF5);
	if (regval != KBD_REPLY_ACK) {
		printk(BIOS_ERR, "Keyboard disable failed ACK: 0x%x\n", regval);
		return 0;
	}

	/* Set scancode command */
	regval = send_keyboard(0xF0);
	if (regval != KBD_REPLY_ACK) {
		printk(BIOS_ERR, "Keyboard set scancode cmd failed ACK: 0x%x\n",
		       regval);
		return 0;
	}
	/* Set scancode mode 2 */
	regval = send_keyboard(0x02);
	if (regval != KBD_REPLY_ACK) {
		printk(BIOS_ERR,
		       "Keyboard set scancode mode failed ACK: 0x%x\n", regval);
		return 0;
	}

	/* All is well - enable keyboard interface */
	if (!kbc_input_buffer_empty())
		return 0;
	outb(0x60, KBD_COMMAND);
	if (!kbc_input_buffer_empty())
		return 0;
	outb(0x65, KBD_DATA);	/* send cmd: enable keyboard and IRQ 1 */
	if (!kbc_input_buffer_empty()) {
		printk(BIOS_ERR, "Timeout during keyboard enable\n");
		return 0;
	}

	/* enable the keyboard */
	regval = send_keyboard(0xF4);
	if (regval != KBD_REPLY_ACK) {
		printk(BIOS_ERR, "Keyboard enable failed ACK: 0x%x\n", regval);
		return 0;
	}

	printk(BIOS_DEBUG, "PS/2 keyboard initialized on primary channel\n");

	return aux_dev_detected;
}

/*
 * Support PS/2 mode -  oddball SIOs(KBC) need this setup
 * Not well documented. Google - 0xcb keyboard controller
 * This is called before pc_keyboard_init().
 */
void set_kbc_ps2_mode(void)
{
	enum cb_err err;

	/* Run a keyboard controller self-test */
	err = kbc_self_test(0, NULL);
	/* Ignore interface failure as it's non-fatal.  */
	if (err != CB_SUCCESS && err != CB_KBD_INTERFACE_FAILURE)
		return;

	/* Support PS/2 mode */
	if (!kbc_input_buffer_empty())
		return;
	outb(0xcb, KBD_COMMAND);

	if (!kbc_input_buffer_empty())
		return;
	outb(0x01, KBD_DATA);

	kbc_cleanup_buffers();
}

enum cb_err pc_keyboard_set_command_byte_bit(u8 bit, u8 value)
{
	if (!kbc_input_buffer_empty()) {
		printk(BIOS_ERR, "Timeout waiting to read command byte\n");
		return CB_KBD_INTERFACE_FAILURE;
	}
	outb(KBC_CMD_READ_COMMAND, KBD_COMMAND);

	if (!kbc_output_buffer_full()) {
		printk(BIOS_ERR, "Timeout waiting to read command byte\n");
		return CB_KBD_INTERFACE_FAILURE;
	}
	u8 byte = inb(KBD_DATA);

	if (!kbc_input_buffer_empty()) {
		printk(BIOS_ERR, "Timeout waiting to write command byte\n");
		return CB_KBD_INTERFACE_FAILURE;
	}
	outb(KBC_CMD_WRITE_COMMAND, KBD_COMMAND);

	byte = value ? (byte | BIT(bit)) : (byte & ~BIT(bit));

	if (!kbc_input_buffer_empty()) {
		printk(BIOS_ERR, "Timeout waiting to write command byte\n");
		return CB_KBD_INTERFACE_FAILURE;
	}
	outb(byte, KBD_DATA);

	return CB_SUCCESS;
}
