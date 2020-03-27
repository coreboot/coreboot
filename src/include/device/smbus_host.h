/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DEVICE_SMBUS_HOST_H__
#define __DEVICE_SMBUS_HOST_H__

#include <stdint.h>
#include <console/console.h>

/* Low-level SMBUS host controller. */

int do_smbus_recv_byte(uintptr_t base, u8 device);
int do_smbus_send_byte(uintptr_t base, u8 device, u8 val);
int do_smbus_read_byte(uintptr_t base, u8 device, u8 address);
int do_smbus_write_byte(uintptr_t base, u8 device, u8 address, u8 data);
int do_smbus_read_word(uintptr_t base, u8 device, u8 address);
int do_smbus_write_word(uintptr_t base, u8 device, u8 address, u16 data);

int do_smbus_block_read(uintptr_t base, u8 device, u8 cmd, size_t max_bytes, u8 *buf);
int do_smbus_block_write(uintptr_t base, u8 device, u8 cmd, size_t bytes, const u8 *buf);
int do_smbus_process_call(uintptr_t base, u8 device, u8 cmd, u16 data, u16 *buf);

/* For Intel, implemented since ICH5. */
int do_i2c_eeprom_read(uintptr_t base, u8 device, u8 offset, size_t bytes, u8 *buf);
int do_i2c_block_write(uintptr_t base, u8 device, size_t bytes, u8 *buf);

/* Upstream API */

uintptr_t smbus_base(void);
int smbus_enable_iobar(uintptr_t base);
void smbus_host_reset(uintptr_t base);
void smbus_set_slave_addr(uintptr_t base, u8 slave_address);

static inline void enable_smbus(void)
{
	uintptr_t base = smbus_base();

	if (smbus_enable_iobar(base) < 0)
		die("SMBus controller not found!");

	smbus_host_reset(base);
	printk(BIOS_DEBUG, "SMBus controller enabled\n");
}

#endif
