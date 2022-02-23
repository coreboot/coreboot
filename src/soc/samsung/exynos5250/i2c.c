/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <soc/clk.h>
#include <soc/i2c.h>
#include <soc/periph.h>
#include <timer.h>

#define I2C_TIMEOUT_US (1000 * USECS_PER_MSEC)

struct __packed i2c_regs
{
	uint8_t con;
	uint8_t _1[3];
	uint8_t stat;
	uint8_t _2[3];
	uint8_t add;
	uint8_t _3[3];
	uint8_t ds;
	uint8_t _4[3];
	uint8_t lc;
	uint8_t _5[3];
};

struct s3c24x0_i2c_bus {
	int bus_num;
	struct i2c_regs *regs;
	enum periph_id periph_id;
};

enum {
	I2cConIntPending = 0x1 << 4,
	I2cConIntEn = 0x1 << 5,
	I2cConAckGen = 0x1 << 7
};

enum {
	I2cStatAck = 0x1 << 0,
	I2cStatAddrZero = 0x1 << 1,
	I2cStatAddrSlave = 0x1 << 2,
	I2cStatArb = 0x1 << 3,
	I2cStatEnable = 0x1 << 4,
	I2cStatStartStop = 0x1 << 5,
	I2cStatBusy = 0x1 << 5,

	I2cStatModeMask = 0x3 << 6,
	I2cStatSlaveRecv = 0x0 << 6,
	I2cStatSlaveXmit = 0x1 << 6,
	I2cStatMasterRecv = 0x2 << 6,
	I2cStatMasterXmit = 0x3 << 6
};

static struct s3c24x0_i2c_bus i2c_busses[] = {
	{
		.bus_num = 0,
		.regs = (void *)0x12c60000,
		.periph_id = PERIPH_ID_I2C0,
	},
	{
		.bus_num = 1,
		.regs = (void *)0x12c70000,
		.periph_id = PERIPH_ID_I2C1,
	},
	{
		.bus_num = 2,
		.regs = (void *)0x12c80000,
		.periph_id = PERIPH_ID_I2C2,
	},
	{
		.bus_num = 3,
		.regs = (void *)0x12c90000,
		.periph_id = PERIPH_ID_I2C3,
	},
	{
		.bus_num = 4,
		.regs = (void *)0x12ca0000,
		.periph_id = PERIPH_ID_I2C4,
	},
	{
		.bus_num = 5,
		.regs = (void *)0x12cb0000,
		.periph_id = PERIPH_ID_I2C5,
	},
	{
		.bus_num = 6,
		.regs = (void *)0x12cc0000,
		.periph_id = PERIPH_ID_I2C6,
	},
	{
		.bus_num = 7,
		.regs = (void *)0x12cd0000,
		.periph_id = PERIPH_ID_I2C7,
	},
};

static int i2c_int_pending(struct i2c_regs *regs)
{
	return read8(&regs->con) & I2cConIntPending;
}

static void i2c_clear_int(struct i2c_regs *regs)
{
	write8(&regs->con, read8(&regs->con) & ~I2cConIntPending);
}

static void i2c_ack_enable(struct i2c_regs *regs)
{
	write8(&regs->con, read8(&regs->con) | I2cConAckGen);
}

static void i2c_ack_disable(struct i2c_regs *regs)
{
	write8(&regs->con, read8(&regs->con) & ~I2cConAckGen);
}

static int i2c_got_ack(struct i2c_regs *regs)
{
	return !(read8(&regs->stat) & I2cStatAck);
}

static int i2c_wait_for_idle(struct i2c_regs *regs, int timeout_us)
{
	int timeout = DIV_ROUND_UP(timeout_us, 10);
	while (timeout--) {
		if (!(read8(&regs->stat) & I2cStatBusy))
			return 0;
		udelay(10);
	}
	printk(BIOS_ERR, "I2C timeout waiting for idle.\n");
	return 1;
}

static int i2c_wait_for_int(struct i2c_regs *regs, int timeout_us)
{
	int timeout = DIV_ROUND_UP(timeout_us, 10);
	while (timeout--) {
		if (i2c_int_pending(regs))
			return 0;
		udelay(10);
	}
	printk(BIOS_ERR, "I2C timeout waiting for I2C interrupt.\n");
	return 1;
}

static int i2c_send_stop(struct i2c_regs *regs)
{
	uint8_t mode = read8(&regs->stat) & (I2cStatModeMask);
	write8(&regs->stat, mode | I2cStatEnable);
	i2c_clear_int(regs);
	return i2c_wait_for_idle(regs, I2C_TIMEOUT_US);
}

static int i2c_send_start(struct i2c_regs *regs, int read, int chip)
{
	write8(&regs->ds, chip << 1);
	uint8_t mode = read ? I2cStatMasterRecv : I2cStatMasterXmit;
	write8(&regs->stat, mode | I2cStatStartStop | I2cStatEnable);
	i2c_clear_int(regs);

	if (i2c_wait_for_int(regs, I2C_TIMEOUT_US))
		return 1;

	if (!i2c_got_ack(regs)) {
		// Nobody home, but they may just be asleep.
		return 1;
	}

	return 0;
}

static int i2c_xmit_buf(struct i2c_regs *regs, uint8_t *data, int len)
{
	ASSERT(len);

	i2c_ack_enable(regs);

	int i;
	for (i = 0; i < len; i++) {
		write8(&regs->ds, data[i]);

		i2c_clear_int(regs);
		if (i2c_wait_for_int(regs, CONFIG_I2C_TRANSFER_TIMEOUT_US))
			return 1;

		if (!i2c_got_ack(regs)) {
			printk(BIOS_INFO, "I2c nacked.\n");
			return 1;
		}
	}

	return 0;
}

static int i2c_recv_buf(struct i2c_regs *regs, uint8_t *data, int len)
{
	ASSERT(len);

	i2c_ack_enable(regs);

	int i;
	for (i = 0; i < len; i++) {
		if (i == len - 1)
			i2c_ack_disable(regs);

		i2c_clear_int(regs);
		if (i2c_wait_for_int(regs, CONFIG_I2C_TRANSFER_TIMEOUT_US))
			return 1;

		data[i] = read8(&regs->ds);
	}

	return 0;
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			  int seg_count)
{
	struct s3c24x0_i2c_bus *i2c = &i2c_busses[bus];
	struct i2c_regs *regs = i2c->regs;
	int res = 0;

	if (!regs || i2c_wait_for_idle(regs, I2C_TIMEOUT_US))
		return 1;

	write8(&regs->stat, I2cStatMasterXmit | I2cStatEnable);

	int i;
	for (i = 0; i < seg_count; i++) {
		struct i2c_msg *seg = &segments[i];

		res = i2c_send_start(regs, seg->flags & I2C_M_RD, seg->slave);
		if (res)
			break;
		if (seg->flags & I2C_M_RD)
			res = i2c_recv_buf(regs, seg->buf, seg->len);
		else
			res = i2c_xmit_buf(regs, seg->buf, seg->len);
		if (res)
			break;
	}

	return i2c_send_stop(regs) || res;
}

void i2c_init(unsigned int bus, int speed, int slaveadd)
{
	struct s3c24x0_i2c_bus *i2c = &i2c_busses[bus];

	unsigned long freq, pres = 16, div;
	unsigned long val;

	freq = clock_get_periph_rate(i2c->periph_id);
	// Calculate prescaler and divisor values.
	if ((freq / pres / (16 + 1)) > speed)
		/* set prescaler to 512 */
		pres = 512;

	div = 0;

	while ((freq / pres / (div + 1)) > speed)
		div++;

	// Set prescaler, divisor according to freq, also set ACKGEN, IRQ.
	val = (div & 0x0f) | 0xa0 | ((pres == 512) ? 0x40 : 0);
	write32(&i2c->regs->con, val);

	// Init to SLAVE RECEIVE mode and clear I2CADDn.
	write32(&i2c->regs->stat, 0);
	write32(&i2c->regs->add, slaveadd);
	// program Master Transmit (and implicit STOP).
	write32(&i2c->regs->stat, I2cStatMasterXmit | I2cStatEnable);
}
