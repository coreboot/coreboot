#ifndef DEVICE_SMBUS_H
#define DEVICE_SMBUS_H

#include <stdint.h>
#include <device/device.h>
#include <device/path.h>
#include <device/smbus_def.h>

/* Common smbus bus operations */
struct smbus_bus_operations {
	int (*quick_read)  (device_t dev);
	int (*quick_write) (device_t dev);
	int (*recv_byte)   (device_t dev);
	int (*send_byte)   (device_t dev, uint8_t value);
	int (*read_byte)   (device_t dev, uint8_t addr);
	int (*write_byte)  (device_t dev, uint8_t addr, uint8_t value);
	int (*read_word)   (device_t dev, uint8_t addr);
	int (*write_word)  (device_t dev, uint8_t addr, uint16_t value);
	int (*process_call)(device_t dev, uint8_t cmd, uint16_t data);
	int (*block_read)  (device_t dev, uint8_t cmd, uint8_t bytes, uint8_t *buffer);
	int (*block_write) (device_t dev, uint8_t cmd, uint8_t bytes, const uint8_t *buffer);
};

static inline int smbus_quick_read(device_t dev)
{
	return dev->bus->dev->ops->ops_smbus_bus->quick_read(dev);
}
static inline int smbus_quick_write(device_t dev)
{
	return dev->bus->dev->ops->ops_smbus_bus->quick_write(dev);
}
static inline int smbus_recv_byte(device_t dev)
{
	return dev->bus->dev->ops->ops_smbus_bus->recv_byte(dev);
}
static inline int smbus_send_byte(device_t dev, uint8_t byte)
{
	return dev->bus->dev->ops->ops_smbus_bus->send_byte(dev, byte);
}
static inline int smbus_read_byte(device_t dev, uint8_t addr)
{
	return dev->bus->dev->ops->ops_smbus_bus->read_byte(dev, addr);
}
static inline int smbus_write_byte(device_t dev, uint8_t addr, uint8_t val)
{
	return dev->bus->dev->ops->ops_smbus_bus->write_byte(dev, addr, val);
}
static inline int smbus_read_word(device_t dev, uint8_t addr)
{
	return dev->bus->dev->ops->ops_smbus_bus->read_word(dev, addr);
}
static inline int smbus_write_word(device_t dev, uint8_t addr, uint16_t val)
{
	return dev->bus->dev->ops->ops_smbus_bus->write_word(dev, addr, val);
}
static inline int smbus_process_call(device_t dev, uint8_t cmd, uint16_t data)
{
	return dev->bus->dev->ops->ops_smbus_bus->process_call(dev, cmd, data);
}
static inline int smbus_block_read(device_t dev, uint8_t cmd, uint8_t bytes, uint8_t *buffer)
{
	return dev->bus->dev->ops->ops_smbus_bus->block_read(dev, cmd, bytes, buffer);
}
static inline int smbus_block_write(device_t dev, uint8_t cmd, uint8_t bytes, const uint8_t *buffer)
{
	return dev->bus->dev->ops->ops_smbus_bus->block_write(dev, cmd, bytes, buffer);
}


#endif /* DEVICE_SMBUS_H */
