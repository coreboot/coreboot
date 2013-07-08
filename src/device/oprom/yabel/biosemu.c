/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * Copyright (c) 2008, 2009 Pattrick Hueper <phueper@hueper.net>
 * Copyright (c) 2010 coresystems GmbH
 * All rights reserved.
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#include <string.h>
#include <types.h>

#include "debug.h"

#include <x86emu/x86emu.h>
#include <x86emu/regs.h>
#include "../x86emu/prim_ops.h"

#include "biosemu.h"
#include "io.h"
#include "mem.h"
#include "interrupt.h"
#include "device.h"
#include "pmm.h"

#include <device/device.h>
#include "compat/rtas.h"

#if CONFIG_X86EMU_DEBUG_TIMINGS
struct mono_time zero;
#endif

static X86EMU_memFuncs my_mem_funcs = {
	my_rdb, my_rdw, my_rdl,
	my_wrb, my_wrw, my_wrl
};

static X86EMU_pioFuncs my_pio_funcs = {
	my_inb, my_inw, my_inl,
	my_outb, my_outw, my_outl
};

/* interrupt function override array (see biosemu.h) */
yabel_handleIntFunc yabel_intFuncArray[256];

void
mainboard_interrupt_handlers(int interrupt, yabel_handleIntFunc func)
{
	yabel_intFuncArray[interrupt] = func;
}

/* main entry into YABEL biosemu, arguments are:
 * *biosmem = pointer to virtual memory
 * biosmem_size = size of the virtual memory
 * *dev = pointer to the device to be initialised
 * rom_addr = address of the OptionROM to be executed, if this is = 0, YABEL
 * 	will look for an ExpansionROM BAR and use the code from there.
 */
u32
biosemu(u8 *biosmem, u32 biosmem_size, struct device * dev, unsigned long rom_addr)
{
	u8 *rom_image;
	int i = 0;
#if CONFIG_X86EMU_DEBUG
	debug_flags = 0;
#if CONFIG_X86EMU_DEBUG_JMP
	debug_flags |= DEBUG_JMP;
#endif
#if CONFIG_X86EMU_DEBUG_TRACE
	debug_flags |= DEBUG_TRACE_X86EMU;
#endif
#if CONFIG_X86EMU_DEBUG_PNP
	debug_flags |= DEBUG_PNP;
#endif
#if CONFIG_X86EMU_DEBUG_DISK
	debug_flags |= DEBUG_DISK;
#endif
#if CONFIG_X86EMU_DEBUG_PMM
	debug_flags |= DEBUG_PMM;
#endif
#if CONFIG_X86EMU_DEBUG_VBE
	debug_flags |= DEBUG_VBE;
#endif
#if CONFIG_X86EMU_DEBUG_INT10
	debug_flags |= DEBUG_PRINT_INT10;
#endif
#if CONFIG_X86EMU_DEBUG_INTERRUPTS
	debug_flags |= DEBUG_INTR;
#endif
#if CONFIG_X86EMU_DEBUG_CHECK_VMEM_ACCESS
	debug_flags |= DEBUG_CHECK_VMEM_ACCESS;
#endif
#if CONFIG_X86EMU_DEBUG_MEM
	debug_flags |= DEBUG_MEM;
#endif
#if CONFIG_X86EMU_DEBUG_IO
	debug_flags |= DEBUG_IO;
#endif

#endif
#if CONFIG_X86EMU_DEBUG_TIMINGS
	/* required for i915tool compatible output */
	zero.microseconds = 0;
#endif

	if (biosmem_size < MIN_REQUIRED_VMEM_SIZE) {
		printf("Error: Not enough virtual memory: %x, required: %x!\n",
		       biosmem_size, MIN_REQUIRED_VMEM_SIZE);
		return -1;
	}
	if (biosemu_dev_init(dev) != 0) {
		printf("Error initializing device!\n");
		return -1;
	}
	if (biosemu_dev_check_exprom(rom_addr) != 0) {
		printf("Error: Device Expansion ROM invalid!\n");
		return -1;
	}
	biosemu_add_special_memory(0, 0x500); // IVT + BDA
	biosemu_add_special_memory(OPTION_ROM_CODE_SEGMENT << 4, 0x10000); // option ROM

	rom_image = (u8 *) bios_device.img_addr;
	DEBUG_PRINTF("executing rom_image from %p\n", rom_image);
	DEBUG_PRINTF("biosmem at %p\n", biosmem);

	DEBUG_PRINTF("Image Size: %d\n", bios_device.img_size);

	// in case we jump somewhere unexpected, or execution is finished,
	// fill the biosmem with hlt instructions (0xf4)
	// But we have to be careful: If biosmem is 0x00000000 we're running
	// in the lower 1MB and we must not wipe memory like that.
	if (biosmem) {
		DEBUG_PRINTF("Clearing biosmem\n");
		memset(biosmem, 0xf4, biosmem_size);
	}

	X86EMU_setMemBase(biosmem, biosmem_size);

	DEBUG_PRINTF("membase set: %08x, size: %08x\n", (int) M.mem_base,
		     (int) M.mem_size);

	// copy expansion ROM image to segment OPTION_ROM_CODE_SEGMENT
	// NOTE: this sometimes fails, some bytes are 0x00... so we compare
	// after copying and do some retries...
	u8 *mem_img = (u8*)(OPTION_ROM_CODE_SEGMENT << 4);
	u8 copy_count = 0;
	u8 cmp_result = 0;
	do {
#if 0
		set_ci();
		memcpy(mem_img, rom_image, len);
		clr_ci();
#else
		// memcpy fails... try copy byte-by-byte with set/clr_ci
		u8 c;
		for (i = 0; i < bios_device.img_size; i++) {
			set_ci();
			c = *(rom_image + i);
			if (c != *(rom_image + i)) {
				clr_ci();
				printf("Copy failed at: %x/%x\n", i,
				       bios_device.img_size);
				printf("rom_image(%x): %x, mem_img(%x): %x\n",
				       i, *(rom_image + i), i, *(mem_img + i));
				break;
			}
			clr_ci();
			my_wrb((u32)mem_img + i, c);
		}
#endif
		copy_count++;
		set_ci();
		cmp_result = memcmp(mem_img, rom_image, bios_device.img_size);
		clr_ci();
	}
	while ((copy_count < 5) && (cmp_result != 0));
	if (cmp_result != 0) {
		printf
		    ("\nCopying Expansion ROM Image to Memory failed after %d retries! (%x)\n",
		     copy_count, cmp_result);
		dump(rom_image, 0x20);
		dump(mem_img, 0x20);
		return 0;
	}
	// setup default Interrupt Vectors
	// some expansion ROMs seem to check for these addresses..
	// each handler is only an IRET (0xCF) instruction
	// ROM BIOS Int 10 Handler F000:F065
	my_wrl(0x10 * 4, 0xf000f065);
	my_wrb(0x000ff065, 0xcf);
	// ROM BIOS Int 11 Handler F000:F84D
	my_wrl(0x11 * 4, 0xf000f84d);
	my_wrb(0x000ff84d, 0xcf);
	// ROM BIOS Int 12 Handler F000:F841
	my_wrl(0x12 * 4, 0xf000f841);
	my_wrb(0x000ff841, 0xcf);
	// ROM BIOS Int 13 Handler F000:EC59
	my_wrl(0x13 * 4, 0xf000ec59);
	my_wrb(0x000fec59, 0xcf);
	// ROM BIOS Int 14 Handler F000:E739
	my_wrl(0x14 * 4, 0xf000e739);
	my_wrb(0x000fe739, 0xcf);
	// ROM BIOS Int 15 Handler F000:F859
	my_wrl(0x15 * 4, 0xf000f859);
	my_wrb(0x000ff859, 0xcf);
	// ROM BIOS Int 16 Handler F000:E82E
	my_wrl(0x16 * 4, 0xf000e82e);
	my_wrb(0x000fe82e, 0xcf);
	// ROM BIOS Int 17 Handler F000:EFD2
	my_wrl(0x17 * 4, 0xf000efd2);
	my_wrb(0x000fefd2, 0xcf);
	// ROM BIOS Int 1A Handler F000:FE6E
	my_wrl(0x1a * 4, 0xf000fe6e);
	my_wrb(0x000ffe6e, 0xcf);

	// setup BIOS Data Area (0000:04xx, or 0040:00xx)
	// we currently 0 this area, meaning "we don't have
	// any hardware" :-) no serial/parallel ports, floppys, ...
	memset(biosmem + 0x400, 0x0, 0x100);

	// at offset 13h in BDA is the memory size in kbytes
	my_wrw(0x413, biosmem_size / 1024);
	// at offset 0eh in BDA is the segment of the Extended BIOS Data Area
	// see setup further down
	my_wrw(0x40e, INITIAL_EBDA_SEGMENT);
	// TODO: setup BDA Video Data ( offset 49h-66h)
	// e.g. to store video mode, cursor position, ...
	// in int10 (done) handler and VBE Functions

	// TODO: setup BDA Fixed Disk Data
	// 74h: Fixed Disk Last Operation Status
	// 75h: Fixed Disk Number of Disk Drives

	// TODO: check BDA for further needed data...

	//setup Extended BIOS Data Area
	//we currently 0 this area
	memset(biosmem + (INITIAL_EBDA_SEGMENT << 4), 0, INITIAL_EBDA_SIZE);
	// at offset 0h in EBDA is the size of the EBDA in KB
	my_wrw((INITIAL_EBDA_SEGMENT << 4) + 0x0, INITIAL_EBDA_SIZE / 1024);
	//TODO: check for further needed EBDA data...

	// setup  original ROM BIOS Area (F000:xxxx)
	const char *date = "06/11/99";
	for (i = 0; date[i]; i++)
		my_wrb(0xffff5 + i, date[i]);
	// set up eisa ident string
	const char *ident = "PCI_ISA";
	for (i = 0; ident[i]; i++)
		my_wrb(0xfffd9 + i, ident[i]);

	// write system model id for IBM-AT
	// according to "Ralf Browns Interrupt List" Int15 AH=C0 Table 515,
	// model FC is the original AT and also used in all DOSEMU Versions.
	my_wrb(0xFFFFE, 0xfc);

	//setup interrupt handler
	X86EMU_intrFuncs intrFuncs[256];
	for (i = 0; i < 256; i++)
		intrFuncs[i] = handleInterrupt;
	X86EMU_setupIntrFuncs(intrFuncs);
	X86EMU_setupPioFuncs(&my_pio_funcs);
	X86EMU_setupMemFuncs(&my_mem_funcs);

	//setup PMM struct in BIOS_DATA_SEGMENT, offset 0x0
	u8 pmm_length = pmm_setup(BIOS_DATA_SEGMENT, 0x0);
	if (pmm_length <= 0) {
		printf ("\nYABEL: Warning: PMM Area could not be setup. PMM not available (%x)\n",
		     pmm_length);
		return 0;
	} else {
		CHECK_DBG(DEBUG_PMM) {
			/* test the PMM */
			pmm_test();
			/* and clean it again by calling pmm_setup... */
			pmm_length = pmm_setup(BIOS_DATA_SEGMENT, 0x0);
		}
	}
	// setup the CPU
	M.x86.R_AH = bios_device.bus;
	M.x86.R_AL = bios_device.devfn;
	M.x86.R_DX = 0x80;
	M.x86.R_EIP = 3;
	M.x86.R_CS = OPTION_ROM_CODE_SEGMENT;

	// Initialize stack and data segment
	M.x86.R_SS = STACK_SEGMENT;
	M.x86.R_SP = STACK_START_OFFSET;
	M.x86.R_DS = DATA_SEGMENT;

	// push a HLT instruction and a pointer to it onto the stack
	// any return will pop the pointer and jump to the HLT, thus
	// exiting (more or less) cleanly
	push_word(0xf4f4);	// F4=HLT
	push_word(M.x86.R_SS);
	push_word(M.x86.R_SP + 2);

	CHECK_DBG(DEBUG_TRACE_X86EMU) {
		X86EMU_trace_on();
#if 0
	} else {
		M.x86.debug |= DEBUG_SAVE_IP_CS_F;
		M.x86.debug |= DEBUG_DECODE_F;
		M.x86.debug |= DEBUG_DECODE_NOPRINT_F;
#endif
	}
	CHECK_DBG(DEBUG_JMP) {
		M.x86.debug |= DEBUG_TRACEJMP_F;
		M.x86.debug |= DEBUG_TRACEJMP_REGS_F;
		M.x86.debug |= DEBUG_TRACECALL_F;
		M.x86.debug |= DEBUG_TRACECALL_REGS_F;
	}

	DEBUG_PRINTF("Executing Initialization Vector...\n");
	X86EMU_exec();
	DEBUG_PRINTF("done\n");

	/* According to the PNP BIOS Spec, Option ROMs should upon exit, return
	 * some boot device status in AX (see PNP BIOS Spec Section 3.3
	 */
	DEBUG_PRINTF_CS_IP("Option ROM Exit Status: %04x\n", M.x86.R_AX);
#if CONFIG_X86EMU_DEBUG
	DEBUG_PRINTF("Exit Status Decode:\n");
	if (M.x86.R_AX & 0x100) {	// bit 8
		DEBUG_PRINTF
		    ("  IPL Device supporting INT 13h Block Device Format:\n");
		switch (((M.x86.R_AX >> 4) & 0x3)) {	// bits 5:4
		case 0:
			DEBUG_PRINTF("    No IPL Device attached\n");
			break;
		case 1:
			DEBUG_PRINTF("    IPL Device status unknown\n");
			break;
		case 2:
			DEBUG_PRINTF("    IPL Device attached\n");
			break;
		case 3:
			DEBUG_PRINTF("    IPL Device status RESERVED!!\n");
			break;
		}
	}
	if (M.x86.R_AX & 0x80) {	// bit 7
		DEBUG_PRINTF
		    ("  Output Device supporting INT 10h Character Output:\n");
		switch (((M.x86.R_AX >> 4) & 0x3)) {	// bits 5:4
		case 0:
			DEBUG_PRINTF("    No Display Device attached\n");
			break;
		case 1:
			DEBUG_PRINTF("    Display Device status unknown\n");
			break;
		case 2:
			DEBUG_PRINTF("    Display Device attached\n");
			break;
		case 3:
			DEBUG_PRINTF("    Display Device status RESERVED!!\n");
			break;
		}
	}
	if (M.x86.R_AX & 0x40) {	// bit 6
		DEBUG_PRINTF
		    ("  Input Device supporting INT 9h Character Input:\n");
		switch (((M.x86.R_AX >> 4) & 0x3)) {	// bits 5:4
		case 0:
			DEBUG_PRINTF("    No Input Device attached\n");
			break;
		case 1:
			DEBUG_PRINTF("    Input Device status unknown\n");
			break;
		case 2:
			DEBUG_PRINTF("    Input Device attached\n");
			break;
		case 3:
			DEBUG_PRINTF("    Input Device status RESERVED!!\n");
			break;
		}
	}
#endif
	/* Check whether the stack is "clean" i.e. containing the HLT
	 * instruction we pushed before executing and pointing to the original
	 * stack address... indicating that the initialization probably was
	 * successful
	 */
	if ((pop_word() == 0xf4f4) && (M.x86.R_SS == STACK_SEGMENT)
	    && (M.x86.R_SP == STACK_START_OFFSET)) {
		DEBUG_PRINTF("Stack is clean, initialization successful!\n");
	} else {
		printf("Stack unclean, initialization probably NOT COMPLETE!\n");
		DEBUG_PRINTF("SS:SP = %04x:%04x, expected: %04x:%04x\n",
			     M.x86.R_SS, M.x86.R_SP, STACK_SEGMENT,
			     STACK_START_OFFSET);
	}

	// TODO: according to the BIOS Boot Spec initializations may be ended using INT18h and setting
	// the status.
	// We need to implement INT18 accordingly, pseudo code is in specsbbs101.pdf page 30
	// (also for Int19)
	return 0;
}
