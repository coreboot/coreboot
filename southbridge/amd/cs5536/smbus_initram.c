/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <console.h>
#include <io.h>
#include "cs5536.h"

#define SMBUS_ERROR			-1
#define SMBUS_WAIT_UNTIL_READY_TIMEOUT	-2
#define SMBUS_WAIT_UNTIL_DONE_TIMEOUT	-3
#define SMBUS_TIMEOUT			1000

/**
 * Enable the SMBus.
 *
 * Basically, set the enable bit in the controller. This can be (and is)
 * called multiple times.
 */
static void smbus_enable(void)
{
	/* Set the Serial Clock Line (SCL) frequency and enable the SMBus
	 * controller. 0x20 is one possible frequency. For DRAM we use 0x7f,
	 * probably because it is a slower and presumably more reliable clock.
	 */
	/* outb((0x20 << 1) | SMB_CTRL2_ENABLE, smbus_io_base + SMB_CTRL2); */
	outb((0x7F << 1) | SMB_CTRL2_ENABLE, SMBUS_IO_BASE + SMB_CTRL2);
}

/**
 * Initialize the SMBus controller.
 *
 * Basically, set the enable bit in the controller and set the host
 * controller address. Code can call this more than once, but the effect of
 * doing so is uncertain due to SMBus address set.
 */
static void smbus_init(void)
{
	smbus_enable();

	/* Setup SMBus host controller address to 0xEF. This is not an
	 * I/O port address but an SMBus address.
	 */
	outb((0xEF | SMB_ADD_SAEN), SMBUS_IO_BASE + SMB_ADD);
}

/**
 * Fixed delay for the SMBus controller.
 *
 * Currently, this is a no-op. No delay. We are going to leave it here as it
 * indicates where a delay might be needed, for future chipset issues that
 * might happen. And if such issues might happen, they usually do.
 */
static void smbus_delay(void)
{
	/* udelay(1); */
}

/**
 * Wait for the SMBus controller to become ready.
 *
 * There are three ways this can happen. Either the controller becomes
 * ready (good); or we get an error (bad), in which case we return the
 * error; or, we time out and give up, in which case we return
 * SMBUS_WAIT_UNTIL_READY_TIMEOUT (very bad).
 *
 * @param smbus_io_base The SMBus I/O base.
 * @return The error code, or 0 on success.
 */
static int smbus_wait(u16 smbus_io_base)
{
	u8 val;
	unsigned long loops = SMBUS_TIMEOUT;

	do {
		smbus_delay();
		val = inb(smbus_io_base + SMB_STS);
		if ((val & SMB_STS_SDAST) != 0)
			break;
		if (val & (SMB_STS_BER | SMB_STS_NEGACK)) {
			printk(BIOS_DEBUG, "SMBus WAIT ERROR %x\n", val);
			return SMBUS_ERROR;
		}
	} while (--loops);

	return loops ? 0 : SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

/**
 * Generate an SMBus start condition.
 *
 * Kick off the SMBus. At this point we will own the bus and can issue an
 * address or other part of the transaction. This code can either exit
 * immediately due to a bus conflict, returning SMBUS_ERROR, or it will,
 * once the bus is started, return the error code (or success!) from
 * smbus_wait().
 *
 * @param smbus_io_base The SMBus I/O base.
 * @return The error code, or 0 on success.
 */
static int smbus_start_condition(u16 smbus_io_base)
{
	u8 val;

	/* Issue a start condition. */
	val = inb(smbus_io_base + SMB_CTRL1);
	outb(val | SMB_CTRL1_START, smbus_io_base + SMB_CTRL1);

	/* Check for bus conflict. */
	val = inb(smbus_io_base + SMB_STS);
	if ((val & SMB_STS_BER) != 0)
		return SMBUS_ERROR;

	return smbus_wait(smbus_io_base);
}

/**
 * Check the SMBus stop condition.
 *
 * Wait until the SDA status is set, indicating that data has been returned.
 *
 * @param smbus_io_base The SMBus I/O base.
 * @return The error code, or 0 on success.
 */
static int smbus_check_stop_condition(u16 smbus_io_base)
{
	u8 val;
	unsigned long loops = SMBUS_TIMEOUT;

	/* Check for SDA status. */
	do {
		smbus_delay();
		val = inb(smbus_io_base + SMB_CTRL1);
		if ((val & SMB_CTRL1_STOP) == 0) {
			break;
		}
		smbus_enable();
	} while (--loops);

	return loops ? 0 : SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

/**
 * Stop the SMBus and wait for the stop to be acknowledged by the
 * hardware, using smbus_wait() to wait.
 *
 * @param smbus_io_base The SMBus I/O base.
 * @return The error code, or 0 on success.
 */
static int smbus_stop_condition(u16 smbus_io_base)
{
	outb(SMB_CTRL1_STOP, smbus_io_base + SMB_CTRL1);
	return smbus_wait(smbus_io_base);
}

/**
 * Acknowledge the SMBus.
 *
 * Always succeeds. Always sends the ack.
 *
 * @param smbus_io_base The SMBus I/O base.
 */
static void smbus_ack(u16 smbus_io_base)
{
	u8 val = inb(smbus_io_base + SMB_CTRL1);
	outb(val | SMB_CTRL1_ACK, smbus_io_base + SMB_CTRL1);
}

/**
 * Set the slave address (e.g. 0x50 for DRAM socket 0) into the SMBus packet.
 * 
 * @param smbus_io_base The SMBus I/O base.
 * @param device The device.
 * @return The error code, or 0 on success.
 */
static int smbus_send_slave_address(u16 smbus_io_base, u8 device)
{
	u8 val;

	/* Send the slave address. */
	outb(device, smbus_io_base + SMB_SDA);

	/* Check for bus conflict and NACK. */
	val = inb(smbus_io_base + SMB_STS);
	if (((val & SMB_STS_BER) != 0) || ((val & SMB_STS_NEGACK) != 0)) {
		printk(BIOS_DEBUG, "SEND SLAVE ERROR (%x)\n", val);
		return SMBUS_ERROR;
	}

	return smbus_wait(smbus_io_base);
}

/**
 * Issue an SMBus command.
 *
 * This can fail if there is a bus conflict. The chipset will indicate
 * an error.
 *
 * @param smbus_io_base The SMBus I/O base.
 * @param command The command.
 * @return The error code, or 0 on success.
 */
static int smbus_send_command(u16 smbus_io_base, u8 command)
{
	u8 val;

	/* Send the command. */
	outb(command, smbus_io_base + SMB_SDA);

	/* Check for bus conflict and NACK. */
	val = inb(smbus_io_base + SMB_STS);
	if (((val & SMB_STS_BER) != 0) || ((val & SMB_STS_NEGACK) != 0))
		return SMBUS_ERROR;

	return smbus_wait(smbus_io_base);
}

/**
 * Get the data from the returned SMBus packet.
 *
 * @param smbus_io_base The SMBus I/O base.
 * @return The data from the SMBus packet area.
 */
static u8 smbus_get_result(u16 smbus_io_base)
{
	return inb(smbus_io_base + SMB_SDA);
}

/**
 * Read a byte from the SMBus.
 *
 * All the previous functions exist to support this one. Read a byte from
 * device 'device' at address 'address'.
 *
 * @param smbus_io_base The SMBus I/O base.
 * @param device The device.
 * @param address The address.
 * @return The data from the SMBus packet area.
 */
static u8 do_smbus_read_byte(u16 smbus_io_base, u8 device, u8 address)
{
	char *error = NULL;
	int errno;

	if (smbus_check_stop_condition(smbus_io_base)) {
		error = "smbus_check_stop_condition timed out";
		goto err;
	}

	if ((errno = smbus_start_condition(smbus_io_base))) {
		error = "smbus_start_condition";
		goto err;
	}

	if ((errno = smbus_send_slave_address(smbus_io_base, device))) {
		error = "smbus_send_slave_address";
		goto err;
	}

	smbus_ack(smbus_io_base);

	if ((errno = smbus_send_command(smbus_io_base, address))) {
		error = "smbus_send_command";
		goto err;
	}

	if ((errno = smbus_start_condition(smbus_io_base))) {
		error = "smbus_start_condition";
		goto err;
	}

	if ((errno = smbus_send_slave_address(smbus_io_base, device | 0x01))) {
		error = "smbus_send_slave_address";
		goto err;
	}

	if ((errno = smbus_stop_condition(smbus_io_base))) {
		error = "second smbus_stop_condition";
		goto err;
	}

	return smbus_get_result(smbus_io_base);

err:
	printk(BIOS_ERR, "SMBus READ ERROR: %s; device %02x", error, device);
	/* Stop, clean up the error, and leave. */
	smbus_stop_condition(smbus_io_base);
	outb(inb(smbus_io_base + SMB_STS), smbus_io_base + SMB_STS);
	outb(0x0, smbus_io_base + SMB_STS);

	return 0xFF;
}

/**
 * Read a byte from the SMBus.
 *
 * All other functions in this file are static and support this one function.
 * Since we are using gcc now, we use a static variable to tell us whether
 * to enable the SMBus. Thus the northbridge code need only worry about
 * one thing -- calling this function to read DRAM parameters.
 *
 * @param device The device.
 * @param address The address.
 * @return The data from the SMBus packet area or an error of 0xff (i.e. -1).
 */
int smbus_read_byte(u16 device, u8 address)
{
	static int first_time = 1;

	if (first_time) {
		smbus_init();
		first_time = 0;
	}

	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}
