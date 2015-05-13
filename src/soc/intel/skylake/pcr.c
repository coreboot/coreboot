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

/*
 * Read PCR register. (This is internal function)
 * It returns PCR register and size in 1/2/4 bytes.
 * The offset should not exceed 0xFFFF and must be aligned with size
 *
 * PCH_SBI_PID defines as 8 bit Port ID that will be used when sending
 * transaction to sideband.
 */
static u8 pch_pcr_read(PCH_SBI_PID pid, u16 offset, u32 size, void *data)
{
	if ((offset & (size - 1)) != 0) {
		printk(BIOS_DEBUG,
			"PchPcrRead error. Invalid Offset: %x Size: %x",
			offset, size);
		return -1;
	}
	switch (size) {
	case 4:
		*(u32 *) data = read32(PCH_PCR_ADDRESS(pid, offset));
		break;
	case 2:
		*(u16 *) data = read16(PCH_PCR_ADDRESS(pid, offset));
		break;
	case 1:
		*(u8 *) data = read8(PCH_PCR_ADDRESS(pid, offset));
		break;
	default:
		break;
	}
	return 0;
}

u8 pcr_read32(PCH_SBI_PID pid, u16 offset, u32 *outdata)
{
	return pch_pcr_read(pid, offset, sizeof(u32), (u32 *)outdata);
}

u8 pcr_read16(PCH_SBI_PID pid, u16 offset, u16 *outdata)
{
	return pch_pcr_read(pid, offset, sizeof(u16), (u32 *)outdata);
}

u8 pcr_read8(PCH_SBI_PID pid, u16 offset, u8 *outdata)
{
	return pch_pcr_read(pid, offset, sizeof(u8), (u32 *)outdata);
}

/*
 * Write PCR register. (This is internal function)
 * It returns PCR register and size in 1/2/4 bytes.
 * The offset should not exceed 0xFFFF and must be aligned with size
 *
 * PCH_SBI_PID defines as 8 bit Port ID that will be used when sending
 * transaction to sideband.
 */
static u8 pch_pcr_write(PCH_SBI_PID pid, u16 offset, u32 size, u32 data)
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
		write32(PCH_PCR_ADDRESS(pid, offset), (u32) data);
		break;
	case 2:
		write16(PCH_PCR_ADDRESS(pid, offset), (u16) data);
		break;
	case 1:
		write8(PCH_PCR_ADDRESS(pid, offset), (u8) data);
		break;
	default:
		break;
	}
	read32(PCH_PCR_ADDRESS(PID_LPC, R_PCH_PCR_LPC_GCFD));

	return 0;
}

u8 pcr_write32(PCH_SBI_PID pid, u16 offset, u32 indata)
{
	return pch_pcr_write(pid, offset, sizeof(u32), indata);
}

u8 pcr_write16(PCH_SBI_PID pid, u16 offset, u16 indata)
{
	return pch_pcr_write(pid, offset, sizeof(u16), indata);
}

u8 pcr_write8(PCH_SBI_PID pid, u16 offset, u8 indata)
{
	return pch_pcr_write(pid, offset, sizeof(u8), indata);
}

/*
 * Write PCR register. (This is internal function)
 * It programs  PCR register and size in 1/2/4 bytes.
 * The offset should not exceed 0xFFFF and must be aligned with size
 *
 * PCH_SBI_PID defines as 8 bit Port ID that will be used when sending
 * transaction to sideband.
 */
static u8 pcr_and_then_or(PCH_SBI_PID pid, u16 offset, u32 size, u32 anddata,
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

u8 pcr_andthenor32(PCH_SBI_PID pid, u16 offset, u32 anddata, u32 ordata)
{
	return pcr_and_then_or(pid, offset, sizeof(u32), anddata, ordata);
}

u8 pcr_andthenor16(PCH_SBI_PID pid, u16 offset, u16 anddata, u16 ordata)
{
	return pcr_and_then_or(pid, offset, sizeof(u16), anddata, ordata);
}

u8 pcr_andthenor8(PCH_SBI_PID pid, u16 offset, u8 anddata, u8 ordata)
{
	return pcr_and_then_or(pid, offset, sizeof(u8), anddata, ordata);
}
