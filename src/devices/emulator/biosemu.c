#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <x86emu/x86emu.h>

#define MEM_WB(where, what) wrb(where, what)
#define MEM_WW(where, what) wrw(where, what)
#define MEM_WL(where, what) wrl(where, what)

#define MEM_RB(where) rdb(where)
#define MEM_RW(where) rdw(where)
#define MEM_RL(where) rdl(where)

u8 x_inb(u16 port);
u16 x_inw(u16 port);
void x_outb(u16 port, u8 val);
void x_outw(u16 port, u16 val);
u32 x_inl(u16 port);
void x_outl(u16 port, u32 val);

/* general software interrupt handler */
u32 getIntVect(int num)
{
	return MEM_RW(num << 2) + (MEM_RW((num << 2) + 2) << 4);
}

/* FixME: There is already a push_word() in the emulator */
void pushw(u16 val)
{
	X86_ESP -= 2;
	MEM_WW(((u32) X86_SS << 4) + X86_SP, val);
}

int run_bios_int(int num)
{
	u32 eflags;

	eflags = X86_EFLAGS;
	pushw(eflags);
	pushw(X86_CS);
	pushw(X86_IP);
	X86_CS = MEM_RW((num << 2) + 2);
	X86_IP = MEM_RW(num << 2);

	return 1;
}

u8 x_inb(u16 port)
{
	u8 val;

	val = inb(port);

	//if (port != 0x40)
	//    printk_debug("inb(0x%04x) = 0x%02x\n", port, val);

	return val;
}

u16 x_inw(u16 port)
{
	u16 val;

	val = inw(port);

	//printk_debug("inw(0x%04x) = 0x%04x\n", port, val);
	return val;
}

u32 x_inl(u16 port)
{
	u32 val;

	val = inl(port);

	//printk_debug("inl(0x%04x) = 0x%08x\n", port, val);
	return val;
}

void x_outb(u16 port, u8 val)
{
	//if (port != 0x43)
	//	printk_debug("outb(0x%02x, 0x%04x)\n", val, port);
	outb(val, port);
}

void x_outw(u16 port, u16 val)
{
	//printk_debug("outw(0x%04x, 0x%04x)\n", val, port);
	outw(val, port);
}

void x_outl(u16 port, u32 val)
{
	//printk_debug("outl(0x%08x, 0x%04x)\n", val, port);
	outl(val, port);
}

X86EMU_pioFuncs myfuncs = {
	x_inb, x_inw, x_inl,
	x_outb, x_outw, x_outl
};



/* Interrupt multiplexer */

void do_int(int num)
{
	int ret = 0;

//	printk_debug("int%x vector at %x\n", num, getIntVect(num));

	switch (num) {
#ifndef _PC
	case 0x10:
	case 0x42:
	case 0x6D:
		if (getIntVect(num) == 0x0000) {
			printk_debug("un-inited int vector\n");
			ret = 1;
		}
		if (getIntVect(num) == 0xFF065) {
			//ret = int42_handler();
			ret = 1;
		}
		break;
#endif
	case 0x15:
		//ret = int15_handler();
		ret = 1;
		break;
	case 0x16:
		//ret = int16_handler();
		ret = 0;
		break;
	case 0x1A:
		ret = pcibios_handler();
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
#if 0
#define SYS_BIOS 0xf0000
/*
 * here we are really paranoid about faking a "real"
 * BIOS. Most of this information was pulled from
 * dosemu.
 */
#if 0
void setup_int_vect(void)
{
	int i;

	/* let the int vects point to the SYS_BIOS seg */
	for (i = 0; i < 0x80; i++) {
		MEM_WW(i << 2, 0);
		MEM_WW((i << 2) + 2, SYS_BIOS >> 4);
	}

	reset_int_vect();

	/* font tables default location (int 1F) */
	MEM_WW(0x1f << 2, 0xfa6e);
	/* int 11 default location (Get Equipment Configuration) */
	MEM_WW(0x11 << 2, 0xf84d);
	/* int 12 default location (Get Conventional Memory Size) */
	MEM_WW(0x12 << 2, 0xf841);
	/* int 15 default location (I/O System Extensions) */
	MEM_WW(0x15 << 2, 0xf859);
	/* int 1A default location (RTC, PCI and others) */
	MEM_WW(0x1a << 2, 0xff6e);
	/* int 05 default location (Bound Exceeded) */
	MEM_WW(0x05 << 2, 0xff54);
	/* int 08 default location (Double Fault) */
	MEM_WW(0x08 << 2, 0xfea5);
	/* int 13 default location (Disk) */
	MEM_WW(0x13 << 2, 0xec59);
	/* int 0E default location (Page Fault) */
	MEM_WW(0x0e << 2, 0xef57);
	/* int 17 default location (Parallel Port) */
	MEM_WW(0x17 << 2, 0xefd2);
	/* fdd table default location (int 1e) */
	MEM_WW(0x1e << 2, 0xefc7);

	/* Set Equipment flag to VGA */
	i = MEM_RB(0x0410) & 0xCF;
	MEM_WB(0x0410, i);
	/* XXX Perhaps setup more of the BDA here.  See also int42(0x00). */
}

int setup_system_bios(void *base_addr)
{
	char *base = (char *) base_addr;

	/*
	 * we trap the "industry standard entry points" to the BIOS
	 * and all other locations by filling them with "hlt"
	 * TODO: implement hlt-handler for these
	 */
	memset(base, 0xf4, 0x10000);

	/* set bios date */
	//strcpy(base + 0x0FFF5, "06/11/99");
	/* set up eisa ident string */
	//strcpy(base + 0x0FFD9, "PCI_ISA");
	/* write system model id for IBM-AT */
	//*((unsigned char *) (base + 0x0FFFE)) = 0xfc;

	return 1;
}
#endif

void reset_int_vect(void)
{
	/*
	 * This table is normally located at 0xF000:0xF0A4.  However, int 0x42,
	 * function 0 (Mode Set) expects it (or a copy) somewhere in the bottom
	 * 64kB.  Note that because this data doesn't survive POST, int 0x42 should
	 * only be used during EGA/VGA BIOS initialisation.
	 */
	static const u8 VideoParms[] = {
		/* Timing for modes 0x00 & 0x01 */
		0x38, 0x28, 0x2d, 0x0a, 0x1f, 0x06, 0x19, 0x1c,
		0x02, 0x07, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
		/* Timing for modes 0x02 & 0x03 */
		0x71, 0x50, 0x5a, 0x0a, 0x1f, 0x06, 0x19, 0x1c,
		0x02, 0x07, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
		/* Timing for modes 0x04, 0x05 & 0x06 */
		0x38, 0x28, 0x2d, 0x0a, 0x7f, 0x06, 0x64, 0x70,
		0x02, 0x01, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
		/* Timing for mode 0x07 */
		0x61, 0x50, 0x52, 0x0f, 0x19, 0x06, 0x19, 0x19,
		0x02, 0x0d, 0x0b, 0x0c, 0x00, 0x00, 0x00, 0x00,
		/* Display page lengths in little endian order */
		0x00, 0x08,	/* Modes 0x00 and 0x01 */
		0x00, 0x10,	/* Modes 0x02 and 0x03 */
		0x00, 0x40,	/* Modes 0x04 and 0x05 */
		0x00, 0x40,	/* Modes 0x06 and 0x07 */
		/* Number of columns for each mode */
		40, 40, 80, 80, 40, 40, 80, 80,
		/* CGA Mode register value for each mode */
		0x2c, 0x28, 0x2d, 0x29, 0x2a, 0x2e, 0x1e, 0x29,
		/* Padding */
		0x00, 0x00, 0x00, 0x00
	};
	int i;

	for (i = 0; i < sizeof(VideoParms); i++)
		MEM_WB(i + (0x1000 - sizeof(VideoParms)), VideoParms[i]);
	MEM_WW(0x1d << 2, 0x1000 - sizeof(VideoParms));
	MEM_WW((0x1d << 2) + 2, 0);

	printk_debug("SETUP INT\n");
	MEM_WW(0x10 << 2, 0xf065);
	MEM_WW((0x10 << 2) + 2, SYS_BIOS >> 4);
	MEM_WW(0x42 << 2, 0xf065);
	MEM_WW((0x42 << 2) + 2, SYS_BIOS >> 4);
	MEM_WW(0x6D << 2, 0xf065);
	MEM_WW((0x6D << 2) + 2, SYS_BIOS >> 4);
}
#endif
void run_bios(struct device * dev, unsigned long addr)
{
#if 1
	int i;
	unsigned short initialcs = (addr & 0xF0000) >> 4;
	unsigned short initialip = (addr + 3) & 0xFFFF;
	unsigned short devfn = dev->bus->secondary << 8 | dev->path.u.pci.devfn;
	X86EMU_intrFuncs intFuncs[256];

	X86EMU_setMemBase(0, 0x100000);
	X86EMU_setupPioFuncs(&myfuncs);
	for (i = 0; i < 256; i++)
		intFuncs[i] = do_int;
	X86EMU_setupIntrFuncs(intFuncs);

	{
		char *date = "01/01/99";
		for (i = 0; date[i]; i++)
			wrb(0xffff5 + i, date[i]);
		wrb(0xffff7, '/');
		wrb(0xffffa, '/');
	}

	{
	    /* FixME: move PIT init to its own file */
	    outb(0x36, 0x43);
	    outb(0x00, 0x40);
	    outb(0x00, 0x40);
	}
	//setup_int_vect();

	/* cpu setup */
	X86_AX = devfn ? devfn : 0xff;
	X86_DX = 0x80;
	X86_EIP = initialip;
	X86_CS = initialcs;

	/* Initialize stack and data segment */
	X86_SS = initialcs;
	X86_SP = 0xfffe;
	X86_DS = 0x0040;
	X86_ES = 0x0000;

	/* We need a sane way to return from bios
	 * execution. A hlt instruction and a pointer
	 * to it, both kept on the stack, will do.
	 */
	pushw(0xf4f4);		/* hlt; hlt */
	pushw(X86_SS);
	pushw(X86_SP + 2);

//	X86EMU_trace_on();

	printk_info("entering emulator\n");

	X86EMU_exec();
#endif
}
