#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <stdlib.h>
#include <console/console.h>
#include "southbridge/intel/i82801ex/early_smbus.c"
#include "northbridge/intel/e7520/raminit.h"
#include "superio/nsc/pc8374/early_init.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "debug.c"
#include "watchdog.c"
// Remove comment if resets in this file are actually used.
// #include "reset.c"
#include "s1850_fixups.c"
#include "northbridge/intel/e7520/memory_initialized.c"
#include "cpu/x86/bist.h"
#include <spd.h>

#define CONSOLE_SERIAL_DEV PNP_DEV(0x2e, PC8374_SP1)

#define DEVPRES_CONFIG  ( \
	DEVPRES_D0F0 | \
	DEVPRES_D1F0 | \
	DEVPRES_D2F0 | \
	DEVPRES_D3F0 | \
	DEVPRES_D4F0 | \
	DEVPRES_D6F0 | \
	0 )
#define DEVPRES1_CONFIG (DEVPRES1_D0F1 | DEVPRES1_D8F0)

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/e7520/raminit.c"
#include "lib/generic_sdram.c"

/* IPMI garbage. This is all test stuff, if it really works we'll move it somewhere
 */

#define nftransport  0xc

#define OBF  0
#define IBF 1

#define ipmidata  0xca0
#define ipmicsr  0xca4

static inline void  ibfzero(void)
{
	while(inb(ipmicsr) &  (1<<IBF))
		;
}
static inline void  clearobf(void)
{
	(void) inb(ipmidata);
}

static inline void  waitobf(void)
{
	while((inb(ipmicsr) &  (1<<OBF)) == 0)
		;
}

/* quite possibly the stupidest interface ever designed. */
static inline void  first_cmd_byte(unsigned char byte)
{
	ibfzero();
	clearobf();
	outb(0x61, ipmicsr);
	ibfzero();
	clearobf();
	outb(byte, ipmidata);
}

static inline void  next_cmd_byte(unsigned char byte)
{

	ibfzero();
	clearobf();
	outb(byte, ipmidata);
}

static inline void  last_cmd_byte(unsigned char byte)
{
	outb(0x62, ipmicsr);

	ibfzero();
	clearobf();
	outb(byte,  ipmidata);
}

static inline void read_response_byte(void)
{
	int val = -1;
	if ((inb(ipmicsr)>>6) != 1)
		return;

	ibfzero();
	waitobf();
	val = inb(ipmidata);
	outb(0x68, ipmidata);

	/* see if it is done */
	if ((inb(ipmicsr)>>6) != 1){
		/* wait for the dummy read. Which describes this protocol */
		waitobf();
		(void)inb(ipmidata);
	}
}

static inline void ipmidelay(void)
{
	int i;
	for(i = 0; i < 1000; i++) {
		inb(0x80);
	}
}

static inline void bmc_foad(void)
{
	unsigned char c;
	/* be safe; make sure it is really ready */
	while ((inb(ipmicsr)>>6)) {
		outb(0x60, ipmicsr);
		inb(ipmidata);
	}
	first_cmd_byte(nftransport << 2);
	ipmidelay();
	next_cmd_byte(0x12);
	ipmidelay();
	next_cmd_byte(2);
	ipmidelay();
	last_cmd_byte(3);
	ipmidelay();
}

/* end IPMI garbage */

#include "arch/x86/lib/stages.c"

static void main(unsigned long bist)
{
	u8 b;
	u16 w;
	u32 l;
	int do_reset;

	static const struct mem_controller mch[] = {
		{
			.node_id = 0,
			/* the wiring on this part is really messed up */
			/* this is my best guess so far */
			.channel0 = {DIMM0, DIMM1, DIMM2, DIMM3, },
			.channel1 = {DIMM4, DIMM5, DIMM6, DIMM7, },
		}
	};

	/* superio setup */
	/* observed from serialice */
	static const u8 earlyinit[] = {
		0x21, 0x11, 0x11,
		0x22, 1, 1,
		0x23, 05, 05,
		0x24, 0x81, 0x81,
		0x26, 0, 0,
		0,
	};

	/* using SerialICE, we've seen this basic reset sequence on the dell.
	 * we don't understand it as it uses undocumented registers, but
	 * we're going to clone it.
	 */
	/* enable a hidden device. */
	b = pci_read_config8(PCI_DEV(0, 0, 0), 0xf4);
	b |= 0x8;
	pci_write_config8(PCI_DEV(0, 0, 0), 0xf4, b);

	/* read-write lock in CMOS on LPC bridge on ICH5 */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xd8, 4);

	/* operate on undocumented device */
	l = pci_read_config32(PCI_DEV(0, 0, 2), 0xa4);
	l |= 0x1000;
	pci_write_config32(PCI_DEV(0, 0, 2), 0xa4, l);

	l = pci_read_config32(PCI_DEV(0, 0, 2), 0x9c);
	l |= 0x8000;
	pci_write_config32(PCI_DEV(0, 0, 2), 0x9c, l);

	/* disable undocumented device */
	b = pci_read_config8(PCI_DEV(0, 0, 0), 0xf4);
	b &= ~0x8;
	pci_write_config8(PCI_DEV(0, 0, 0), 0xf4, b);

	/* set up LPC bridge bits, some of which reply on undocumented
	 * registers
	 */

	b= pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xd8);
	b |= 4;
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xd8, b);

	b= pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xd4);
	b |= 2;
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xd4, b);

	/* ACPI base address */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x40, 0x800);

	/* Enable specific ACPI features */
	b= pci_read_config8(PCI_DEV(0, 0x1f, 0), 0x44);
	b |= 0x10;
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x44, b);

	/* ACPI control */
	w = inw(0x868);
	outw(w|0x800, 0x868);
	w = inw(0x866);
	outw(w|2, 0x866);

#if 0
	/*seriaice shows
	dell does this so leave it here so I don't forget
 	 */
	/* SMBUS */
	pci_write_config16(PCI_DEV(0, 0x1f, 3), 0x20, 0x08c0);

	/* unknown */
	b = inb(0x8c2);
	outb(0xdf, 0x8c2);
#endif

	/* another device enable? */
	b = pci_read_config8(PCI_DEV(0, 0, 0), 0xf4);
	b |= 2;
	pci_write_config8(PCI_DEV(0, 0, 0), 0xf4, b);

	/* ?? */
	l = pci_read_config32(PCI_DEV(0, 8, 0), 0xc0);
	do_reset = l & 0x8000000;
	l |= 0x8000000;
	pci_write_config32(PCI_DEV(0, 8, 0), 0xc0, l);

	if (! do_reset) {
		outb(2, 0xcf9);
		outb(6, 0xcf9);
	}
	if (bist == 0) {
		/* Skip this if there was a built in self test failure */
		early_mtrr_init();
		if (memory_initialized())
			skip_romstage();
	}
	/* Setup the console */
	mainboard_set_ich5();
	//bmc_foad();
	pc8374_enable_dev(CONSOLE_SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* stuff we seem to need */
	pc8374_enable_dev(PNP_DEV(0x2e, PC8374_KBCK), 0);

	/* GPIOs */
	pc8374_enable_dev(PNP_DEV(0x2e, PC8374_GPIO), 0xc20);

	/* keep this in mind.
	SerialICE-hlp: outb 002e <= 23
	SerialICE-hlp:  inb 002f => 05
	SerialICE-hlp: outb 002f <= 05
	SerialICE-hlp: outb 002e <= 24
	SerialICE-hlp:  inb 002f => c1
	SerialICE-hlp: outb 002f <= c1
	 */

	/* Halt if there was a built in self test failure */
//	report_bist_failure(bist);

	/* MOVE ME TO A BETTER LOCATION !!! */
	/* config LPC decode for flash memory access */
        device_t dev;
        dev = pci_locate_device(PCI_ID(0x8086, 0x24d0), 0);
        if (dev == PCI_DEV_INVALID) {
                die("Missing ich5?");
        }
        pci_write_config32(dev, 0xe8, 0x00000000);
        pci_write_config8(dev, 0xf0, 0x00);

#if 0
	display_cpuid_update_microcode();
#endif
#if 1
	print_pci_devices();
#endif
#if 1
	enable_smbus();
#endif
#if 0
//	dump_spd_registers(&cpu[0]);
	int i;
	for(i = 0; i < 1; i++)
		dump_spd_registers();
#endif
#if 1
	show_dram_slots();
#endif
	disable_watchdogs();
//	dump_ipmi_registers();
	mainboard_set_e7520_leds();

	sdram_initialize(ARRAY_SIZE(mch), mch);
#if 0
	dump_pci_devices();
#endif
#if 1
	dump_pci_device(PCI_DEV(0, 0x00, 0));
//	dump_bar14(PCI_DEV(0, 0x00, 0));
#endif
}
