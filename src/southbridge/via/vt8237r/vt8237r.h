/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

#ifndef SOUTHBRIDGE_VIA_VT8237R_VT8237R_H
#define SOUTHBRIDGE_VIA_VT8237R_VT8237R_H

#include <stdint.h>

/* Static resources for the VT8237R southbridge */

#define VT8237R_APIC_ID			0x2
#define VT8237R_ACPI_IO_BASE		0x500
#define VT8237R_SMBUS_IO_BASE		0x400
/* 0x0 disabled, 0x2 reserved, 0xf = IRQ15 */
#define VT8237R_ACPI_IRQ		0x9
#if defined(__GNUC__)
#define VT8237S_SPI_MEM_BASE		0xfed02000ULL
#else
#define VT8237S_SPI_MEM_BASE		0xfed02000UL
#endif
#define VT8237R_HPET_ADDR		0xfed00000ULL

/* IDE */
#define IDE_CS				0x40
#define IDE_CONF_I			0x41
#define IDE_CONF_II			0x42
#define IDE_CONF_FIFO			0x43
#define IDE_MISC_I			0x44
#define IDE_MISC_II			0x45
#define IDE_UDMA			0x50

#define VT8237R_IDE0_80PIN_CABLE ((1UL<<28)|(1UL<<20))
#define VT8237R_IDE1_80PIN_CABLE ((1UL<<12)|(1UL<< 4))
#define VT8237R_IDE_CABLESEL_MASK (VT8237R_IDE0_80PIN_CABLE|VT8237R_IDE1_80PIN_CABLE)

/* SMBus */
#define VT8237R_PSON			0x82
#define VT8237R_POWER_WELL		0x94
#define VT8237R_SMBUS_IO_BASE_REG	0xd0
#define VT8237R_SMBUS_HOST_CONF		0xd2

#define SMBHSTSTAT			(VT8237R_SMBUS_IO_BASE + 0x0)
#define SMBSLVSTAT			(VT8237R_SMBUS_IO_BASE + 0x1)
#define SMBHSTCTL			(VT8237R_SMBUS_IO_BASE + 0x2)
#define SMBHSTCMD			(VT8237R_SMBUS_IO_BASE + 0x3)
#define SMBXMITADD			(VT8237R_SMBUS_IO_BASE + 0x4)
#define SMBHSTDAT0			(VT8237R_SMBUS_IO_BASE + 0x5)

#define HOST_RESET 			0xff
/* 1 in the 0 bit of SMBHSTADD states to READ. */
#define READ_CMD			0x01
#define SMBUS_TIMEOUT			(100 * 1000 * 10)
#define I2C_TRANS_CMD			0x40
#define CLOCK_SLAVE_ADDRESS		0x69

#if CONFIG_DEBUG_SMBUS
#define PRINT_DEBUG(x)		print_debug(x)
#define PRINT_DEBUG_HEX16(x)	print_debug_hex16(x)
#else
#define PRINT_DEBUG(x)
#define PRINT_DEBUG_HEX16(x)
#endif

#define SMBUS_DELAY() inb(0x80)

struct vt8237_network_rom {
	u8 mac_address[6];
	u8 phy_addr;
	u8 res1;
	u16 sub_sid;
	u16 sub_vid;
	u16 pid;
	u16 vid;
	u8 pmcc;
	u8 data_sel;
	u8 pmu_data_reg;
	u8 aux_curr;
	u16 reserved;
	u8 min_gnt;
	u8 max_lat;
	u8 bcr0;
	u8 bcr1;
	u8 cfg_a;
	u8 cfg_b;
	u8 cfg_c;
	u8 cfg_d;
	u8 checksum;
}
#if defined(__GNUC__)
__attribute__ ((packed))
#endif
;

#ifdef __PRE_RAM__
#ifndef __ROMCC__
u8 smbus_read_byte(u8 dimm, u8 offset);
void enable_smbus(void);
void smbus_fixup(const struct mem_controller *ctrl);
// these are in vt8237_early_smbus.c - do they really belong there?
void vt8237_sb_enable_fid_vid(void);
void enable_rom_decode(void);
void vt8237_early_spi_init(void);
int vt8237_early_network_init(struct vt8237_network_rom *rom);
#endif
#else
#include <device/device.h>
void writeback(struct device *dev, u16 where, u8 what);
void dump_south(device_t dev);
u32 vt8237_ide_80pin_detect(struct device *dev);
#endif

#endif
