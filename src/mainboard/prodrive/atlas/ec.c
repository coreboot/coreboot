/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <device/pnp.h>
#include <device/pnp_ops.h>
#include <types.h>

#include "ec.h"

/* 10.7.2 ENABLE CONFIG MODE */
static void pnp_enter_conf_state(const pnp_devfn_t dev)
{
	const u16 port = dev >> 8;
	outb(0x55, port);
}

/* 10.7.3 DISABLE CONFIG MODE */
static void pnp_exit_conf_state(const pnp_devfn_t dev)
{
	const u16 port = dev >> 8;
	outb(0xaa, port);
}

void ec_espi_io_program_iobase(const u16 port, const u8 iobase_index, const u16 base)
{
	const pnp_devfn_t dev = PNP_DEV(port, LDN_ESPI_IO_COMPONENT);

	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_write_config(dev, iobase_index + 2, (base & 0x00ff) >> 0);	/* Addr LSB */
	pnp_write_config(dev, iobase_index + 3, (base & 0xff00) >> 8);	/* Addr MSB */
	pnp_write_config(dev, iobase_index + 0, base != 0x0000);	/* Valid bit */
	pnp_exit_conf_state(dev);
}

/* TABLE 14-5: RUNTIME REGISTER SUMMARY */
#define HOST_EC_MBOX		0x00
#define EC_HOST_MBOX		0x01
#define EC_ADDRESS_LSB		0x02
#define EC_ADDRESS_MSB		0x03
#define EC_DATA_BYTE(n)		(0x04 + (n) % sizeof(u32))
#define INTERRUPT_SOURCE_LSB	0x08
#define INTERRUPT_SOURCE_MSB	0x09
#define INTERRUPT_MASK_LSB	0x0a
#define INTERRUPT_MASK_MSB	0x0b
#define APPLICATION_ID		0x0c

/* 14.8.3 ACCESS TYPES */
enum emi_access_type {
	EMI_ACCESS_8_BIT		= 0,
	EMI_ACCESS_16_BIT		= 1,
	EMI_ACCESS_32_BIT		= 2,
	EMI_ACCESS_32_BIT_AUTO_INC	= 3,
};

void ec_emi_read(u8 *dest, const u16 base, const u8 region, const u16 offset, const u16 length)
{
	const u16 addr = ((region & 1) << 15) | (offset & 0x7ffc) | EMI_ACCESS_32_BIT_AUTO_INC;

	outb((addr & 0x00ff) >> 0, base + EC_ADDRESS_LSB);
	outb((addr & 0xff00) >> 8, base + EC_ADDRESS_MSB);

	/* EC_ADDRESS auto-increment happens when accessing EC_DATA_BYTE_3 */
	for (u16 i = 0; i < length; i++)
		dest[i] = inb(base + EC_DATA_BYTE(offset + i));
}
