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
	/* This can only be done if cpuid says fid changing is supported
	 * I need to look up the base frequency another way for other
	 * cpus.  Is it worth dedicating a global register to this?
	 * Are the PET timers useable for this purpose?
	 */
	msr_t msr;
	msr = rdmsr(0xc0010042);
	return ((msr.lo & 0x3f) + 8) * 100 *usecs;
#endif
}

static void init_apic_timer(void)
{
	volatile uint32_t *apic_reg = (volatile uint32_t *)0xfee00000;
	uint32_t start, end;
	/* Set the apic timer to no interrupts and periodic mode */
	apic_reg[0x320 >> 2] = (1 << 17)|(1<< 16)|(0 << 12)|(0 << 0);
	/* Set the divider to 1, no divider */
	apic_reg[0x3e0 >> 2] = (1 << 3) | 3;
	/* Set the initial counter to 0xffffffff */
	apic_reg[0x380 >> 2] = 0xffffffff;
}

static void udelay(unsigned usecs)
{
#if 1
	uint32_t start, ticks;
	tsc_t tsc;
	/* Calculate the number of ticks to run for */
	ticks = usecs_to_ticks(usecs);
	/* Find the current time */
	tsc = rdtsc();
	start = tsc.lo;
	do {
		tsc = rdtsc();
	} while((tsc.lo - start) < ticks);
#else
	volatile uint32_t *apic_reg = (volatile uint32_t *)0xfee00000;
	uint32_t start, value, ticks;
	/* Calculate the number of ticks to run for */
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
	/* Set the memreset low */
	outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 28);
	/* Ensure the BIOS has control of the memory lines */
	outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 29);
	print_debug("memreset lo\r\n");
}

static void memreset(const struct mem_controller *ctrl)
{
	udelay(800);
	/* Set memreset_high */
	outb((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 28);
	print_debug("memreset hi\r\n");
	udelay(50);
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
		print_debug("Bootstrap cpu\r\n");
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

static void pnp_write_config(unsigned char port, unsigned char value, unsigned char reg)
{
	outb(reg, port);
	outb(value, port +1);
}

static unsigned char pnp_read_config(unsigned char port, unsigned char reg)
{
	outb(reg, port);
	return inb(port +1);
}

static void pnp_set_logical_device(unsigned char port, int device)
{
	pnp_write_config(port, device, 0x07);
}

static void pnp_set_enable(unsigned char port, int enable)
{
	pnp_write_config(port, enable?0x1:0x0, 0x30);
}

static int pnp_read_enable(unsigned char port)
{
	return !!pnp_read_config(port, 0x30);
}

static void pnp_set_iobase0(unsigned char port, unsigned iobase)
{
	pnp_write_config(port, (iobase >> 8) & 0xff, 0x60);
	pnp_write_config(port, iobase & 0xff, 0x61);
}

static void pnp_set_iobase1(unsigned char port, unsigned iobase)
{
	pnp_write_config(port, (iobase >> 8) & 0xff, 0x62);
	pnp_write_config(port, iobase & 0xff, 0x63);
}

static void pnp_set_irq0(unsigned char port, unsigned irq)
{
	pnp_write_config(port, irq, 0x70);
}

static void pnp_set_irq1(unsigned char port, unsigned irq)
{
	pnp_write_config(port, irq, 0x72);
}

static void pnp_set_drq(unsigned char port, unsigned drq)
{
	pnp_write_config(port, drq & 0xff, 0x74);
}

#define PC87360_FDC  0x00
#define PC87360_PP   0x01
#define PC87360_SP2  0x02
#define PC87360_SP1  0x03
#define PC87360_SWC  0x04
#define PC87360_KBCM 0x05
#define PC87360_KBCK 0x06
#define PC87360_GPIO 0x07
#define PC87360_ACB  0x08
#define PC87360_FSCM 0x09
#define PC87360_WDT  0x0A

static void pc87360_enable_serial(void)
{
	pnp_set_logical_device(SIO_BASE, PC87360_SP1);
	pnp_set_enable(SIO_BASE, 1);
	pnp_set_iobase0(SIO_BASE, 0x3f8);
}

static void main(void)
{
	/*
	 * GPIO28 of 8111 will control H0_MEMRESET_L
	 * GPIO29 of 8111 will control H1_MEMRESET_L
	 */

	static const struct mem_controller cpu0 = {
		.f0 = PCI_DEV(0, 0x18, 0),
		.f1 = PCI_DEV(0, 0x18, 1),
		.f2 = PCI_DEV(0, 0x18, 2),
		.f3 = PCI_DEV(0, 0x18, 3),
		.channel0 = { (0xa<<3)|0, (0xa<<3)|2, 0, 0 },
		.channel1 = { (0xa<<3)|1, (0xa<<3)|3, 0, 0 },
		.memreset = 28,
	};
	static const struct mem_controller cpu1 = {
		.f0 = PCI_DEV(0, 0x19, 0),
		.f1 = PCI_DEV(0, 0x19, 1),
		.f2 = PCI_DEV(0, 0x19, 2),
		.f3 = PCI_DEV(0, 0x19, 3),
		.channel0 = { (0xa<<3)|4, (0xa<<3)|6, 0, 0 },
		.channel1 = { (0xa<<3)|5, (0xa<<3)|7, 0, 0 },
		.memreset = 29,
	};
	pc87360_enable_serial();
	uart_init();
	console_init();
	if (boot_cpu() && !cpu_init_detected()) {
#if 1
		init_apic_timer();
#endif
		setup_default_resource_map();
		setup_coherent_ht_domain();
		enumerate_ht_chain();
		print_pci_devices();
		enable_smbus();
		dump_spd_registers(&cpu0);
		sdram_initialize(&cpu0);

#if 0
		dump_pci_devices();
#endif
#if 0
		dump_pci_device(PCI_DEV(0, 0x18, 2));
#endif

		/* Check all of memory */
		msr_t msr;
		msr = rdmsr(TOP_MEM);
		print_debug("TOP_MEM: ");
		print_debug_hex32(msr.hi);
		print_debug_hex32(msr.lo);
		print_debug("\r\n");
#if 0
		ram_check(0x00000000, msr.lo);
#else
		/* Check 16MB of memory */
		ram_check(0x00000000, 0x1600000);
#endif
#if 0
		print_debug("sleeping 15s\r\n");
		delay(15);
		print_debug("sleeping 15s done\r\n");
#endif
	}
}
