/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * I2C bus driver for Kontron COM modules
 *
 * Based on the similar driver in Linux.
 */

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c_bus.h>
#include <timer.h>
#include <delay.h>

#include "chip.h"
#include "kempld.h"
#include "kempld_internal.h"

#define KEMPLD_I2C_PRELOW	0x0b
#define KEMPLD_I2C_PREHIGH	0x0c
#define KEMPLD_I2C_DATA		0x0e

#define KEMPLD_I2C_CTRL		0x0d
#define  I2C_CTRL_IEN		0x40
#define  I2C_CTRL_EN		0x80

#define KEMPLD_I2C_STAT		0x0f
#define  I2C_STAT_IF		0x01
#define  I2C_STAT_TIP		0x02
#define  I2C_STAT_ARBLOST	0x20
#define  I2C_STAT_BUSY		0x40
#define  I2C_STAT_NACK		0x80

#define KEMPLD_I2C_CMD		0x0f
#define  I2C_CMD_START		0x91
#define  I2C_CMD_STOP		0x41
#define  I2C_CMD_READ		0x21
#define  I2C_CMD_WRITE		0x11
#define  I2C_CMD_READ_ACK	0x21
#define  I2C_CMD_READ_NACK	0x29
#define  I2C_CMD_IACK		0x01

#define EIO		  5
#define ENXIO		  6
#define EAGAIN		 11
#define EBUSY		 16
#define ETIMEDOUT	110

enum kempld_i2c_state {
	STATE_DONE = 0,
	STATE_INIT,
	STATE_ADDR,
	STATE_ADDR10,
	STATE_START,
	STATE_WRITE,
	STATE_READ,
	STATE_ERROR,
};

struct kempld_i2c_data {
	const struct i2c_msg	*msg;
	size_t			pos;
	size_t			nmsgs;
	enum kempld_i2c_state	state;
};

/*
 * kempld_get_mutex must be called prior to calling this function.
 */
static int kempld_i2c_process(struct kempld_i2c_data *const i2c)
{
	u8 stat = kempld_read8(KEMPLD_I2C_STAT);
	const struct i2c_msg *msg = i2c->msg;
	u8 addr;

	/* Ready? */
	if (stat & I2C_STAT_TIP)
		return -EBUSY;

	if (i2c->state == STATE_DONE || i2c->state == STATE_ERROR) {
		/* Stop has been sent */
		kempld_write8(KEMPLD_I2C_CMD, I2C_CMD_IACK);
		if (i2c->state == STATE_ERROR)
			return -EIO;
		return 0;
	}

	/* Error? */
	if (stat & I2C_STAT_ARBLOST) {
		i2c->state = STATE_ERROR;
		kempld_write8(KEMPLD_I2C_CMD, I2C_CMD_STOP);
		return -EAGAIN;
	}

	if (i2c->state == STATE_INIT) {
		if (stat & I2C_STAT_BUSY)
			return -EBUSY;

		i2c->state = STATE_ADDR;
	}

	if (i2c->state == STATE_ADDR) {
		/* 10 bit address? */
		if (i2c->msg->flags & I2C_M_TEN) {
			addr = 0xf0 | ((i2c->msg->slave >> 7) & 0x6);
			i2c->state = STATE_ADDR10;
		} else {
			addr = (i2c->msg->slave << 1);
			i2c->state = STATE_START;
		}

		/* Set read bit if necessary */
		addr |= (i2c->msg->flags & I2C_M_RD) ? 1 : 0;

		kempld_write8(KEMPLD_I2C_DATA, addr);
		kempld_write8(KEMPLD_I2C_CMD, I2C_CMD_START);

		return 0;
	}

	/* Second part of 10 bit addressing */
	if (i2c->state == STATE_ADDR10) {
		kempld_write8(KEMPLD_I2C_DATA, i2c->msg->slave & 0xff);
		kempld_write8(KEMPLD_I2C_CMD, I2C_CMD_WRITE);

		i2c->state = STATE_START;
		return 0;
	}

	if (i2c->state == STATE_START || i2c->state == STATE_WRITE) {
		i2c->state = (msg->flags & I2C_M_RD) ? STATE_READ : STATE_WRITE;

		if (stat & I2C_STAT_NACK) {
			i2c->state = STATE_ERROR;
			kempld_write8(KEMPLD_I2C_CMD, I2C_CMD_STOP);
			return -ENXIO;
		}
	} else {
		msg->buf[i2c->pos++] = kempld_read8(KEMPLD_I2C_DATA);
	}

	if (i2c->pos >= msg->len) {
		i2c->nmsgs--;
		i2c->msg++;
		i2c->pos = 0;
		msg = i2c->msg;

		if (i2c->nmsgs) {
			if (!(msg->flags & I2C_M_NOSTART)) {
				i2c->state = STATE_ADDR;
				return 0;
			}
			i2c->state = (msg->flags & I2C_M_RD) ? STATE_READ : STATE_WRITE;
		} else {
			i2c->state = STATE_DONE;
			kempld_write8(KEMPLD_I2C_CMD, I2C_CMD_STOP);
			return 0;
		}
	}

	if (i2c->state == STATE_READ) {
		kempld_write8(KEMPLD_I2C_CMD, i2c->pos == (msg->len - 1) ?
			      I2C_CMD_READ_NACK : I2C_CMD_READ_ACK);
	} else {
		kempld_write8(KEMPLD_I2C_DATA, msg->buf[i2c->pos++]);
		kempld_write8(KEMPLD_I2C_CMD, I2C_CMD_WRITE);
	}

	return 0;
}

static int kempld_i2c_xfer(struct device *const dev,
			   const struct i2c_msg *const msgs,
			   const size_t num)
{
	struct kempld_i2c_data i2c;
	struct stopwatch sw;
	int ret;

	if (kempld_get_mutex(100) < 0)
		return -ENXIO;

	i2c.msg = msgs;
	i2c.pos = 0;
	i2c.nmsgs = num;
	i2c.state = STATE_INIT;

	/* Handle the transfer */
	stopwatch_init_msecs_expire(&sw, 1000);
	while (!stopwatch_expired(&sw)) {
		ret = kempld_i2c_process(&i2c);

		if (i2c.state == STATE_DONE || i2c.state == STATE_ERROR) {
			if (i2c.state == STATE_DONE) {
				printk(BIOS_SPEW, "kempld_i2c: Processed %zu segments.\n", num);
				ret = 0;
			} else {
				printk(BIOS_INFO, "kempld_i2c: Transfer failed.\n");
			}
			goto _release;
		}

		if (ret == 0)
			stopwatch_init_msecs_expire(&sw, 1000);

		udelay(10);
	}

	i2c.state = STATE_ERROR;
	ret = -ETIMEDOUT;
	printk(BIOS_INFO, "kempld_i2c: Transfer failed.\n");

_release:
	kempld_release_mutex();
	return ret;
}

static const struct i2c_bus_operations kempld_i2c_bus_ops = {
	.transfer = kempld_i2c_xfer,
};

static struct device_operations kempld_i2c_dev_ops = {
	.scan_bus	= &scan_smbus,
	.ops_i2c_bus	= &kempld_i2c_bus_ops,
};

void kempld_i2c_device_init(struct device *const dev)
{
	const struct ec_kontron_kempld_config *const config = dev->chip_info;
	u16 prescale_corr, frequency;
	long prescale;
	u8 ctrl;
	u8 stat;
	u8 cfg;

	if (kempld_get_mutex(100) < 0)
		return;

	/* Make sure the device is disabled */
	ctrl = kempld_read8(KEMPLD_I2C_CTRL);
	ctrl &= ~(I2C_CTRL_EN | I2C_CTRL_IEN);
	kempld_write8(KEMPLD_I2C_CTRL, ctrl);

	frequency = KEMPLD_I2C_FREQ_STANDARD_MODE_100KHZ;
	if (config && config->i2c_frequency) {
		if (config->i2c_frequency <= KEMPLD_I2C_FREQ_MAX) {
			frequency = config->i2c_frequency;
		} else {
			printk(BIOS_NOTICE,
				"kempld_i2c: %d kHz is too high!\n",
				config->i2c_frequency);
		}
	}
	printk(BIOS_INFO, "kempld_i2c: Use frequency %d\n", frequency);

	const u8 spec_major = KEMPLD_SPEC_GET_MAJOR(kempld_read8(KEMPLD_SPEC));
	if (spec_major == 1)
		prescale = KEMPLD_CLK / (frequency * 5) - 1000;
	else
		prescale = KEMPLD_CLK / (frequency * 4) - 3000;

	if (prescale < 0)
		prescale = 0;

	/* Round to the best matching value */
	prescale_corr = prescale / 1000;
	if (prescale % 1000 >= 500)
		prescale_corr++;

	kempld_write8(KEMPLD_I2C_PRELOW, prescale_corr & 0xff);
	kempld_write8(KEMPLD_I2C_PREHIGH, prescale_corr >> 8);

	/* Disable I2C bus output on GPIO pins */
	cfg = kempld_read8(KEMPLD_CFG);
	cfg &= ~KEMPLD_CFG_GPIO_I2C_MUX;
	kempld_write8(KEMPLD_CFG, cfg);

	/* Enable the device */
	kempld_write8(KEMPLD_I2C_CMD, I2C_CMD_IACK);
	ctrl |= I2C_CTRL_EN;
	kempld_write8(KEMPLD_I2C_CTRL, ctrl);

	stat = kempld_read8(KEMPLD_I2C_STAT);
	if (stat & I2C_STAT_BUSY)
		kempld_write8(KEMPLD_I2C_CMD, I2C_CMD_STOP);

	dev->ops = &kempld_i2c_dev_ops;

	kempld_release_mutex();
}
