#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <cpu/p6/apic.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "northbridge/amd/amdk8/early_ht.c"
#include "southbridge/amd/amd8111/amd8111_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/k8/apic_timer.c"
#include "lib/delay.c"
#include "cpu/p6/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "debug.c"
#include "northbridge/amd/amdk8/cpu_rev.c"


#define REV_B_RESET 0
static void memreset_setup(void)
{
#if REV_B_RESET==1
        outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 16);  //REVC_MEMRST_EN=0
#else
        outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 16);  //REVC_MEMRST_EN=1
#endif
        outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 17); 
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
        udelay(800);
#if REV_B_RESET==1
        outb((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 17); //REVB_MEMRST_L=1
#endif
        udelay(90);
}

static unsigned int generate_row(uint8_t node, uint8_t row, uint8_t maxnodes)
{
	/* Routing Table Node i 
	 *
	 * F0: 0x40, 0x44, 0x48, 0x4c, 0x50, 0x54, 0x58, 0x5c 
	 *  i:    0,    1,    2,    3,    4,    5,    6,    7
	 *
	 * [ 0: 3] Request Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [11: 8] Response Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [19:16] Broadcast route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 */
        uint32_t ret=0x00010101; /* default row entry */


        static const unsigned int rows_4p[4][4] = {
                { 0x00070101, 0x00010202, 0x00030404, 0x00010204 },
                { 0x00010202, 0x000b0101, 0x00010208, 0x00030808 },
                { 0x00030808, 0x00010208, 0x000b0101, 0x00010202 },
                { 0x00010204, 0x00030404, 0x00010202, 0x00070101 }
        };
        
        if (!(node>=maxnodes || row>=maxnodes)) {
		ret=rows_4p[node][row];
        }

        return ret;
}

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_HUB 0x18
        unsigned device=(ctrl->channel0[0])>>8;
        smbus_write_byte(SMBUS_HUB , 0x01, device);
        smbus_write_byte(SMBUS_HUB , 0x03, 0);
}
#if 0
static inline void change_i2c_mux(unsigned device)
{
#define SMBUS_HUB 0x18
        smbus_write_byte(SMBUS_HUB , 0x01, device);
        smbus_write_byte(SMBUS_HUB , 0x03, 0);
}
#endif

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

/* include mainboard specific ht code */
#include "hypertransport.c"

//#include "northbridge/amd/amdk8/cpu_ldtstop.c"
//#include "southbridge/amd/amd8111/amd8111_ldtstop.c"

#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

#include "resourcemap.c" /* tyan does not want the default */

static void enable_lapic(void)
{
	msr_t msr;
	msr = rdmsr(0x1b);
	msr.hi &= 0xffffff00;
	msr.lo &= 0x000007ff;
	msr.lo |= APIC_DEFAULT_BASE | (1 << 11);
	wrmsr(0x1b, msr);
}

static void stop_this_cpu(void)
{
	unsigned apicid;
	apicid = apic_read(APIC_ID) >> 24;

	/* Send an APIC INIT to myself */
	apic_write(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));
	apic_write(APIC_ICR, APIC_INT_LEVELTRIG | APIC_INT_ASSERT | APIC_DM_INIT);
	/* Wait for the ipi send to finish */
	apic_wait_icr_idle();

	/* Deassert the APIC INIT */
	apic_write(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));
	apic_write(APIC_ICR,  APIC_INT_LEVELTRIG | APIC_DM_INIT);
	/* Wait for the ipi send to finish */
	apic_wait_icr_idle();

	/* If I haven't halted spin forever */
	for(;;) {
		hlt();
	}
}
#define FIRST_CPU  1
#define SECOND_CPU 1

#define THIRD_CPU  1 
#define FOURTH_CPU 1 

#define TOTAL_CPUS (FIRST_CPU + SECOND_CPU + THIRD_CPU + FOURTH_CPU)

#define RC0 ((1<<1)<<8)
#define RC1 ((1<<2)<<8)
#define RC2 ((1<<3)<<8)
#define RC3 ((1<<4)<<8)

#define DIMM0 0x50
#define DIMM1 0x51
#define DIMM2 0x52
#define DIMM3 0x53
        
static void main(void)
{
	static const struct mem_controller cpu[] = {
#if FIRST_CPU
                {
                        .node_id = 0,
                        .f0 = PCI_DEV(0, 0x18, 0),
                        .f1 = PCI_DEV(0, 0x18, 1),
                        .f2 = PCI_DEV(0, 0x18, 2),
                        .f3 = PCI_DEV(0, 0x18, 3),
                        .channel0 = { RC0|DIMM0, RC0|DIMM2, 0, 0 },
                        .channel1 = { RC0|DIMM1, RC0|DIMM3, 0, 0 },
                },
#endif
#if SECOND_CPU
                {
                        .node_id = 1,
                        .f0 = PCI_DEV(0, 0x19, 0),
                        .f1 = PCI_DEV(0, 0x19, 1),
                        .f2 = PCI_DEV(0, 0x19, 2),
                        .f3 = PCI_DEV(0, 0x19, 3),
                        .channel0 = { RC1|DIMM0, 0 , 0, 0 },
                        .channel1 = { RC1|DIMM1, 0, 0, 0 },

                },
#endif

#if THIRD_CPU
                {
                        .node_id = 2,
                        .f0 = PCI_DEV(0, 0x1a, 0),
                        .f1 = PCI_DEV(0, 0x1a, 1),
                        .f2 = PCI_DEV(0, 0x1a, 2),
                        .f3 = PCI_DEV(0, 0x1a, 3),
                        .channel0 = { RC2|DIMM0, 0, 0, 0 },
                        .channel1 = { RC2|DIMM1, 0, 0, 0 },

                },
#endif
#if FOURTH_CPU
                {
                        .node_id = 3,
                        .f0 = PCI_DEV(0, 0x1b, 0),
                        .f1 = PCI_DEV(0, 0x1b, 1),
                        .f2 = PCI_DEV(0, 0x1b, 2),
                        .f3 = PCI_DEV(0, 0x1b, 3),
                        .channel0 = { RC3|DIMM0, 0, 0, 0 },
                        .channel1 = { RC3|DIMM1, 0, 0, 0 },

                },
#endif
	};
	int i;
	if (cpu_init_detected()) {
		asm("jmp __cpu_reset");
	}
	enable_lapic();
	init_timer();
	if (!boot_cpu() ) {
//		notify_bsp_ap_is_stopped();
		stop_this_cpu();
	}
	uart_init();
	console_init();
	setup_s4880_resource_map();
	setup_coherent_ht_domain();
	enumerate_ht_chain(0);
	distinguish_cpu_resets(0);
	
#if 0
	dump_pci_devices();
#endif
	enable_smbus();
#if 0

//	activate_spd_rom(&cpu[0]); 
//	dump_spd_registers(&cpu[0]);

//	for(i=0;i<4;i++) {
//		activate_spd_rom(&cpu[i]); 
//        	dump_smbus_registers();
//	}
        for(i=1;i<256;i=i*2) {
                change_i2c_mux(i);
                dump_smbus_registers();
        }

#endif
	memreset_setup();
	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);

#if 0
	dump_pci_devices();
#endif
#if 0
	dump_pci_device(PCI_DEV(0, 0x18, 1));
#endif

	/* Check all of memory */
#if 0
	msr_t msr;
	msr = rdmsr(TOP_MEM2);
	print_debug("TOP_MEM2: ");
	print_debug_hex32(msr.hi);
	print_debug_hex32(msr.lo);
	print_debug("\r\n");
#endif
/*
#if  0
	ram_check(0x00000000, msr.lo+(msr.hi<<32));
#else
#if TOTAL_CPUS < 2
	// Check 16MB of memory @ 0
	ram_check(0x00000000, 0x01000000);
#else
	// Check 16MB of memory @ 2GB 
	ram_check(0x80000000, 0x81000000);
#endif
#endif
*/
}
