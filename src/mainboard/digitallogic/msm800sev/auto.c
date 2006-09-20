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
//#include "superio/winbond/w83627hf/w83627hf_early_serial.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/lxdef.h>

//#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

#include "southbridge/amd/cs5536/cs5536_early_smbus.c"
#include "southbridge/amd/cs5536/cs5536_early_setup.c"

static inline int spd_read_byte(unsigned device, unsigned address)
{
        return smbus_read_byte(device, address);
}

#include "northbridge/amd/lx/raminit.h"

static inline unsigned int fls(unsigned int x)
{
        int r;

        __asm__("bsfl %1,%0\n\t"
                "jnz 1f\n\t"
                "movl $32,%0\n"
                "1:" : "=r" (r) : "g" (x));
        return r;
}



static void sdram_set_spd_registers(const struct mem_controller *ctrl) 
{
	/* Total size of DIMM = 2^row address (byte 3) * 2^col address (byte 4) *
	 *                      component Banks (byte 17) * module banks, side (byte 5) *
	 *                      width in bits (byte 6,7)
	 *                    = Density per side (byte 31) * number of sides (byte 5) */
	/* 1. Initialize GLMC registers base on SPD values, do one DIMM for now */
	msr_t msr;
	unsigned char module_banks, val;

	
	msr.hi = 0x10075012;
	msr.lo = 0x00000040;
	
	wrmsr(MC_CF07_DATA, msr);		//GX3

	/* timing and mode ... */

	//msr = rdmsr(0x20000019);
	
	/* per standard bios settings */	
/*
	msr.hi = 0x18000108;
	msr.lo = 
			(6<<28) |		// cas_lat
			(10<<24)|		// ref2act
			(7<<20)|		// act2pre
			(3<<16)|		// pre2act
			(3<<12)|		// act2cmd
			(2<<8)|			// act2act
			(2<<6)|			// dplwr
			(2<<4)|			// dplrd
			(3);			// dal
	* the msr value reported by quanta is very, very different. 
	 * we will go with that value for now. 
	 *
	//msr.lo = 0x286332a3;
*/
	//wrmsr(0x20000019, msr);	//GX3

}

#include "northbridge/amd/lx/raminit.c"
#include "sdram/generic_sdram.c"

/* CPU and GLIU mult/div */
#define PLLMSRhi 0x0000039C
/* Hold Count - how long we will sit in reset */
#define PLLMSRlo 0x00DE0000

#include "northbridge/amd/lx/pll_reset.c"
#include "cpu/amd/model_lx/cpureginit.c"
#include "cpu/amd/model_lx/syspreinit.c"
static void msr_init(void)
{

	__builtin_wrmsr(0x10000020, 0xfff80, 0x20000000);
        __builtin_wrmsr(0x10000021, 0x80fffe0, 0x20000000);

        __builtin_wrmsr(0x40000020, 0xfff80, 0x20000000);
        __builtin_wrmsr(0x40000021, 0x80fffe0, 0x20000000);
}

	
static void main(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {(0xa<<3)|0, (0xa<<3)|1}}
	};

	SystemPreInit();			//GX3 OK

	msr_init();					//GX3 OK

	cs5536_early_setup();		//GX3 OK

	/* NOTE: must do this AFTER the early_setup!
	 * it is counting on some early MSR setup
	 * for cs5536
	 */
	cs5536_setup_onchipuart();	//GX3 OK

	uart_init();				//GX3 OK
	console_init();				//GX3 OK

	pll_reset();				//GX3 OK

	cpuRegInit();				//GX3 OK

	print_err("done cpuRegInit\n");
	
	sdram_initialize(1, memctrl);	//GX3 OK almost
	
	/* Check all of memory */
	//ram_check(0x00000000, 640*1024);
}
