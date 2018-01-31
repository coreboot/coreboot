#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/amdk8_sysconf.h>

extern  unsigned char bus_ck804_0; //1
extern  unsigned char bus_ck804_1; //2
extern  unsigned char bus_ck804_2; //3
extern  unsigned char bus_ck804_3; //4
extern  unsigned char bus_ck804_4; //5
extern  unsigned char bus_ck804_5; //6
extern  unsigned apicid_ck804;


static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	unsigned sbdn;
	int i, bus_isa;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();
	sbdn = sysconf.sbdn;

	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:	APIC ID	Version	State		Address*/

	device_t dev;
	struct resource *res;
	uint32_t dword;

	dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(sbdn + 0x1, 0));
	if (dev) {
		res = find_resource(dev, PCI_BASE_ADDRESS_1);
		if (res)
			smp_write_ioapic(mc, apicid_ck804, 0x11,
					 res2mmio(res, 0, 0));

	/* Initialize interrupt mapping*/

		dword = 0x0120d218;
		pci_write_config32(dev, 0x7c, dword);

		dword = 0x12008a00;
		pci_write_config32(dev, 0x80, dword);

		dword = 0x0000007d;
		pci_write_config32(dev, 0x84, dword);
	}


	mptable_add_isa_interrupts(mc, bus_isa, apicid_ck804, 1);

// Onboard ck804 smbus
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
				bus_ck804_0, ((sbdn + 1) << 2) | 1, apicid_ck804, 0xa);

// Onboard ck804 USB 1.1
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
				bus_ck804_0, ((sbdn + 2) << 2) | 0, apicid_ck804, 0x15);

// Onboard ck804 USB 2
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
				bus_ck804_0, ((sbdn + 2) << 2 ) | 1, apicid_ck804, 0x14);

// Onboard ck804 SATA 0
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
				bus_ck804_0, ((sbdn + 7) << 2 ) | 0, apicid_ck804, 0x17);

// Onboard ck804 SATA 1
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
				bus_ck804_0, ((sbdn + 8) << 2) | 0, apicid_ck804, 0x16);

//Slot PCIE x16
	for (i = 0; i < 4; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			bus_ck804_5, (0x00 << 2) | i, apicid_ck804, 0x10 + (2 + i + 4 - sbdn%4)%4);

//Slot  PCIE x4
	for (i = 0; i < 4; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			bus_ck804_4, (0x00 << 2) | i, apicid_ck804, 0x10 + (1 + i + 4 - sbdn%4)%4);

//Onboard SM720 VGA
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			bus_ck804_1, (6 << 2) | 0, apicid_ck804, 0x13);

/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	mptable_lintsrc(mc, bus_isa);
	/* There is no extension information... */

	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
