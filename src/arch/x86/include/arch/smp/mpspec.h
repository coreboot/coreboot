#ifndef __ASM_MPSPEC_H
#define __ASM_MPSPEC_H

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

struct intel_mp_floating
{
	char mpf_signature[4];		/* "_MP_" 			*/
	unsigned long mpf_physptr;	/* Configuration table address	*/
	unsigned char mpf_length;	/* Our length (paragraphs)	*/
	unsigned char mpf_specification;/* Specification version	*/
	unsigned char mpf_checksum;	/* Checksum (makes sum 0)	*/
	unsigned char mpf_feature1;	/* Standard or configuration ? 	*/
	unsigned char mpf_feature2;	/* Bit7 set for IMCR|PIC	*/
#define MP_FEATURE_VIRTUALWIRE (1 << 7)
#define MP_FEATURE_PIC         (0 << 7)
	unsigned char mpf_feature3;	/* Unused (0)			*/
	unsigned char mpf_feature4;	/* Unused (0)			*/
	unsigned char mpf_feature5;	/* Unused (0)			*/
} __attribute__((packed));

struct mp_config_table
{
	char mpc_signature[4];
#define MPC_SIGNATURE "PCMP"
	unsigned short mpc_length;	/* Size of table */
	char  mpc_spec;			/* 0x01 */
	char  mpc_checksum;
	char  mpc_oem[8];
	char  mpc_productid[12];
	unsigned long mpc_oemptr;	/* 0 if not present */
	unsigned short mpc_oemsize;	/* 0 if not present */
	unsigned short mpc_entry_count;
	unsigned long mpc_lapic;	/* APIC address */
	unsigned short mpe_length;	/* Extended Table size */
	unsigned char mpe_checksum;	/* Extended Table checksum */
	unsigned char reserved;
} __attribute__((packed));

/* Followed by entries */

#define	MP_PROCESSOR	0
#define	MP_BUS		1
#define	MP_IOAPIC	2
#define	MP_INTSRC	3
#define	MP_LINTSRC	4

struct mpc_config_processor
{
	unsigned char mpc_type;
	unsigned char mpc_apicid;	/* Local APIC number */
	unsigned char mpc_apicver;	/* Its versions */
	unsigned char mpc_cpuflag;
#define MPC_CPU_ENABLED		1	/* Processor is available */
#define MPC_CPU_BOOTPROCESSOR	2	/* Processor is the BP */
	unsigned long mpc_cpufeature;
#define MPC_CPU_STEPPING_MASK 0x0F
#define MPC_CPU_MODEL_MASK	0xF0
#define MPC_CPU_FAMILY_MASK	0xF00
	unsigned long mpc_featureflag;	/* CPUID feature value */
	unsigned long mpc_reserved[2];
} __attribute__((packed));

struct mpc_config_bus
{
	unsigned char mpc_type;
	unsigned char mpc_busid;
	unsigned char mpc_bustype[6];
} __attribute__((packed));

#define BUSTYPE_EISA	"EISA"
#define BUSTYPE_ISA	"ISA"
#define BUSTYPE_INTERN	"INTERN"	/* Internal BUS */
#define BUSTYPE_MCA	"MCA"
#define BUSTYPE_VL	"VL"		/* Local bus */
#define BUSTYPE_PCI	"PCI"
#define BUSTYPE_PCMCIA	"PCMCIA"

struct mpc_config_ioapic
{
	unsigned char mpc_type;
	unsigned char mpc_apicid;
	unsigned char mpc_apicver;
	unsigned char mpc_flags;
#define MPC_APIC_USABLE		0x01
	unsigned long mpc_apicaddr;
} __attribute__((packed));

struct mpc_config_intsrc
{
	unsigned char mpc_type;
	unsigned char mpc_irqtype;
	unsigned short mpc_irqflag;
	unsigned char mpc_srcbus;
	unsigned char mpc_srcbusirq;
	unsigned char mpc_dstapic;
	unsigned char mpc_dstirq;
} __attribute__((packed));

enum mp_irq_source_types {
	mp_INT = 0,
	mp_NMI = 1,
	mp_SMI = 2,
	mp_ExtINT = 3
};

#define MP_IRQ_POLARITY_DEFAULT	0x0
#define MP_IRQ_POLARITY_HIGH	0x1
#define MP_IRQ_POLARITY_LOW	0x3
#define MP_IRQ_POLARITY_MASK    0x3
#define MP_IRQ_TRIGGER_DEFAULT	0x0
#define MP_IRQ_TRIGGER_EDGE	0x4
#define MP_IRQ_TRIGGER_LEVEL	0xc
#define MP_IRQ_TRIGGER_MASK     0xc


struct mpc_config_lintsrc
{
	unsigned char mpc_type;
	unsigned char mpc_irqtype;
	unsigned short mpc_irqflag;
	unsigned char mpc_srcbusid;
	unsigned char mpc_srcbusirq;
	unsigned char mpc_destapic;
#define MP_APIC_ALL	0xFF
	unsigned char mpc_destapiclint;
} __attribute__((packed));

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
	MP_BUS_ISA,
	MP_BUS_EISA,
	MP_BUS_PCI,
	MP_BUS_MCA
};

/* Followed by entries */

#define	MPE_SYSTEM_ADDRESS_SPACE	0x80
#define	MPE_BUS_HIERARCHY		0x81
#define	MPE_COMPATIBILITY_ADDRESS_SPACE	0x82

struct mp_exten_config {
	unsigned char mpe_type;
	unsigned char mpe_length;
} __attribute__((packed));

typedef struct mp_exten_config *mpe_t;

struct mp_exten_system_address_space {
	unsigned char mpe_type;
	unsigned char mpe_length;
	unsigned char mpe_busid;
	unsigned char mpe_address_type;
#define ADDRESS_TYPE_IO       0
#define ADDRESS_TYPE_MEM      1
#define ADDRESS_TYPE_PREFETCH 2
	unsigned int  mpe_address_base_low;
	unsigned int  mpe_address_base_high;
	unsigned int  mpe_address_length_low;
	unsigned int  mpe_address_length_high;
} __attribute__((packed));

struct mp_exten_bus_hierarchy {
	unsigned char mpe_type;
	unsigned char mpe_length;
	unsigned char mpe_busid;
	unsigned char mpe_bus_info;
#define BUS_SUBTRACTIVE_DECODE 1
	unsigned char mpe_parent_busid;
	unsigned char reserved[3];
} __attribute__((packed));

struct mp_exten_compatibility_address_space {
	unsigned char mpe_type;
	unsigned char mpe_length;
	unsigned char mpe_busid;
	unsigned char mpe_address_modifier;
#define ADDRESS_RANGE_SUBTRACT 1
#define ADDRESS_RANGE_ADD      0
	unsigned int  mpe_range_list;
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
} __attribute__((packed));

void mptable_init(struct mp_config_table *mc, u32 lapic_addr);

void *smp_next_mpc_entry(struct mp_config_table *mc);
void *smp_next_mpe_entry(struct mp_config_table *mc);

void smp_write_processor(struct mp_config_table *mc,
	unsigned char apicid, unsigned char apicver,
	unsigned char cpuflag, unsigned int cpufeature,
	unsigned int featureflag);
void smp_write_processors(struct mp_config_table *mc);
void smp_write_ioapic(struct mp_config_table *mc,
	unsigned char id, unsigned char ver,
	unsigned long apicaddr);
void smp_write_intsrc(struct mp_config_table *mc,
	unsigned char irqtype, unsigned short irqflag,
	unsigned char srcbus, unsigned char srcbusirq,
	unsigned char dstapic, unsigned char dstirq);
void smp_write_intsrc_pci_bridge(struct mp_config_table *mc,
	unsigned char irqtype, unsigned short irqflag,
	struct device *dev,
	unsigned char dstapic, unsigned char *dstirq);
void smp_write_lintsrc(struct mp_config_table *mc,
	unsigned char irqtype, unsigned short irqflag,
	unsigned char srcbusid, unsigned char srcbusirq,
	unsigned char destapic, unsigned char destapiclint);
void smp_write_address_space(struct mp_config_table *mc,
	unsigned char busid, unsigned char address_type,
	unsigned int address_base_low, unsigned int address_base_high,
	unsigned int address_length_low, unsigned int address_length_high);
void smp_write_bus_hierarchy(struct mp_config_table *mc,
	unsigned char busid, unsigned char bus_info,
	unsigned char parent_busid);
void smp_write_compatibility_address_space(struct mp_config_table *mc,
	unsigned char busid, unsigned char address_modifier,
	unsigned int range_list);
void *smp_write_floating_table(unsigned long addr, unsigned int virtualwire);
unsigned long write_smp_table(unsigned long addr);

void mptable_lintsrc(struct mp_config_table *mc, unsigned long bus_isa);
void mptable_add_isa_interrupts(struct mp_config_table *mc, unsigned long bus_isa, unsigned long apicid, int external);
void mptable_write_buses(struct mp_config_table *mc, int *max_pci_bus, int *isa_bus);
void *mptable_finalize(struct mp_config_table *mc);

#endif

