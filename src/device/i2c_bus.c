/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <console/console.h>
#include <device/smbus.h>
#include <device/i2c_bus.h>
#include <commonlib/endian.h>

struct bus *i2c_link(const struct device *const dev)
{
	if (!dev || !dev->bus)
		return NULL;

	struct bus *link = dev->bus;
	while (link) {
		struct device *const parent = link->dev;

		if (parent && parent->ops &&
		    (parent->ops->ops_i2c_bus || parent->ops->ops_smbus_bus))
			break;

		if (parent && parent->bus)
			link = parent->bus;
		else
			link = NULL;
	}

	if (!link)
		printk(BIOS_ALERT, "%s Cannot find I2C or SMBus bus operations", dev_path(dev));

	return link;
}

int i2c_dev_readb(struct device *const dev)
{
	struct device *const busdev = i2c_busdev(dev);
	if (!busdev)
		return -1;

	if (busdev->ops->ops_i2c_bus) {
		uint8_t val;
		const struct i2c_msg msg = {
			.flags	= I2C_M_RD,
			.slave	= dev->path.i2c.device,
			.buf	= &val,
			.len	= sizeof(val),
		};

		const int ret = busdev->ops->ops_i2c_bus->transfer(busdev, &msg, 1);
		if (ret)
			return ret;
		else
			return val;
	} else if (busdev->ops->ops_smbus_bus->recv_byte) {
		return busdev->ops->ops_smbus_bus->recv_byte(dev);
	}

	printk(BIOS_ERR, "%s Missing ops_smbus_bus->recv_byte", dev_path(busdev));
	return -1;
}

int i2c_dev_writeb(struct device *const dev, uint8_t val)
{
	struct device *const busdev = i2c_busdev(dev);
	if (!busdev)
		return -1;

	if (busdev->ops->ops_i2c_bus) {
		const struct i2c_msg msg = {
			.flags	= 0,
			.slave	= dev->path.i2c.device,
			.buf	= &val,
			.len	= sizeof(val),
		};
		return busdev->ops->ops_i2c_bus->transfer(busdev, &msg, 1);
	} else if (busdev->ops->ops_smbus_bus->send_byte) {
		return busdev->ops->ops_smbus_bus->send_byte(dev, val);
	}

	printk(BIOS_ERR, "%s Missing ops_smbus_bus->send_byte", dev_path(busdev));
	return -1;
}

int i2c_dev_readb_at(struct device *const dev, uint8_t off)
{
	struct device *const busdev = i2c_busdev(dev);
	if (!busdev)
		return -1;

	if (busdev->ops->ops_i2c_bus) {
		uint8_t val;
		const struct i2c_msg msg[] = {
			{
				.flags	= 0,
				.slave	= dev->path.i2c.device,
				.buf	= &off,
				.len	= sizeof(off),
			},
			{
				.flags	= I2C_M_RD,
				.slave	= dev->path.i2c.device,
				.buf	= &val,
				.len	= sizeof(val),
			},
		};

		const int ret = busdev->ops->ops_i2c_bus->transfer(busdev, msg,
								   ARRAY_SIZE(msg));
		if (ret)
			return ret;
		else
			return val;
	} else if (busdev->ops->ops_smbus_bus->read_byte) {
		return busdev->ops->ops_smbus_bus->read_byte(dev, off);
	}

	printk(BIOS_ERR, "%s Missing ops_smbus_bus->read_byte", dev_path(busdev));
	return -1;
}

int i2c_dev_writeb_at(struct device *const dev, const uint8_t off, const uint8_t val)
{
	struct device *const busdev = i2c_busdev(dev);
	if (!busdev)
		return -1;

	if (busdev->ops->ops_i2c_bus) {
		uint8_t buf[] = { off, val };
		const struct i2c_msg msg = {
			.flags	= 0,
			.slave	= dev->path.i2c.device,
			.buf	= buf,
			.len	= sizeof(buf),
		};
		return busdev->ops->ops_i2c_bus->transfer(busdev, &msg, 1);
	} else if (busdev->ops->ops_smbus_bus->write_byte) {
		return busdev->ops->ops_smbus_bus->write_byte(dev, off, val);
	}

	printk(BIOS_ERR, "%s Missing ops_smbus_bus->write_byte", dev_path(busdev));
	return -1;
}

int i2c_dev_read_at16(struct device *const dev, uint8_t *const buf, const size_t len,
		      uint16_t off)
{
	struct device *const busdev = i2c_busdev(dev);
	if (!busdev)
		return -1;

	if (busdev->ops->ops_i2c_bus) {
		const struct i2c_msg msg[] = {
			{
				.flags	= 0,
				.slave	= dev->path.i2c.device,
				.buf	= (uint8_t *)&off,
				.len	= sizeof(off),
			},
			{
				.flags	= I2C_M_RD,
				.slave	= dev->path.i2c.device,
				.buf	= buf,
				.len	= len,
			},
		};

		write_be16(&off, off);
		const int ret = busdev->ops->ops_i2c_bus->transfer(busdev, msg,
								   ARRAY_SIZE(msg));
		if (ret)
			return ret;
		else
			return len;
	} else {
		printk(BIOS_ERR, "%s Missing ops_i2c_bus->transfer", dev_path(busdev));
		return -1;
	}
}
