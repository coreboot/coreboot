/* $XFree86: xc/programs/Xserver/hw/xfree86/int10/helper_exec.c,v 1.16 2001/04/30 14:34:57 tsi Exp $ */
/*
 *                   XFree86 int10 module
 *   execute BIOS int 10h calls in x86 real mode environment
 *                 Copyright 1999 Egbert Eich
 *
 *   Part of this is based on code taken from DOSEMU
 *   (C) Copyright 1992, ..., 1999 the "DOSEMU-Development-Team"
 */

/*
 * To debug port accesses define PRINT_PORT.
 * Note! You also have to comment out ioperm()
 * in xf86EnableIO(). Otherwise we won't trap
 * on PIO.
 */
#include <x86emu.h>
#include "test.h"
#include <asm/io.h>
#include <sys/time.h>

int
port_rep_inb(ptr pInt,
	     u16 port, u32 base, int d_f, u32 count);
u8 x_inb(u16 port);
u16 x_inw(u16 port);
void x_outb(u16 port, u8 val);
void x_outw(u16 port, u16 val);
u32 x_inl(u16 port);
void x_outl(u16 port, u32 val);

int
setup_int(ptr pInt)
{
/*
    if (pInt != current) {
	if (!MapCurrentInt10(pInt))
	    return -1;
	current = pInt;
    }
 */
    X86_EAX = (u32) pInt->ax;
    X86_EBX = (u32) pInt->bx;
    X86_ECX = (u32) pInt->cx;
    X86_EDX = (u32) pInt->dx;
    X86_ESI = (u32) pInt->si;
    X86_EDI = (u32) pInt->di;
    X86_EBP = (u32) pInt->bp;
    X86_ESP = 0x1000; X86_SS = pInt->stackseg >> 4;
    X86_EIP = 0x0600; X86_CS = 0x0;	/* address of 'hlt' */
    X86_DS = 0x40;			/* standard pc ds */
    X86_ES = pInt->es;
    X86_FS = 0;
    X86_GS = 0;
    printf("FIX ME FIX ME FIXME NO IOPL\n");
    //X86_EFLAGS = X86_IF_MASK | X86_IOPL_MASK;

	/* blocks sigio here */
    return 0;
}

void
finish_int(ptr pInt, int sig)
{
    pInt->ax = (u16) X86_EAX;
    pInt->bx = (u16) X86_EBX;
    pInt->cx = (u16) X86_ECX;
    pInt->dx = (u16) X86_EDX;
    pInt->si = (u16) X86_ESI;
    pInt->di = (u16) X86_EDI;
    pInt->es = (u16) X86_ES;
    pInt->bp = (u16) X86_EBP;
    pInt->flags = (u16) X86_FLAGS;
}

/* general software interrupt handler */
u32
getIntVect(ptr pInt,int num)
{
    return MEM_RW(pInt, num << 2) + (MEM_RW(pInt, (num << 2) + 2) << 4);
}

void
pushw(ptr pInt, u16 val)
{
    X86_ESP -= 2;
    MEM_WW(pInt, ((u32) X86_SS << 4) + X86_SP, val);
}

int
run_bios_int(int num, ptr pInt)
{
    u32 eflags;
    eflags = X86_EFLAGS;
    pushw(pInt, eflags);
    pushw(pInt, X86_CS);
    pushw(pInt, X86_IP);
    X86_CS = MEM_RW(pInt, (num << 2) + 2);
    X86_IP = MEM_RW(pInt,  num << 2);

    return 1;
}

int
port_rep_inb(ptr pInt,
	     u16 port, u32 base, int d_f, u32 count)
{
    register int inc = d_f ? -1 : 1;
    u32 dst = base;
    while (count--) {
	MEM_WB(pInt, dst, x_inb(port));
	dst += inc;
    }
    return dst - base;
}

int
port_rep_inw(ptr pInt,
	     u16 port, u32 base, int d_f, u32 count)
{
    register int inc = d_f ? -2 : 2;
    u32 dst = base;
    while (count--) {
	MEM_WW(pInt, dst, x_inw(port));
	dst += inc;
    }
    return dst - base;
}

int
port_rep_inl(ptr pInt,
	     u16 port, u32 base, int d_f, u32 count)
{
    register int inc = d_f ? -4 : 4;
    u32 dst = base;
    while (count--) {
	MEM_WL(pInt, dst, x_inl(port));
	dst += inc;
    }
    return dst - base;
}

int
port_rep_outb(ptr pInt,
	      u16 port, u32 base, int d_f, u32 count)
{
    register int inc = d_f ? -1 : 1;
    u32 dst = base;
    while (count--) {
	x_outb(port, MEM_RB(pInt, dst));
	dst += inc;
    }
    return dst - base;
}

int
port_rep_outw(ptr pInt,
	      u16 port, u32 base, int d_f, u32 count)
{
    register int inc = d_f ? -2 : 2;
    u32 dst = base;
    while (count--) {
	x_outw(port, MEM_RW(pInt, dst));
	dst += inc;
    }
    return dst - base;
}

int
port_rep_outl(ptr pInt,
	      u16 port, u32 base, int d_f, u32 count)
{
    register int inc = d_f ? -4 : 4;
    u32 dst = base;
    while (count--) {
	x_outl(port, MEM_RL(pInt, dst));
	dst += inc;
    }
    return dst - base;
}

u8
x_inb(u16 port)
{
    u8 val;

    if (port == 0x40) {
	current->inb40time++;
	val = (u8)(current->inb40time >>
		      ((current->inb40time & 1) << 3));
    } else if (port < 0x0100) {		/* Don't interfere with mainboard */
	val = 0;
    } else {
	val = inb(port);
    }
    return val;
}

void
getsecs(unsigned long *sec, unsigned long *usec)
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  *sec = tv.tv_sec;
  *usec = tv.tv_usec;
}
u16
x_inw(u16 port)
{
    u16 val;

    if (port == 0x5c) {
	long sec, usec;
	(void)getsecs(&sec, &usec);
	val = (u16)(usec / 3);
    } else {
	val = inw(port);
    }
    return val;
}

void
x_outb(u16 port, u8 val)
{
    if ((port == 0x43) && (val == 0)) {
	/*
	 * Emulate a PC's timer 0.  Such timers typically have a resolution of
	 * some .838 usec per tick, but this can only provide 1 usec per tick.
	 * (Not that this matters much, given inherent emulation delays.)  Use
	 * the bottom bit as a byte select.  See inb(0x40) above.
	 */
	long sec, usec;
	(void) getsecs(&sec, &usec);
	current->inb40time = (u16)(usec | 1);
    } else {
	outb(port, val);
    }
}

void
x_outw(u16 port, u16 val)
{

    outw(port, val);
}

u32
x_inl(u16 port)
{
    u32 val;

    val = inl(port);

    return val;
}

void
x_outl(u16 port, u32 val)
{
    outl(port, val);
}

u8
Mem_rb(int addr)
{
    return (*current->mem->rb)(current, addr);
}

u16
Mem_rw(int addr)
{
    return (*current->mem->rw)(current, addr);
}

u32
Mem_rl(int addr)
{
    return (*current->mem->rl)(current, addr);
}

void
Mem_wb(int addr, u8 val)
{
    (*current->mem->wb)(current, addr, val);
}

void
Mem_ww(int addr, u16 val)
{
    (*current->mem->ww)(current, addr, val);
}

void
Mem_wl(int addr, u32 val)
{
    (*current->mem->wl)(current, addr, val);
}


#define TAG(Cfg1Addr) (Cfg1Addr & 0xffff00)
#define OFFSET(Cfg1Addr) (Cfg1Addr & 0xff)

u8
bios_checksum(u8 *start, int size)
{
    u8 sum = 0;

    while (size-- > 0)
	sum += *start++;
    return sum;
}

/*
 * Lock/Unlock legacy VGA. Some Bioses try to be very clever and make
 * an attempt to detect a legacy ISA card. If they find one they might
 * act very strange: for example they might configure the card as a
 * monochrome card. This might cause some drivers to choke.
 * To avoid this we attempt legacy VGA by writing to all know VGA
 * disable registers before we call the BIOS initialization and
 * restore the original values afterwards. In beween we hold our
 * breath. To get to a (possibly exising) ISA card need to disable
 * our current PCI card.
 */
/*
 * This is just for booting: we just want to catch pure
 * legacy vga therefore we don't worry about mmio etc.
 * This stuff should really go into vgaHW.c. However then
 * the driver would have to load the vga-module prior to
 * doing int10.
 */
/*void
LockLegacyVGA(int screenIndex,legacyVGAPtr vga)
{
    xf86SetCurrentAccess(FALSE, xf86Screens[screenIndex]);
    vga->save_msr = inb(0x3CC);
    vga->save_vse = inb(0x3C3);
    vga->save_46e8 = inb(0x46e8);
    vga->save_pos102 = inb(0x102);
    outb(0x3C2, ~(u8)0x03 & vga->save_msr);
    outb(0x3C3, ~(u8)0x01 & vga->save_vse);
    outb(0x46e8, ~(u8)0x08 & vga->save_46e8);
    outb(0x102, ~(u8)0x01 & vga->save_pos102);
    xf86SetCurrentAccess(TRUE, xf86Screens[screenIndex]);
}

void
UnlockLegacyVGA(int screenIndex, legacyVGAPtr vga)
{
    xf86SetCurrentAccess(FALSE, xf86Screens[screenIndex]);
    outb(0x102, vga->save_pos102);
    outb(0x46e8, vga->save_46e8);
    outb(0x3C3, vga->save_vse);
    outb(0x3C2, vga->save_msr);
    xf86SetCurrentAccess(TRUE, xf86Screens[screenIndex]);
}
*/
