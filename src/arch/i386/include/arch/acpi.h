/*
 * coreboot ACPI Support - headers and defines.
 * 
 * written by Stefan Reinauer <stepan@openbios.org>
 * (C) 2004 SUSE LINUX AG
 *
 * The ACPI table structs are based on the Linux kernel sources.
 * 
 */
/* ACPI FADT & FACS added by Nick Barker <nick.barker9@btinternet.com>
 * those parts (C) 2004 Nick Barker
 */


#ifndef __ASM_ACPI_H
#define __ASM_ACPI_H

#if HAVE_ACPI_TABLES==1

#include <stdint.h>
typedef unsigned long long u64;

#define RSDP_SIG              "RSD PTR "  /* RSDT Pointer signature */
#define RSDP_NAME             "RSDP"

#define RSDT_NAME             "RSDT"
#define HPET_NAME             "HPET"
#define MADT_NAME             "APIC"
#define MCFG_NAME             "MCFG"
#define SRAT_NAME             "SRAT"
#define SLIT_NAME	      "SLIT"
#define SSDT_NAME	      "SSDT"

#define RSDT_TABLE            "RSDT    "
#define HPET_TABLE            "AMD64   "
#define MCFG_TABLE            "MCFG    "
#define MADT_TABLE            "MADT    "
#define SRAT_TABLE	      "SRAT    "
#define SLIT_TABLE	      "SLIT    "

#define OEM_ID                "CORE  "
#define ASLC                  "CORE"

/* ACPI 2.0 table RSDP */

typedef struct acpi_rsdp {
	char  signature[8];	/* RSDP signature "RSD PTR" */
	u8    checksum;		/* checksum of the first 20 bytes */
	char  oem_id[6];	/* OEM ID, "LXBIOS" */
	u8    revision;		/* 0 for APCI 1.0, 2 for ACPI 2.0 */
	u32   rsdt_address;	/* physical address of RSDT */
	u32   length;		/* total length of RSDP (including extended part) */
	u64   xsdt_address;	/* physical address of XSDT */
	u8    ext_checksum;	/* chechsum of whole table */
	u8    reserved[3];
} __attribute__((packed)) acpi_rsdp_t;

/* Generic Address Container */

typedef struct acpi_gen_regaddr {
	u8  space_id;
	u8  bit_width;
	u8  bit_offset;
	u8  resv;
	u32 addrl;
	u32 addrh;
} __attribute__ ((packed)) acpi_addr_t;

/* Generic ACPI Header, provided by (almost) all tables */

typedef struct acpi_table_header         /* ACPI common table header */
{
	char signature [4];          /* ACPI signature (4 ASCII characters) */\
	u32  length;                 /* Length of table, in bytes, including header */\
	u8   revision;               /* ACPI Specification minor version # */\
	u8   checksum;               /* To make sum of entire table == 0 */\
	char oem_id [6];             /* OEM identification */\
	char oem_table_id [8];       /* OEM table identification */\
	u32  oem_revision;           /* OEM revision number */\
	char asl_compiler_id [4];    /* ASL compiler vendor ID */\
	u32  asl_compiler_revision;  /* ASL compiler revision number */
} __attribute__ ((packed)) acpi_header_t;

/* RSDT */
typedef struct acpi_rsdt {
	struct acpi_table_header header;
	u32 entry[7+ACPI_SSDTX_NUM+CONFIG_MAX_CPUS]; /* MCONFIG, HPET, FADT, SRAT, SLIT, MADT(APIC), SSDT, SSDTX, and SSDT for CPU pstate*/
} __attribute__ ((packed)) acpi_rsdt_t;

/* XSDT */
typedef struct acpi_xsdt {
	struct acpi_table_header header;
	u64 entry[6+ACPI_SSDTX_NUM];
} __attribute__ ((packed)) acpi_xsdt_t;

/* HPET TIMERS */
typedef struct acpi_hpet {
	struct acpi_table_header header;
	u32 id;
	struct acpi_gen_regaddr addr;
	u8 number;
	u16 min_tick;
	u8 attributes;
} __attribute__ ((packed)) acpi_hpet_t;

/* MCFG taken from include/linux/acpi.h */
typedef struct acpi_mcfg {
	struct acpi_table_header header;
	u8 reserved[8];
} __attribute__ ((packed)) acpi_mcfg_t;

typedef struct acpi_mcfg_mmconfig {
	u32 base_address;
	u32 base_reserved;
	u16 pci_segment_group_number;
	u8 start_bus_number;
	u8 end_bus_number;
	u8 reserved[4];
} __attribute__ ((packed)) acpi_mcfg_mmconfig_t;


/* SRAT */
typedef struct acpi_srat {
        struct acpi_table_header header;
        u32 resv;
	u64 resv1;
	/* followed by static resource allocation structure[n]*/
} __attribute__ ((packed)) acpi_srat_t;


typedef struct acpi_srat_lapic {
        u8 type;
        u8 length;
        u8 proximity_domain_7_0;
        u8 apic_id;
        u32 flags; /* enable bit 0  = 1, other bits reserved to 0 */
	u8 local_sapic_eid;
	u8 proximity_domain_31_8[3];
	u32 resv;
} __attribute__ ((packed)) acpi_srat_lapic_t;

typedef struct acpi_srat_mem {
        u8 type;
        u8 length;
        u32 proximity_domain;
        u16 resv;
	u32 base_address_low;
	u32 base_address_high;
	u32 length_low;
	u32 length_high;
	u32 resv1;
        u32 flags; /* enable bit 0,  hot pluggable bit 1; Non Volatile bit 2, other bits reserved */
        u32 resv2[2];
} __attribute__ ((packed)) acpi_srat_mem_t;

/* SLIT */
typedef struct acpi_slit {
        struct acpi_table_header header;
	/* followed by static resource allocation 8+byte[num*num]*/
} __attribute__ ((packed)) acpi_slit_t;


/* MADT */
typedef struct acpi_madt {
	struct acpi_table_header header;
	u32 lapic_addr;
	u32 flags;
} __attribute__ ((packed)) acpi_madt_t;

enum acpi_apic_types {
	LocalApic		= 0,
	IOApic			= 1,
	IRQSourceOverride	= 2,
	NMI			= 3,
	LocalApicNMI		= 4,
	LApicAddressOverride	= 5,
	IOSApic			= 6,
	LocalSApic		= 7,
	PlatformIRQSources	= 8
};

typedef struct acpi_madt_lapic {
	u8 type;
	u8 length;
	u8 processor_id;
	u8 apic_id;
	u32 flags;
} __attribute__ ((packed)) acpi_madt_lapic_t;

typedef struct acpi_madt_lapic_nmi {
	u8 type;
	u8 length;
	u8 processor_id;
	u16 flags;
	u8 lint;
} __attribute__ ((packed)) acpi_madt_lapic_nmi_t;


typedef struct acpi_madt_ioapic {
	u8 type;
	u8 length;
	u8 ioapic_id;
	u8 reserved;
	u32 ioapic_addr;
	u32 gsi_base;
} __attribute__ ((packed)) acpi_madt_ioapic_t;

typedef struct acpi_madt_irqoverride {
	u8 type;
	u8 length;
	u8 bus;
	u8 source;
	u32 gsirq;
	u16 flags;
} __attribute__ ((packed)) acpi_madt_irqoverride_t;

/* FADT */

typedef struct acpi_fadt {
	struct acpi_table_header header;
	u32 firmware_ctrl;
	u32 dsdt;
	u8 model;
	u8 preferred_pm_profile;
	u16 sci_int;
	u32 smi_cmd;
	u8 acpi_enable;
	u8 acpi_disable;
	u8 s4bios_req;
	u8 pstate_cnt;
	u32 pm1a_evt_blk;
	u32 pm1b_evt_blk;
	u32 pm1a_cnt_blk;
	u32 pm1b_cnt_blk;
	u32 pm2_cnt_blk;
	u32 pm_tmr_blk;
	u32 gpe0_blk;
	u32 gpe1_blk;
	u8 pm1_evt_len;
	u8 pm1_cnt_len;
	u8 pm2_cnt_len;
	u8 pm_tmr_len;
	u8 gpe0_blk_len;
	u8 gpe1_blk_len;
	u8 gpe1_base;
	u8 cst_cnt;
	u16 p_lvl2_lat;
	u16 p_lvl3_lat;
	u16 flush_size;
	u16 flush_stride;
	u8 duty_offset;
	u8 duty_width;
	u8 day_alrm;
	u8 mon_alrm;
	u8 century;
	u16 iapc_boot_arch;
	u8 res2;
	u32 flags;
	struct acpi_gen_regaddr reset_reg;
	u8 reset_value;
	u8 res3;
	u8 res4;
	u8 res5;
	u32 x_firmware_ctl_l;
	u32 x_firmware_ctl_h;
	u32 x_dsdt_l;
	u32 x_dsdt_h;
	struct acpi_gen_regaddr x_pm1a_evt_blk;
	struct acpi_gen_regaddr x_pm1b_evt_blk;
	struct acpi_gen_regaddr x_pm1a_cnt_blk;
	struct acpi_gen_regaddr	x_pm1b_cnt_blk;
	struct acpi_gen_regaddr x_pm2_cnt_blk;
	struct acpi_gen_regaddr x_pm_tmr_blk;
	struct acpi_gen_regaddr x_gpe0_blk;
	struct acpi_gen_regaddr x_gpe1_blk;
} __attribute__ ((packed)) acpi_fadt_t;

/* FACS */
typedef struct acpi_facs {
	char signature[4];
	u32 length;
	u32 hardware_signature;
	u32 firmware_waking_vector;
	u32 global_lock;
	u32 flags;
	u32 x_firmware_waking_vector_l;
	u32 x_firmware_waking_vector_h;
	u8 version;
	u8 resv[31];
} __attribute__ ((packed)) acpi_facs_t;

/* These are implemented by the target port */
unsigned long write_acpi_tables(unsigned long addr);
unsigned long acpi_fill_madt(unsigned long current);
unsigned long acpi_fill_mcfg(unsigned long current);
unsigned long acpi_fill_srat(unsigned long current); 
unsigned long acpi_fill_ssdt_generator(unsigned long current, char *oem_table_id);
void acpi_create_ssdt_generator(acpi_header_t *ssdt, char *oem_table_id);
void acpi_create_fadt(acpi_fadt_t *fadt,acpi_facs_t *facs,void *dsdt);

/* These can be used by the target port */
u8 acpi_checksum(u8 *table, u32 length);

void acpi_add_table(acpi_rsdt_t *rsdt, void *table);

int acpi_create_madt_lapic(acpi_madt_lapic_t *lapic, u8 cpu, u8 apic);
int acpi_create_madt_ioapic(acpi_madt_ioapic_t *ioapic, u8 id, u32 addr,u32 gsi_base);
int acpi_create_madt_irqoverride(acpi_madt_irqoverride_t *irqoverride,
                u8 bus, u8 source, u32 gsirq, u16 flags);
int acpi_create_madt_lapic_nmi(acpi_madt_lapic_nmi_t *lapic_nmi, u8 cpu,
                u16 flags, u8 lint);
void acpi_create_madt(acpi_madt_t *madt);
unsigned long acpi_create_madt_lapics(unsigned long current);
unsigned long acpi_create_madt_lapic_nmis(unsigned long current, u16 flags, u8 lint);


int acpi_create_srat_lapic(acpi_srat_lapic_t *lapic, u8 node, u8 apic);
int acpi_create_srat_mem(acpi_srat_mem_t *mem, u8 node, u32 basek,u32 sizek, u32 flags);
int acpi_create_mcfg_mmconfig(acpi_mcfg_mmconfig_t *mmconfig, u32 base, u16 seg_nr, u8 start, u8 end);
unsigned long acpi_create_srat_lapics(unsigned long current);
void acpi_create_srat(acpi_srat_t *srat);

void acpi_create_slit(acpi_slit_t *slit);

void acpi_create_hpet(acpi_hpet_t *hpet);

void acpi_create_mcfg(acpi_mcfg_t *mcfg);

void acpi_create_facs(acpi_facs_t *facs);

void acpi_write_rsdt(acpi_rsdt_t *rsdt);
void acpi_write_rsdp(acpi_rsdp_t *rsdp, acpi_rsdt_t *rsdt);

#define ACPI_WRITE_MADT_IOAPIC(dev,id)        		\
do {                                                    \
        struct resource *res;                           \
        res = find_resource(dev, PCI_BASE_ADDRESS_0);   \
        if (!res) break;                                \
	current += acpi_create_madt_ioapic(		\
		(acpi_madt_ioapic_t *)current,		\
		id, res->base, gsi_base);		\
	gsi_base+=4;					\
} while(0);

#define IO_APIC_ADDR	0xfec00000UL

#else // HAVE_ACPI_TABLES

#define write_acpi_tables(start) (start)

#endif

#endif
