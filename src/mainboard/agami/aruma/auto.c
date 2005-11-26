#define ASSEMBLY 1
#define ENABLE_APIC_EXT_ID 1
#define APIC_ID_OFFSET 0x10
#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <arch/cpu.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "southbridge/amd/amd8111/amd8111_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/cpu_rev.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"
#include "cpu/amd/mtrr/amd_earlymtrr.c"
#include "cpu/x86/bist.h"
#include "cpu/amd/dualcore/dualcore.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

static void hard_reset(void)
{
	set_bios_reset();
	// pci_write_config8(PCI_DEV(0, 0x04, 3), 0x41, 0xf1);
	// outb(0x0e, 0x0cf9);
	outb(0x06, 0x0cf9); /* this value will assert RESET_L and LDTRST_L */
}

static void soft_reset(void)
{
	set_bios_reset();
	pci_write_config8(PCI_DEV(0, 0x04, 0), 0x47, 1);
}

/*
 * GPIO28 of 8111 will control H0_MEMRESET_L
 * GPIO29 of 8111 will control H1_MEMRESET_L
 */
static void memreset_setup(void)
{
	if (is_cpu_pre_c0()) {
		/* Set the memreset low */
		outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), 
				SMBUS_IO_BASE + 0xc0 + 28);
		/* Ensure the BIOS has control of the memory lines */
		outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0),
				SMBUS_IO_BASE + 0xc0 + 29);
	} else {
		/* Ensure the CPU has controll of the memory lines */
		outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(1<<0),
				SMBUS_IO_BASE + 0xc0 + 29);
	}
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
	if (is_cpu_pre_c0()) {
		udelay(800);
		/* Set memreset_high */
		outb((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), 
				SMBUS_IO_BASE + 0xc0 + 28);
		udelay(90);
	}
}

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_SWITCH1 0x71
#define SMBUS_SWITCH2 0x73
	/* Switch 1: pca 9545, Switch 2: pca 9543 */
	unsigned device = (ctrl->channel0[0]) >> 8;
	/* Disable all outputs on SMBus switch 1 */
	smbus_send_byte(SMBUS_SWITCH1, 0x0);
	/* Select SMBus switch 2 Channel 0/1 */
	smbus_send_byte(SMBUS_SWITCH2, device);
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"
#include "resourcemap.c"

#define CHAN0 0x100
#define CHAN1 0x200

#define NODE_RAM(x) 			\
	.node_id = 0+x,			\
	.f0 = PCI_DEV(0, 0x18+x, 0),	\
	.f1 = PCI_DEV(0, 0x18+x, 1),	\
	.f2 = PCI_DEV(0, 0x18+x, 2),	\
	.f3 = PCI_DEV(0, 0x18+x, 3)

static void main(unsigned long bist)
{
	static const struct mem_controller cpu[] = {
	{	 NODE_RAM(0),
		.channel0 = { (0xa0>>1)|CHAN0, (0xa4>>1)|CHAN0, 0, 0 },
		.channel1 = { (0xa2>>1)|CHAN0, (0xa6>>1)|CHAN0, 0, 0 } 
	},
	{	NODE_RAM(1),
		.channel0 = { (0xa8>>1)|CHAN0, (0xac>>1)|CHAN0, 0, 0 },
		.channel1 = { (0xaa>>1)|CHAN0, (0xae>>1)|CHAN0, 0, 0 }
	},
	{	NODE_RAM(2),
		.channel0 = { (0xa8>>1)|CHAN1, (0xac>>1)|CHAN1, 0, 0 },
		.channel1 = { (0xaa>>1)|CHAN1, (0xae>>1)|CHAN1, 0, 0 }
	},
	{	NODE_RAM(3),
		.channel0 = { (0xa0>>1)|CHAN1, (0xa4>>1)|CHAN1, 0, 0 },
		.channel1 = { (0xa2>>1)|CHAN1, (0xa6>>1)|CHAN1, 0, 0 }
	}
	};

	int needs_reset;

	if (bist == 0) {
	    	k8_init_and_stop_secondaries();
	}
	/* Setup the console */
	w83627hf_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	setup_aruma_resource_map();
	needs_reset = setup_coherent_ht_domain();
	needs_reset=ht_setup_chains_x();

#if 0
	dump_pci_devices();
#endif
#if 0
        print_pci_devices();
#endif
	
//#if (ALLOW_HT_OVERCLOCKING==1) && (USE_FALLBACK_IMAGE==0)
//	if(read_option(CMOS_VSTART_amdk8_1GHz, CMOS_VLEN_amdk8_1GHz, 0)) 
//	{
//		print_debug("AMDK8 allowed at 1GHz\r\n");
//	} else {
//		print_debug("AMDK8 allowed at 800Hz only\r\n");
//	}
//	if(read_option(CMOS_VSTART_amd8131_800MHz, CMOS_VLEN_amd8131_800MHz, 0))
//	{
//		print_debug("AMD8131 allowed at 800MHz\r\n");
//	} else {
//		print_debug("AMD8131 allowed at 600Hz only\r\n");
//	}
//#endif
	if (needs_reset) {
		print_info("HyperT reset -\r\n");
		soft_reset();
	}

	enable_smbus();

	memreset_setup();
	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);

#if 0
	/* Check the first 1M */
	ram_check(0x00000000, 0x000100000);
#endif
}
