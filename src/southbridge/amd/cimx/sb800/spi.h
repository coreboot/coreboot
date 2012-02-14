/*
 *****************************************************************************
 *
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
 * ***************************************************************************
 *
 */

#ifndef _SB800_CIMX_SPI_H_
#define _SB800_CIMX_SPI_H_

void executeCommand(volatile u8 * spi_address);
void wait4CommandComplete(volatile u8 * spi_address);
void resetInternalFIFOPointer(volatile u8 * spi_address);
u8 readSPIStatus(volatile u8 * spi_address);
void wait4FlashPartReady(volatile u8 * spi_address);
void writeSPIStatus(volatile u8 * spi_address, u8 status);
void readSPIID(volatile u8 * spi_address);
void SPIWriteEnable(volatile u8 * spi_address);
void sectorEraseSPI(volatile u8 * spi_address, u32 address);
void chipEraseSPI(volatile u8 * spi_address);
void byteProgram(volatile u8 * spi_address, u32 address, u32 data);
void dwordnoneAAIProgram(volatile u8 * spi_address, u32 address, u32 data);
void dwordProgram(volatile u8 * spi_address, u32 address, u32 data);
void directByteProgram(volatile u8 * spi_address, volatile u32 * address, u32 data);

#endif
