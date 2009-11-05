/*
 * This software and ancillary information (herein called SOFTWARE )
 * called LinuxBIOS          is made available under the terms described
 * here.  The SOFTWARE has been approved for release with associated
 * LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
 * been authored by an employee or employees of the University of
 * California, operator of the Los Alamos National Laboratory under
 * Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
 * U.S. Government has rights to use, reproduce, and distribute this
 * SOFTWARE.  The public may copy, distribute, prepare derivative works
 * and publicly display this SOFTWARE without charge, provided that this
 * Notice and any statement of authorship are reproduced on all copies.
 * Neither the Government nor the University makes any warranty, express
 * or implied, or assumes any liability or responsibility for the use of
 * this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
 * such modified SOFTWARE should be clearly marked, so as not to confuse
 * it with the version available from LANL.
 */
 /*
 * This file is part of the coreboot project.
 *
 *  (c) Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 *  Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <string.h>
#ifdef CONFIG_COREBOOT_V2
#include <arch/io.h>
#include <console/console.h>
#else
#include <io.h>
#include <console.h>
#endif
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <x86emu/x86emu.h>
#include "x86emu/prim_ops.h"

#define DATA_SEGMENT 0x2000
#define STACK_SEGMENT 0x1000	//1000:xxxx
#define STACK_START_OFFSET 0xfffe
#define INITIAL_EBDA_SEGMENT 0xF600	// segment of the Extended BIOS Data Area
#define INITIAL_EBDA_SIZE 0x400	// size of the EBDA (at least 1KB!! since size is stored in KB!)

enum {
	PCI_BIOS_PRESENT	= 0xB101,
	FIND_PCI_DEVICE		= 0xB102,
	FIND_PCI_CLASS_CODE	= 0xB103,
	GENERATE_SPECIAL_CYCLE	= 0xB106,
	READ_CONFIG_BYTE	= 0xB108,
	READ_CONFIG_WORD	= 0xB109,
	READ_CONFIG_DWORD	= 0xB10A,
	WRITE_CONFIG_BYTE	= 0xB10B,
	WRITE_CONFIG_WORD	= 0xB10C,
	WRITE_CONFIG_DWORD	= 0xB10D,
	GET_IRQ_ROUTING_OPTIONS	= 0xB10E,
	SET_PCI_IRQ		= 0xB10F
};

enum {
	SUCCESSFUL		= 0x00,
	FUNC_NOT_SUPPORTED	= 0x81,
	BAD_VENDOR_ID		= 0x83,
	DEVICE_NOT_FOUND	= 0x86,
	BAD_REGISTER_NUMBER	= 0x87,
	SET_FAILED		= 0x88,
	BUFFER_TOO_SMALL	= 0x89
};

#define MEM_WB(where, what) wrb(where, what)
#define MEM_WW(where, what) wrw(where, what)
#define MEM_WL(where, what) wrl(where, what)

#define MEM_RB(where) rdb(where)
#define MEM_RW(where) rdw(where)
#define MEM_RL(where) rdl(where)

static u8 biosemu_inb(u16 port)
{
	u8 val;

	val = inb(port);
#ifdef CONFIG_DEBUG
	if (port != 0x40)
	    printk("inb(0x%04x) = 0x%02x\n", port, val);
#endif

	return val;
}

static u16 biosemu_inw(u16 port)
{
	u16 val;

	val = inw(port);

#ifdef CONFIG_DEBUG
	printk("inw(0x%04x) = 0x%04x\n", port, val);
#endif
	return val;
}

static u32 biosemu_inl(u16 port)
{
	u32 val;

	val = inl(port);

#ifdef CONFIG_DEBUG
	printk("inl(0x%04x) = 0x%08x\n", port, val);
#endif
	return val;
}

static void biosemu_outb(u16 port, u8 val)
{
#ifdef CONFIG_DEBUG
	if (port != 0x43)
		printk("outb(0x%02x, 0x%04x)\n", val, port);
#endif
	outb(val, port);
}

static void biosemu_outw(u16 port, u16 val)
{
#ifdef CONFIG_DEBUG
	printk("outw(0x%04x, 0x%04x)\n", val, port);
#endif
	outw(val, port);
}

static void biosemu_outl(u16 port, u32 val)
{
#ifdef CONFIG_DEBUG
	printk("outl(0x%08x, 0x%04x)\n", val, port);
#endif
	outl(val, port);
}

static X86EMU_pioFuncs biosemu_piofuncs = {
	biosemu_inb,  biosemu_inw,  biosemu_inl,
	biosemu_outb, biosemu_outw, biosemu_outl
};

/* Interrupt Handlers */

static int int15_handler(void)
{
	/* This int15 handler is VIA Tech. and Intel specific. Other chipsets need other
	 * handlers. The right way to do this is to move this handler code into
	 * the mainboard or northbridge code.
	 */
	switch (X86_AX) {
	case 0x5f19:
		X86_EFLAGS |= FB_CF;	/* set carry flag */
		break;
	case 0x5f18:
		X86_EAX = 0x5f;
		// MCLK = 133, 32M frame buffer, 256 M main memory
		X86_EBX = 0x545;
		X86_ECX = 0x060;
		X86_EFLAGS &= ~FB_CF;
		break;
	case 0x5f00:
		X86_EAX = 0x8600;
		X86_EFLAGS |= FB_CF;	/* set carry flag */
		break;
	case 0x5f01:
		X86_EAX = 0x5f;
		X86_ECX = (X86_ECX & 0xffffff00 ) | 2; // panel type =  2 = 1024 * 768
		X86_EFLAGS &= ~FB_CF;
		break;
	case 0x5f02:
		X86_EAX = 0x5f;
		X86_EBX = (X86_EBX & 0xffff0000) | 2;
		X86_ECX = (X86_ECX & 0xffff0000) | 0x401;  // PAL + crt only
		X86_EDX = (X86_EDX & 0xffff0000) | 0;  // TV Layout - default
		X86_EFLAGS &= ~FB_CF;
		break;
	case 0x5f0f:
		X86_EAX = 0x860f;
		X86_EFLAGS |= FB_CF;	/* set carry flag */
		break;
	/* And now Intel IGD code */
#define BOOT_DISPLAY_DEFAULT    0
#define BOOT_DISPLAY_CRT        (1 << 0)
#define BOOT_DISPLAY_TV         (1 << 1)
#define BOOT_DISPLAY_EFP        (1 << 2)
#define BOOT_DISPLAY_LCD        (1 << 3)
#define BOOT_DISPLAY_CRT2       (1 << 4)
#define BOOT_DISPLAY_TV2        (1 << 5)
#define BOOT_DISPLAY_EFP2       (1 << 6)
#define BOOT_DISPLAY_LCD2       (1 << 7)

	case 0x5f35:
		X86_EAX = 0x5f;
		X86_ECX = BOOT_DISPLAY_DEFAULT;
		X86_EFLAGS &= ~FB_CF;
		break;
	case 0x5f40:
		X86_EAX = 0x5f;
		X86_ECX = 3; // This is mainboard specific
		printk("DISPLAY=%x\n", X86_ECX);
		X86_EFLAGS &= ~FB_CF;
		break;
	default:
		printk("Unknown INT15 function %04x!\n", X86_AX);
		X86_EFLAGS |= FB_CF;	/* set carry flag */
	}

	return 1;
}

static int int1a_handler(void)
{
	int ret = 0;
	struct device *dev = 0;

	switch (X86_AX) {
	case PCI_BIOS_PRESENT:
		X86_AH	= 0x00;		/* no config space/special cycle support */
		X86_AL	= 0x01;		/* config mechanism 1 */
		X86_EDX = 'P' | 'C' << 8 | 'I' << 16 | ' ' << 24;
		X86_EBX = 0x0210;	/* Version 2.10 */
		X86_ECX = 0xFF00;	/* FIXME: Max bus number */
		X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
		ret = 1;
		break;
	case FIND_PCI_DEVICE:
		/* FIXME: support SI != 0 */
#ifdef CONFIG_COREBOOT_V2
		dev = dev_find_device(X86_DX, X86_CX, dev);
#else
		dev = dev_find_pci_device(X86_DX, X86_CX, dev);
#endif
		if (dev != 0) {
			X86_BH = dev->bus->secondary;
			X86_BL = dev->path.pci.devfn;
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */
			ret = 0;
		}
		break;
	case FIND_PCI_CLASS_CODE:
		/* FixME: support SI != 0 */
		dev = dev_find_class(X86_ECX, dev);
		if (dev != 0) {
			X86_BH = dev->bus->secondary;
			X86_BL = dev->path.pci.devfn;
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */
			ret = 0;
		}
		break;
	case READ_CONFIG_BYTE:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			X86_CL = pci_read_config8(dev, X86_DI);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case READ_CONFIG_WORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			X86_CX = pci_read_config16(dev, X86_DI);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case READ_CONFIG_DWORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			X86_ECX = pci_read_config32(dev, X86_DI);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case WRITE_CONFIG_BYTE:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			pci_write_config8(dev, X86_DI, X86_CL);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case WRITE_CONFIG_WORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			pci_write_config16(dev, X86_DI, X86_CX);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case WRITE_CONFIG_DWORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			pci_write_config16(dev, X86_DI, X86_ECX);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	default:
		X86_AH = FUNC_NOT_SUPPORTED;
		X86_EFLAGS |= FB_CF; 
		break;
	}

	return ret;
}

/* Interrupt multiplexer */

/* Find base address of interrupt handler */
static u32 getIntVect(int num)
{
	return MEM_RW(num << 2) + (MEM_RW((num << 2) + 2) << 4);
}

static int run_bios_int(int num)
{
	u32 eflags;

	eflags = X86_EFLAGS;
	push_word(eflags);
	push_word(X86_CS);
	push_word(X86_IP);
	X86_CS = MEM_RW((num << 2) + 2);
	X86_IP = MEM_RW(num << 2);

	return 1;
}

static void do_int(int num)
{
	int ret = 0;

	printk("int%x (AX=%04x) vector at %x\n", num, X86_AX, getIntVect(num));

	switch (num) {
	case 0x10:
	case 0x42:
	case 0x6D:
		if (getIntVect(num) == 0x0000) {
			printk("uninitialized interrupt vector\n");
			ret = 1;
		}
		if (getIntVect(num) == 0xFF065) {
			//ret = int42_handler();
			ret = 1;
		}
		break;
	case 0x15:
		ret = int15_handler();
		ret = 1;
		break;
	case 0x16:
		//ret = int16_handler();
		ret = 0;
		break;
	case 0x1A:
		ret = int1a_handler();
		ret = 1;
		break;
	case 0xe6:
		//ret = intE6_handler();
		ret = 0;
		break;
	default:
		break;
	}

	if (!ret)
		ret = run_bios_int(num);

}

/*
 * here we are really paranoid about faking a "real"
 * BIOS. Most of this information was pulled from
 * dosemu.
 */
static void setup_system_bios(void)
{
	int i;

	/* Set up Interrupt Vectors. The IVT starts at 0x0000:0x0000
	 * Additionally, we put some stub code into the F segment for
	 * those pesky little buggers that jmp to the hard coded addresses
	 * instead of calling int XX. This stub code looks like this
	 *
	 *  CD XX	int 0xXX
	 *  C3		ret
	 *  F4		hlt
	 */

	/* int 05 default location (Bound Exceeded) */
	MEM_WL(0x05 << 2, 0xf000ff54);
	MEM_WL(0xfff54, 0xf4c305cd);
	/* int 08 default location (Double Fault) */
	MEM_WL(0x08 << 2, 0xf000fea5);
	MEM_WL(0xffea5, 0xf4c308cd);
	/* int 0E default location (Page Fault) */
	MEM_WL(0x0e << 2, 0xf000ef57);
	MEM_WL(0xfef57, 0xf4c30ecd);
	/* int 10 default location */
	MEM_WL(0x10 << 2, 0xf000f065);
	MEM_WL(0xff065, 0xf4c310cd);
	/* int 11 default location (Get Equipment Configuration) */
	MEM_WL(0x11 << 2, 0xf000f84d);
	MEM_WL(0xff84d, 0xf4c311cd);
	/* int 12 default location (Get Conventional Memory Size) */
	MEM_WL(0x12 << 2, 0xf000f841);
	MEM_WL(0xff841, 0xf4c312cd);
	/* int 13 default location (Disk) */
	MEM_WL(0x13 << 2, 0xf000ec59);
	MEM_WL(0xfec59, 0xf4c313cd);
	/* int 14 default location (Disk) */
	MEM_WL(0x14 << 2, 0xf000e739);
	MEM_WL(0xfe739, 0xf4c314cd);
	/* int 15 default location (I/O System Extensions) */
	MEM_WL(0x15 << 2, 0xf000f859);
	MEM_WL(0xf859, 0xf4c315cd);
	/* int 16 default location */
	MEM_WL(0x16 << 2, 0xf000e82e);
	MEM_WL(0xfe82e, 0xf4c316cd);
	/* int 17 default location (Parallel Port) */
	MEM_WL(0x17 << 2, 0xf000efd2);
	MEM_WL(0xfefd2, 0xf4c317cd);
	/* int 1A default location (RTC, PCI and others) */
	MEM_WL(0x1a << 2, 0xf000fe6e);
	MEM_WL(0xffe6e, 0xf4c31acd);
	/* int 1E default location (FDD table) */
	MEM_WL(0x1e << 2, 0xf000efc7);
	MEM_WL(0xfefc7, 0xf4c31ecd);
	/* font tables default location (int 1F) */
	MEM_WL(0x1f << 2, 0xf000fa6e);
	MEM_WL(0xffa6e, 0xf4c31fcd);
	/* int 42 default location */
	MEM_WL(0x42 << 2, 0xf000f065);
	/* int 6D default location */
	MEM_WL(0x6D << 2, 0xf000f065);

	/* Clear EBDA */
	for (i=(INITIAL_EBDA_SEGMENT << 4); 
		i<(INITIAL_EBDA_SEGMENT << 4) + INITIAL_EBDA_SIZE; i++)
		MEM_WB(i, 0);
	/* at offset 0h in EBDA is the size of the EBDA in KB */
	MEM_WW((INITIAL_EBDA_SEGMENT << 4) + 0x0, INITIAL_EBDA_SIZE / 1024);

	/* Clear BDA */
	for (i=0x400; i<0x500; i+=4)
		MEM_WL(i, 0);

	/* Set up EBDA */
	MEM_WW(0x40e, INITIAL_EBDA_SEGMENT);

	/* Set RAM size to 16MB (fake) */
	MEM_WW(0x413, 16384);

	// TODO Set up more of BDA here

	/* setup original ROM BIOS Area (F000:xxxx) */
	const char *date = "06/23/99";
	for (i = 0; date[i]; i++)
		MEM_WB(0xffff5 + i, date[i]);
	/* set up eisa ident string */
        const char *ident = "PCI_ISA";
        for (i = 0; ident[i]; i++)
                MEM_WB(0xfffd9 + i, ident[i]);

        // write system model id for IBM-AT
        // according to "Ralf Browns Interrupt List" Int15 AH=C0 Table 515,
        // model FC is the original AT and also used in all DOSEMU Versions.
        MEM_WB(0xFFFFE, 0xfc);
}

#define BIOSEMU_MEM_BASE 0x00000000
#define BIOSEMU_MEM_SIZE 0x00100000
void run_bios(struct device * dev, unsigned long addr)
{
	int i;
	u16 initialcs = (addr & 0xF0000) >> 4;
	u16 initialip = (addr + 3) & 0xFFFF;
	u16 devfn = (dev->bus->secondary << 8) | dev->path.pci.devfn;
	X86EMU_intrFuncs intFuncs[256];

	X86EMU_setMemBase(BIOSEMU_MEM_BASE, BIOSEMU_MEM_SIZE);
	X86EMU_setupPioFuncs(&biosemu_piofuncs);
	for (i = 0; i < 256; i++)
		intFuncs[i] = do_int;
	X86EMU_setupIntrFuncs(intFuncs);

	setup_system_bios();

	/* cpu setup */
	X86_AX = devfn ? devfn : 0xff;
	X86_DX = 0x80;
	X86_EIP = initialip;
	X86_CS = initialcs;

	/* Initialize stack and data segment */
	X86_SS = STACK_SEGMENT;
	X86_SP = STACK_START_OFFSET;;
	X86_DS = DATA_SEGMENT;

	/* We need a sane way to return from bios
	 * execution. A hlt instruction and a pointer
	 * to it, both kept on the stack, will do.
	 */
	push_word(0xf4f4);		/* hlt; hlt */
	push_word(X86_SS);
	push_word(X86_SP + 2);

#ifdef CONFIG_DEBUG
	//X86EMU_trace_on();
#endif

	printk("Executing Initialization Vector...\n");
	X86EMU_exec();
	printk("Option ROM Exit Status: %04x\n", X86_AX);

	/* Check whether the stack is "clean" i.e. containing the HLT
         * instruction we pushed before executing and pointing to the original
	 * stack address... indicating that the initialization probably was
	 * successful
	 */
	if ((pop_word() == 0xf4f4) && (X86_SS == STACK_SEGMENT)
	    && (X86_SP == STACK_START_OFFSET)) {
		printk("Stack is clean, initialization successfull!\n");
	} else {
		printk("Stack unclean, initialization probably NOT COMPLETE!!\n");
		printk("SS:SP = %04x:%04x, expected: %04x:%04x\n",
			X86_SS, X86_SP, STACK_SEGMENT, STACK_START_OFFSET);
        }
}
