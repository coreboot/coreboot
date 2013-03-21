/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009-2010 iWave Systems
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/cache.h>
#include <arch/cpu.h>
#include <console/console.h>
#if 0
#include "ram/ramtest.c"
#include "southbridge/intel/sch/early_smbus.c"
#endif

#define RFID_TEST 0

#if RFID_TEST
#define RFID_ADDR 0xA0
#define RFID_SELECT_CARD_COMMAND 0x01
#define SELECT_COMMAND_LENGTH 0x01

#define SMBUS_BASE_ADDRESS 0x400

static u32 sch_SMbase_read(void)
{
	u32 SMBusBase;

	/* SMBus address */
	SMBusBase = pci_read_config32(PCI_DEV(0, 0x1f, 0), 0x40);
	SMBusBase &= 0xFFFF;
	printk(BIOS_DEBUG, "SMBus base = %x\r\n", SMBusBase);
	return SMBusBase;
}

static void sch_SMbase_init(void)
{
	u32 SMBusBase;

	SMBusBase = sch_SMbase_read();
	outb(0x3F, SMBusBase + SMBCLKDIV);
}

static void sch_SMbus_regs(void)
{
	u32 SMBusBase;

	SMBusBase = sch_SMbase_read();
	printk(BIOS_DEBUG, "SMBHSTCNT. =%x\r\n", inb(SMBusBase + SMBHSTCNT));
	printk(BIOS_DEBUG, "SMBHSTSTS. =%x\r\n", inb(SMBusBase + SMBHSTSTS));
	printk(BIOS_DEBUG, "SMBCLKDIV. =%x\r\n", inb(SMBusBase + SMBCLKDIV));

	printk(BIOS_DEBUG, "SMBHSTADD. =%x\r\n", inb(SMBusBase + SMBHSTADD));
	printk(BIOS_DEBUG, "SMBHSTCMD. =%x\r\n", inb(SMBusBase + SMBHSTCMD));
}

void smb_clear(void)
{
	u32 SMBusBase;

	SMBusBase = sch_SMbase_read();
	outb(0x00, SMBusBase + SMBHSTCNT);
	outb(0x07, SMBusBase + SMBHSTSTS);
}

void data_clear(void)
{
	u32 SMBusBase;

	SMBusBase = sch_SMbase_read();
	outb(0x00, SMBusBase + SMBHSTDAT0);
	outb(0x00, SMBusBase + SMBHSTCMD);
	outb(0x00, SMBusBase + SMBHSTDAT1);
	outb(0x00, SMBusBase + SMBHSTDATB);
	outb(0x00, SMBusBase + (SMBHSTDATB + 0x1));
	outb(0x00, SMBusBase + (SMBHSTDATB + 0x2));
	outb(0x00, SMBusBase + (SMBHSTDATB + 0x3));
	outb(0x00, SMBusBase + (SMBHSTDATB + 0x4));
	outb(0x00, SMBusBase + (SMBHSTDATB + 0x5));
	outb(0x00, SMBusBase + (SMBHSTDATB + 0x6));
}

void transaction1(unsigned char dev_addr)
{
	int temp, a;
	u32 SMBusBase;

	SMBusBase = sch_SMbase_read();
	printk(BIOS_DEBUG, "Transaction 1");
	//clear the control and status registers
	smb_clear();
	//clear the data register
	data_clear();
	//program TSA register
	outb(dev_addr, SMBusBase + SMBHSTADD);
	//program command register
	outb(0x04, SMBusBase + SMBHSTCMD);
	//write data register
	outb(0x04, SMBusBase + SMBHSTDAT0);
	outb(0x04, SMBusBase + SMBHSTDATB);

	outb(0x09, SMBusBase + (SMBHSTDATB + 0x1));
	outb(0x11, SMBusBase + (SMBHSTDATB + 0x2));
	outb(0x22, SMBusBase + (SMBHSTDATB + 0x3));

	//set the control register
	outb(0x15, SMBusBase + SMBHSTCNT);
	//check the status register for busy state
	//sch_SMbus_regs ();
	temp = inb(SMBusBase + SMBHSTSTS);
	//printk(BIOS_DEBUG, "SMBus Busy.. status =%x\r\n",temp);
	//printk(BIOS_DEBUG, "SMBHSTSTS. =%x\r\n",inb(SMBusBase+SMBHSTSTS));
	do {
		temp = inb(SMBusBase + SMBHSTSTS);
		printk(BIOS_DEBUG, "SMBus Busy.. status =%x\r\n", temp);
		//sch_SMbus_regs ();
		printk(BIOS_DEBUG, "SMBHSTSTS. =%x\r\n",
		       inb(SMBusBase + SMBHSTSTS));
		if (temp > 0)
			break;
	} while (1);

	switch (temp) {
	case 1:
		printk(BIOS_DEBUG, "SMBus Success");
		break;
	default:
		printk(BIOS_DEBUG, "SMBus error %d", temp);
		break;

	}
	sch_SMbus_regs();
	printk(BIOS_DEBUG, "Command in TRansaction 1=%x\r\n\n",
	       inb(SMBusBase + SMBHSTCMD));
}

void transaction2(unsigned char dev_addr)
{
	int temp, a;
	u32 SMBusBase;

	SMBusBase = sch_SMbase_read();
	printk(BIOS_DEBUG, "Transaction 2");
	//clear the control and status registers
	smb_clear();
	//clear the data register
	data_clear();
	//program TSA register
	outb(dev_addr, SMBusBase + SMBHSTADD);
	//program command register
	outb(0x03, SMBusBase + SMBHSTCMD);
	//write data register
	outb(0x02, SMBusBase + SMBHSTDAT0);
	outb(0x03, SMBusBase + SMBHSTDATB);
	outb(0x09, SMBusBase + (SMBHSTDATB + 0x1));
	outb(0x15, SMBusBase + SMBHSTCNT);
	//check the status register for busy state
	//sch_SMbus_regs ();
	temp = inb(SMBusBase + SMBHSTSTS);
	//printk(BIOS_DEBUG, "SMBus Busy.. status =%x\r\n",temp);
	//printk(BIOS_DEBUG, "SMBHSTSTS. =%x\r\n",inb(SMBusBase+SMBHSTSTS));
	do {
		temp = inb(SMBusBase + SMBHSTSTS);
		printk(BIOS_DEBUG, "SMBus Busy.. status =%x\r\n", temp);
		//sch_SMbus_regs ();
		printk(BIOS_DEBUG, "SMBHSTSTS. =%x\r\n",
		       inb(SMBusBase + SMBHSTSTS));
		if (temp > 0)
			break;
	} while (1);

	switch (temp) {
	case 1:
		printk(BIOS_DEBUG, "SMBus Success");
		break;
	default:
		printk(BIOS_DEBUG, "SMBus error %d", temp);
		break;

	}
	sch_SMbus_regs();

	printk(BIOS_DEBUG, "Command in TRansaction 2=%x\r\n\n",
	       inb(SMBusBase + SMBHSTCMD));
}

void transaction3(unsigned char dev_addr)
{
	int temp, index, length;
	u32 SMBusBase;

	SMBusBase = sch_SMbase_read();
	printk(BIOS_DEBUG, "smb_read_multiple_bytes");
	smb_clear();
	data_clear();
	outb(dev_addr, SMBusBase + SMBHSTADD);
	outb(0x03, SMBusBase + SMBHSTCMD);
	outb(0x11, SMBusBase + SMBHSTCNT);

	//data_clear();
	outb(dev_addr + 1, SMBusBase + SMBHSTADD);

	outb(0x15, SMBusBase + SMBHSTCNT);

	// sch_SMbus_regs ();
	//check the status register for busy state
	//temp=inb(SMBusBase+SMBHSTSTS);
	//printk(BIOS_DEBUG, "SMBus Busy.. status =%x\r\n",temp);
	//sch_SMbus_regs ();
	//printk(BIOS_DEBUG, "SMBHSTSTS. =%x\r\n",inb(SMBusBase+SMBHSTSTS));
	do {
		temp = inb(SMBusBase + SMBHSTSTS);
		printk(BIOS_DEBUG, "SMBHSTSTS. =%x\r\n",
		       inb(SMBusBase + SMBHSTSTS));
		//sch_SMbus_regs ();
		if (temp > 0)
			break;
	} while (1);

	switch (temp) {
	case 1:
		printk(BIOS_DEBUG, "SMBus Success\n");
		break;
	default:
		printk(BIOS_DEBUG, "SMBus error %d", temp);
		break;

	}

	sch_SMbus_regs();
	printk(BIOS_DEBUG, "ADDRESS is.. %x\r\n", inb(SMBusBase + SMBHSTADD));
	length = inb(SMBusBase + SMBHSTDAT0);

	printk(BIOS_DEBUG, "Length is.. %x\r\n", inb(SMBusBase + SMBHSTDAT0));

	printk(BIOS_DEBUG, "Command is... %x\r\n", inb(SMBusBase + SMBHSTDATB));
	printk(BIOS_DEBUG, "Status .. %x\r\n", inb(SMBusBase + SMBHSTDATB + 1));
	for (index = 0; index < length; index++)
		printk(BIOS_DEBUG, "Serial Byte[%x]..%x\r\n", index,
		       inb(SMBusBase + SMBHSTDATB + index));
}

int selectcard(void)
{
	int i;

	printk(BIOS_DEBUG, "%s", "\r\nCase 9....... \n\r");
	// send the length byte and command code through RFID interface

	transaction1(RFID_ADDR);
	transaction2(RFID_ADDR);
	transaction3(RFID_ADDR);
	return (1);
}
#endif

#include "northbridge/intel/sch/early_init.c"
#include "northbridge/intel/sch/raminit.h"
#include "northbridge/intel/sch/raminit.c"

static void sch_enable_lpc(void)
{
	/* Initialize the FWH decode/Enable registers according to platform design */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xD0, 0x00112233);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xD4, 0xC0000000);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x60, 0x808A8B8B);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x64, 0x8F898F89);
}

static void sch_shadow_CMC(void)
{
	u32 reg32;

	/* FIXME: proper dest, proper src, and wbinvd, too */
	memcpy((void *)CMC_SHADOW, (void *)0xfffd0000, 64 * 1024);
	// __asm__ volatile ("wbinvd \n"
	//);
	printk(BIOS_DEBUG, "copy done ");
	memcpy((void *)0x3f5f0000, (void *)0x3faf0000, 64 * 1024);
	printk(BIOS_DEBUG, "copy 2 done ");
	reg32 = cpuid_eax(0x00000001);
	printk(BIOS_INFO, "CPU ID: %d.\n", reg32);

	reg32 = cpuid_eax(0x80000008);
	printk(BIOS_INFO, "Physical Address size: %d.\n", (reg32 & 0xFF));
	printk(BIOS_INFO, "Virtual Address size: %d.\n",
	       ((reg32 & 0xFF00) >> 8));
	sch_port_access_write_ram_cmd(0xB8, 4, 0, 0x3faf0000);
	printk(BIOS_DEBUG, "1 ");
	sch_port_access_write_ram_cmd(0xBA, 4, 0, reg32);
	printk(BIOS_DEBUG, "2 ");
}

static void poulsbo_setup_Stage1Regs(void)
{
	u32 reg32;

	printk(BIOS_DEBUG, "E000/F000 Routing ");
	reg32 = sch_port_access_read(2, 3, 4);
	sch_port_access_write(2, 3, 4, (reg32 | 0x6));
}

static void poulsbo_setup_Stage2Regs(void)
{
	u16 reg16;

	printk(BIOS_DEBUG, "Reserved");
	reg16 = pci_read_config16(PCI_DEV(0, 0x2, 0), 0x62);
	pci_write_config16(PCI_DEV(0, 0x2, 0), 0x62, (reg16 | 0x3));
	/* Slot capabilities */
	pci_write_config32(PCI_DEV(0, 28, 0), 0x54, 0x80500);
	pci_write_config32(PCI_DEV(0, 28, 1), 0x54, 0x100500);
	/* FIXME: CPU ID identification */
	printk(BIOS_DEBUG, " done.\n");
}

void main(unsigned long bist)
{
	int boot_mode = 0;

	if (bist == 0)
		enable_lapic();

	sch_enable_lpc();
	console_init();

	/* Halt if there was a built in self test failure */
	// report_bist_failure(bist);
	// outl (0x00, 0x1088);

	/*
	 * Perform some early chipset initialization required
	 * before RAM initialization can work.
	 */
	sch_early_initialization();
	sdram_initialize(boot_mode);

	sch_shadow_CMC();
	poulsbo_setup_Stage1Regs();
	poulsbo_setup_Stage2Regs();
#if 0
	sch_SMbase_init();

	/* Perform some initialization that must run before stage2. */
#endif

	/*
	 * This should probably go away. Until now it is required
	 * and mainboard specific.
	 */

	/* Chipset Errata! */
	pci_write_config16(PCI_DEV(0, 0x2, 0), GGC, 0x20);
	pci_write_config32(PCI_DEV(0, 0x2, 0), 0xc4, 0x00000002);
	pci_write_config32(PCI_DEV(0, 0x2, 0), 0xe0, 0x00008000);
	pci_write_config32(PCI_DEV(0, 0x2, 0), 0xf0, 0x00000005);
	pci_write_config16(PCI_DEV(0, 0x2, 0), 0xf7, 0x80);
	pci_write_config16(PCI_DEV(0, 0x2, 0), 0x4, 0x7);

#if RFID_TEST
	sch_SMbase_init();
	selectcard();
#endif
}
