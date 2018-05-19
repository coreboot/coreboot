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
 */

#ifndef SOUTHBRIDGE_VIA_VT8237R_VT8237R_H
#define SOUTHBRIDGE_VIA_VT8237R_VT8237R_H

#include <compiler.h>

/* Static resources for the VT8237R southbridge */

#define VT8237R_APIC_ID			0x2
#define VT8237R_ACPI_IO_BASE		0x500
#define DEFAULT_PMBASE			VT8237R_ACPI_IO_BASE
#define VT8237R_SMBUS_IO_BASE		0x400
/* 0x0 disabled, 0x2 reserved, 0xf = IRQ15 */
#define VT8237R_ACPI_IRQ		0x9
#define VT8237S_SPI_MEM_BASE		0xfed02000UL

/* PMBASE FIXME mostly taken from ich7 */
#define PM1_STS		0x00
#define   WAK_STS	(1 << 15)
#define   PCIEXPWAK_STS	(1 << 14)
#define   PRBTNOR_STS	(1 << 11)
#define   RTC_STS	(1 << 10)
#define   PWRBTN_STS	(1 << 8)
#define   GBL_STS	(1 << 5)
#define   BM_STS	(1 << 4)
#define   TMROF_STS	(1 << 0)
#define PM1_EN		0x02
#define   PCIEXPWAK_DIS	(1 << 14)
#define   RTC_EN	(1 << 10)
#define   PWRBTN_EN	(1 << 8)
#define   GBL_EN	(1 << 5)
#define   TMROF_EN	(1 << 0)
#define PM1_CNT		0x04
#define   SLP_EN	(1 << 13)
#define   SLP_TYP	(7 << 10)
#define   GBL_RLS	(1 << 2)
#define   BM_RLD	(1 << 1)
#define   SCI_EN	(1 << 0)
#define PM1_TMR		0x08
#define PROC_CNT	0x10
#define LV2		0x14
#define LV3		0x15
#define SMI_STS		0x28
#define SMI_EN		0x2d
#define EOS		(1 << 0)

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

#if IS_ENABLED(CONFIG_DEBUG_SMBUS)
#define PRINT_DEBUG(x)		printk(BIOS_DEBUG, x)
#define PRINT_DEBUG_HEX16(x)	printk(BIOS_DEBUG, "%04x", x)
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
} __packed;

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif


#ifdef __PRE_RAM__
u8 smbus_read_byte(u8 dimm, u8 offset);
void smbus_write_byte(u8 dimm, u8 offset, u8 data);
void enable_smbus(void);
void smbus_fixup(const u8 channel0[], int ram_slots);
void vt8237_sb_enable_fid_vid(void);
void enable_rom_decode(void);
void vt8237_early_spi_init(void);
int vt8237_early_network_init(struct vt8237_network_rom *rom);
#else
void writeback(struct device *dev, u16 where, u8 what);
void dump_south(struct device *dev);
u32 vt8237_ide_80pin_detect(struct device *dev);
#endif

#endif
