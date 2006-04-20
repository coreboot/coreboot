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
	module_banks = spd_read_byte(0xA0, 5);
	if (module_banks < 1 || module_banks > 2)
		print_err("Module banks per dimm\r\n");
	module_banks >>= 1;
	msr.hi &= ~(1 << CF07_UPPER_D0_MB_SHIFT);
	msr.hi |= (module_banks << CF07_UPPER_D0_MB_SHIFT);

	/* get component banks per module bank, SPD byte 17 */
	val = spd_read_byte(0xA0, 17);
	if (val < 2 || val > 4)
		print_err("Component banks per module bank\r\n");
	val >>= 2;
	msr.hi &= ~(0x1 << CF07_UPPER_D0_CB_SHIFT);
	msr.hi |=  (val << CF07_UPPER_D0_CB_SHIFT);

	/* get the module bank density, SPD byte 31  */
	val = spd_read_byte(0xA0, 31);
	val = fls(val);
	val <<= module_banks;
	msr.hi &= ~(0xf << CF07_UPPER_D0_SZ_SHIFT);
	msr.hi |=  (val << CF07_UPPER_D0_SZ_SHIFT);

	/* page size = 2^col address */
	val = spd_read_byte(0xA0, 4);
	val -= 7;
	msr.hi &= ~(0x7 << CF07_UPPER_D0_PSZ_SHIFT);
	msr.hi |=  (val << CF07_UPPER_D0_PSZ_SHIFT);

	print_debug("computed msr.hi ");
	print_debug_hex32(msr.hi);
	print_debug("\r\n");

	msr.lo = 0x00003000;
	wrmsr(MC_CF07_DATA, msr);

	msr = rdmsr(0x20000019);
	msr.hi = 0x18000108;
	msr.lo = 0x696332a3;
	wrmsr(0x20000019, msr);		

}

#include "northbridge/amd/gx2/raminit.c"
#include "sdram/generic_sdram.c"

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


static void main(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {(0xa<<3)|0, (0xa<<3)|1}}
	};

	SystemPreInit();
	msr_init();

	w83627hf_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	cs5536_early_setup();

	pll_reset();

	cpuRegInit();
	print_err("done cpuRegInit\n");
	
	sdram_initialize(1, memctrl);

	/* Check all of memory */
	//ram_check(0x00000000, 640*1024);
}
