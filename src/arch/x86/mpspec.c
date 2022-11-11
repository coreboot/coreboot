/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <string.h>

/* Initialize the specified "mc" struct with initial values. */
void mptable_init(struct mp_config_table *mc)
{
	int i;
	u32 lapic_addr = cpu_get_lapic_addr();

	memset(mc, 0, sizeof(*mc));

	memcpy(mc->mpc_signature, MPC_SIGNATURE, 4);

	mc->mpc_length = sizeof(*mc);	/* Initially just the header size. */
	mc->mpc_spec = 0x04;		/* MultiProcessor specification 1.4 */
	mc->mpc_checksum = 0;		/* Not yet computed. */
	mc->mpc_oemptr = 0;
	mc->mpc_oemsize = 0;
	mc->mpc_entry_count = 0;	/* No entries yet... */
	mc->mpc_lapic = lapic_addr;
	mc->mpe_length = 0;
	mc->mpe_checksum = 0;
	mc->reserved = 0;

	strncpy(mc->mpc_oem, CONFIG_MAINBOARD_VENDOR, 8);
	strncpy(mc->mpc_productid, CONFIG_MAINBOARD_PART_NUMBER, 12);

	/*
	 * The oem/productid fields are exactly 8/12 bytes long. If the resp.
	 * entry is shorter, the remaining bytes are filled with spaces.
	 */
	for (i = MIN(strlen(CONFIG_MAINBOARD_VENDOR), 8); i < 8; i++)
		mc->mpc_oem[i] = ' ';
	for (i = MIN(strlen(CONFIG_MAINBOARD_PART_NUMBER), 12); i < 12; i++)
		mc->mpc_productid[i] = ' ';
}

static unsigned char smp_compute_checksum(void *v, int len)
{
	unsigned char *bytes;
	unsigned char checksum;
	int i;
	bytes = v;
	checksum = 0;
	for (i = 0; i < len; i++)
		checksum -= bytes[i];
	return checksum;
}

static void *smp_write_floating_table_physaddr(uintptr_t addr,
	uintptr_t mpf_physptr, unsigned int virtualwire)
{
	struct intel_mp_floating *mf;
	void *v;

	v = (void *)addr;
	mf = v;
	mf->mpf_signature[0] = '_';
	mf->mpf_signature[1] = 'M';
	mf->mpf_signature[2] = 'P';
	mf->mpf_signature[3] = '_';
	mf->mpf_physptr = mpf_physptr;
	mf->mpf_length = 1;
	mf->mpf_specification = 4;
	mf->mpf_checksum = 0;
	mf->mpf_feature1 = 0;
	mf->mpf_feature2 = virtualwire?MP_FEATURE_PIC:MP_FEATURE_VIRTUALWIRE;
	mf->mpf_feature3 = 0;
	mf->mpf_feature4 = 0;
	mf->mpf_feature5 = 0;
	mf->mpf_checksum = smp_compute_checksum(mf, mf->mpf_length*16);
	return v;
}

void *smp_write_floating_table(unsigned long addr, unsigned int virtualwire)
{
	/* 16 byte align the table address */
	addr = (addr + 0xf) & (~0xf);
	return smp_write_floating_table_physaddr(addr, addr
		+ SMP_FLOATING_TABLE_LEN, virtualwire);
}

void *smp_next_mpc_entry(struct mp_config_table *mc)
{
	void *v;
	v = (void *)(((char *)mc) + mc->mpc_length);

	return v;
}
static void smp_add_mpc_entry(struct mp_config_table *mc, u16 length)
{
	mc->mpc_length += length;
	mc->mpc_entry_count++;
}

void *smp_next_mpe_entry(struct mp_config_table *mc)
{
	void *v;
	v = (void *)(((char *)mc) + mc->mpc_length + mc->mpe_length);

	return v;
}
static void smp_add_mpe_entry(struct mp_config_table *mc, mpe_t mpe)
{
	mc->mpe_length += mpe->mpe_length;
}

/*
 * Type 0: Processor Entries:
 * Entry Type, LAPIC ID, LAPIC Version, CPU Flags EN/BP,
 * CPU Signature (Stepping, Model, Family), Feature Flags
 */
void smp_write_processor(struct mp_config_table *mc,
	u8 apicid, u8 apicver, u8 cpuflag,
	u32 cpufeature, u32 featureflag)
{
	struct mpc_config_processor *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_PROCESSOR;
	mpc->mpc_apicid = apicid;
	mpc->mpc_apicver = apicver;
	mpc->mpc_cpuflag = cpuflag;
	mpc->mpc_cpufeature = cpufeature;
	mpc->mpc_featureflag = featureflag;
	smp_add_mpc_entry(mc, sizeof(*mpc));
}

/*
 * If we assume a symmetric processor configuration we can
 * get all of the information we need to write the processor
 * entry from the bootstrap processor.
 * Plus I don't think linux really even cares.
 * Having the proper apicid's in the table so the non-bootstrap
 *  processors can be woken up should be enough.
 */
void smp_write_processors(struct mp_config_table *mc)
{
	int boot_apic_id;
	int order_id;
	unsigned int apic_version;
	unsigned int cpu_features;
	unsigned int cpu_feature_flags;
	struct device *cpu;

	boot_apic_id = lapicid();
	apic_version = lapic_read(LAPIC_LVR) & 0xff;
	cpu_features = cpu_get_cpuid();
	cpu_feature_flags = cpu_get_feature_flags_edx();
	/* order the output of the cpus to fix a bug in kernel 2.6.11 */
	for (order_id = 0; order_id < 256; order_id++) {
		for (cpu = all_devices; cpu; cpu = cpu->next) {
			unsigned long cpu_flag;
			if (!is_enabled_cpu(cpu))
				continue;

			cpu_flag = MPC_CPU_ENABLED;

			if (boot_apic_id == cpu->path.apic.apic_id)
				cpu_flag = MPC_CPU_ENABLED
					| MPC_CPU_BOOTPROCESSOR;

			if (cpu->path.apic.apic_id == order_id) {
				smp_write_processor(mc,
					cpu->path.apic.apic_id, apic_version,
					cpu_flag, cpu_features,
					cpu_feature_flags
				);
				break;
			}
		}
	}
}

/*
 * Type 1: Bus Entries:
 * Entry Type, Bus ID, Bus Type
 */
static void smp_write_bus(struct mp_config_table *mc,
	u8 id, const char *bustype)
{
	struct mpc_config_bus *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_BUS;
	mpc->mpc_busid = id;
	memcpy(mpc->mpc_bustype, bustype, sizeof(mpc->mpc_bustype));
	smp_add_mpc_entry(mc, sizeof(*mpc));
}

/*
 * Type 2: I/O APIC Entries:
 * Entry Type, APIC ID, Version,
 * APIC Flags:EN, Address
 */
void smp_write_ioapic(struct mp_config_table *mc,
	u8 id, u8 ver, void *apicaddr)
{
	struct mpc_config_ioapic *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_IOAPIC;
	mpc->mpc_apicid = id;
	mpc->mpc_apicver = ver;
	mpc->mpc_flags = MPC_APIC_USABLE;
	mpc->mpc_apicaddr = apicaddr;
	smp_add_mpc_entry(mc, sizeof(*mpc));
}

u8 smp_write_ioapic_from_hw(struct mp_config_table *mc, void *apicaddr)
{
	u8 id = get_ioapic_id(apicaddr);
	u8 ver = get_ioapic_version(apicaddr);
	smp_write_ioapic(mc, id, ver, apicaddr);
	return id;
}

/*
 * Type 3: I/O Interrupt Table Entries:
 * Entry Type, Int Type, Int Polarity, Int Level,
 * Source Bus ID, Source Bus IRQ, Dest APIC ID, Dest PIN#
 */
void smp_write_intsrc(struct mp_config_table *mc,
	u8 irqtype, u16 irqflag,
	u8 srcbus, u8 srcbusirq,
	u8 dstapic, u8 dstirq)
{
	struct mpc_config_intsrc *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_INTSRC;
	mpc->mpc_irqtype = irqtype;
	mpc->mpc_irqflag = irqflag;
	mpc->mpc_srcbus = srcbus;
	mpc->mpc_srcbusirq = srcbusirq;
	mpc->mpc_dstapic = dstapic;
	mpc->mpc_dstirq = dstirq;
	smp_add_mpc_entry(mc, sizeof(*mpc));
}

/*
 * Type 3: I/O Interrupt Table Entries for PCI Devices:
 * This has the same fields as 'Type 3: I/O Interrupt Table Entries'
 * but the Source Bus IRQ field has a slightly different
 * definition:
 * Bits 1-0: PIRQ pin: INT_A# = 0, INT_B# = 1, INT_C# = 2, INT_D# = 3
 * Bits 2-6: Originating PCI Device Number (Not its parent bridge device number)
 * Bit 7: Reserved
 */
void smp_write_pci_intsrc(struct mp_config_table *mc,
	u8 irqtype, u8 srcbus, u8 dev, u8 pirq,
	u8 dstapic, u8 dstirq)
{
	u8 srcbusirq = (dev << 2) | pirq;
	printk(BIOS_SPEW,
		"\tPCI srcbusirq = 0x%x from dev = 0x%x and pirq = %x\n",
		srcbusirq, dev, pirq);
	smp_write_intsrc(mc, irqtype, MP_IRQ_TRIGGER_LEVEL
		| MP_IRQ_POLARITY_LOW, srcbus, srcbusirq, dstapic, dstirq);
}

void smp_write_intsrc_pci_bridge(struct mp_config_table *mc,
	u8 irqtype, u16 irqflag, struct device *dev,
	unsigned char dstapic, unsigned char *dstirq)
{
	struct device *child;

	int i;
	int srcbus;
	int slot;

	struct bus *link;
	unsigned char dstirq_x[4];

	for (link = dev->link_list; link; link = link->next) {

		child = link->children;
		srcbus = link->secondary;

		while (child) {
			if (child->path.type != DEVICE_PATH_PCI)
				goto next;

			slot = (child->path.pci.devfn >> 3);
			/* round pins */
			for (i = 0; i < 4; i++)
				dstirq_x[i] = dstirq[(i + slot) % 4];

			if ((child->class >> 16) != PCI_BASE_CLASS_BRIDGE) {
				/* pci device */
				printk(BIOS_DEBUG, "route irq: %s\n",
					dev_path(child));
				for (i = 0; i < 4; i++)
					smp_write_intsrc(mc, irqtype, irqflag,
						srcbus, (slot<<2)|i, dstapic,
						dstirq_x[i]);
				goto next;
			}

			switch (child->class>>8) {
			case PCI_CLASS_BRIDGE_PCI:
			case PCI_CLASS_BRIDGE_PCMCIA:
			case PCI_CLASS_BRIDGE_CARDBUS:
				printk(BIOS_DEBUG, "route irq bridge: %s\n",
					dev_path(child));
				smp_write_intsrc_pci_bridge(mc, irqtype,
					irqflag, child, dstapic, dstirq_x);
			}

next:
			child = child->sibling;
		}

	}
}

/*
 * Type 4: Local Interrupt Assignment Entries:
 * Entry Type, Int Type, Int Polarity, Int Level,
 * Source Bus ID, Source Bus IRQ, Dest LAPIC ID,
 * Dest LAPIC LINTIN#
 */
void smp_write_lintsrc(struct mp_config_table *mc,
	u8 irqtype, u16 irqflag,
	u8 srcbusid, u8 srcbusirq,
	u8 destapic, u8 destapiclint)
{
	struct mpc_config_lintsrc *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_LINTSRC;
	mpc->mpc_irqtype = irqtype;
	mpc->mpc_irqflag = irqflag;
	mpc->mpc_srcbusid = srcbusid;
	mpc->mpc_srcbusirq = srcbusirq;
	mpc->mpc_destapic = destapic;
	mpc->mpc_destapiclint = destapiclint;
	smp_add_mpc_entry(mc, sizeof(*mpc));
}

/*
 * Type 128: System Address Space Mapping Entries
 * Entry Type, Entry Length, Bus ID, Address Type,
 * Address Base Lo/Hi, Address Length Lo/Hi
 */
void smp_write_address_space(struct mp_config_table *mc,
	u8 busid, u8 address_type,
	u32 address_base_low, u32 address_base_high,
	u32 address_length_low, u32 address_length_high)
{
	struct mp_exten_system_address_space *mpe;
	mpe = smp_next_mpe_entry(mc);
	memset(mpe, '\0', sizeof(*mpe));
	mpe->mpe_type = MPE_SYSTEM_ADDRESS_SPACE;
	mpe->mpe_length = sizeof(*mpe);
	mpe->mpe_busid = busid;
	mpe->mpe_address_type = address_type;
	mpe->mpe_address_base_low  = address_base_low;
	mpe->mpe_address_base_high = address_base_high;
	mpe->mpe_address_length_low  = address_length_low;
	mpe->mpe_address_length_high = address_length_high;
	smp_add_mpe_entry(mc, (mpe_t)mpe);
}

/*
 * Type 129: Bus Hierarchy Descriptor Entry
 * Entry Type, Entry Length, Bus ID, Bus Info,
 * Parent Bus ID
 */
void smp_write_bus_hierarchy(struct mp_config_table *mc,
	u8 busid, u8 bus_info, u8 parent_busid)
{
	struct mp_exten_bus_hierarchy *mpe;
	mpe = smp_next_mpe_entry(mc);
	memset(mpe, '\0', sizeof(*mpe));
	mpe->mpe_type = MPE_BUS_HIERARCHY;
	mpe->mpe_length = sizeof(*mpe);
	mpe->mpe_busid = busid;
	mpe->mpe_bus_info = bus_info;
	mpe->mpe_parent_busid = parent_busid;
	smp_add_mpe_entry(mc, (mpe_t)mpe);
}

/*
 * Type 130: Compatibility Bus Address Space Modifier Entry
 * Entry Type, Entry Length, Bus ID, Address Modifier
 * Predefined Range List
 */
void smp_write_compatibility_address_space(struct mp_config_table *mc,
	u8 busid, u8 address_modifier,
	u32 range_list)
{
	struct mp_exten_compatibility_address_space *mpe;
	mpe = smp_next_mpe_entry(mc);
	memset(mpe, '\0', sizeof(*mpe));
	mpe->mpe_type = MPE_COMPATIBILITY_ADDRESS_SPACE;
	mpe->mpe_length = sizeof(*mpe);
	mpe->mpe_busid = busid;
	mpe->mpe_address_modifier = address_modifier;
	mpe->mpe_range_list = range_list;
	smp_add_mpe_entry(mc, (mpe_t)mpe);
}

void mptable_lintsrc(struct mp_config_table *mc, unsigned long bus_isa)
{
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE
		| MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x0);
	smp_write_lintsrc(mc, mp_NMI, MP_IRQ_TRIGGER_EDGE
		| MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x1);
}

void mptable_add_isa_interrupts(struct mp_config_table *mc,
	unsigned long bus_isa, unsigned long apicid, int external_int2)
{
/*I/O Ints:                   Type         Trigger            Polarity
 *                                   Bus ID   IRQ  APIC ID   PIN# */
	smp_write_intsrc(mc, external_int2?mp_INT:mp_ExtINT,
				     MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0x0, apicid, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0x1, apicid, 0x1);
	smp_write_intsrc(mc, external_int2?mp_ExtINT:mp_INT,
				     MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0x0, apicid, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0x3, apicid, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0x4, apicid, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0x6, apicid, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0x7, apicid, 0x7);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0x8, apicid, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0x9, apicid, 0x9);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0xa, apicid, 0xa);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0xb, apicid, 0xb);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0xc, apicid, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0xd, apicid, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0xe, apicid, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
				     bus_isa, 0xf, apicid, 0xf);
}

void mptable_write_buses(struct mp_config_table *mc, int *max_pci_bus,
	int *isa_bus)
{
	int dummy, i, highest;
	char buses[256];
	struct device *dev;

	if (!max_pci_bus)
		max_pci_bus = &dummy;
	if (!isa_bus)
		isa_bus = &dummy;

	*max_pci_bus = 0;
	highest = 0;
	memset(buses, 0, sizeof(buses));

	for (dev = all_devices; dev; dev = dev->next) {
		struct bus *bus;
		for (bus = dev->link_list; bus; bus = bus->next) {
			if (bus->secondary > 255) {
				printk(BIOS_ERR,
					"A bus claims to have a bus ID > 255?!? Aborting");
				return;
			}
			buses[bus->secondary] = 1;
			if (highest < bus->secondary)
				highest = bus->secondary;
		}
	}
	for (i = 0; i <= highest; i++) {
		if (buses[i]) {
			smp_write_bus(mc, i, "PCI   ");
			*max_pci_bus = i;
		}
	}
	*isa_bus = *max_pci_bus + 1;
	smp_write_bus(mc, *isa_bus, "ISA   ");
}

void *mptable_finalize(struct mp_config_table *mc)
{
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc),
		mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk(BIOS_DEBUG, "Wrote the mp table end at: %p - %p\n",
		mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}
