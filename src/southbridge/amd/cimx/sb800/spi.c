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
#include "SBPLATFORM.h"


void executeCommand(volatile u8 * spi_address)
{
	*(spi_address + 2) |= 1;
}

void wait4CommandComplete(volatile u8 * spi_address)
{
	while (*(spi_address + 2) & 1)
		printk(BIOS_DEBUG, "wait4CommandComplete\n");
}

void resetInternalFIFOPointer(volatile u8 * spi_address)
{
	u8 val;

	do {
		*(spi_address + 2) |= 0x10;
		val = *(spi_address + 0xd);
	} while (val & 0x7);
}

u8 readSPIStatus(volatile u8 * spi_address)
{
	u8 val;
	*spi_address = 0x05;
	*(spi_address + 1) = 0x11;
	resetInternalFIFOPointer(spi_address);
	*(spi_address + 0xC) = 0x0;	/* dummy */
	resetInternalFIFOPointer(spi_address);
	executeCommand(spi_address);
	wait4CommandComplete(spi_address);
	resetInternalFIFOPointer(spi_address);
	val = *(spi_address + 0xC);
	val = *(spi_address + 0xC);
	return val;
}

void wait4FlashPartReady(volatile u8 * spi_address)
{
	while (readSPIStatus(spi_address) & 1) ;
}

void writeSPIStatus(volatile u8 * spi_address, u8 status)
{
	*spi_address = 0x50;	/* EWSR */
	*(spi_address + 1) = 0;	/* RxByte=TxByte=0 */
	executeCommand(spi_address);
	wait4CommandComplete(spi_address);

	*spi_address = 0x01;	/* WRSR */
	*(spi_address + 1) = 0x01;
	resetInternalFIFOPointer(spi_address);
	*(spi_address + 0xC) = status;
	resetInternalFIFOPointer(spi_address);
	executeCommand(spi_address);
	wait4CommandComplete(spi_address);
	wait4FlashPartReady(spi_address);

	readSPIStatus(spi_address);
}

void readSPIID(volatile u8 * spi_address)
{
	u8 mid = 0, did = 0;
	*spi_address = 0x90;
	*(spi_address + 1) = 0x23;	/* RxByte=2, TxByte=3 */
	resetInternalFIFOPointer(spi_address);
	*(spi_address + 0xC) = 0;
	*(spi_address + 0xC) = 0;
	*(spi_address + 0xC) = 0;
	resetInternalFIFOPointer(spi_address);
	executeCommand(spi_address);
	wait4CommandComplete(spi_address);
	resetInternalFIFOPointer(spi_address);
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

void SPIWriteEnable(volatile u8 * spi_address)
{
	*spi_address = 0x06;	/* Write Enable */
	*(spi_address + 1) = 0x0;	/* RxByte=0, TxByte=0 */
	executeCommand(spi_address);
	wait4CommandComplete(spi_address);
	//wait4FlashPartReady(spi_address);
}

void sectorEraseSPI(volatile u8 * spi_address, u32 address)
{
	SPIWriteEnable(spi_address);
	*spi_address = 0x20;
	*(spi_address + 1) = 0x03;	/* RxByte=0, TxByte=3 */

	resetInternalFIFOPointer(spi_address);
	*(spi_address + 0xC) = (address >> 16) & 0xFF;
	*(spi_address + 0xC) = (address >> 8) & 0xFF;
	*(spi_address + 0xC) = (address >> 0) & 0xFF;
	resetInternalFIFOPointer(spi_address);
	executeCommand(spi_address);
	wait4CommandComplete(spi_address);
	wait4FlashPartReady(spi_address);
}

void chipEraseSPI(volatile u8 * spi_address)
{
	SPIWriteEnable(spi_address);
	*spi_address = 0xC7;
	*(spi_address + 1) = 0x00;
	executeCommand(spi_address);
	wait4CommandComplete(spi_address);
	wait4FlashPartReady(spi_address);
}

void byteProgram(volatile u8 * spi_address, u32 address, u32 data)
{
	SPIWriteEnable(spi_address);
	*spi_address = 0x02;
	*(spi_address + 1) = 0x0 << 4 | 4;
	resetInternalFIFOPointer(spi_address);
	*(spi_address + 0xC) = (address >> 16) & 0xFF;
	*(spi_address + 0xC) = (address >> 8) & 0xFF;
	*(spi_address + 0xC) = (address >> 0) & 0xFF;
	*(spi_address + 0xC) = data & 0xFF;
	resetInternalFIFOPointer(spi_address);
	executeCommand(spi_address);
	wait4CommandComplete(spi_address);
	wait4FlashPartReady(spi_address);
}

void dwordnoneAAIProgram(volatile u8 * spi_address, u32 address, u32 data)
{
	u8 i;
	/*
	 * printk(BIOS_SPEW, "%s: addr=%x, data=%x\n", __func__, address, data);
	 */
	for (i = 0; i < 4; i++) {
		SPIWriteEnable(spi_address);
		*spi_address = 0x02;
		*(spi_address + 1) = 0x0 << 4 | 4;
		resetInternalFIFOPointer(spi_address);
		*(spi_address + 0xC) = (address >> 16) & 0xFF;
		*(spi_address + 0xC) = (address >> 8) & 0xFF;
		*(spi_address + 0xC) = (address >> 0) & 0xFF;
		*(spi_address + 0xC) = data & 0xFF;
		data >>= 8;
		address++;
		resetInternalFIFOPointer(spi_address);
		executeCommand(spi_address);
		wait4CommandComplete(spi_address);
		wait4FlashPartReady(spi_address);
	}
}

void dwordProgram(volatile u8 * spi_address, u32 address, u32 data)
{
	SPIWriteEnable(spi_address);
	*spi_address = 0x02;
	*(spi_address + 1) = 0x0 << 4 | 7;
	resetInternalFIFOPointer(spi_address);
	*(spi_address + 0xC) = (address >> 16) & 0xFF;
	*(spi_address + 0xC) = (address >> 8) & 0xFF;
	*(spi_address + 0xC) = (address >> 0) & 0xFF;
	*(spi_address + 0xC) = data & 0xFF;
	*(spi_address + 0xC) = (data >> 8) & 0xFF;
	*(spi_address + 0xC) = (data >> 16) & 0xFF;
	*(spi_address + 0xC) = (data >> 24) & 0xFF;
	resetInternalFIFOPointer(spi_address);
	executeCommand(spi_address);
	wait4CommandComplete(spi_address);
	wait4FlashPartReady(spi_address);
}

void directByteProgram(volatile u8 * spi_address, volatile u32 * address, u32 data)
{
	SPIWriteEnable(spi_address);
	*address = data;
	wait4FlashPartReady(spi_address);
}
