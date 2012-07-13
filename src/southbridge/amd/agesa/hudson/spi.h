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

#ifndef _HUDSON_CIMX_SPI_H_
#define _HUDSON_CIMX_SPI_H_

void execute_command(volatile u8 * spi_address);
void wait4command_complete(volatile u8 * spi_address);
void reset_internal_fifo_pointer(volatile u8 * spi_address);
u8 read_spi_status(volatile u8 * spi_address);
void wait4flashpart_ready(volatile u8 * spi_address);
void write_spi_status(volatile u8 * spi_address, u8 status);
void read_spi_id(volatile u8 * spi_address);
void spi_write_enable(volatile u8 * spi_address);
void spi_write_disable(volatile u8 * spi_address);
void sector_erase_spi(volatile u8 * spi_address, u32 address);
void chip_erase_spi(volatile u8 * spi_address);
void byte_program(volatile u8 * spi_address, u32 address, u32 data);
void dword_noneAAI_program(volatile u8 * spi_address, u32 address, u32 data);
void dword_program(volatile u8 * spi_address, u32 address, u32 data);
void direct_byte_program(volatile u8 * spi_address, volatile u32 * address, u32 data);

#endif
