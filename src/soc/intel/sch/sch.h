/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2009-2010 iWave Systems
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SCH_POULSBO_H__
#define __SCH_POULSBO_H__

#if !defined(__ASSEMBLER__)
int sch_port_access_read(int port, int reg, int bytes);
void sch_port_access_write(int port, int reg, int bytes, long data);
void sch_port_access_write_ram_cmd(int cmd, int port, int reg, int data);
#endif

/* Southbridge IO BARs */
/* TODO Make sure these don't get changed by stage2 */
#define SCH_ENABLE_BIT                        (1<<31)
#define DEFAULT_ACPIPBLKBASE	              0x510

#define DEFAULT_SMBUSBASE	              0x540
#define DEFAULT_GPIOBASE		      0x588
#define DEFAULT_GPE0BASE	              0x5C0
#define DEFAULT_SMMCNTRLBASE	              0x3F703F76

#define DEFAULT_RCBABASE                      ((u8 *)0xfed1c000)

#define DEFAULT_PCIEXBAR		CONFIG_MMCONF_BASE_ADDRESS	/* 4 KB per PCIe device */

/* IGD */
#define GGC	0x52

/* Root Complex Register Block */
#define RCBA8(x) *((volatile u8 *)(DEFAULT_RCBABASE + x))
#define RCBA16(x) *((volatile u16 *)(DEFAULT_RCBABASE + x))
#define RCBA32(x) *((volatile u32 *)(DEFAULT_RCBABASE + x))

/* FIXME: should probably be in southbridge, but is setup in romstage, too */
#define CMC_SHADOW 0x3faf0000

#define DEFAULT_PMBASE		0x500

/* SMBus I/O bits. */
#define SMBHSTSTAT		0x0
#define SMBHSTCTL		0x2
#define SMBHSTCMD		0x3
#define SMBXMITADD		0x4
#define SMBHSTDAT0		0x5
#define SMBHSTDAT1		0x6
#define SMBBLKDAT		0x7
#define SMBTRNSADD		0x9
#define SMBSLVDATA		0xa
#define SMLINK_PIN_CTL		0xe
#define SMBUS_PIN_CTL		0xf

#define SMBUS_TIMEOUT		(10 * 1000 * 100)

#endif /* __SCH_POULSBO_H__ */
