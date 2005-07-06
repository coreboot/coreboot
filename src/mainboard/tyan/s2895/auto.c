#define ASSEMBLY 1
 
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

#include "northbridge/amd/amdk8/cpu_rev.c"
#define K8_HT_FREQ_1G_SUPPORT 1
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "southbridge/nvidia/ck804/ck804_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "cpu/amd/model_fxx/model_fxx_msr.h"
#include "superio/smsc/lpc47b397/lpc47b397_early_serial.c"

#include "cpu/amd/mtrr/amd_earlymtrr.c"
#include "cpu/x86/bist.h"

#include "superio/smsc/lpc47b397/lpc47b397_early_gpio.c"

#include "northbridge/amd/amdk8/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, LPC47B397_SP1)

static void hard_reset(void)
{
        set_bios_reset();

        /* full reset */
	outb(0x0a, 0x0cf9);
        outb(0x0e, 0x0cf9);
}

static void soft_reset(void)
{
        set_bios_reset();
#if 1
        /* link reset */
	outb(0x02, 0x0cf9);
        outb(0x06, 0x0cf9);
#endif
}

static void memreset_setup(void)
{
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
}

#define SUPERIO_GPIO_DEV PNP_DEV(0x2e, LPC47B397_RT)
        
#define SUPERIO_GPIO_IO_BASE 0x400

static void sio_gpio_setup(void){

	unsigned value;

#if 1
	/*Enable onboard scsi*/
	lpc47b397_gpio_offset_out(SUPERIO_GPIO_IO_BASE, 0x2c, (1<<7)|(0<<2)|(0<<1)|(0<<0)); // GP21, offset 0x2c, DISABLE_SCSI_L 
	value = lpc47b397_gpio_offset_in(SUPERIO_GPIO_IO_BASE, 0x4c);
	lpc47b397_gpio_offset_out(SUPERIO_GPIO_IO_BASE, 0x4c, (value|(1<<1)));
#endif
	
}

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
	/* nothing to do */
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#define K8_4RANK_DIMM_SUPPORT 1

#include "northbridge/amd/amdk8/raminit.c"
#if 0
        #define ENABLE_APIC_EXT_ID 1
        #define APIC_ID_OFFSET 0x10
        #define LIFT_BSP_APIC_ID 0
#else
        #define ENABLE_APIC_EXT_ID 0
#endif
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

/* tyan does not want the default */
#include "resourcemap.c"

#if CONFIG_LOGICAL_CPUS==1
#define SET_NB_CFG_54 1
#include "cpu/amd/dualcore/dualcore.c"
#else
#include "cpu/amd/model_fxx/node_id.c"
#endif

#define FIRST_CPU  1
#define SECOND_CPU 1
#define TOTAL_CPUS (FIRST_CPU + SECOND_CPU)

#define CK804_NUM 2
#define CK804B_BUSN 0xc
#define CK804_USE_NIC 1
#define CK804_USE_ACI 1
#include "southbridge/nvidia/ck804/ck804_early_setup.h"
#include "southbridge/nvidia/ck804/ck804_early_setup_ss.h"

//set GPIO to input mode
#define CK804_MB_SETUP \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+ 5, ~(0xff),((0<<4)|(0<<2)|(0<<0)),/* M9,GPIO6, PCIXB2_PRSNT1_L*/  \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+15, ~(0xff),((0<<4)|(0<<2)|(0<<0)),/* M8,GPIO16, PCIXB2_PRSNT2_L*/ \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+44, ~(0xff),((0<<4)|(0<<2)|(0<<0)),/* P5,GPIO45, PCIXA_PRSNT1_L*/  \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+ 7, ~(0xff),((0<<4)|(0<<2)|(0<<0)),/* M5,GPIO8, PCIXA_PRSNT2_L*/   \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+16, ~(0xff),((0<<4)|(0<<2)|(0<<0)),/* K4,GPIO17, PCIXB_PRSNT1_L*/  \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+45, ~(0xff),((0<<4)|(0<<2)|(0<<0)),/* P7,GPIO46, PCIXB_PRSNT2_L*/
		
#include "southbridge/nvidia/ck804/ck804_early_setup.c"


static void main(unsigned long bist)
{
	static const struct mem_controller cpu[] = {
#if FIRST_CPU
		{
			.node_id = 0,
			.f0 = PCI_DEV(0, 0x18, 0),
			.f1 = PCI_DEV(0, 0x18, 1),
			.f2 = PCI_DEV(0, 0x18, 2),
			.f3 = PCI_DEV(0, 0x18, 3),
			.channel0 = { (0xa<<3)|0, (0xa<<3)|2, 0, 0 },
			.channel1 = { (0xa<<3)|1, (0xa<<3)|3, 0, 0 },
		},
#endif
#if SECOND_CPU
		{
			.node_id = 1,
			.f0 = PCI_DEV(0, 0x19, 0),
			.f1 = PCI_DEV(0, 0x19, 1),
			.f2 = PCI_DEV(0, 0x19, 2),
			.f3 = PCI_DEV(0, 0x19, 3),
			.channel0 = { (0xa<<3)|4, (0xa<<3)|6, 0, 0 },
			.channel1 = { (0xa<<3)|5, (0xa<<3)|7, 0, 0 },
		},
#endif
	};

        int needs_reset;
#if CONFIG_LOGICAL_CPUS==1
        struct node_core_id id;
#else
        unsigned nodeid;
#endif

        if (bist == 0) {
                /* Skip this if there was a built in self test failure */
                amd_early_mtrr_init();

#if CONFIG_LOGICAL_CPUS==1
                set_apicid_cpuid_lo();
                
                id = get_node_core_id_x(); // that is initid
        #if ENABLE_APIC_EXT_ID == 1
                if(id.coreid == 0) {
                        enable_apic_ext_id(id.nodeid);
                }
        #endif
#else           
                nodeid = get_node_id();
        #if ENABLE_APIC_EXT_ID == 1
                enable_apic_ext_id(nodeid);
        #endif
#endif

                enable_lapic();
                init_timer();


#if CONFIG_LOGICAL_CPUS==1
        #if ENABLE_APIC_EXT_ID == 1
            #if LIFT_BSP_APIC_ID == 0
                if( id.nodeid != 0 ) 
            #endif
                        lapic_write(LAPIC_ID, ( lapic_read(LAPIC_ID) | (APIC_ID_OFFSET<<24) ) );
        #endif

                if(id.coreid == 0) {
                        if (cpu_init_detected(id.nodeid)) {
                                asm volatile ("jmp __cpu_reset");
                        }
                        distinguish_cpu_resets(id.nodeid);
                }

#else
        #if ENABLE_APIC_EXT_ID == 1
            #if LIFT_BSP_APIC_ID == 0
                if(nodeid != 0)
            #endif
                        lapic_write(LAPIC_ID, ( lapic_read(LAPIC_ID) | (APIC_ID_OFFSET<<24) ) ); 

        #endif

                if (cpu_init_detected(nodeid)) {
                        asm volatile ("jmp __cpu_reset");
                }
                distinguish_cpu_resets(nodeid);
#endif


                if (!boot_cpu()
#if CONFIG_LOGICAL_CPUS==1 
                        || (id.coreid != 0)
#endif
                ) {
                        stop_this_cpu(); 
                }
        }


        lpc47b397_enable_serial(SERIAL_DEV, TTYS0_BASE);
        uart_init();
        console_init();
	
	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	sio_gpio_setup();

        setup_s2895_resource_map();

	needs_reset = setup_coherent_ht_domain();
#if CONFIG_LOGICAL_CPUS==1
        start_other_cores();
#endif

        needs_reset |= ht_setup_chains_x();

	needs_reset |= ck804_early_setup_x();	

       	if (needs_reset) {
               	print_info("ht reset -\r\n");
               	soft_reset();
       	}


	enable_smbus();

	memreset_setup();
	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);


}
