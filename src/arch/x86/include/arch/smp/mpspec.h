/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_MPSPEC_H
#define __ASM_MPSPEC_H

#include <acpi/acpi.h>
#include <device/device.h>
#include <cpu/x86/lapic_def.h>

/*
 * Structure definitions for SMP machines following the
 * Intel Multiprocessing Specification 1.1 and 1.4.
 */

/*
 * This tag identifies where the SMP configuration
 * information is.
 */

#define SMP_MAGIC_IDENT	(('_'<<24)|('P'<<16)|('M'<<8)|'_')

/*
 * a maximum of 16 APICs with the current APIC ID architecture.
 */
#define MAX_APICS 16

#define SMP_FLOATING_TABLE_LEN sizeof(struct intel_mp_floating)

struct intel_mp_floating {
	char mpf_signature[4];	/* "_MP_" */
	u32 mpf_physptr;	/* Configuration table address */
	u8 mpf_length;	/* Our length (paragraphs) */
	u8 mpf_specification;/* Specification version */
	u8 mpf_checksum;	/* Checksum (makes sum 0) */
	u8 mpf_feature1;	/* Predefined or Unique configuration? */
	u8 mpf_feature2;	/* Bit7 set for IMCR/PIC */
#define MP_FEATURE_VIRTUALWIRE (0 << 7)
#define MP_FEATURE_PIC         (1 << 7)
	u8 mpf_feature3;	/* Unused (0) */
	u8 mpf_feature4;	/* Unused (0) */
	u8 mpf_feature5;	/* Unused (0) */
} __packed;

struct mp_config_table {
	char mpc_signature[4];
#define MPC_SIGNATURE "PCMP"
	u16 mpc_length;	/* Size of table */
	u8 mpc_spec;			/* 0x01 */
	u8 mpc_checksum;
	char  mpc_oem[8];
	char  mpc_productid[12];
	u32 mpc_oemptr;	/* 0 if not present */
	u16 mpc_oemsize;	/* 0 if not present */
	u16 mpc_entry_count;
	u32 mpc_lapic;	/* APIC address */
	u16 mpe_length;	/* Extended Table size */
	u8 mpe_checksum;	/* Extended Table checksum */
	u8 reserved;
} __packed;

/* Followed by entries */

#define	MP_PROCESSOR	0
#define	MP_BUS		1
#define	MP_IOAPIC	2
#define	MP_INTSRC	3
#define	MP_LINTSRC	4

struct mpc_config_processor {
	u8 mpc_type;
	u8 mpc_apicid;	/* Local APIC number */
	u8 mpc_apicver;	/* Its versions */
	u8 mpc_cpuflag;
#define MPC_CPU_ENABLED		1	/* Processor is available */
#define MPC_CPU_BOOTPROCESSOR	2	/* Processor is the BP */
	u32 mpc_cpufeature;
#define MPC_CPU_STEPPING_MASK 0x0F
#define MPC_CPU_MODEL_MASK	0xF0
#define MPC_CPU_FAMILY_MASK	0xF00
	u32 mpc_featureflag;	/* CPUID feature value */
	u32 mpc_reserved[2];
} __packed;

struct mpc_config_bus {
	u8 mpc_type;
	u8 mpc_busid;
	u8 mpc_bustype[6];
} __packed;

#define BUSTYPE_EISA	"EISA"
#define BUSTYPE_ISA	"ISA"
#define BUSTYPE_INTERN	"INTERN"	/* Internal BUS */
#define BUSTYPE_MCA	"MCA"
#define BUSTYPE_VL	"VL"		/* Local bus */
#define BUSTYPE_PCI	"PCI"
#define BUSTYPE_PCMCIA	"PCMCIA"

struct mpc_config_ioapic {
	u8 mpc_type;
	u8 mpc_apicid;
	u8 mpc_apicver;
	u8 mpc_flags;
#define MPC_APIC_USABLE		0x01
	void *mpc_apicaddr;
} __packed;

struct mpc_config_intsrc {
	u8 mpc_type;
	u8 mpc_irqtype;
	u16 mpc_irqflag;
	u8 mpc_srcbus;
	u8 mpc_srcbusirq;
	u8 mpc_dstapic;
	u8 mpc_dstirq;
} __packed;

enum mp_irq_source_types {
	mp_INT = 0,
	mp_NMI = 1,
	mp_SMI = 2,
	mp_ExtINT = 3
};

struct mpc_config_lintsrc {
	u8 mpc_type;
	u8 mpc_irqtype;
	u16 mpc_irqflag;
	u8 mpc_srcbusid;
	u8 mpc_srcbusirq;
	u8 mpc_destapic;
#define MP_APIC_ALL	0xFF
	u8 mpc_destapiclint;
} __packed;

/*
 *	Default configurations
 *
 *	1	2 CPU ISA 82489DX
 *	2	2 CPU EISA 82489DX neither IRQ 0 timer nor IRQ 13 DMA chaining
 *	3	2 CPU EISA 82489DX
 *	4	2 CPU MCA 82489DX
 *	5	2 CPU ISA+PCI
 *	6	2 CPU EISA+PCI
 *	7	2 CPU MCA+PCI
 */

#define MAX_IRQ_SOURCES 128
#define MAX_MP_BUSSES 32
enum mp_bustype {
	MP_BUS_ISA = 0,
	MP_BUS_EISA,
	MP_BUS_PCI,
	MP_BUS_MCA
};

/* Followed by entries */

#define	MPE_SYSTEM_ADDRESS_SPACE	0x80
#define	MPE_BUS_HIERARCHY		0x81
#define	MPE_COMPATIBILITY_ADDRESS_SPACE	0x82

struct mp_exten_config {
	u8 mpe_type;
	u8 mpe_length;
} __packed;

typedef struct mp_exten_config *mpe_t;

struct mp_exten_system_address_space {
	u8 mpe_type;
	u8 mpe_length;
	u8 mpe_busid;
	u8 mpe_address_type;
#define ADDRESS_TYPE_IO       0
#define ADDRESS_TYPE_MEM      1
#define ADDRESS_TYPE_PREFETCH 2
	u32 mpe_address_base_low;
	u32 mpe_address_base_high;
	u32 mpe_address_length_low;
	u32 mpe_address_length_high;
} __packed;

struct mp_exten_bus_hierarchy {
	u8 mpe_type;
	u8 mpe_length;
	u8 mpe_busid;
	u8 mpe_bus_info;
#define BUS_SUBTRACTIVE_DECODE 1
	u8 mpe_parent_busid;
	u8 reserved[3];
} __packed;

struct mp_exten_compatibility_address_space {
	u8 mpe_type;
	u8 mpe_length;
	u8 mpe_busid;
	u8 mpe_address_modifier;
#define ADDRESS_RANGE_SUBTRACT 1
#define ADDRESS_RANGE_ADD      0
	u32 mpe_range_list;
#define RANGE_LIST_IO_ISA	0
	/* X100 - X3FF
	 * X500 - X7FF
	 * X900 - XBFF
	 * XD00 - XFFF
	 */
#define RANGE_LIST_IO_VGA	1
	/* X3B0 - X3BB
	 * X3C0 - X3DF
	 * X7B0 - X7BB
	 * X7C0 - X7DF
	 * XBB0 - XBBB
	 * XBC0 - XBDF
	 * XFB0 - XFBB
	 * XFC0 - XCDF
	 */
} __packed;

void mptable_init(struct mp_config_table *mc);
void *smp_next_mpc_entry(struct mp_config_table *mc);
void *smp_next_mpe_entry(struct mp_config_table *mc);

void smp_write_processor(struct mp_config_table *mc,
	u8 apicid, u8 apicver,
	u8 cpuflag, u32 cpufeature,
	u32 featureflag);
void smp_write_processors(struct mp_config_table *mc);
void smp_write_ioapic(struct mp_config_table *mc,
	u8 id, u8 ver, void *apicaddr);

/* Call smp_write_ioapic() and return IOAPIC ID field. */
u8 smp_write_ioapic_from_hw(struct mp_config_table *mc, void *apicaddr);

void smp_write_intsrc(struct mp_config_table *mc,
	u8 irqtype, u16 irqflag, u8 srcbus, u8 srcbusirq,
	u8 dstapic, u8 dstirq);
void smp_write_pci_intsrc(struct mp_config_table *mc,
	u8 irqtype, u8 srcbus, u8 dev, u8 pirq,
	u8 dstapic, u8 dstirq);
void smp_write_intsrc_pci_bridge(struct mp_config_table *mc,
	u8 irqtype, u16 irqflag,
	struct device *dev,
	unsigned char dstapic, unsigned char *dstirq);
void smp_write_lintsrc(struct mp_config_table *mc,
	u8 irqtype, u16 irqflag,
	u8 srcbusid, u8 srcbusirq,
	u8 destapic, u8 destapiclint);
void smp_write_address_space(struct mp_config_table *mc,
	u8 busid, u8 address_type,
	u32 address_base_low, u32 address_base_high,
	u32 address_length_low, u32 address_length_high);
void smp_write_bus_hierarchy(struct mp_config_table *mc,
	u8 busid, u8 bus_info,
	u8 parent_busid);
void smp_write_compatibility_address_space(struct mp_config_table *mc,
	u8 busid, u8 address_modifier,
	u32 range_list);
void *smp_write_floating_table(unsigned long addr, unsigned int virtualwire);
unsigned long write_smp_table(unsigned long addr);

void mptable_lintsrc(struct mp_config_table *mc, unsigned long bus_isa);
void mptable_add_isa_interrupts(struct mp_config_table *mc,
	unsigned long bus_isa, unsigned long apicid, int external);
void mptable_write_buses(struct mp_config_table *mc, int *max_pci_bus,
	int *isa_bus);
void *mptable_finalize(struct mp_config_table *mc);

#endif
