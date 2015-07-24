/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/pcr.h>
#include <soc/iomap.h>
#include <console/console.h>

static inline void *pcr_reg_address(u8 pid, u16 offset)
{
	uintptr_t reg_addr;

	/* Create an address based off of port id and offset. */
	reg_addr = PCH_PCR_BASE_ADDRESS;
	reg_addr += ((uintptr_t)pid) << PCR_PORTID_SHIFT;
	reg_addr += ((uintptr_t)offset) << PCR_OFFSET_SHIFT;

	return (void *)reg_addr;
}

uint8_t *pcr_port_regs(u8 pid)
{
	return pcr_reg_address(pid, 0);
}

/*
 * Read PCR register. (This is internal function)
 * It returns PCR register and size in 1/2/4 bytes.
 * The offset should not exceed 0xFFFF and must be aligned with size
 */
static int pch_pcr_read(u8 pid, u16 offset, u32 size, void *data)
{
	if ((offset & (size - 1)) != 0) {
		printk(BIOS_DEBUG,
			"PchPcrRead error. Invalid Offset: %x Size: %x",
			offset, size);
		return -1;
	}
	switch (size) {
	case 4:
		*(u32 *) data = read32(pcr_reg_address(pid, offset));
		break;
	case 2:
		*(u16 *) data = read16(pcr_reg_address(pid, offset));
		break;
	case 1:
		*(u8 *) data = read8(pcr_reg_address(pid, offset));
		break;
	default:
		break;
	}
	return 0;
}

int pcr_read32(u8 pid, u16 offset, u32 *outdata)
{
	return pch_pcr_read(pid, offset, sizeof(u32), (u32 *)outdata);
}

int pcr_read16(u8 pid, u16 offset, u16 *outdata)
{
	return pch_pcr_read(pid, offset, sizeof(u16), (u32 *)outdata);
}

int pcr_read8(u8 pid, u16 offset, u8 *outdata)
{
	return pch_pcr_read(pid, offset, sizeof(u8), (u32 *)outdata);
}

/*
 * After every write one needs to perform a read an innocuous register to
 * ensure the writes are completed for certain ports. This is done for
 * all ports so that the callers don't need the per-port knowledge for
 * each transaction.
 */
static inline void complete_write(void)
{
	/* Read the general control and function disable register. */
	const size_t R_PCH_PCR_LPC_GCFD = 0x3418;
	read32(pcr_reg_address(PID_LPC, R_PCH_PCR_LPC_GCFD));
}

/*
 * Write PCR register. (This is internal function)
 * It returns PCR register and size in 1/2/4 bytes.
 * The offset should not exceed 0xFFFF and must be aligned with size
 */
static int pch_pcr_write(u8 pid, u16 offset, u32 size, u32 data)
{
	if ((offset & (size - 1)) != 0) {
		printk(BIOS_DEBUG,
			"PchPcrWrite error. Invalid Offset: %x Size: %x",
			offset, size);
		return -1;
	}
	/* Write the PCR register with provided data
	 * Then read back PCR register to prevent from back to back write.
	 */
	switch (size) {
	case 4:
		write32(pcr_reg_address(pid, offset), (u32) data);
		break;
	case 2:
		write16(pcr_reg_address(pid, offset), (u16) data);
		break;
	case 1:
		write8(pcr_reg_address(pid, offset), (u8) data);
		break;
	default:
		break;
	}
	/* Ensure the writes complete. */
	complete_write();

	return 0;
}

int pcr_write32(u8 pid, u16 offset, u32 indata)
{
	return pch_pcr_write(pid, offset, sizeof(u32), indata);
}

int pcr_write16(u8 pid, u16 offset, u16 indata)
{
	return pch_pcr_write(pid, offset, sizeof(u16), indata);
}

int pcr_write8(u8 pid, u16 offset, u8 indata)
{
	return pch_pcr_write(pid, offset, sizeof(u8), indata);
}

/*
 * Write PCR register. (This is internal function)
 * It programs  PCR register and size in 1/2/4 bytes.
 * The offset should not exceed 0xFFFF and must be aligned with size
 *
 * u8 defines as 8 bit Port ID that will be used when sending
 * transaction to sideband.
 */
static int pcr_and_then_or(u8 pid, u16 offset, u32 size, u32 anddata,
		   u32 ordata)
{
	u8 status;
	u32 data32;

	status = pch_pcr_read(pid, offset, size, &data32);
	if (status != 0)
		return -1;

	data32 &= anddata;
	data32 |= ordata;

	status = pch_pcr_write(pid, offset, size, data32);
	return status;
}

int pcr_andthenor32(u8 pid, u16 offset, u32 anddata, u32 ordata)
{
	return pcr_and_then_or(pid, offset, sizeof(u32), anddata, ordata);
}

int pcr_andthenor16(u8 pid, u16 offset, u16 anddata, u16 ordata)
{
	return pcr_and_then_or(pid, offset, sizeof(u16), anddata, ordata);
}

int pcr_andthenor8(u8 pid, u16 offset, u8 anddata, u8 ordata)
{
	return pcr_and_then_or(pid, offset, sizeof(u8), anddata, ordata);
}
