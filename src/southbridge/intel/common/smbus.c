/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/smbus_def.h>
#include <device/smbus_host.h>
#include <types.h>

#if CONFIG(DEBUG_SMBUS)
#define dprintk(args...) printk(BIOS_DEBUG, ##args)
#else
#define dprintk(args...) do {} while (0)
#endif

/* SMBus register offsets. */
#define SMBHSTSTAT		0x0
#define SMBHSTCTL		0x2
#define SMBHSTCMD		0x3
#define SMBXMITADD		0x4
#define SMBHSTDAT0		0x5
#define SMBHSTDAT1		0x6
#define SMBBLKDAT		0x7
#define SMBTRNSADD		0x9
#define SMBSLVDATA		0xa
#define SMLINK_PIN_CTL		0xe
#define SMBUS_PIN_CTL		0xf
#define SMBSLVCMD		0x11

#define SMB_RCV_SLVA		SMBTRNSADD

/* I801 command constants */
#define I801_QUICK		(0 << 2)
#define I801_BYTE		(1 << 2)
#define I801_BYTE_DATA		(2 << 2)
#define I801_WORD_DATA		(3 << 2)
#define I801_PROCESS_CALL	(4 << 2)
#define I801_BLOCK_DATA		(5 << 2)
#define I801_I2C_BLOCK_DATA	(6 << 2) /* ICH5 and later */

/* I801 Host Control register bits */
#define SMBHSTCNT_INTREN	(1 << 0)
#define SMBHSTCNT_KILL		(1 << 1)
#define SMBHSTCNT_LAST_BYTE	(1 << 5)
#define SMBHSTCNT_START		(1 << 6)
#define SMBHSTCNT_PEC_EN	(1 << 7) /* ICH3 and later */

/* I801 Hosts Status register bits */
#define SMBHSTSTS_BYTE_DONE	(1 << 7)
#define SMBHSTSTS_INUSE_STS	(1 << 6)
#define SMBHSTSTS_SMBALERT_STS	(1 << 5)
#define SMBHSTSTS_FAILED	(1 << 4)
#define SMBHSTSTS_BUS_ERR	(1 << 3)
#define SMBHSTSTS_DEV_ERR	(1 << 2)
#define SMBHSTSTS_INTR		(1 << 1)
#define SMBHSTSTS_HOST_BUSY	(1 << 0)

/* For SMBXMITADD register. */
#define XMIT_WRITE(dev)		(((dev) << 1) | 0)
#define XMIT_READ(dev)		(((dev) << 1) | 1)

#define SMBUS_TIMEOUT		(10 * 1000 * 100)
#define SMBUS_BLOCK_MAXLEN	32

/* block_cmd_loop flags */
#define BLOCK_READ	0
#define BLOCK_WRITE	(1 << 0)
#define BLOCK_I2C	(1 << 1)

static void smbus_delay(void)
{
	inb(0x80);
}

static void host_outb(uintptr_t base, u8 reg, u8 value)
{
	outb(value, base + reg);
}

static u8 host_inb(uintptr_t base, u8 reg)
{
	return inb(base + reg);
}

static void host_and_or(uintptr_t base, u8 reg, u8 mask, u8 or)
{
	u8 value;
	value = host_inb(base, reg);
	value &= mask;
	value |= or;
	host_outb(base, reg, value);
}

void smbus_host_reset(uintptr_t base)
{
	/* Disable interrupt generation. */
	host_outb(base, SMBHSTCTL, 0);

	/* Clear any lingering errors, so transactions can run. */
	host_and_or(base, SMBHSTSTAT, 0xff, 0);
}

void smbus_set_slave_addr(uintptr_t base, u8 slave_address)
{
	host_outb(base, SMB_RCV_SLVA, slave_address);
}

static int host_completed(u8 status)
{
	if (status & SMBHSTSTS_HOST_BUSY)
		return 0;

	/* These status bits do not imply completion of transaction. */
	status &= ~(SMBHSTSTS_BYTE_DONE | SMBHSTSTS_INUSE_STS |
		    SMBHSTSTS_SMBALERT_STS);
	return status != 0;
}

static int recover_master(uintptr_t base, int ret)
{
	/* TODO: Depending of the failure, drive KILL transaction
	 * or force soft reset on SMBus master controller.
	 */
	printk(BIOS_ERR, "SMBus: Fatal master timeout (%d)\n", ret);
	return ret;
}

static int cb_err_from_stat(u8 status)
{
	/* These status bits do not imply errors. */
	status &= ~(SMBHSTSTS_BYTE_DONE | SMBHSTSTS_INUSE_STS |
		    SMBHSTSTS_SMBALERT_STS);

	if (status == SMBHSTSTS_INTR)
		return 0;

	return SMBUS_ERROR;
}

static int setup_command(uintptr_t base, u8 ctrl, u8 xmitadd)
{
	unsigned int loops = SMBUS_TIMEOUT;
	u8 host_busy;

	do {
		smbus_delay();
		host_busy = host_inb(base, SMBHSTSTAT) & SMBHSTSTS_HOST_BUSY;
	} while (--loops && host_busy);

	if (loops == 0)
		return recover_master(base, SMBUS_WAIT_UNTIL_READY_TIMEOUT);

	/* Clear any lingering errors, so the transaction will run. */
	host_and_or(base, SMBHSTSTAT, 0xff, 0);

	/* Set up transaction */
	/* Disable interrupts */
	host_outb(base, SMBHSTCTL, ctrl);

	/* Set the device I'm talking to. */
	host_outb(base, SMBXMITADD, xmitadd);

	return 0;
}

static int execute_command(uintptr_t base)
{
	unsigned int loops = SMBUS_TIMEOUT;
	u8 status;

	/* Start the command. */
	host_and_or(base, SMBHSTCTL, 0xff, SMBHSTCNT_START);

	/* Poll for it to start. */
	do {
		smbus_delay();

		/* If we poll too slow, we could miss HOST_BUSY flag
		 * set and detect INTR or x_ERR flags instead here.
		 */
		status = host_inb(base, SMBHSTSTAT);
		status &= ~(SMBHSTSTS_SMBALERT_STS | SMBHSTSTS_INUSE_STS);
	} while (--loops && status == 0);

	if (loops == 0)
		return recover_master(base,
				      SMBUS_WAIT_UNTIL_ACTIVE_TIMEOUT);

	return 0;
}

static int complete_command(uintptr_t base)
{
	unsigned int loops = SMBUS_TIMEOUT;
	u8 status;

	do {
		smbus_delay();
		status = host_inb(base, SMBHSTSTAT);
	} while (--loops && !host_completed(status));

	if (loops == 0)
		return recover_master(base,
				      SMBUS_WAIT_UNTIL_DONE_TIMEOUT);

	return cb_err_from_stat(status);
}

static int smbus_read_cmd(uintptr_t base, u8 ctrl, u8 device, u8 address)
{
	int ret;
	u16 word;

	/* Set up for a byte data read. */
	ret = setup_command(base, ctrl, XMIT_READ(device));
	if (ret < 0)
		return ret;

	/* Set the command/address... */
	host_outb(base, SMBHSTCMD, address);

	/* Clear the data bytes... */
	host_outb(base, SMBHSTDAT0, 0);
	host_outb(base, SMBHSTDAT1, 0);

	/* Start the command */
	ret = execute_command(base);
	if (ret < 0)
		return ret;

	/* Poll for transaction completion */
	ret = complete_command(base);
	if (ret < 0)
		return ret;

	/* Read results of transaction */
	word = host_inb(base, SMBHSTDAT0);
	if (ctrl == I801_WORD_DATA)
		word |= host_inb(base, SMBHSTDAT1) << 8;

	return word;
}

static int smbus_write_cmd(uintptr_t base, u8 ctrl, u8 device, u8 address, u16 data)
{
	int ret;

	/* Set up for a byte data write. */
	ret = setup_command(base, ctrl, XMIT_WRITE(device));
	if (ret < 0)
		return ret;

	/* Set the command/address... */
	host_outb(base, SMBHSTCMD, address);

	/* Set the data bytes... */
	host_outb(base, SMBHSTDAT0, data & 0xff);
	if (ctrl == I801_WORD_DATA)
		host_outb(base, SMBHSTDAT1, data >> 8);

	/* Start the command */
	ret = execute_command(base);
	if (ret < 0)
		return ret;

	/* Poll for transaction completion */
	return complete_command(base);
}

static int block_cmd_loop(uintptr_t base, u8 *buf, size_t max_bytes, int flags)
{
	u8 status;
	unsigned int loops = SMBUS_TIMEOUT;
	int ret;
	size_t bytes = 0;
	int is_write_cmd = flags & BLOCK_WRITE;
	int sw_drives_nak = flags & BLOCK_I2C;

	/* Hardware limitations. */
	if (flags == (BLOCK_WRITE | BLOCK_I2C))
		return SMBUS_ERROR;

	/* Set number of bytes to transfer. */
	/* Reset number of bytes to transfer so we notice later it
	 * was really updated with the transaction. */
	if (!sw_drives_nak) {
		if (is_write_cmd)
			host_outb(base, SMBHSTDAT0, max_bytes);
		else
			host_outb(base, SMBHSTDAT0, 0);
	}

	/* Send first byte from buffer, bytes_sent increments after
	 * hardware acknowledges it.
	 */
	if (is_write_cmd)
		host_outb(base, SMBBLKDAT, *buf++);

	/* Start the command */
	ret = execute_command(base);
	if (ret < 0)
		return ret;

	/* Poll for transaction completion */
	do {
		status = host_inb(base, SMBHSTSTAT);

		if (status & SMBHSTSTS_BYTE_DONE) { /* Byte done */

			if (is_write_cmd) {
				bytes++;
				if (bytes < max_bytes)
					host_outb(base, SMBBLKDAT, *buf++);
			} else {
				if (bytes < max_bytes)
					*buf++ = host_inb(base, SMBBLKDAT);
				bytes++;

				/* Indicate that next byte is the last one. */
				if (sw_drives_nak && (bytes + 1 >= max_bytes)) {
					host_and_or(base, SMBHSTCTL, 0xff,
						    SMBHSTCNT_LAST_BYTE);
				}

			}

			/* Engine internally completes the transaction
			 * and clears HOST_BUSY flag once the byte count
			 * has been reached or LAST_BYTE was set.
			 */
			host_outb(base, SMBHSTSTAT, SMBHSTSTS_BYTE_DONE);
		}

	} while (--loops && !host_completed(status));

	dprintk("%s: status = %02x, len = %zd / %zd, loops = %d\n",
		__func__, status, bytes, max_bytes, SMBUS_TIMEOUT - loops);

	if (loops == 0)
		return recover_master(base, SMBUS_WAIT_UNTIL_DONE_TIMEOUT);

	ret = cb_err_from_stat(status);
	if (ret < 0)
		return ret;

	return bytes;
}

int do_smbus_read_byte(uintptr_t base, u8 device, u8 address)
{
	return smbus_read_cmd(base, I801_BYTE_DATA, device, address);
}

int do_smbus_read_word(uintptr_t base, u8 device, u8 address)
{
	return smbus_read_cmd(base, I801_WORD_DATA, device, address);
}

int do_smbus_write_byte(uintptr_t base, u8 device, u8 address, u8 data)
{
	return smbus_write_cmd(base, I801_BYTE_DATA, device, address, data);
}

int do_smbus_write_word(uintptr_t base, u8 device, u8 address, u16 data)
{
	return smbus_write_cmd(base, I801_WORD_DATA, device, address, data);
}

int do_smbus_block_read(uintptr_t base, u8 device, u8 cmd, size_t max_bytes, u8 *buf)
{
	int ret, slave_bytes;

	max_bytes = MIN(SMBUS_BLOCK_MAXLEN, max_bytes);

	/* Set up for a block data read. */
	ret = setup_command(base, I801_BLOCK_DATA, XMIT_READ(device));
	if (ret < 0)
		return ret;

	/* Set the command/address... */
	host_outb(base, SMBHSTCMD, cmd);

	/* Execute block transaction. */
	ret = block_cmd_loop(base, buf, max_bytes, BLOCK_READ);
	if (ret < 0)
		return ret;

	/* Post-check we received complete message. */
	slave_bytes = host_inb(base, SMBHSTDAT0);
	if (ret < slave_bytes)
		return SMBUS_ERROR;

	return ret;
}

/*
 * The caller is responsible of settings HOSTC I2C_EN bit prior to making this
 * call!
 */
int do_smbus_process_call(uintptr_t base, u8 device, u8 cmd, u16 data, u16 *buf)
{
	int ret;

	/* Set up for process call */
	ret = setup_command(base, I801_PROCESS_CALL, XMIT_WRITE(device));
	if (ret < 0)
		return ret;

	/* cmd will only be send if I2C_EN is zero */
	host_outb(base, SMBHSTCMD, cmd);

	host_outb(base, SMBHSTDAT0, data & 0x00ff);
	host_outb(base, SMBHSTDAT1, (data & 0xff00) >> 8);

	/* Start the command */
	ret = execute_command(base);
	if (ret < 0)
		return ret;

	/* Poll for transaction completion */
	ret = complete_command(base);
	if (ret < 0)
		return ret;

	/* Read results of transaction */
	*buf = host_inb(base, SMBHSTDAT0);
	*buf |= (host_inb(base, SMBHSTDAT1) << 8);

	return ret;
}

int do_smbus_block_write(uintptr_t base, u8 device, u8 cmd, const size_t bytes, const u8 *buf)
{
	int ret;

	if (bytes > SMBUS_BLOCK_MAXLEN)
		return SMBUS_ERROR;

	/* Set up for a block data write. */
	ret = setup_command(base, I801_BLOCK_DATA, XMIT_WRITE(device));
	if (ret < 0)
		return ret;

	/* Set the command/address... */
	host_outb(base, SMBHSTCMD, cmd);

	/* Execute block transaction. */
	ret = block_cmd_loop(base, (u8 *)buf, bytes, BLOCK_WRITE);
	if (ret < 0)
		return ret;

	if (ret < bytes)
		return SMBUS_ERROR;

	return ret;
}

/* Only since ICH5 */
static int has_i2c_read_command(void)
{
	if (CONFIG(SOUTHBRIDGE_INTEL_I82371EB) ||
	    CONFIG(SOUTHBRIDGE_INTEL_I82801DX))
		return 0;
	return 1;
}

int do_i2c_eeprom_read(uintptr_t base, u8 device, u8 offset, const size_t bytes, u8 *buf)
{
	int ret;

	if (!has_i2c_read_command())
		return SMBUS_ERROR;

	/* Set up for a i2c block data read.
	 *
	 * FIXME: Address parameter changes to XMIT_READ(device) with
	 * some revision of PCH. Presumably hardware revisions that
	 * do not have i2c block write support internally set LSB.
	 */
	ret = setup_command(base, I801_I2C_BLOCK_DATA,
			    XMIT_WRITE(device));
	if (ret < 0)
		return ret;

	/* device offset */
	host_outb(base, SMBHSTDAT1, offset);

	/* Execute block transaction. */
	ret = block_cmd_loop(base, buf, bytes, BLOCK_READ | BLOCK_I2C);
	if (ret < 0)
		return ret;

	/* Post-check we received complete message. */
	if (ret < bytes)
		return SMBUS_ERROR;

	return ret;
}

/*
 * The caller is responsible of settings HOSTC I2C_EN bit prior to making this
 * call!
 */
int do_i2c_block_write(uintptr_t base, u8 device, size_t bytes, u8 *buf)
{
	u8 cmd;
	int ret;

	if (!CONFIG(SOC_INTEL_BRASWELL))
		return SMBUS_ERROR;

	if (!bytes || (bytes > SMBUS_BLOCK_MAXLEN))
		return SMBUS_ERROR;

	/* Set up for a block data write. */
	ret = setup_command(base, I801_BLOCK_DATA, XMIT_WRITE(device));
	if (ret < 0)
		return ret;

	/*
	 * In i2c mode SMBus controller sequence on bus will be:
	 * <SMBXINTADD> <SMBHSTDAT1> <SMBBLKDAT> .. <SMBBLKDAT>
	 * The SMBHSTCMD must be written also to ensure the SMBUs controller
	 * will generate the i2c sequence.
	*/
	cmd = *buf++;
	bytes--;
	host_outb(base, SMBHSTCMD, cmd);
	host_outb(base, SMBHSTDAT1, cmd);

	/* Execute block transaction. */
	ret = block_cmd_loop(base, buf, bytes, BLOCK_WRITE);
	if (ret < 0)
		return ret;

	if (ret < bytes)
		return SMBUS_ERROR;

	ret++; /* 1st byte has been written using SMBHSTDAT1 */
	return ret;
}
