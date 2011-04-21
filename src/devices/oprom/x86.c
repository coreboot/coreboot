/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2009-2010 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <device/pci.h>
#include <string.h>

#include <arch/io.h>
#include <arch/registers.h>
#include <console/console.h>
#include <arch/interrupt.h>

#define REALMODE_BASE ((void *)0x600)

struct realmode_idt {
	u16 offset, cs;
};

void x86_exception(struct eregs *info);

/* From x86_asm.S */
extern unsigned char __idt_handler, __idt_handler_size;
extern unsigned char __realmode_code, __realmode_code_size;
extern unsigned char __realmode_call, __realmode_interrupt;

void (*realmode_call)(u32 addr, u32 eax, u32 ebx, u32 ecx, u32 edx,
		u32 esi, u32 edi) __attribute__((regparm(0))) = (void *)&__realmode_call;

void (*realmode_interrupt)(u32 intno, u32 eax, u32 ebx, u32 ecx, u32 edx, 
		u32 esi, u32 edi) __attribute__((regparm(0))) = (void *)&__realmode_interrupt;

#define FAKE_MEMORY_SIZE (1024*1024) // only 1MB
#define INITIAL_EBDA_SEGMENT 0xF600
#define INITIAL_EBDA_SIZE 0x400

static void setup_bda(void)
{
	/* clear BIOS DATA AREA */
	memset((void *)0x400, 0, 0x200);

	write16(0x413, FAKE_MEMORY_SIZE / 1024);
	write16(0x40e, INITIAL_EBDA_SEGMENT);

	/* Set up EBDA */
	memset((void *)(INITIAL_EBDA_SEGMENT << 4), 0, INITIAL_EBDA_SIZE);
	write16((INITIAL_EBDA_SEGMENT << 4) + 0x0, INITIAL_EBDA_SIZE / 1024);
}

static void setup_rombios(void)
{
	const char date[] = "06/11/99";
	memcpy((void *)0xffff5, &date, 8);

	const char ident[] = "PCI_ISA";
	memcpy((void *)0xfffd9, &ident, 7);

	/* system model: IBM-AT */
	write8(0xffffe, 0xfc);
}

int (*intXX_handler[256])(struct eregs *regs) = { NULL };

static int intXX_exception_handler(struct eregs *regs)
{
	printk(BIOS_INFO, "Oops, exception %d while executing option rom\n",
			regs->vector);
#if 0
	// Odd: The i945GM VGA oprom chokes on a pushl %eax and will
	// die with an exception #6 if we run the coreboot exception 
	// handler. Just continue, as it executes fine.
	x86_exception(regs);	// Call coreboot exception handler
#endif

	return 0;		// Never returns?
}

static int intXX_unknown_handler(struct eregs *regs)
{
	printk(BIOS_INFO, "Unsupported software interrupt #0x%x eax 0x%x\n",
			regs->vector, regs->eax);

	return -1;
}

/* setup interrupt handlers for mainboard */
void mainboard_interrupt_handlers(int intXX, void *intXX_func)
{
	intXX_handler[intXX] = intXX_func;
}

static int int10_handler(struct eregs *regs)
{
	int res=-1;
	static u8 cursor_row=0, cursor_col=0;
	switch((regs->eax & 0xff00)>>8) {
	case 0x01: // Set cursor shape
		res = 0;
		break;
	case 0x02: // Set cursor position
		if (cursor_row != ((regs->edx >> 8) & 0xff) ||
		    cursor_col >= (regs->edx & 0xff)) {
			printk(BIOS_INFO, "\n");
		}
		cursor_row = (regs->edx >> 8) & 0xff;
		cursor_col = regs->edx & 0xff;
		res = 0;
		break;
	case 0x03: // Get cursor position
		regs->eax &= 0x00ff;
		regs->ecx = 0x0607;
		regs->edx = (cursor_row << 8) | cursor_col;
		res = 0;
		break;
	case 0x06: // Scroll up
		printk(BIOS_INFO, "\n");
		res = 0;
		break;
	case 0x08: // Get Character and Mode at Cursor Position
		regs->eax = 0x0f00 | 'A'; // White on black 'A'
		res = 0;
		break;
	case 0x09: // Write Character and attribute
	case 0x10: // Write Character
		printk(BIOS_INFO, "%c", regs->eax & 0xff);
		res = 0;
		break;
	case 0x0f: // Get video mode
		regs->eax = 0x5002; //80x25
		regs->ebx &= 0x00ff;
		res = 0;
		break;
        default:
		printk(BIOS_WARNING, "Unknown INT10 function %04x!\n",
				regs->eax & 0xffff);
		break;
	}
	return res;
}

static int int16_handler(struct eregs *regs)
{
	int res=-1;
	switch((regs->eax & 0xff00)>>8) {
	case 0x00: // Check for Keystroke
		regs->eax = 0x6120; // Space Bar, Space
		res = 0;
		break;
	case 0x01: // Check for Keystroke
		regs->eflags |= 1<<6; // Zero Flag set (no key available)
		res = 0;
		break;
        default:
		printk(BIOS_WARNING, "Unknown INT16 function %04x!\n",
				regs->eax & 0xffff);
		break;
	}
	return res;
}

int int12_handler(struct eregs *regs);
int int15_handler(struct eregs *regs);
int int1a_handler(struct eregs *regs);

static void setup_interrupt_handlers(void)
{
	int i;

	/* The first 16 intXX functions are not BIOS services,
	 * but the CPU-generated exceptions ("hardware interrupts")
	 */
	for (i = 0; i < 0x10; i++)
		intXX_handler[i] = &intXX_exception_handler;

	/* Mark all other intXX calls as unknown first */
	for (i = 0x10; i < 0x100; i++)
	{
		/* If the mainboard_interrupt_handler isn't called first.
		 */
		if(!intXX_handler[i])
		{
			/* Now set the default functions that are actually
			 * needed to initialize the option roms. This is very
			 * slick, as it allows us to implement mainboard specific
			 * interrupt handlers, such as the int15
			 */
			switch (i) {
			case 0x10:
				intXX_handler[0x10] = &int10_handler;
				break;
			case 0x12:
				intXX_handler[0x12] = &int12_handler;
				break;
			case 0x15:
				intXX_handler[0x15] = &int15_handler;
				break;
			case 0x16:
				intXX_handler[0x16] = &int16_handler;
				break;
			case 0x1a:
				intXX_handler[0x1a] = &int1a_handler;
				break;
			default:
				intXX_handler[i] = &intXX_unknown_handler;
				break;
			}
		}
	}
}

static void write_idt_stub(void *target, u8 intnum)
{
	unsigned char *codeptr;
	codeptr = (unsigned char *) target;
	memcpy(codeptr, &__idt_handler, (size_t)&__idt_handler_size);
	codeptr[3] = intnum; /* modify int# in the code stub. */
}

static void setup_realmode_idt(void)
{
	struct realmode_idt *idts = (struct realmode_idt *) 0;
	int i;

	/* Copy IDT stub code for each interrupt. This might seem wasteful
	 * but it is really simple
	 */
	 for (i = 0; i < 256; i++) {
		idts[i].cs = 0;
		idts[i].offset = 0x1000 + (i * (u32)&__idt_handler_size);
		write_idt_stub((void *)((u32 )idts[i].offset), i);
	}

	/* Many option ROMs use the hard coded interrupt entry points in the
	 * system bios. So install them at the known locations.
	 */

	/* int42 is the relocated int10 */
	write_idt_stub((void *)0xff065, 0x42);
	/* BIOS Int 11 Handler F000:F84D */
	write_idt_stub((void *)0xff84d, 0x11);
	/* BIOS Int 12 Handler F000:F841 */
	write_idt_stub((void *)0xff841, 0x12);
	/* BIOS Int 13 Handler F000:EC59 */
	write_idt_stub((void *)0xfec59, 0x13);
	/* BIOS Int 14 Handler F000:E739 */
	write_idt_stub((void *)0xfe739, 0x14);
	/* BIOS Int 15 Handler F000:F859 */
	write_idt_stub((void *)0xff859, 0x15);
	/* BIOS Int 16 Handler F000:E82E */
	write_idt_stub((void *)0xfe82e, 0x16);
	/* BIOS Int 17 Handler F000:EFD2 */
	write_idt_stub((void *)0xfefd2, 0x17);
	/* ROM BIOS Int 1A Handler F000:FE6E */
	write_idt_stub((void *)0xffe6e, 0x1a);
}

void run_bios(struct device *dev, unsigned long addr)
{
	u32 num_dev = (dev->bus->secondary << 8) | dev->path.pci.devfn;

	/* Set up BIOS Data Area */
	setup_bda();

	/* Set up some legacy information in the F segment */
	setup_rombios();

	/* Set up C interrupt handlers */
	setup_interrupt_handlers();

	/* Set up real-mode IDT */
	setup_realmode_idt();

	memcpy(REALMODE_BASE, &__realmode_code, (size_t)&__realmode_code_size);
	printk(BIOS_SPEW, "Real mode stub @%p: %d bytes\n", REALMODE_BASE,
			(u32)&__realmode_code_size);

	printk(BIOS_DEBUG, "Calling Option ROM...\n");
	/* TODO ES:DI Pointer to System BIOS PnP Installation Check Structure */
	/* Option ROM entry point is at OPROM start + 3 */
	realmode_call(addr + 0x0003, num_dev, 0xffff, 0x0000, 0xffff, 0x0, 0x0);
	printk(BIOS_DEBUG, "... Option ROM returned.\n");
}

#if CONFIG_GEODE_VSA
#include <cpu/amd/lxdef.h>
#include <cpu/amd/vr.h>
#include <cbfs.h>

#define VSA2_BUFFER		0x60000
#define VSA2_ENTRY_POINT	0x60020

// TODO move to a header file.
void do_vsmbios(void);

/* VSA virtual register helper */
static u32 VSA_vrRead(u16 classIndex)
{
	u32 eax, ebx, ecx, edx;
	asm volatile (
		"movw	$0x0AC1C, %%dx\n"
		"orl	$0x0FC530000, %%eax\n"
		"outl	%%eax, %%dx\n"
		"addb	$2, %%dl\n"
		"inw	%%dx, %%ax\n"
		: "=a" (eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
		: "a"(classIndex)
	);

	return eax;
}

void do_vsmbios(void)
{
	printk(BIOS_DEBUG, "Preparing for VSA...\n");

	/* Set up C interrupt handlers */
	setup_interrupt_handlers();

	/* Setting up realmode IDT */
	setup_realmode_idt();

	memcpy(REALMODE_BASE, &__realmode_code, (size_t)&__realmode_code_size);
	printk(BIOS_SPEW, "VSA: Real mode stub @%p: %d bytes\n", REALMODE_BASE,
			(u32)&__realmode_code_size);

	if ((unsigned int)cbfs_load_stage("vsa") != VSA2_ENTRY_POINT) {
		printk(BIOS_ERR, "Failed to load VSA.\n");
		return;
	}

	unsigned char *buf = (unsigned char *)VSA2_BUFFER;
	printk(BIOS_DEBUG, "VSA: Buffer @%p *[0k]=%02x\n", buf, buf[0]);
	printk(BIOS_DEBUG, "VSA: Signature *[0x20-0x23] is %02x:%02x:%02x:%02x\n",
		     buf[0x20], buf[0x21], buf[0x22], buf[0x23]);

	/* Check for code to emit POST code at start of VSA. */
	if ((buf[0x20] != 0xb0) || (buf[0x21] != 0x10) ||
	    (buf[0x22] != 0xe6) || (buf[0x23] != 0x80)) {
		printk(BIOS_WARNING, "VSA: Signature incorrect. Install failed.\n");
		return;
	}

	printk(BIOS_DEBUG, "Calling VSA module...\n");

	/* ECX gets SMM, EDX gets SYSMEM */
	realmode_call(VSA2_ENTRY_POINT, 0x0, 0x0, MSR_GLIU0_SMM, 
			MSR_GLIU0_SYSMEM, 0x0, 0x0);

	printk(BIOS_DEBUG, "... VSA module returned.\n");

	/* Restart timer 1 */
	outb(0x56, 0x43);
	outb(0x12, 0x41);

	/* Check that VSA is running OK */
	if (VSA_vrRead(SIGNATURE) == VSA2_SIGNATURE)
		printk(BIOS_DEBUG, "VSM: VSA2 VR signature verified.\n");
	else
		printk(BIOS_ERR, "VSM: VSA2 VR signature not valid. Install failed.\n");
}
#endif

/* interrupt_handler() is called from assembler code only,
 * so there is no use in putting the prototype into a header file.
 */
int __attribute__((regparm(0))) interrupt_handler(u32 intnumber,
	    u32 gsfs, u32 dses,
	    u32 edi, u32 esi,
	    u32 ebp, u32 esp,
	    u32 ebx, u32 edx,
	    u32 ecx, u32 eax,
	    u32 cs_ip, u16 stackflags);

int __attribute__((regparm(0))) interrupt_handler(u32 intnumber,
	    u32 gsfs, u32 dses,
	    u32 edi, u32 esi,
	    u32 ebp, u32 esp,
	    u32 ebx, u32 edx,
	    u32 ecx, u32 eax,
	    u32 cs_ip, u16 stackflags)
{
	u32 ip;
	u32 cs;
	u32 flags;
	int ret = -1;
	struct eregs reg_info;

	ip = cs_ip & 0xffff;
	cs = cs_ip >> 16;
	flags = stackflags;

#if CONFIG_REALMODE_DEBUG
	printk(BIOS_DEBUG, "oprom: INT# 0x%x\n", intnumber);
	printk(BIOS_DEBUG, "oprom: eax: %08x ebx: %08x ecx: %08x edx: %08x\n",
		      eax, ebx, ecx, edx);
	printk(BIOS_DEBUG, "oprom: ebp: %08x esp: %08x edi: %08x esi: %08x\n",
		     ebp, esp, edi, esi);
	printk(BIOS_DEBUG, "oprom:  ip: %04x      cs: %04x   flags: %08x\n",
		     ip, cs, flags);
#endif

	// Fetch arguments from the stack and put them into
	// a structure that we want to pass on to our sub interrupt
	// handlers.
	reg_info = (struct eregs) {
		.eax=eax,
		.ecx=ecx,
		.edx=edx,
		.ebx=ebx,
		.esp=esp,
		.ebp=ebp,
		.esi=esi,
		.edi=edi,
		.vector=intnumber,
		.error_code=0, // ??
		.eip=ip,
		.cs=cs,
		.eflags=flags // ??
	};

	// Call the interrupt handler for this int#
	ret = intXX_handler[intnumber](&reg_info);

	// Put registers back on the stack. The assembler code
	// will later pop them.
	// What happens here is that we force (volatile!) changing
	// the values of the parameters of this function. We do this
	// because we know that they stay alive on the stack after
	// we leave this function. Don't say this is bollocks.
	*(volatile u32 *)&eax = reg_info.eax;
	*(volatile u32 *)&ecx = reg_info.ecx;
	*(volatile u32 *)&edx = reg_info.edx;
	*(volatile u32 *)&ebx = reg_info.ebx;
	*(volatile u32 *)&esi = reg_info.esi;
	*(volatile u32 *)&edi = reg_info.edi;
	flags = reg_info.eflags;

	/* Pass errors back to our caller via the CARRY flag */
	if (ret) {
		printk(BIOS_DEBUG,"int%02x call returned error.\n", intnumber);
		flags |= 1;  // error: set carry
	}else{
		flags &= ~1; // no error: clear carry
	}
	*(volatile u16 *)&stackflags = flags;

	return ret;
}

