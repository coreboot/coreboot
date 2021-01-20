/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <arch/interrupt.h>
#include <arch/registers.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/i8259.h>
#include <pc80/i8254.h>
#include <string.h>
#include <vbe.h>
#include <framebuffer_info.h>

/* we use x86emu's register file representation */
#include <x86emu/regs.h>

#include "x86.h"

typedef struct {
	char signature[4];
	u16 version;
	u8 *oem_string_ptr;
	u32 capabilities;
	u32 video_mode_ptr;
	u16 total_memory;
	char reserved[236];
} __packed vbe_info_block;

/* The following symbols cannot be used directly. They need to be fixed up
 * to point to the correct address location after the code has been copied
 * to REALMODE_BASE. Absolute symbols are not used because those symbols are
 * relocated when a relocatable ramstage is enabled.
 */
extern unsigned char __realmode_call, __realmode_interrupt;
extern unsigned char __realmode_buffer;

#define PTR_TO_REAL_MODE(sym)\
	(void *)(REALMODE_BASE + ((char *)&(sym) - (char *)&__realmode_code))

/* to have a common register file for interrupt handlers */
X86EMU_sysEnv _X86EMU_env;

unsigned int (*realmode_call)(u32 addr, u32 eax, u32 ebx, u32 ecx, u32 edx,
		u32 esi, u32 edi) asmlinkage;

unsigned int (*realmode_interrupt)(u32 intno, u32 eax, u32 ebx, u32 ecx,
		u32 edx, u32 esi, u32 edi) asmlinkage;

static void setup_realmode_code(void)
{
	memcpy(REALMODE_BASE, &__realmode_code, __realmode_code_size);

	/* Ensure the global pointers are relocated properly. */
	realmode_call = PTR_TO_REAL_MODE(__realmode_call);
	realmode_interrupt = PTR_TO_REAL_MODE(__realmode_interrupt);

	printk(BIOS_SPEW, "Real mode stub @%p: %d bytes\n", REALMODE_BASE,
			__realmode_code_size);
}

static void setup_rombios(void)
{
	const char date[] = "06/11/99";
	memcpy((void *)0xffff5, &date, 8);

	const char ident[] = "PCI_ISA";
	memcpy((void *)0xfffd9, &ident, 7);

	/* system model: IBM-AT */
	write8((void *)0xffffe, 0xfc);
}

static int (*intXX_handler[256])(void) = { NULL };

static int intXX_exception_handler(void)
{
	/* compatibility shim */
	struct eregs reg_info = {
		.eax=X86_EAX,
		.ecx=X86_ECX,
		.edx=X86_EDX,
		.ebx=X86_EBX,
		.esp=X86_ESP,
		.ebp=X86_EBP,
		.esi=X86_ESI,
		.edi=X86_EDI,
		.vector=M.x86.intno,
		.error_code=0, // FIXME: fill in
		.eip=X86_EIP,
		.cs=X86_CS,
		.eflags=X86_EFLAGS
	};
	struct eregs *regs = &reg_info;

	printk(BIOS_INFO, "Oops, exception %d while executing option rom\n",
			regs->vector);
	x86_exception(regs);	// Call coreboot exception handler

	return 0;		// Never really returns
}

static int intXX_unknown_handler(void)
{
	printk(BIOS_INFO, "Unsupported software interrupt #0x%x eax 0x%x\n",
			M.x86.intno, X86_EAX);

	return -1;
}

/* setup interrupt handlers for mainboard */
void mainboard_interrupt_handlers(int intXX, int (*intXX_func)(void))
{
	intXX_handler[intXX] = intXX_func;
}

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
		if (!intXX_handler[i])
		{
			/* Now set the default functions that are actually
			 * needed to initialize the option roms. This is
			 * very slick, as it allows us to implement mainboard
			 * specific interrupt handlers, such as the int15.
			 */
			switch (i) {
			case 0x10:
				intXX_handler[0x10] = &int10_handler;
				break;
			case 0x12:
				intXX_handler[0x12] = &int12_handler;
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
	memcpy(codeptr, &__idt_handler, __idt_handler_size);
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
		idts[i].offset = 0x1000 + (i * __idt_handler_size);
		write_idt_stub((void *)((uintptr_t)idts[i].offset), i);
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

#if CONFIG(FRAMEBUFFER_SET_VESA_MODE)
static vbe_mode_info_t mode_info;
static int mode_info_valid;

const vbe_mode_info_t *vbe_mode_info(void)
{
	if (!mode_info_valid || !mode_info.vesa.phys_base_ptr)
		return NULL;
	return &mode_info;
}

static int vbe_check_for_failure(int ah);

static u8 vbe_get_ctrl_info(vbe_info_block *info)
{
	char *buffer = PTR_TO_REAL_MODE(__realmode_buffer);
	u16 buffer_seg = (((unsigned long)buffer) >> 4) & 0xff00;
	u16 buffer_adr = ((unsigned long)buffer) & 0xffff;
	X86_EAX = realmode_interrupt(0x10, VESA_GET_INFO, 0x0000, 0x0000,
			0x0000, buffer_seg, buffer_adr);
	/* If the VBE function completed successfully, 0x0 is returned in AH */
	if (X86_AH) {
		printk(BIOS_WARNING, "Warning: Error from VGA BIOS in %s\n", __func__);
		return 1;
	}
	memcpy(info, buffer, sizeof(vbe_info_block));
	return 0;
}

static void vbe_oprom_list_supported_mode(uint16_t *video_mode_ptr)
{
	uint16_t mode;
	printk(BIOS_DEBUG, "Supported Video Mode list for OpRom:\n");
	do {
		mode = *video_mode_ptr++;
		if (mode != 0xffff)
			printk(BIOS_DEBUG, "%x\n", mode);
	} while (mode != 0xffff);
}

static u8 vbe_oprom_supported_mode_list(void)
{
	uint16_t segment, offset;
	vbe_info_block info;

	if (vbe_get_ctrl_info(&info))
		return 1;

	offset = info.video_mode_ptr;
	segment = info.video_mode_ptr >> 16;

	vbe_oprom_list_supported_mode((uint16_t *)((segment << 4) + offset));
	return 0;
}
/*
 * EAX register is used to indicate the completion status upon return from
 * VBE function in real mode.
 *
 * If the VBE function completed successfully then 0x0 is returned in the AH
 * register. Otherwise the AH register is set with the nature of the failure:
 *
 * AH == 0x00: Function call successful
 * AH == 0x01: Function call failed
 * AH == 0x02: Function is not supported in the current HW configuration
 * AH == 0x03: Function call invalid in current video mode
 *
 * Return 0 on success else -1 for failure
 */
static int vbe_check_for_failure(int ah)
{
	int status;

	switch (ah) {
	case 0x0:
		status = 0;
		break;
	case 1:
		printk(BIOS_DEBUG, "VBE: Function call failed!\n");
		status = -1;
		break;
	case 2:
		printk(BIOS_DEBUG, "VBE: Function is not supported!\n");
		status = -1;
		break;
	case 3:
	default:
		printk(BIOS_DEBUG, "VBE: Unsupported video mode %x!\n",
			CONFIG_FRAMEBUFFER_VESA_MODE);
		if (vbe_oprom_supported_mode_list())
			printk(BIOS_WARNING, "VBE Warning: Could not get VBE mode list.\n");
		status = -1;
		break;
	}

	return status;
}
static u8 vbe_get_mode_info(vbe_mode_info_t * mi)
{
	printk(BIOS_DEBUG, "VBE: Getting information about VESA mode %04x\n",
		mi->video_mode);
	char *buffer = PTR_TO_REAL_MODE(__realmode_buffer);
	u16 buffer_seg = (((unsigned long)buffer) >> 4) & 0xff00;
	u16 buffer_adr = ((unsigned long)buffer) & 0xffff;
	X86_EAX = realmode_interrupt(0x10, VESA_GET_MODE_INFO, 0x0000,
			mi->video_mode, 0x0000, buffer_seg, buffer_adr);
	if (vbe_check_for_failure(X86_AH)) {
		printk(BIOS_WARNING, "VBE Warning: Error from VGA BIOS in %s\n", __func__);
		return 1;
	}
	memcpy(mi->mode_info_block, buffer, sizeof(mi->mode_info_block));
	mode_info_valid = 1;
	return 0;
}

static u8 vbe_set_mode(vbe_mode_info_t * mi)
{
	printk(BIOS_DEBUG, "VBE: Setting VESA mode %04x\n", mi->video_mode);
	// request linear framebuffer mode
	mi->video_mode |= (1 << 14);
	// request clearing of framebuffer
	mi->video_mode &= ~(1 << 15);
	X86_EAX = realmode_interrupt(0x10, VESA_SET_MODE, mi->video_mode,
			0x0000, 0x0000, 0x0000, 0x0000);
	if (vbe_check_for_failure(X86_AH)) {
		printk(BIOS_WARNING, "VBE Warning: Error from VGA BIOS in %s\n", __func__);
		return 1;
	}
	return 0;
}

/* These two functions could probably even be generic between
 * yabel and x86 native. TBD later.
 */
void vbe_set_graphics(void)
{
	mode_info.video_mode = (1 << 14) | CONFIG_FRAMEBUFFER_VESA_MODE;
	if (vbe_get_mode_info(&mode_info)) {
		printk(BIOS_WARNING, "VBE Warning: Could not get VBE graphics mode info.\n");
		return;
	}
	unsigned char *framebuffer =
		(unsigned char *)mode_info.vesa.phys_base_ptr;
	printk(BIOS_DEBUG, "VBE: resolution:  %dx%d@%d\n",
		le16_to_cpu(mode_info.vesa.x_resolution),
		le16_to_cpu(mode_info.vesa.y_resolution),
		mode_info.vesa.bits_per_pixel);

	printk(BIOS_DEBUG, "VBE: framebuffer: %p\n", framebuffer);
	if (!framebuffer) {
		printk(BIOS_DEBUG, "VBE: Mode does not support linear "
			"framebuffer\n");
		return;
	}

	if (vbe_set_mode(&mode_info)) {
		printk(BIOS_WARNING, "VBE Warning: Could not set VBE graphics mode.\n");
		return;
	}
	const struct lb_framebuffer fb = {
		.physical_address    = mode_info.vesa.phys_base_ptr,
		.x_resolution        = le16_to_cpu(mode_info.vesa.x_resolution),
		.y_resolution        = le16_to_cpu(mode_info.vesa.y_resolution),
		.bytes_per_line      = le16_to_cpu(mode_info.vesa.bytes_per_scanline),
		.bits_per_pixel      = mode_info.vesa.bits_per_pixel,
		.red_mask_pos        = mode_info.vesa.red_mask_pos,
		.red_mask_size       = mode_info.vesa.red_mask_size,
		.green_mask_pos      = mode_info.vesa.green_mask_pos,
		.green_mask_size     = mode_info.vesa.green_mask_size,
		.blue_mask_pos       = mode_info.vesa.blue_mask_pos,
		.blue_mask_size      = mode_info.vesa.blue_mask_size,
		.reserved_mask_pos   = mode_info.vesa.reserved_mask_pos,
		.reserved_mask_size  = mode_info.vesa.reserved_mask_size,
		.orientation         = LB_FB_ORIENTATION_NORMAL,
	};

	fb_add_framebuffer_info_ex(&fb);
}

void vbe_textmode_console(void)
{
	u8 retval = 1;
	if (mode_info.vesa.phys_base_ptr) {
		delay(2);
		X86_EAX = realmode_interrupt(0x10, 0x0003, 0x0000, 0x0000,
					0x0000, 0x0000, 0x0000);
		if (!vbe_check_for_failure(X86_AH))
			retval = 0;
	}

	if (retval)
		printk(BIOS_WARNING, "VBE Warning: Could not set VBE text mode.\n");
}

#endif

void run_bios(struct device *dev, unsigned long addr)
{
	u32 num_dev = (dev->bus->secondary << 8) | dev->path.pci.devfn;

	/* Setting up required hardware.
	 * Removing this will cause random illegal instruction exceptions
	 * in some option roms.
	 */
	setup_i8259();
	setup_i8254();

	/* Set up some legacy information in the F segment */
	setup_rombios();

	/* Set up C interrupt handlers */
	setup_interrupt_handlers();

	/* Set up real-mode IDT */
	setup_realmode_idt();

	/* Make sure the code is placed. */
	setup_realmode_code();

	printk(BIOS_DEBUG, "Calling Option ROM...\n");
	/* TODO ES:DI Pointer to System BIOS PnP Installation Check Structure */
	/* Option ROM entry point is at OPROM start + 3 */
	realmode_call(addr + 0x0003, num_dev, 0xffff, 0x0000, 0xffff, 0x0, 0x0);
	printk(BIOS_DEBUG, "... Option ROM returned.\n");

#if CONFIG(FRAMEBUFFER_SET_VESA_MODE)
	if ((dev->class >> 8)== PCI_CLASS_DISPLAY_VGA)
		vbe_set_graphics();
#endif
}

/* interrupt_handler() is called from assembler code only,
 * so there is no use in putting the prototype into a header file.
 */
int asmlinkage interrupt_handler(u32 intnumber,
	    u32 gsfs, u32 dses,
	    u32 edi, u32 esi,
	    u32 ebp, u32 esp,
	    u32 ebx, u32 edx,
	    u32 ecx, u32 eax,
	    u32 cs_ip, u16 stackflags);

int asmlinkage interrupt_handler(u32 intnumber,
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
	int ret = 0;

	ip = cs_ip & 0xffff;
	cs = cs_ip >> 16;
	flags = stackflags;

#if CONFIG(REALMODE_DEBUG)
	printk(BIOS_DEBUG, "oprom: INT# 0x%x\n", intnumber);
	printk(BIOS_DEBUG, "oprom: eax: %08x ebx: %08x ecx: %08x edx: %08x\n",
		      eax, ebx, ecx, edx);
	printk(BIOS_DEBUG, "oprom: ebp: %08x esp: %08x edi: %08x esi: %08x\n",
		     ebp, esp, edi, esi);
	printk(BIOS_DEBUG, "oprom:  ip: %04x      cs: %04x   flags: %08x\n",
		     ip, cs, flags);
#endif

	// Fetch arguments from the stack and put them to a place
	// suitable for the interrupt handlers
	X86_EAX = eax;
	X86_ECX = ecx;
	X86_EDX = edx;
	X86_EBX = ebx;
	X86_ESP = esp;
	X86_EBP = ebp;
	X86_ESI = esi;
	X86_EDI = edi;
	M.x86.intno = intnumber;
	/* TODO: error_code must be stored somewhere */
	X86_EIP = ip;
	X86_CS = cs;
	X86_EFLAGS = flags;

	// Call the interrupt handler for this int#
	ret = intXX_handler[intnumber]();

	// Put registers back on the stack. The assembler code
	// will later pop them.
	// What happens here is that we force (volatile!) changing
	// the values of the parameters of this function. We do this
	// because we know that they stay alive on the stack after
	// we leave this function. Don't say this is bollocks.
	*(volatile u32 *)&eax = X86_EAX;
	*(volatile u32 *)&ecx = X86_ECX;
	*(volatile u32 *)&edx = X86_EDX;
	*(volatile u32 *)&ebx = X86_EBX;
	*(volatile u32 *)&esi = X86_ESI;
	*(volatile u32 *)&edi = X86_EDI;
	flags = X86_EFLAGS;

	/* Pass success or error back to our caller via the CARRY flag */
	if (ret) {
		flags &= ~1; // no error: clear carry
	}else{
		printk(BIOS_DEBUG,"int%02x call returned error.\n", intnumber);
		flags |= 1;  // error: set carry
	}
	*(volatile u16 *)&stackflags = flags;

	/* The assembler code doesn't actually care for the return value,
	 * but keep it around so its expectations are met */
	return ret;
}
