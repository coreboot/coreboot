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

static inline const struct smbus_bus_operations *ops_smbus_bus(struct bus *bus)
{
        const struct smbus_bus_operations *bops;
        bops = 0;
        if (bus && bus->dev && bus->dev->ops) {
                bops = bus->dev->ops->ops_smbus_bus;
        }
        return bops;
}
struct bus *get_pbus_smbus(device_t dev);
int smbus_set_link(device_t dev);

int smbus_quick_read(device_t dev);
int smbus_quick_write(device_t dev);
int smbus_recv_byte(device_t dev);
int smbus_send_byte(device_t dev, uint8_t byte);
int smbus_read_byte(device_t dev, uint8_t addr);
int smbus_write_byte(device_t dev, uint8_t addr, uint8_t val);
int smbus_read_word(device_t dev, uint8_t addr);
int smbus_write_word(device_t dev, uint8_t addr, uint16_t val);
int smbus_process_call(device_t dev, uint8_t cmd, uint16_t data);
int smbus_block_read(device_t dev, uint8_t cmd, uint8_t bytes, uint8_t *buffer);
int smbus_block_write(device_t dev, uint8_t cmd, uint8_t bytes, const uint8_t *buffer);


#endif /* DEVICE_SMBUS_H */
