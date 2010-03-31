#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/serial.c"
#include "console/console.c"
#include "lib/ramtest.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/gx2def.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

#include "southbridge/amd/cs5536/cs5536_early_smbus.c"
#include "southbridge/amd/cs5536/cs5536_early_setup.c"

static inline int spd_read_byte(unsigned device, unsigned address)
{
        return smbus_read_byte(device, address);
}

#include "northbridge/amd/gx2/raminit.h"

static inline unsigned int fls(unsigned int x)
{
        int r;

        __asm__("bsfl %1,%0\n\t"
                "jnz 1f\n\t"
                "movl $32,%0\n"
                "1:" : "=r" (r) : "g" (x));
        return r;
}

/* sdram parameters for OLPC:
	row address = 13
	col address = 9
	banks = 4
	dimm0size=128MB
	d0_MB=1 (module banks)
	d0_cb=4 (component banks)
	do_psz=4KB	(page size)
	Trc=10 (clocks) (ref2act)
	Tras=7 (act2pre)
	Trcd=3 (act2cmd)
	Trp=3   (pre2act)
	Trrd=2 (act2act)
	Tref=17.8ms
  */
static void sdram_set_spd_registers(const struct mem_controller *ctrl) 
{
	/* Total size of DIMM = 2^row address (byte 3) * 2^col address (byte 4) *
	 *                      component Banks (byte 17) * module banks, side (byte 5) *
	 *                      width in bits (byte 6,7)
	 *                    = Density per side (byte 31) * number of sides (byte 5) */
	/* 1. Initialize GLMC registers base on SPD values, do one DIMM for now */
	msr_t msr;
	unsigned char module_banks, val;

	msr = rdmsr(MC_CF07_DATA);

	/* get module banks (sides) per dimm, SPD byte 5 */
	module_banks = 1;
	module_banks >>= 1;
	msr.hi &= ~(1 << CF07_UPPER_D0_MB_SHIFT);
	msr.hi |= (module_banks << CF07_UPPER_D0_MB_SHIFT);

	/* get component banks per module bank, SPD byte 17 */
	val = 4;
	val >>= 2;
	msr.hi &= ~(0x1 << CF07_UPPER_D0_CB_SHIFT);
	msr.hi |=  (val << CF07_UPPER_D0_CB_SHIFT);

	/* get the module bank density, SPD byte 31  */
	/* this is multiples of 8 MB */
	/* actually it is 2^x*4, where x is the value you put in */
	/* for OLPC, set default size */
	/* dimm size - hardcoded 128Mb */
	val = 5;
	msr.hi &= ~(0xf << CF07_UPPER_D0_SZ_SHIFT);
	msr.hi |=  (val << CF07_UPPER_D0_SZ_SHIFT);

	/* page size = 2^col address */
	val = 2; /* 4096 bytes */
	msr.hi &= ~(0x7 << CF07_UPPER_D0_PSZ_SHIFT);
	msr.hi |=  (val << CF07_UPPER_D0_PSZ_SHIFT);

	print_debug("computed msr.hi ");
	print_debug_hex32(msr.hi);
	print_debug("\r\n");

	/* this is a standard value, DOES NOT PROBABLY MATCH FROM ABOVE */
	/* well, it may be close. It's about 200,000 ticks */
	msr.lo = 0x00003000;
	wrmsr(MC_CF07_DATA, msr);

	/* timing and mode ... */

	msr = rdmsr(0x20000019);
	
	/* per standard bios settings */	

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
	/* the msr value reported by quanta is very, very different. 
	 * we will go with that value for now. 
	 */
	msr.lo = 0x286332a3;

	wrmsr(0x20000019, msr);

}

#include "northbridge/amd/gx2/raminit.c"
#include "lib/generic_sdram.c"

#define PLLMSRhi 0x00001490
#define PLLMSRlo 0x02000030
#define PLLMSRlo1 ((0xde << 16) | (1 << 26) | (1 << 24))
#define PLLMSRlo2 ((1<<14) |(1<<13) | (1<<0))
#include "northbridge/amd/gx2/pll_reset.c"
#include "cpu/amd/model_gx2/cpureginit.c"
#include "cpu/amd/model_gx2/syspreinit.c"
static void msr_init(void)
{
	__builtin_wrmsr(0x1808,  0x10f3bf00, 0x22fffc02);

	__builtin_wrmsr(0x10000020, 0xfff80, 0x20000000);
        __builtin_wrmsr(0x10000021, 0x80fffe0, 0x20000000);

        __builtin_wrmsr(0x40000020, 0xfff80, 0x20000000);
        __builtin_wrmsr(0x40000021, 0x80fffe0, 0x20000000);
}

static void gpio_init(void)
{
	unsigned long m;

	/* Make sure events enable for gpio 12 is off */

	m = inl(GPIOL_EVENTS_ENABLE);
	m &= ~GPIOL_12_SET;
	m |= GPIOL_12_CLEAR;
	outl(m, GPIOL_EVENTS_ENABLE);
}

static void main(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {(0xa<<3)|0, (0xa<<3)|1}}
	};

	SystemPreInit();
	msr_init();

	cs5536_early_setup();

	/* NOTE: must do this AFTER the early_setup!
	 * it is counting on some early MSR setup
	 * for cs5536
	 */
	cs5536_setup_onchipuart(1);
	gpio_init();
	uart_init();
	console_init();

	pll_reset();

	cpuRegInit();
	print_err("done cpuRegInit\n");
	
	sdram_initialize(1, memctrl);
	
	/* Check all of memory */
	//ram_check(0x00000000, 640*1024);
}

