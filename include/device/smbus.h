/*
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

*/
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
	int (*send_byte)   (device_t dev, u8 value);
	int (*read_byte)   (device_t dev, u8 addr);
	int (*write_byte)  (device_t dev, u8 addr, u8 value);
	int (*read_word)   (device_t dev, u8 addr);
	int (*write_word)  (device_t dev, u8 addr, u16 value);
	int (*process_call)(device_t dev, u8 cmd, u16 data);
	int (*block_read)  (device_t dev, u8 cmd, u8 bytes, u8 *buffer);
	int (*block_write) (device_t dev, u8 cmd, u8 bytes, const u8 *buffer);
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
int smbus_send_byte(device_t dev, u8 byte);
int smbus_read_byte(device_t dev, u8 addr);
int smbus_write_byte(device_t dev, u8 addr, u8 val);
int smbus_read_word(device_t dev, u8 addr);
int smbus_write_word(device_t dev, u8 addr, u16 val);
int smbus_process_call(device_t dev, u8 cmd, u16 data);
int smbus_block_read(device_t dev, u8 cmd, u8 bytes, u8 *buffer);
int smbus_block_write(device_t dev, u8 cmd, u8 bytes, const u8 *buffer);


#endif /* DEVICE_SMBUS_H */
