/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>

#include "ec.h"
#include "chip.h"

/* helper functions from drivers/pc80/keyboard.c */
static int input_buffer_empty(u16 status_reg)
{
	u32 timeout;
	for (timeout = KBC_TIMEOUT_IN_MS; timeout && (inb(status_reg) & KBD_IBF);
	    timeout--) {
		udelay(1000);
	}

	if (!timeout) {
		printk(BIOS_WARNING, "EC-IT8518 Unexpected input buffer full\n");
		printk(BIOS_WARNING, "  Status (0x%x): 0x%x\n", status_reg, inb(status_reg));
	}
	return !!timeout;
}

static int output_buffer_full(u16 status_reg)
{
	u32 timeout;
	for (timeout = KBC_TIMEOUT_IN_MS; timeout && ((inb(status_reg)
	    & KBD_OBF) == 0); timeout--) {
		udelay(1000);
	}

	if (!timeout) {
		printk(BIOS_INFO, "EC-IT8518 output buffer result timeout\n");
		printk(BIOS_INFO, "  Status (0x%x): 0x%x\n", status_reg, inb(status_reg));
	}
	return !!timeout;
}

/* The IT8518 60/64 EC registers are the same command/status IB/OB KBC pair.
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
	if (!output_buffer_full(KBD_STATUS)) return 0;
	return inb(KBD_DATA);
}

void ec_kbc_write_cmd(u8 cmd)
{
	if (!input_buffer_empty(KBD_STATUS)) return;
	outb(cmd, KBD_COMMAND);
}

void ec_kbc_write_ib(u8 data)
{
	if (!input_buffer_empty(KBD_STATUS)) return;
	outb(data, KBD_DATA);
}

/*
 * These functions are for accessing the IT8518 device RAM space via 0x66/0x68
 */

u8 ec_read_ob(void)
{
	if (!output_buffer_full(EC_SC)) return 0;
	return inb(EC_DATA);
}

void ec_write_cmd(u8 cmd)
{
	if (!input_buffer_empty(EC_SC)) return;
	outb(cmd, EC_SC);
}

void ec_write_ib(u8 data)
{
	if (!input_buffer_empty(EC_SC)) return;
	outb(data, EC_DATA);
}

u8 ec_read(u16 addr)
{
	ec_write_cmd(RD_EC);
	ec_write_ib(addr);
	return ec_read_ob();
}

void ec_write(u16 addr, u8 data)
{
	ec_write_cmd(WR_EC);
	ec_write_ib(addr);
	ec_write_ib(data);
}

u8 ec_it8518_get_event(void)
{
	u8 cmd = 0;
	u8 status = inb(EC_SC);
	if (status & SCI_EVT) {
		ec_write_cmd(QR_EC);
		cmd = ec_read_ob();
	} else if (status & SMI_EVT) {
		ec_kbc_write_cmd(EC_KBD_SMI_EVENT);
		cmd = ec_kbc_read_ob();
	}
	return cmd;
}

void ec_it8518_enable_wake_events(void)
{
	/*
	 * Set the bit in ECRAM that will enable the Lid switch as a wake source
	 */
	u8 reg8 = ec_read(EC_WAKE_SRC_ENABLE);
	ec_write(EC_WAKE_SRC_ENABLE, reg8 | EC_LID_WAKE_ENABLE);
}

static void it8518_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	printk(BIOS_DEBUG, "Quanta IT8518: Initializing keyboard.\n");
	pc_keyboard_init(NO_AUX_DEVICE);
}

static struct device_operations ops = {
	.init             = it8518_init,
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

struct chip_operations ec_quanta_it8518_ops = {
	CHIP_NAME("QUANTA IT8518 EC")
	.enable_dev = enable_dev
};
