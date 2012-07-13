/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include "spi.h"

void execute_command(volatile u8 * spi_address)
{
	*(spi_address + 2) |= 1;
}

void wait4command_complete(volatile u8 * spi_address)
{
//	while (*(spi_address + 2) & 1)
	while ((*(spi_address + 2) & 1) && (*(spi_address + 3) & 0x80))
		printk(BIOS_DEBUG, "wait4CommandComplete\n");
}

void reset_internal_fifo_pointer(volatile u8 * spi_address)
{
	u8 val;

	do {
		*(spi_address + 2) |= 0x10;
		val = *(spi_address + 0xd);
	} while (val & 0x7);
}

u8 read_spi_status(volatile u8 * spi_address)
{
	u8 val;
	*spi_address = 0x05;
	*(spi_address + 1) = 0x21;
	reset_internal_fifo_pointer(spi_address);
	*(spi_address + 0xC) = 0x0;	/* dummy */
	reset_internal_fifo_pointer(spi_address);
	execute_command(spi_address);
	wait4command_complete(spi_address);
	reset_internal_fifo_pointer(spi_address);
	val = *(spi_address + 0xC);
	val = *(spi_address + 0xC);
	val = *(spi_address + 0xC);
	return val;
}

void wait4flashpart_ready(volatile u8 * spi_address)
{
	while (read_spi_status(spi_address) & 1) ;
}

void write_spi_status(volatile u8 * spi_address, u8 status)
{
	*spi_address = 0x50;	/* EWSR */
	*(spi_address + 1) = 0;	/* RxByte=TxByte=0 */
	execute_command(spi_address);
	wait4command_complete(spi_address);

	*spi_address = 0x01;	/* WRSR */
	*(spi_address + 1) = 0x01;
	reset_internal_fifo_pointer(spi_address);
	*(spi_address + 0xC) = status;
	reset_internal_fifo_pointer(spi_address);
	execute_command(spi_address);
	wait4command_complete(spi_address);
	wait4flashpart_ready(spi_address);

	read_spi_status(spi_address);
}

void read_spi_id(volatile u8 * spi_address)
{
	u8 mid = 0, did = 0;
	*spi_address = 0x90;
	*(spi_address + 1) = 0x23;	/* RxByte=2, TxByte=3 */
	reset_internal_fifo_pointer(spi_address);
	*(spi_address + 0xC) = 0;
	*(spi_address + 0xC) = 0;
	*(spi_address + 0xC) = 0;
	reset_internal_fifo_pointer(spi_address);
	execute_command(spi_address);
	wait4command_complete(spi_address);
	reset_internal_fifo_pointer(spi_address);
	mid = *(spi_address + 0xC);
	printk(BIOS_DEBUG, "mid=%x, did=%x\n", mid, did);
	mid = *(spi_address + 0xC);
	printk(BIOS_DEBUG, "mid=%x, did=%x\n", mid, did);
	mid = *(spi_address + 0xC);
	printk(BIOS_DEBUG, "mid=%x, did=%x\n", mid, did);

	mid = *(spi_address + 0xC);
	did = *(spi_address + 0xC);
	printk(BIOS_DEBUG, "mid=%x, did=%x\n", mid, did);
}

void spi_write_enable(volatile u8 * spi_address)
{
	*spi_address = 0x06;	/* Write Enable */
	*(spi_address + 1) = 0x0;	/* RxByte=0, TxByte=0 */
	execute_command(spi_address);
	wait4command_complete(spi_address);
}

void spi_write_disable(volatile u8 * spi_address)
{
	*spi_address = 0x04;	/* Write Enable */
	*(spi_address + 1) = 0x0;	/* RxByte=0, TxByte=0 */
	execute_command(spi_address);
	wait4command_complete(spi_address);
}

void sector_erase_spi(volatile u8 * spi_address, u32 address)
{
	spi_write_enable(spi_address);
	*spi_address = 0x20;
	*(spi_address + 1) = 0x03;	/* RxByte=0, TxByte=3 */

	reset_internal_fifo_pointer(spi_address);
	*(spi_address + 0xC) = (address >> 16) & 0xFF;
	*(spi_address + 0xC) = (address >> 8) & 0xFF;
	*(spi_address + 0xC) = (address >> 0) & 0xFF;
	reset_internal_fifo_pointer(spi_address);
	execute_command(spi_address);
	wait4command_complete(spi_address);
	wait4flashpart_ready(spi_address);
}

void chip_erase_spi(volatile u8 * spi_address)
{
	spi_write_enable(spi_address);
	*spi_address = 0xC7;
	*(spi_address + 1) = 0x00;
	execute_command(spi_address);
	wait4command_complete(spi_address);
	wait4flashpart_ready(spi_address);
}

void byte_program(volatile u8 * spi_address, u32 address, u32 data)
{
	spi_write_enable(spi_address);
	*spi_address = 0x02;
	*(spi_address + 1) = 0x0 << 4 | 4;
	reset_internal_fifo_pointer(spi_address);
	*(spi_address + 0xC) = (address >> 16) & 0xFF;
	*(spi_address + 0xC) = (address >> 8) & 0xFF;
	*(spi_address + 0xC) = (address >> 0) & 0xFF;
	*(spi_address + 0xC) = data & 0xFF;
	reset_internal_fifo_pointer(spi_address);
	execute_command(spi_address);
	wait4command_complete(spi_address);
	wait4flashpart_ready(spi_address);
}

void dword_noneAAI_program(volatile u8 * spi_address, u32 address, u32 data)
{
	u8 i;
	/*
	 * printk(BIOS_SPEW, "%s: addr=%x, data=%x\n", __func__, address, data);
	 */
	for (i = 0; i < 4; i++) {
		spi_write_enable(spi_address);
		*spi_address = 0x02;
		*(spi_address + 1) = 0x0 << 4 | 4;
		reset_internal_fifo_pointer(spi_address);
		*(spi_address + 0xC) = (address >> 16) & 0xFF;
		*(spi_address + 0xC) = (address >> 8) & 0xFF;
		*(spi_address + 0xC) = (address >> 0) & 0xFF;
		*(spi_address + 0xC) = data & 0xFF;
		data >>= 8;
		address++;
		reset_internal_fifo_pointer(spi_address);
		execute_command(spi_address);
		wait4command_complete(spi_address);
		wait4flashpart_ready(spi_address);
	}
}

void dword_program(volatile u8 * spi_address, u32 address, u32 data)
{
	spi_write_enable(spi_address);
	*spi_address = 0x02;
	*(spi_address + 1) = 0x0 << 4 | 7;
	reset_internal_fifo_pointer(spi_address);
	*(spi_address + 0xC) = (address >> 16) & 0xFF;
	*(spi_address + 0xC) = (address >> 8) & 0xFF;
	*(spi_address + 0xC) = (address >> 0) & 0xFF;
	*(spi_address + 0xC) = data & 0xFF;
	*(spi_address + 0xC) = (data >> 8) & 0xFF;
	*(spi_address + 0xC) = (data >> 16) & 0xFF;
	*(spi_address + 0xC) = (data >> 24) & 0xFF;
	reset_internal_fifo_pointer(spi_address);
	execute_command(spi_address);
	wait4command_complete(spi_address);
	wait4flashpart_ready(spi_address);
}

void direct_byte_program(volatile u8 * spi_address, volatile u32 * address, u32 data)
{
	spi_write_enable(spi_address);
	*address = data;
	wait4flashpart_ready(spi_address);
}
