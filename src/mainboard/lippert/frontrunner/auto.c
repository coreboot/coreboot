#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
//#include "southbridge/intel/i440bx/i440bx_early_smbus.c"
//#include "superio/NSC/pc97317/pc97317_early_serial.c"
//#include "northbridge/intel/i440bx/raminit.h"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"

#define SERIAL_DEV PNP_DEV(0x2e, PC97317_SP1)

//#include "debug.c"
//#include "lib/delay.c"

#include "northbridge/amd/gx2/raminit.c"

static void main(unsigned long bist)
{
//	pc97317_enable_serial(SERIAL_DEV, TTYS0_BASE);
	unsigned char i;
	
#if 0
        __builtin_wrmsr(0x10000020, 0xfff80, 0x20000000);
        __builtin_wrmsr(0x10000021, 0x80fffe0, 0x20000000);
        __builtin_wrmsr(0x10000026, 0x400fffc0, 0x2cfbc040);
        __builtin_wrmsr(0x10000027, 0xfff00000, 0xff);
        __builtin_wrmsr(0x10000028, 0x7bf00100, 0x2000000f);
        __builtin_wrmsr(0x1000002c, 0xff030003, 0x20000000);
        __builtin_wrmsr(0x10000080, 0x3, 0x0);
        __builtin_wrmsr(0x20000020, 0x6, 0x0);
        __builtin_wrmsr(0x20000021, 0x3bd45, 0x3daa8);
        __builtin_wrmsr(0x20000023, 0x3df07, 0x3daa8);
        __builtin_wrmsr(0x20000024, 0x397c3, 0x3daa8);
        __builtin_wrmsr(0x20000029, 0x3b441, 0x3daa8);
        __builtin_wrmsr(0x2000002d, 0x3bea5, 0x3daa8);
        __builtin_wrmsr(0x200000e3, 0x3b441, 0x3daa8);
        __builtin_wrmsr(0x2000, 0x86002, 0x0);
        __builtin_wrmsr(0x4c002001, 0x1, 0x0);
        __builtin_wrmsr(0x50002001, 0x27, 0x0);
        __builtin_wrmsr(0x80002001, 0x86002, 0x0);
        __builtin_wrmsr(0xa0002001, 0x86002, 0x0);
        __builtin_wrmsr(0xc0002001, 0x86002, 0x0);
#endif
        __builtin_wrmsr(0x10000020, 0xfff80, 0x20000000);
        __builtin_wrmsr(0x10000021, 0x80fffe0, 0x20000000);
        __builtin_wrmsr(0x1000002c, 0xff030003, 0x20000000);
        __builtin_wrmsr(0x10000028, 0x7bf00100, 0x2000000f);
        __builtin_wrmsr(0x10000027, 0xfff00000, 0xff);
        __builtin_wrmsr(0x10000026, 0x400fffc0, 0x2cfbc040);
        __builtin_wrmsr(0x10000080, 0x3, 0x0);
        __builtin_wrmsr(0x40000020, 0xfff80, 0x20000000);
        __builtin_wrmsr(0x40000021, 0x80fffe0, 0x20000000);
        __builtin_wrmsr(0x4000002d, 0xff030003, 0x20000000);
        __builtin_wrmsr(0x40000029, 0x7bf00100, 0x2000000f);
        __builtin_wrmsr(0x40000023, 0x400fffc0, 0x20000040);
        __builtin_wrmsr(0x40000024, 0xff4ffffc, 0x200000ef);
        __builtin_wrmsr(0x400000e3, 0xf0309c10, 0x0);
        __builtin_wrmsr(0x2000, 0x86002, 0x0);
        __builtin_wrmsr(0xc0002001, 0x86002, 0x0);
        __builtin_wrmsr(0x80002001, 0x86002, 0x0);
        __builtin_wrmsr(0xa0002001, 0x86002, 0x0);
        __builtin_wrmsr(0x50002001, 0x27, 0x0);
        __builtin_wrmsr(0x4c002001, 0x1, 0x0);
        __builtin_wrmsr(0x20000010, 0x3df04, 0x449);
        __builtin_wrmsr(0x20000011, 0x4a14, 0x3bbba);
        __builtin_wrmsr(0x20000012, 0xfffff, 0xfffff);
        __builtin_wrmsr(0x20000013, 0xfffff, 0xfffff);
        __builtin_wrmsr(0x20000014, 0xfffff, 0xfffff);
        __builtin_wrmsr(0x20000015, 0xfffff, 0xfffff);
        __builtin_wrmsr(0x20000016, 0xfffff, 0xfffff);
        __builtin_wrmsr(0x20000017, 0xfffff, 0xfffff);
        __builtin_wrmsr(0x20000018, 0x3400, 0x10076013);
        __builtin_wrmsr(0x20000019, 0x696332a3, 0x18000008);
        __builtin_wrmsr(0x2000001a, 0x101, 0x0);
        __builtin_wrmsr(0x2000001b, 0x0, 0x0);
        __builtin_wrmsr(0x2000001c, 0xff00ff, 0x0);
        __builtin_wrmsr(0x2000001d, 0x0, 0x0);
        __builtin_wrmsr(0x2000001e, 0x66, 0x0);
        __builtin_wrmsr(0x2000001f, 0x0, 0x0);
        __builtin_wrmsr(0x20000020, 0x6, 0x0);




//	while(1)
	outb(0x80, 0x55);
	uart_init();
	outb(0x80, 0x56);
	console_init();
	outb(0x80, 0x57);
	while (1)
	  uart_tx_byte('h');
	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);
	
	sdram_init();
	
	/* Check all of memory */
#if 0
	ram_check(0x00000000, msr.lo);
#endif
#if 0
	static const struct {
		unsigned long lo, hi;
	} check_addrs[] = {
		/* Check 16MB of memory @ 0*/
		{ 0x00000000, 0x01000000 },
#if TOTAL_CPUS > 1
		/* Check 16MB of memory @ 2GB */
		{ 0x80000000, 0x81000000 },
#endif
	};
	int i;
	for(i = 0; i < sizeof(check_addrs)/sizeof(check_addrs[0]); i++) {
		ram_check(check_addrs[i].lo, check_addrs[i].hi);
	}
#endif
}
