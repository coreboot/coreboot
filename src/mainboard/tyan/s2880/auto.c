#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include "arch/romcc_io.h"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "northbridge/amd/amdk8/early_ht.c"
#include "southbridge/amd/amd8111/amd8111_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
/*
#warning "FIXME move these delay functions somewhere more appropriate"
#warning "FIXME use the apic timer instead it needs no calibration on an Opteron it runs at 200Mhz"
static void print_clock_multiplier(void)
{
        msr_t msr;
        print_debug("clock multipler: 0x");
        msr = rdmsr(0xc0010042);
        print_debug_hex32(msr.lo & 0x3f);
        print_debug(" = 0x");
        print_debug_hex32(((msr.lo & 0x3f) + 8) * 100);
        print_debug("Mhz\r\n");
}

static unsigned usecs_to_ticks(unsigned usecs)
{
#warning "FIXME make usecs_to_ticks work properly"
#if 1
        return usecs *2000;
#else
        // This can only be done if cpuid says fid changing is supported
        // I need to look up the base frequency another way for other
        // cpus.  Is it worth dedicating a global register to this?
        // Are the PET timers useable for this purpose?
         
        msr_t msr;
        msr = rdmsr(0xc0010042);
        return ((msr.lo & 0x3f) + 8) * 100 *usecs;
#endif
}

static void init_apic_timer(void)
{
        volatile uint32_t *apic_reg = (volatile uint32_t *)0xfee00000;
        uint32_t start, end;
        // Set the apic timer to no interrupts and periodic mode 
        apic_reg[0x320 >> 2] = (1 << 17)|(1<< 16)|(0 << 12)|(0 << 0);
        // Set the divider to 1, no divider 
        apic_reg[0x3e0 >> 2] = (1 << 3) | 3;
        // Set the initial counter to 0xffffffff 
        apic_reg[0x380 >> 2] = 0xffffffff;
}

static void udelay(unsigned usecs)
{
#if 1
        uint32_t start, ticks;
        tsc_t tsc;
        // Calculate the number of ticks to run for 
        ticks = usecs_to_ticks(usecs);
        // Find the current time 
        tsc = rdtsc();
        start = tsc.lo;
        do {
                tsc = rdtsc();
        } while((tsc.lo - start) < ticks);
#else
        volatile uint32_t *apic_reg = (volatile uint32_t *)0xfee00000;
        uint32_t start, value, ticks;
        // Calculate the number of ticks to run for 
        ticks = usecs * 200;
        start = apic_reg[0x390 >> 2];
        do {
                value = apic_reg[0x390 >> 2];
        } while((start - value) < ticks);
#endif
}

static void mdelay(unsigned msecs)
{
        int i;
        for(i = 0; i < msecs; i++) {
                udelay(1000);
        }
}

static void delay(unsigned secs)
{
        int i;
        for(i = 0; i < secs; i++) {
                mdelay(1000);
        }
}

static void memreset_setup(const struct mem_controller *ctrl)
{
        // Set the memreset low 
        outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 28);
        // Ensure the BIOS has control of the memory lines 
        outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 29);
        print_debug("memreset lo\r\n");
}

static void memreset(const struct mem_controller *ctrl)
{
        udelay(800);
        // Set memreset_high 
        outb((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 28);
        print_debug("memreset hi\r\n");
        udelay(50);
}
*/              

#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

#define NODE_ID         0x60
#define HT_INIT_CONTROL 0x6c
 
#define HTIC_ColdR_Detect  (1<<4)
#define HTIC_BIOSR_Detect  (1<<5)
#define HTIC_INIT_Detect   (1<<6)
 
#define APIC_DEFAULT_BASE 0xfee00000

#define APIC_ID         0x020

static int boot_cpu(void)
{
	volatile unsigned long *local_apic;
	unsigned long apic_id;
	int bsp;
	int apicEn;
	msr_t msr;
	msr = rdmsr(0x1b);
	bsp = !!(msr.lo & (1 << 8));
	apicEn = !!(msr.lo & (1<<11));
	if(apicEn) {
		print_debug("apic enabled\r\n");
	} else {
		msr.lo |= (1<<11);
		wrmsr(0x1b,msr);
	}
        apic_id = *((volatile unsigned long *)(APIC_DEFAULT_BASE+APIC_ID));
        print_debug("apic_id: ");
        print_debug_hex32(apic_id>>24);
        print_debug("\r\n");
	
	if (bsp) {
		print_debug("Bootstrap cpu\r\n");
	} else {
                print_debug("Application processor\r\n");
	//	asm("hlt"); // move to end before halt should notify BSP
		   // if you start AP in coherent.c you can just stop it here	
        }

	return bsp;
}

static int cpu_init_detected(void)
{
	unsigned long dcl;
	int cpu_init;

	unsigned long htic;

	htic = pci_read_config32(PCI_DEV(0, 0x18, 0), HT_INIT_CONTROL);
#if 0
	print_debug("htic: ");
	print_debug_hex32(htic);
	print_debug("\r\n");

	if (!(htic & HTIC_ColdR_Detect)) {
		print_debug("Cold Reset.\r\n");
	}
	if ((htic & HTIC_ColdR_Detect) && !(htic & HTIC_BIOSR_Detect)) {
		print_debug("BIOS generated Reset.\r\n");
	}
	if (htic & HTIC_INIT_Detect) {
		print_debug("Init event.\r\n");
	}
#endif
	cpu_init = (htic & HTIC_INIT_Detect);
	if (cpu_init) {
		print_debug("CPU INIT Detected.\r\n");
	}
	return cpu_init;
}
/*
static void print_debug_pci_dev(unsigned dev)
{
        print_debug("PCI: ");
        print_debug_hex8((dev >> 16) & 0xff);
        print_debug_char(':');
        print_debug_hex8((dev >> 11) & 0x1f);
        print_debug_char('.');
        print_debug_hex8((dev >> 8) & 7);
}


static void print_pci_devices(void)
{
	device_t dev;
	for(dev = PCI_DEV(0, 0, 0); 
		dev <= PCI_DEV(0, 0x1f, 0x7); 
		dev += PCI_DEV(0,0,1)) {
		uint32_t id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
                print_debug_pci_dev(dev);
                print_debug("\r\n");
	}
}
*/
/*
static void dump_pci_device(unsigned dev)
{
        int i;
        print_debug_pci_dev(dev);
        print_debug("\r\n");

        for(i = 0; i <= 255; i++) {
                unsigned char val;
                if ((i & 0x0f) == 0) {
                        print_debug_hex8(i);
                        print_debug_char(':');
                }
                val = pci_read_config8(dev, i);
                print_debug_char(' ');
                print_debug_hex8(val);
                if ((i & 0x0f) == 0x0f) {
                        print_debug("\r\n");
                }
        }
}
static void dump_pci_devices(void)
{
        device_t dev;
        for(dev = PCI_DEV(0, 0, 0);
                dev <= PCI_DEV(0, 0x1f, 0x7);
                dev += PCI_DEV(0,0,1)) {
                uint32_t id;
                id = pci_read_config32(dev, PCI_VENDOR_ID);
                if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0x0000)) {
                        continue;
                }
                dump_pci_device(dev);
        }
}



static void dump_spd_registers(const struct mem_controller *ctrl)
{
        int i;
        print_debug("\r\n");
        for(i = 0; i < 4; i++) {
                unsigned device;
                device = ctrl->channel0[i];
                if (device) {
                        int j;
                        print_debug("dimm: ");
                        print_debug_hex8(i);
                        print_debug(".0: ");
                        print_debug_hex8(device);
                        for(j = 0; j < 256; j++) {
                                int status;
                                unsigned char byte;
                                if ((j & 0xf) == 0) {
                                        print_debug("\r\n");
                                        print_debug_hex8(j);
                                        print_debug(": ");
                                }
                                status = smbus_read_byte(device, j);
                                if (status < 0) {
                                        print_debug("bad device\r\n");
     
                                        break;
                                }
                                byte = status & 0xff;
                                print_debug_hex8(byte);
                                print_debug_char(' ');
                        }
                        print_debug("\r\n");
                }
                device = ctrl->channel1[i];
                if (device) {
                        int j;
                        print_debug("dimm: ");
                        print_debug_hex8(i);
                        print_debug(".1: ");
                        print_debug_hex8(device);
                        for(j = 0; j < 256; j++) {
                                int status;
                                unsigned char byte;
                                if ((j & 0xf) == 0) {
                                        print_debug("\r\n");
                                        print_debug_hex8(j);
                                        print_debug(": ");
                                }
                                status = smbus_read_byte(device, j);
      
                                if (status < 0) {
                                        print_debug("bad device\r\n");
                                        break;
                                }
                                byte = status & 0xff;
                                print_debug_hex8(byte);
                                print_debug_char(' ');
                        }
                        print_debug("\r\n");
                }
        }
}

*/



static void main(void)
{
        static const struct mem_controller cpu0 = {
                .f0 = PCI_DEV(0, 0x18, 0),
                .f1 = PCI_DEV(0, 0x18, 1),
                .f2 = PCI_DEV(0, 0x18, 2),
                .f3 = PCI_DEV(0, 0x18, 3),
                .channel0 = { (0xa<<3)|0, (0xa<<3)|2, 0, 0 },
                .channel1 = { (0xa<<3)|1, (0xa<<3)|3, 0, 0 },
        };
        static const struct mem_controller cpu1 = {
                .f0 = PCI_DEV(0, 0x19, 0),
                .f1 = PCI_DEV(0, 0x19, 1),
                .f2 = PCI_DEV(0, 0x19, 2),
                .f3 = PCI_DEV(0, 0x19, 3),
                .channel0 = { (0xa<<3)|4, (0xa<<3)|6, 0, 0 },
                .channel1 = { (0xa<<3)|5, (0xa<<3)|7, 0, 0 },
        };

 //               device_t dev;
 //               unsigned where;
                unsigned long reg;
//                dev = PCI_ADDR(0, 0x19, 0, 0x6C) & ~0xff;
//                where = PCI_ADDR(0, 0x19, 0, 0x6C) & 0xff;
#if 0
                init_apic_timer();
#endif
  
	uart_init();
	console_init();
	if (boot_cpu() && !cpu_init_detected()) {
		setup_default_resource_map();
		setup_coherent_ht_domain();
		enumerate_ht_chain();
//		print_pci_devices();
		enable_smbus();
//		sdram_initialize();
 //               dump_spd_registers(&cpu0);
                sdram_initialize(&cpu0);
 //               dump_spd_registers(&cpu1);
//                sdram_initialize(&cpu1);

//		dump_pci_device(PCI_DEV(0, 0x18, 2));
#if 0
		ram_fill(  0x00100000, 0x00180000);
		ram_verify(0x00100000, 0x00180000);
#endif
//#ifdef MEMORY_1024MB
//		ram_fill(  0x00000000, 0x00001000);
//		ram_verify(0x00000000, 0x00001000);
//#endif
//#ifdef MEMROY_512MB
//		ram_fill(  0x00000000, 0x01ffffff);
//		ram_verify(0x00000000, 0x01ffffff);
//#endif
		                /* Check the first 512M */
/*                msr_t msr;
                msr = rdmsr(TOP_MEM);
                print_debug("TOP_MEM: ");
                print_debug_hex32(msr.hi);
                print_debug_hex32(msr.lo);
                print_debug("\r\n");
                ram_check(0x00000000, msr.lo);
  */    	
/*
        reg = *((volatile unsigned long *)(APIC_DEFAULT_BASE+APIC_ID));
        print_debug("bootstrap cpu apic_id: ");
        print_debug_hex32(reg>>24);
        print_debug("\r\n");
*/

		// Start AP now
		reg = pci_read_config32(PCI_DEV(0, 0x19, 0), 0x6C);
		reg &= 0xffffff8c;
		reg |= 0x00000070;
		pci_write_config32(PCI_DEV(0, 0x19, 0), 0x6C, reg); //start AP
                for(;;) {
                        reg = pci_read_config32(PCI_DEV(0, 0x19, 0), 0x6C);
                        if((reg & (1<<4))==0) break;  // wait until AP stop
                }
                reg |= 1<<4;
                pci_write_config32(PCI_DEV(0, 0x19, 0), 0x6C, reg);

	}
 else {
	  // Need to init second cpu's APIC id
	// It's AP 
 
//	apic_write(APIC_ID,(1<<24));
	reg = *((volatile unsigned long *)(APIC_DEFAULT_BASE+APIC_ID));
/*	print_debug("applicaton cpu apic_id: ");
	print_debug_hex32(reg>>24);
	print_debug("\r\n");
	if((reg>>24)==7){ // FIXME: Need to read NodeID at first.
		*((volatile unsigned long *)(APIC_DEFAULT_BASE+APIC_ID))=1<<24;
	}*/
	if((reg>>24)!=0) {
//		before hlt clear the ColdResetbit
		
		//notify BSP that AP is stopped
                reg = pci_read_config32(PCI_DEV(0, 0x19, 0), 0x6C);
                reg &= ~(1<<4);
                pci_write_config32(PCI_DEV(0, 0x19, 0),  0x6C, reg);

		asm("hlt");
	}
       

	}
	
}
