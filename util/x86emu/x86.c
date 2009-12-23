/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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

#ifdef CONFIG_COREBOOT_V2
#include <arch/io.h>
#include <arch/registers.h>
#include <console/console.h>
#define printk(x...) do_printk(x)
#else
#include <console.h>
#endif

#include <arch/interrupt.h>

#define REALMODE_BASE ((void *)0x600)

struct realmode_idt {
	u16 offset, cs;
};

void x86_exception(struct eregs *info);
void run_bios(struct device *dev, unsigned long addr);

extern unsigned char __idt_handler, __idt_handler_size;
extern unsigned char __realmode_code, __realmode_code_size;
extern unsigned char __run_optionrom, __run_interrupt;

void (*run_optionrom)(u32 devfn) __attribute__((regparm(0))) = (void *)&__run_optionrom;
void (*vga_enable_console)(void) __attribute__((regparm(0))) = (void *)&__run_interrupt;

int (*intXX_handler[256])(struct eregs *regs) = { NULL };

static int intXX_exception_handler(struct eregs *regs)
{
	printk(BIOS_INFO, "Oops, exception %d while executing option rom\n", 
			regs->vector);
	x86_exception(regs);	// Call coreboot exception handler 

	return 0;		// Never returns?
}

static int intXX_unknown_handler(struct eregs *regs)
{
	printk(BIOS_INFO, "Unsupported software interrupt #0x%x\n", 
			regs->vector);

	return -1;
}

/* setup interrupt handlers for mainboard */
void mainboard_interrupt_handlers(int intXX, void *intXX_func)
{
	intXX_handler[intXX] = intXX_func;
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
			case 0x12:
				intXX_handler[0x12] = &int12_handler;
				break;
			case 0x15:
				intXX_handler[0x15] = &int15_handler;
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
	 * Only need int10 so far.
	 */
	
	/* int42 is the relocated int10 */
	write_idt_stub((void *)0xff065, 0x42); 
}

void run_bios(struct device *dev, unsigned long addr)
{
	/* clear vga bios data area */
	memset((void *)0x400, 0, 0x200);

	/* Set up C interrupt handlers */
	setup_interrupt_handlers();

	/* Setting up realmode IDT */
	setup_realmode_idt();

	memcpy(REALMODE_BASE, &__realmode_code, (size_t)&__realmode_code_size);
	printk(BIOS_SPEW, "Real mode stub @%p: %d bytes\n", REALMODE_BASE,
			(u32)&__realmode_code_size);

	printk(BIOS_DEBUG, "Calling Option ROM...\n");
	run_optionrom((dev->bus->secondary << 8) | dev->path.pci.devfn);
	printk(BIOS_DEBUG, "... Option ROM returned.\n");
}

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

	printk(BIOS_DEBUG, "oprom: INT# 0x%x\n", intnumber);
	printk(BIOS_DEBUG, "oprom: eax: %08x ebx: %08x ecx: %08x edx: %08x\n",
		      eax, ebx, ecx, edx);
	printk(BIOS_DEBUG, "oprom: ebp: %08x esp: %08x edi: %08x esi: %08x\n",
		     ebp, esp, edi, esi);
	printk(BIOS_DEBUG, "oprom:  ip: %04x      cs: %04x   flags: %08x\n",
		     ip, cs, flags);

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
		printk(BIOS_DEBUG,"error!\n");
		flags |= 1;  // error: set carry
	}else{
		flags &= ~1; // no error: clear carry
	}
	*(volatile u16 *)&stackflags = flags;

	return ret;
}

