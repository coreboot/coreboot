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

static void memreset_setup(const struct mem_controller *ctrl)
{
}

static void memreset(const struct mem_controller *ctrl)
{
}

static unsigned int generate_row(uint8_t node, uint8_t row, uint8_t maxnodes)
{
	/* since the AMD Solo is a UP only machine, we can 
	 * always return the default row entry value
	 */
        return 0x00010101; /* default row entry */
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}


#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

#define NODE_ID		0x60
#define	HT_INIT_CONTROL 0x6c

#define HTIC_ColdR_Detect  (1<<4)
#define HTIC_BIOSR_Detect  (1<<5)
#define HTIC_INIT_Detect   (1<<6)

static int boot_cpu(void)
{
	volatile unsigned long *local_apic;
	unsigned long apic_id;
	int bsp;
	msr_t msr;
	msr = rdmsr(0x1b);
	bsp = !!(msr.lo & (1 << 8));
	if (bsp) {
		print_debug("Bootstrap processor\r\n");
	} else {
		print_debug("Application processor\r\n");
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


static void main(void)
{
	static const struct mem_controller cpu0 = {
		.f0 = PCI_DEV(0, 0x18, 0),
		.f1 = PCI_DEV(0, 0x18, 1),
		.f2 = PCI_DEV(0, 0x18, 2),
		.f3 = PCI_DEV(0, 0x18, 3),
		.channel0 = { (0xa << 3), (0xa << 3)|1, 0, 0 },
		.channel1 = { 0, 0, 0, 0},
	};
	uart_init();
	console_init();
#if 0
	print_debug(" XIP_ROM_BASE: ");
	print_debug_hex32(XIP_ROM_BASE);
	print_debug(" XIP_ROM_SIZE: ");
	print_debug_hex32(XIP_ROM_SIZE);
	print_debug("\r\n");
#endif
	if (boot_cpu() && !cpu_init_detected()) {
		setup_default_resource_map();
		setup_coherent_ht_domain();
		enumerate_ht_chain();
		print_pci_devices();
		enable_smbus();
		dump_spd_registers(&cpu0);
		sdram_initialize(&cpu0);

		dump_pci_device(PCI_DEV(0, 0x18, 2));
		
		/* Check the first 512M */
		msr_t msr;
		msr = rdmsr(TOP_MEM);
		print_debug("TOP_MEM: ");
		print_debug_hex32(msr.hi);
		print_debug_hex32(msr.lo);
		print_debug("\r\n");
		ram_check(0x00000000, msr.lo);
	}
}
