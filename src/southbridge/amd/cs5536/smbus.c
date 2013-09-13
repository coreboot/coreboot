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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include "cs5536.h"
#include "smbus.h"

#define SMBUS_TIMEOUT (1000)

static void smbus_delay(void)
{
	/* inb(0x80); */
}

static int smbus_wait(unsigned smbus_io_base)
{
	unsigned long loops = SMBUS_TIMEOUT;
	unsigned char val;

	do {
		smbus_delay();
		val = inb(smbus_io_base + SMB_STS);
		if ((val & SMB_STS_SDAST) != 0)
			break;
		if (val & (SMB_STS_BER | SMB_STS_NEGACK)) {
			/*printk(BIOS_DEBUG, "SMBUS WAIT ERROR %x\n", val); */
			return SMBUS_ERROR;
		}
	} while (--loops);
	return loops ? 0 : SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

/* generate a smbus start condition */
int smbus_start_condition(unsigned smbus_io_base)
{
	unsigned char val;

	/* issue a START condition */
	val = inb(smbus_io_base + SMB_CTRL1);
	outb(val | SMB_CTRL1_START, smbus_io_base + SMB_CTRL1);

	/* check for bus conflict */
	val = inb(smbus_io_base + SMB_STS);
	if ((val & SMB_STS_BER) != 0)
		return SMBUS_ERROR;

	return smbus_wait(smbus_io_base);
}

int smbus_check_stop_condition(unsigned smbus_io_base)
{
	unsigned char val;
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	/* check for SDA status */
	do {
		smbus_delay();
		val = inb(smbus_io_base + SMB_CTRL1);
		if ((val & SMB_CTRL1_STOP) == 0) {
			break;
		}
		outb((0x7F << 1) | SMB_CTRL2_ENABLE, smbus_io_base + SMB_CTRL2);
	} while (--loops);
	return loops ? 0 : SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

int smbus_stop_condition(unsigned smbus_io_base)
{
	outb(SMB_CTRL1_STOP, smbus_io_base + SMB_CTRL1);
	return smbus_wait(smbus_io_base);
}

static int smbus_ack(unsigned smbus_io_base, int state)
{
	unsigned char val = inb(smbus_io_base + SMB_CTRL1);

/*	if (state) */
	outb(val | SMB_CTRL1_ACK, smbus_io_base + SMB_CTRL1);
/*	else
		outb(val & ~SMB_CTRL1_ACK, smbus_io_base + SMB_CTRL1);
*/
	return 0;
}

int smbus_send_slave_address(unsigned smbus_io_base,
				    unsigned char device)
{
	unsigned char val;

	/* send the slave address */
	outb(device, smbus_io_base + SMB_SDA);

	/* check for bus conflict and NACK */
	val = inb(smbus_io_base + SMB_STS);
	if (((val & SMB_STS_BER) != 0) || ((val & SMB_STS_NEGACK) != 0)) {
		/* printk(BIOS_DEBUG, "SEND SLAVE ERROR (%x)\n", val); */
		return SMBUS_ERROR;
	}
	return smbus_wait(smbus_io_base);
}

int smbus_send_command(unsigned smbus_io_base, unsigned char command)
{
	unsigned char val;

	/* send the command */
	outb(command, smbus_io_base + SMB_SDA);

	/* check for bus conflict and NACK */
	val = inb(smbus_io_base + SMB_STS);
	if (((val & SMB_STS_BER) != 0) || ((val & SMB_STS_NEGACK) != 0))
		return SMBUS_ERROR;

	return smbus_wait(smbus_io_base);
}

static unsigned char smbus_get_result(unsigned smbus_io_base)
{
	return inb(smbus_io_base + SMB_SDA);
}

unsigned char do_smbus_read_byte(unsigned smbus_io_base,
					unsigned char device,
					unsigned char address)
{
	unsigned char error = 0;

	if ((smbus_check_stop_condition(smbus_io_base))) {
		error = 1;
		goto err;
	}

	if ((smbus_start_condition(smbus_io_base))) {
		error = 2;
		goto err;
	}

	if ((smbus_send_slave_address(smbus_io_base, device << 1))) {
		error = 3;
		goto err;
	}

	smbus_ack(smbus_io_base, 1);

	if ((smbus_send_command(smbus_io_base, address))) {
		error = 4;
		goto err;
	}

	if ((smbus_start_condition(smbus_io_base))) {
		error = 5;
		goto err;
	}

	if ((smbus_send_slave_address(smbus_io_base, (device << 1) | 0x01))) {
		error = 6;
		goto err;
	}

	if ((smbus_stop_condition(smbus_io_base))) {
		error = 7;
		goto err;
	}

	return smbus_get_result(smbus_io_base);

err:
	print_debug("SMBUS READ ERROR:");
	print_debug_hex8(error);
	print_debug(" device:");
	print_debug_hex8(device);
	print_debug("\n");
	/* stop, clean up the error, and leave */
	smbus_stop_condition(smbus_io_base);
	outb(inb(smbus_io_base + SMB_STS), smbus_io_base + SMB_STS);
	outb(0x0, smbus_io_base + SMB_STS);
	return 0xFF;
}

