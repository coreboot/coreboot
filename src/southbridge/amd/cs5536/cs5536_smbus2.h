/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#define SMBUS_ERROR -1
#define SMBUS_WAIT_UNTIL_READY_TIMEOUT -2
#define SMBUS_WAIT_UNTIL_DONE_TIMEOUT  -3

#define	SMB_SDA		0x00
#define SMB_STS		0x01
#define SMB_CTRL_STS	0x02
#define	SMB_CTRL1	0x03
#define SMB_ADD		0x04
#define SMB_CTRL2	0x05
#define	SMB_CTRL3	0x06

#define SMB_STS_SLVSTP	(0x01 << 7)
#define SMB_STS_SDAST	(0x01 << 6)
#define	SMB_STS_BER	(0x01 << 5)
#define SMB_STS_NEGACK	(0x01 << 4)
#define	SMB_STS_STASTR	(0x01 << 3)
#define SMB_STS_NMATCH	(0x01 << 2)
#define	SMB_STS_MASTER	(0x01 << 1)
#define SMB_STS_XMIT	(0x01 << 0)

#define	SMB_CSTS_TGSCL	(0x01 << 5)
#define SMB_CSTS_TSDA	(0x01 << 4)
#define	SMB_CSTS_GCMTCH	(0x01 << 3)
#define SMB_CSTS_MATCH	(0x01 << 2)
#define	SMB_CSTS_BB	(0x01 << 1)
#define SMB_CSTS_BUSY	(0x01 << 0)

#define	SMB_CTRL1_STASTRE (0x01 << 7)
#define SMB_CTRL1_NMINTE  (0x01 << 6)
#define	SMB_CTRL1_GCMEN   (0x01 << 5)
#define SMB_CTRL1_ACK     (0x01 << 4)
#define	SMB_CTRL1_RSVD    (0x01 << 3)
#define SMB_CTRL1_INTEN   (0x01 << 2)
#define	SMB_CTRL1_STOP    (0x01 << 1)
#define SMB_CTRL1_START   (0x01 << 0)

#define	SMB_ADD_SAEN	  (0x01 << 7)

#define	SMB_CTRL2_ENABLE  0x01

#define SMBUS_TIMEOUT (100*1000*10)
#define SMBUS_STATUS_MASK 0xfbff

static void smbus_delay(void)
{
	inb(0x80);
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
			printk(BIOS_DEBUG, "SMBUS WAIT ERROR %x\n", val);
			return SMBUS_ERROR;
		}
	} while (--loops);

	outb(0, smbus_io_base + SMB_STS);
	return loops ? 0 : SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

static int smbus_write(unsigned smbus_io_base, unsigned char byte)
{

	outb(byte, smbus_io_base + SMB_SDA);
	return smbus_wait(smbus_io_base);
}

/* generate a smbus start condition */
static int smbus_start_condition(unsigned smbus_io_base)
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

static int smbus_check_stop_condition(unsigned smbus_io_base)
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
	} while (--loops);
	return loops ? 0 : SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Make sure everything is cleared and ready to go */

	val = inb(smbus_io_base + SMB_CTRL1);
	outb(val & ~(SMB_CTRL1_STASTRE | SMB_CTRL1_NMINTE),
	     smbus_io_base + SMB_CTRL1);

	outb(SMB_STS_BER | SMB_STS_NEGACK | SMB_STS_STASTR,
	     smbus_io_base + SMB_STS);

	val = inb(smbus_io_base + SMB_CTRL_STS);
	outb(val | SMB_CSTS_BB, smbus_io_base + SMB_CTRL_STS);
}

static int smbus_stop_condition(unsigned smbus_io_base)
{
	unsigned char val;
	val = inb(smbus_io_base + SMB_CTRL1);
	outb(SMB_CTRL1_STOP, smbus_io_base + SMB_CTRL1);

	return 0;
}

static int smbus_ack(unsigned smbus_io_base, int state)
{
	unsigned char val = inb(smbus_io_base + SMB_CTRL1);

	if (state)
		outb(val | SMB_CTRL1_ACK, smbus_io_base + SMB_CTRL1);
	else
		outb(val & ~SMB_CTRL1_ACK, smbus_io_base + SMB_CTRL1);

	return 0;
}

static int smbus_send_slave_address(unsigned smbus_io_base,
				    unsigned char device)
{
	unsigned char val;

	/* send the slave address */
	outb(device, smbus_io_base + SMB_SDA);

	/* check for bus conflict and NACK */
	val = inb(smbus_io_base + SMB_STS);
	if (((val & SMB_STS_BER) != 0) || ((val & SMB_STS_NEGACK) != 0)) {
		printk(BIOS_DEBUG, "SEND SLAVE ERROR (%x)\n", val);
		return SMBUS_ERROR;
	}
	return smbus_wait(smbus_io_base);
}

static int smbus_send_command(unsigned smbus_io_base, unsigned char command)
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

static void _doread(unsigned smbus_io_base, unsigned char device,
		    unsigned char address, unsigned char *data, int count)
{
	int ret;
	int index = 0;
	unsigned char val;

	if ((ret = smbus_check_stop_condition(smbus_io_base)))
		goto err;

	index++;

	if ((ret = smbus_start_condition(smbus_io_base)))
		goto err;

	index++;		/* 2 */
	if ((ret = smbus_send_slave_address(smbus_io_base, device)))
		goto err;

	index++;
	if ((ret = smbus_send_command(smbus_io_base, address)))
		goto err;

	index++;
	if ((ret = smbus_start_condition(smbus_io_base)))
		goto err;

	/* Clear the ack for multiple byte reads */
	smbus_ack(smbus_io_base, (count == 1) ? 1 : 0);

	index++;
	if ((ret = smbus_send_slave_address(smbus_io_base, device | 0x01)))
		goto err;

	while (count) {
		/* Set the ACK if this is the next to last byte */
		smbus_ack(smbus_io_base, (count == 2) ? 1 : 0);

		/* Set the stop bit if this is the last byte to read */

		if (count == 1)
			smbus_stop_condition(smbus_io_base);

		val = inb(smbus_io_base + SMB_SDA);
		*data++ = val;

		if (count > 1) {
			ret = smbus_wait(smbus_io_base);
			if (ret)
				return;
		}

		count--;
	}

	return;

      err:
	printk(BIOS_DEBUG, "SMBUS READ ERROR (%d): %d\n", index, ret);
}

static inline unsigned char do_smbus_read_byte(unsigned smbus_io_base,
		unsigned char device, unsigned char address)
{
	unsigned char val = 0;
	_doread(smbus_io_base, device, address, &val, sizeof(val));
	return val;
}

static inline unsigned short do_smbus_read_word(unsigned smbus_io_base,
		 unsigned char device, unsigned char address)
{
	unsigned short val = 0;
	_doread(smbus_io_base, device, address, (unsigned char *)&val,
		sizeof(val));
	return val;
}

static int _dowrite(unsigned smbus_io_base, unsigned char device,
		    unsigned char address, unsigned char *data, int count)
{

	int ret;

	if ((ret = smbus_check_stop_condition(smbus_io_base)))
		goto err;

	if ((ret = smbus_start_condition(smbus_io_base)))
		goto err;

	if ((ret = smbus_send_slave_address(smbus_io_base, device)))
		goto err;

	if ((ret = smbus_send_command(smbus_io_base, address)))
		goto err;

	while (count) {
		if ((ret = smbus_write(smbus_io_base, *data++)))
			goto err;
		count--;
	}

	smbus_stop_condition(smbus_io_base);
	return 0;

      err:
	printk(BIOS_DEBUG, "SMBUS WRITE ERROR: %d\n", ret);
	return -1;
}

static inline int do_smbus_write_byte(unsigned smbus_io_base,
		unsigned char device, unsigned char address, unsigned char data)
{
	return _dowrite(smbus_io_base, device, address,
			(unsigned char *)&data, 1);
}

static inline int do_smbus_write_word(unsigned smbus_io_base,
		unsigned char device, unsigned char address, unsigned short data)
{
	return _dowrite(smbus_io_base, device, address, (unsigned char *)&data,
			2);
}
