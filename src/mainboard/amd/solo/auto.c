#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include "arch/romcc_io.h"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "northbridge/amd/amdk8/early_ht.c"
#include "southbridge/amd/amd8111/amd8111_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

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

	htic = pci_read_config32(PCI_ADDR(0, 0x18, 0, HT_INIT_CONTROL));
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


static void print_pci_devices(void)
{
	uint32_t addr;
	for(addr = PCI_ADDR(0, 0, 0, 0); 
		addr <= PCI_ADDR(0, 0x1f, 0x7, 0); 
		addr += PCI_ADDR(0,0,1,0)) {
		uint32_t id;
		id = pci_read_config32(addr + PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		print_debug("PCI: 00:");
		print_debug_hex8(addr >> 11);
		print_debug_char('.');
		print_debug_hex8((addr >> 8) & 7);
		print_debug("\r\n");
	}
}


static void dump_spd_registers(void)
{
	unsigned device;
	device = SMBUS_MEM_DEVICE_START;
	print_debug("\r\n");
	while(device <= SMBUS_MEM_DEVICE_END) {
		int i;
		print_debug("dimm: "); 
		print_debug_hex8(device); 
		for(i = 0; i < 256; i++) {
			int status;
			unsigned char byte;
			if ((i & 0xf) == 0) {
				print_debug("\r\n");
				print_debug_hex8(i);
				print_debug(": ");
			}
			status = smbus_read_byte(device, i);
			if (status < 0) {
				print_debug("bad device\r\n");
				break;
			}
			byte = status & 0xff;
			print_debug_hex8(byte);
			print_debug_char(' ');
		}
		device += SMBUS_MEM_DEVICE_INC;
		print_debug("\r\n");
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
	pc87360_enable_serial();
	uart_init();
	console_init();
	if (boot_cpu() && !cpu_init_detected()) {
		setup_coherent_ht_domain();
		enumerate_ht_chain();
		print_pci_devices();
		enable_smbus();
		sdram_initialize();

		dump_spd_registers();
#if 0
		ram_fill(  0x00100000, 0x00180000);
		ram_verify(0x00100000, 0x00180000);
#endif
#ifdef MEMORY_1024MB
		ram_fill(  0x00000000, 0x00001000);
		ram_verify(0x00000000, 0x00001000);
#endif
#ifdef MEMROY_512MB
		ram_fill(  0x00000000, 0x01ffffff);
		ram_verify(0x00000000, 0x01ffffff);
#endif
	}
}
