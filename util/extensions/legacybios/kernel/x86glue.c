#include <types.h>
#include <io.h>
#include <x86emu.h>
#include <x86glue.h>

void printk(const char *fmt, ...);
void x86emu_dump_xregs(void);
void pci_init(void);

int int10_handler(void);
int int1a_handler(void);

void pushw(u16 val);
unsigned char biosmem[1024 * 1024];
int verbose = 0;

u8 x_inb(u16 port)
{
	return inb(port);
}

u16 x_inw(u16 port)
{
	return inw(port);
}

u32 x_inl(u16 port)
{
	return inb(port);
}

void x_outb(u16 port, u8 val)
{
	outb(port, val);
}

void x_outw(u16 port, u16 val)
{
	outb(port, val);
}

void x_outl(u16 port, u32 val)
{
	outb(port, val);
}

X86EMU_pioFuncs myfuncs = {
	x_inb, x_inw, x_inl,
	x_outb, x_outw, x_outl
};

void irq_multiplexer(int num)
{
	int ret = 0;
	switch (num) {
	case 0x10:
	case 0x42:
	case 0x6d:
		ret = int10_handler();
		break;
	case 0x1a:
		ret = int1a_handler();
		break;
	default:
		break;
	}

	if (!ret) {
		printk("int%x not implemented\n", num);
		x86emu_dump_xregs();
	}
}

ptr current = 0;                                                                                                                   
int startrom(unsigned char *addr)
{
	X86EMU_intrFuncs intFuncs[256];
	void X86EMU_setMemBase(void *base, size_t size);
	int trace = 1;
	int i;
	
	int devfn=0x18; // FIXME
	int size=64*1024; // FIXME
	int initialcs=0xc000;
	int initialip=0x0003; 
	int base=0xc0000;

	X86EMU_setMemBase(biosmem, sizeof(biosmem));
	X86EMU_setupPioFuncs(&myfuncs);
	pci_init();
	for (i = 0; i < 256; i++)
		intFuncs[i] = irq_multiplexer;
	X86EMU_setupIntrFuncs(intFuncs);

	
	current->ax = devfn ? devfn : 0xff; // FIXME
	/* above we need to search the device on the bus */
	
	current->dx = 0x80;
	//      current->ip = 0;

	for (i = 0; i < size; i++)
		wrb(base + i, addr[i]);

	/* cpu setup */
	X86_AX = devfn ? devfn : 0xff;
	X86_DX = 0x80;
	X86_EIP = initialip;
	X86_CS = initialcs;

	/* Initialize stack and data segment */
	X86_SS = 0x0030;
	X86_DS = 0x0040;
	X86_SP = 0xfffe;

	/* We need a sane way to return from bios
	 * execution. A hlt instruction and a pointer
	 * to it, both kept on the stack, will do.
	 */

	pushw(0xf4f4);		/* hlt; hlt */
	pushw(X86_SS);
	pushw(X86_SP + 2);

	X86_ES = 0x0000;

	if (trace) {
		printk("Switching to single step mode.\n");
		X86EMU_trace_on();
	}

	X86EMU_exec();



	return 0;
}
