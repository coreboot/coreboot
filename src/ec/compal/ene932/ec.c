/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <arch/io.h>
#include <delay.h>
#include <pc80/keyboard.h>
#include "ec.h"
#include "chip.h"

/* kbc helper functions from drivers/pc80/keyboard.c. TODO: share functions. */
static int kbc_input_buffer_empty(void)
{
	u32 timeout;
	for (timeout = KBC_TIMEOUT_IN_MS; timeout && (inb(KBD_STATUS) & KBD_IBF);
	    timeout--) {
		mdelay(1);
	}

	if (!timeout) {
		printk(BIOS_WARNING,
		       "Unexpected Keyboard controller input buffer full\n");
	}
	return !!timeout;
}

static int kbc_output_buffer_full(void)
{
	u32 timeout;
	for (timeout = KBC_TIMEOUT_IN_MS; timeout && ((inb(KBD_STATUS)
	    & KBD_OBF) == 0); timeout--) {
		mdelay(1);
	}

	if (!timeout) {
		printk(BIOS_INFO, "Keyboard controller output buffer result timeout\n");
	}
	return !!timeout;
}

int kbc_cleanup_buffers(void)
{
	u32 timeout;
	for (timeout = KBC_TIMEOUT_IN_MS; timeout && (inb(KBD_STATUS)
	    & (KBD_OBF | KBD_IBF)); timeout--) {
		mdelay(1);
		inb(KBD_DATA);
	}

	if (!timeout) {
		printk(BIOS_ERR, "Couldn't cleanup the keyboard controller buffers\n");
		printk(BIOS_ERR, "Status (0x%x): 0x%x, Buffer (0x%x): 0x%x\n",
				KBD_STATUS, inb(KBD_STATUS), KBD_DATA, inb(KBD_DATA));
	}

	return !!timeout;
}

/* The ENE 60/64 EC registers are the same command/status IB/OB KBC pair.
 * Check status from 64 port before each command.
 *
 *  Ex. Get panel ID command C43/D77
 *  Check IBF empty. Then Write 0x43(CMD) to 0x64 Port
 *  Check IBF empty. Then Write 0x77(DATA) to 0x60 Port
 *  Check OBF set. Then Get Data(0x03:panel ID) from 0x60
 * Different commands return may or may not respond and may have multiple
 * bytes. Keep it simple for nor
 */

u8 ec_kbc_read_ob(void)
{
	if (!kbc_output_buffer_full()) return 0;
	return inb(KBD_DATA);
}

void ec_kbc_write_cmd(u8 cmd)
{
	if (!kbc_input_buffer_empty()) return;
	outb(cmd, KBD_COMMAND);
}

void ec_kbc_write_ib(u8 data)
{
	if (!kbc_input_buffer_empty()) return;
	outb(data, KBD_DATA);
}

/*
 * These functions are for accessing the ENE932 device space, but are not
 * currently used.
 */
/*
static u8 ec_io_read(u16 addr)
{
	outb(addr >> 8, EC_IO_HIGH);
	outb(addr & 0xff, EC_IO_LOW);
	return inb(EC_IO_DATA);
}
*/
/*static void ec_write(u16 addr, u8 data)
{
	outb(addr >> 8, EC_IO_HIGH);
	outb(addr & 0xff, EC_IO_LOW;
	outb(data, EC_IO_DATA);
}
*/

static void ene932_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	printk(BIOS_DEBUG, "Compal ENE932: Initializing keyboard.\n");
	pc_keyboard_init(NO_AUX_DEVICE);

}

static struct device_operations ops = {
	.init             = ene932_init,
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, 0, 0, 0, }
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations ec_compal_ene932_ops = {
	CHIP_NAME("COMPAL ENE932 EC")
	.enable_dev = enable_dev
};
