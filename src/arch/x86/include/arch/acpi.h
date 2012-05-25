/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 SUSE LINUX AG
 * Copyright (C) 2004 Nick Barker
 * Copyright (C) 2008-2009 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de>)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * coreboot ACPI support - headers and defines.
 */

#ifndef __ASM_ACPI_H
#define __ASM_ACPI_H

#if CONFIG_GENERATE_ACPI_TABLES

#include <stdint.h>

#define RSDP_SIG		"RSD PTR "  /* RSDT pointer signature */
#define ACPI_TABLE_CREATOR	"COREBOOT"  /* Must be exactly 8 bytes long! */
#define OEM_ID			"CORE  "    /* Must be exactly 6 bytes long! */
#define ASLC			"CORE"      /* Must be exactly 4 bytes long! */

/* RSDP (Root System Description Pointer) */
typedef struct acpi_rsdp {
	char  signature[8];	/* RSDP signature */
	u8    checksum;		/* Checksum of the first 20 bytes */
	char  oem_id[6];	/* OEM ID */
	u8    revision;		/* 0 for ACPI 1.0, 2 for ACPI 2.0/3.0/4.0 */
	u32   rsdt_address;	/* Physical address of RSDT (32 bits) */
	u32   length;		/* Total RSDP length (incl. extended part) */
	u64   xsdt_address;	/* Physical address of XSDT (64 bits) */
	u8    ext_checksum;	/* Checksum of the whole table */
	u8    reserved[3];
} __attribute__ ((packed)) acpi_rsdp_t;
/* Note: ACPI 1.0 didn't have length, xsdt_address, and ext_checksum. */

/* GAS (Generic Address Structure) */
typedef struct acpi_gen_regaddr {
	u8  space_id;		/* Address space ID */
	u8  bit_width;		/* Register size in bits */
	u8  bit_offset;		/* Register bit offset */
	union {
		u8  resv;			/* Reserved in ACPI 2.0 - 2.0b */
		u8  access_size;	/* Access size in ACPI 2.0c/3.0/4.0/5.0 */
	};
	u32 addrl;		/* Register address, low 32 bits */
	u32 addrh;		/* Register address, high 32 bits */
} __attribute__ ((packed)) acpi_addr_t;

#define ACPI_ADDRESS_SPACE_MEMORY	   0	/* System memory */
#define ACPI_ADDRESS_SPACE_IO		   1	/* System I/O */
#define ACPI_ADDRESS_SPACE_PCI		   2	/* PCI config space */
#define ACPI_ADDRESS_SPACE_EC		   3	/* Embedded controller */
#define ACPI_ADDRESS_SPACE_SMBUS	   4	/* SMBus */
#define ACPI_ADDRESS_SPACE_PCC		0x0A	/* Platform Comm. Channel */
#define ACPI_ADDRESS_SPACE_FIXED	0x7f	/* Functional fixed hardware */
#define  ACPI_FFIXEDHW_VENDOR_INTEL	   1	/* Intel */
#define  ACPI_FFIXEDHW_CLASS_HLT	   0	/* C1 Halt */
#define  ACPI_FFIXEDHW_CLASS_IO_HLT	   1	/* C1 I/O then Halt */
#define  ACPI_FFIXEDHW_CLASS_MWAIT	   2	/* MWAIT Native C-state */
#define  ACPI_FFIXEDHW_FLAG_HW_COORD	   1	/* Hardware Coordination bit */
#define  ACPI_FFIXEDHW_FLAG_BM_STS	   2	/* BM_STS avoidance bit */
/* 0x80-0xbf: Reserved */
/* 0xc0-0xff: OEM defined */

/* Access size definitions for Generic address structure */
#define ACPI_ACCESS_SIZE_UNDEFINED		0	/* Undefined (legacy reasons) */
#define ACPI_ACCESS_SIZE_BYTE_ACCESS	1
#define ACPI_ACCESS_SIZE_WORD_ACCESS	2
#define ACPI_ACCESS_SIZE_DWORD_ACCESS	3
#define ACPI_ACCESS_SIZE_QWORD_ACCESS	4

/* Generic ACPI header, provided by (almost) all tables */
typedef struct acpi_table_header {
	char signature[4];           /* ACPI signature (4 ASCII characters) */
	u32  length;                 /* Table length in bytes (incl. header) */
	u8   revision;               /* Table version (not ACPI version!) */
	u8   checksum;               /* To make sum of entire table == 0 */
	char oem_id[6];              /* OEM identification */
	char oem_table_id[8];        /* OEM table identification */
	u32  oem_revision;           /* OEM revision number */
	char asl_compiler_id[4];     /* ASL compiler vendor ID */
	u32  asl_compiler_revision;  /* ASL compiler revision number */
} __attribute__ ((packed)) acpi_header_t;

/* A maximum number of 32 ACPI tables ought to be enough for now. */
#define MAX_ACPI_TABLES 32

/* RSDT (Root System Description Table) */
typedef struct acpi_rsdt {
	struct acpi_table_header header;
	u32 entry[MAX_ACPI_TABLES];
} __attribute__ ((packed)) acpi_rsdt_t;

/* XSDT (Extended System Description Table) */
typedef struct acpi_xsdt {
	struct acpi_table_header header;
	u64 entry[MAX_ACPI_TABLES];
} __attribute__ ((packed)) acpi_xsdt_t;

/* HPET timers */
typedef struct acpi_hpet {
	struct acpi_table_header header;
	u32 id;
	struct acpi_gen_regaddr addr;
	u8 number;
	u16 min_tick;
	u8 attributes;
} __attribute__ ((packed)) acpi_hpet_t;

/* MCFG (PCI Express MMIO config space BAR description table) */
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

/* SRAT (System Resource Affinity Table) */
typedef struct acpi_srat {
	struct acpi_table_header header;
	u32 resv;
	u64 resv1;
	/* Followed by static resource allocation structure[n] */
} __attribute__ ((packed)) acpi_srat_t;

/* SRAT: Processor Local APIC/SAPIC Affinity Structure */
typedef struct acpi_srat_lapic {
	u8 type;			/* Type (0) */
	u8 length;			/* Length in bytes (16) */
	u8 proximity_domain_7_0;	/* Proximity domain bits[7:0] */
	u8 apic_id;			/* Local APIC ID */
	u32 flags; /* Enable bit 0 = 1, other bits reserved to 0 */
	u8 local_sapic_eid;		/* Local SAPIC EID */
	u8 proximity_domain_31_8[3];	/* Proximity domain bits[31:8] */
	u32 resv;			/* TODO: Clock domain in ACPI 4.0. */
} __attribute__ ((packed)) acpi_srat_lapic_t;

/* SRAT: Memory Affinity Structure */
typedef struct acpi_srat_mem {
	u8 type;			/* Type (1) */
	u8 length;			/* Length in bytes (40) */
	u32 proximity_domain;		/* Proximity domain */
	u16 resv;
	u32 base_address_low;		/* Mem range base address, low */
	u32 base_address_high;		/* Mem range base address, high */
	u32 length_low;			/* Mem range length, low */
	u32 length_high;		/* Mem range length, high */
	u32 resv1;
	u32 flags; /* Enable bit 0, hot pluggable bit 1; Non Volatile bit 2, other bits reserved to 0 */
	u32 resv2[2];
} __attribute__ ((packed)) acpi_srat_mem_t;

/* SLIT (System Locality Distance Information Table) */
typedef struct acpi_slit {
	struct acpi_table_header header;
	/* Followed by static resource allocation 8+byte[num*num] */
} __attribute__ ((packed)) acpi_slit_t;

/* MADT (Multiple APIC Description Table) */
typedef struct acpi_madt {
	struct acpi_table_header header;
	u32 lapic_addr;			/* Local APIC address */
	u32 flags;			/* Multiple APIC flags */
} __attribute__ ((packed)) acpi_madt_t;

/* MADT: APIC Structure Types */
/* TODO: Convert to ALLCAPS. */
enum acpi_apic_types {
	LocalApic		= 0,	/* Processor local APIC */
	IOApic			= 1,	/* I/O APIC */
	IRQSourceOverride	= 2,	/* Interrupt source override */
	NMIType			= 3,	/* NMI source */
	LocalApicNMI		= 4,	/* Local APIC NMI */
	LApicAddressOverride	= 5,	/* Local APIC address override */
	IOSApic			= 6,	/* I/O SAPIC */
	LocalSApic		= 7,	/* Local SAPIC */
	PlatformIRQSources	= 8,	/* Platform interrupt sources */
	Localx2Apic		= 9,	/* Processor local x2APIC */
	Localx2ApicNMI		= 10,	/* Local x2APIC NMI */
	/* 0x0b-0x7f: Reserved */
	/* 0x80-0xff: Reserved for OEM use */
};

/* MADT: Processor Local APIC Structure */
typedef struct acpi_madt_lapic {
	u8 type;			/* Type (0) */
	u8 length;			/* Length in bytes (8) */
	u8 processor_id;		/* ACPI processor ID */
	u8 apic_id;			/* Local APIC ID */
	u32 flags;			/* Local APIC flags */
} __attribute__ ((packed)) acpi_madt_lapic_t;

/* MADT: Local APIC NMI Structure */
typedef struct acpi_madt_lapic_nmi {
	u8 type;			/* Type (4) */
	u8 length;			/* Length in bytes (6) */
	u8 processor_id;		/* ACPI processor ID */
	u16 flags;			/* MPS INTI flags */
	u8 lint;			/* Local APIC LINT# */
} __attribute__ ((packed)) acpi_madt_lapic_nmi_t;

/* MADT: I/O APIC Structure */
typedef struct acpi_madt_ioapic {
	u8 type;			/* Type (1) */
	u8 length;			/* Length in bytes (12) */
	u8 ioapic_id;			/* I/O APIC ID */
	u8 reserved;
	u32 ioapic_addr;		/* I/O APIC address */
	u32 gsi_base;			/* Global system interrupt base */
} __attribute__ ((packed)) acpi_madt_ioapic_t;

/* MADT: Interrupt Source Override Structure */
typedef struct acpi_madt_irqoverride {
	u8 type;			/* Type (2) */
	u8 length;			/* Length in bytes (10) */
	u8 bus;				/* ISA (0) */
	u8 source;			/* Bus-relative int. source (IRQ) */
	u32 gsirq;			/* Global system interrupt */
	u16 flags;			/* MPS INTI flags */
} __attribute__ ((packed)) acpi_madt_irqoverride_t;

/* FADT (Fixed ACPI Description Table) */
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
	struct acpi_gen_regaddr x_pm1b_cnt_blk;
	struct acpi_gen_regaddr x_pm2_cnt_blk;
	struct acpi_gen_regaddr x_pm_tmr_blk;
	struct acpi_gen_regaddr x_gpe0_blk;
	struct acpi_gen_regaddr x_gpe1_blk;
} __attribute__ ((packed)) acpi_fadt_t;

/* FADT TABLE Revision values */
#define ACPI_FADT_REV_ACPI_1_0		1
#define ACPI_FADT_REV_ACPI_2_0		3
#define ACPI_FADT_REV_ACPI_3_0		4
#define ACPI_FADT_REV_ACPI_4_0		4
#define ACPI_FADT_REV_ACPI_5_0		5

/* Flags for p_lvl2_lat and p_lvl3_lat */
#define ACPI_FADT_C2_NOT_SUPPORTED	101
#define ACPI_FADT_C3_NOT_SUPPORTED	1001

/* FADT Feature Flags */
#define ACPI_FADT_WBINVD		(1 << 0)
#define ACPI_FADT_WBINVD_FLUSH		(1 << 1)
#define ACPI_FADT_C1_SUPPORTED		(1 << 2)
#define ACPI_FADT_C2_MP_SUPPORTED	(1 << 3)
#define ACPI_FADT_POWER_BUTTON		(1 << 4)
#define ACPI_FADT_SLEEP_BUTTON		(1 << 5)
#define ACPI_FADT_FIXED_RTC		(1 << 6)
#define ACPI_FADT_S4_RTC_WAKE		(1 << 7)
#define ACPI_FADT_32BIT_TIMER		(1 << 8)
#define ACPI_FADT_DOCKING_SUPPORTED	(1 << 9)
#define ACPI_FADT_RESET_REGISTER	(1 << 10)
#define ACPI_FADT_SEALED_CASE		(1 << 11)
#define ACPI_FADT_HEADLESS		(1 << 12)
#define ACPI_FADT_SLEEP_TYPE		(1 << 13)
#define ACPI_FADT_PCI_EXPRESS_WAKE	(1 << 14)
#define ACPI_FADT_PLATFORM_CLOCK	(1 << 15)
#define ACPI_FADT_S4_RTC_VALID		(1 << 16)
#define ACPI_FADT_REMOTE_POWER_ON	(1 << 17)
#define ACPI_FADT_APIC_CLUSTER		(1 << 18)
#define ACPI_FADT_APIC_PHYSICAL		(1 << 19)
/* Bits 20-31: reserved ACPI 3.0 & 4.0 */
#define ACPI_FADT_HW_REDUCED_ACPI	(1 << 20)
#define ACPI_FADT_LOW_PWR_IDLE_S0	(1 << 21)
/* bits 22-31: reserved ACPI 5.0 */

/* FADT Boot Architecture Flags */
#define ACPI_FADT_LEGACY_DEVICES	(1 << 0)
#define ACPI_FADT_8042			(1 << 1)
#define ACPI_FADT_VGA_NOT_PRESENT	(1 << 2)
#define ACPI_FADT_MSI_NOT_SUPPORTED	(1 << 3)
#define ACPI_FADT_NO_PCIE_ASPM_CONTROL	(1 << 4)
#define ACPI_FADT_LEGACY_FREE	0; /* No legacy devices (including 8042) */

/* FADT Preferred Power Management Profile */
enum acpi_preferred_pm_profiles {
	PM_UNSPECIFIED		= 0,
	PM_DESKTOP		= 1,
	PM_MOBILE		= 2,
	PM_WORKSTATION		= 3,
	PM_ENTERPRISE_SERVER	= 4,
	PM_SOHO_SERVER  	= 5,
	PM_APPLIANCE_PC		= 6,
	PM_PERFORMANCE_SERVER	= 7,
	PM_TABLET		= 8,	/* ACPI 5.0 */
};

/* FACS (Firmware ACPI Control Structure) */
typedef struct acpi_facs {
	char signature[4];			/* "FACS" */
	u32 length;				/* Length in bytes (>= 64) */
	u32 hardware_signature;			/* Hardware signature */
	u32 firmware_waking_vector;		/* Firmware waking vector */
	u32 global_lock;			/* Global lock */
	u32 flags;				/* FACS flags */
	u32 x_firmware_waking_vector_l;		/* X FW waking vector, low */
	u32 x_firmware_waking_vector_h;		/* X FW waking vector, high */
	u8 version;				/* ACPI 4.0: 2 */
	u8 resv[31];				/* FIXME: 4.0: ospm_flags */
} __attribute__ ((packed)) acpi_facs_t;

/* FACS flags */
#define ACPI_FACS_S4BIOS_F	(1 << 0)
#define ACPI_FACS_64BIT_WAKE_F	(1 << 1)
/* Bits 31..2: reserved */

/* ECDT (Embedded Controller Boot Resources Table) */
typedef struct acpi_ecdt {
	struct acpi_table_header header;
	struct acpi_gen_regaddr ec_control;	/* EC control register */
	struct acpi_gen_regaddr ec_data;	/* EC data register */
	u32 uid;				/* UID */
	u8 gpe_bit;				/* GPE bit */
	u8 ec_id[];				/* EC ID  */
} __attribute__ ((packed)) acpi_ecdt_t;

/* HEST (Hardware Error Source Table) */
typedef struct acpi_hest {
	struct acpi_table_header header;
	u32 error_source_count;
	/* error_source_struct(s) */
} __attribute__ ((packed)) acpi_hest_t;

/* Error Source Descriptors */
typedef struct acpi_hest_esd {
	u16 type;
	u16 source_id;
	u16 resv;
	u8 flags;
	u8 enabled;
	u32 prealloc_erecords;			/* The number of error records to pre-allocate for this error source. */
	u32 max_section_per_record;
} __attribute__ ((packed)) acpi_hest_esd_t;

/* Hardware Error Notification */
typedef struct acpi_hest_hen {
	u8 type;
	u8 length;
	u16 conf_we;		/* Configuration Write Enable */
	u32 poll_interval;
	u32 vector;
	u32 sw2poll_threshold_val;
	u32 sw2poll_threshold_win;
	u32 error_threshold_val;
	u32 error_threshold_win;
} __attribute__ ((packed)) acpi_hest_hen_t;

typedef struct acpi_cstate {
	u8  ctype;
	u16 latency;
	u32 power;
	acpi_addr_t resource;
} __attribute__ ((packed)) acpi_cstate_t;

typedef struct acpi_tstate {
	u32 percent;
	u32 power;
	u32 latency;
	u32 control;
	u32 status;
} __attribute__ ((packed)) acpi_tstate_t;

/* These are implemented by the target port or north/southbridge. */
unsigned long write_acpi_tables(unsigned long addr);
unsigned long acpi_fill_madt(unsigned long current);
unsigned long acpi_fill_mcfg(unsigned long current);
unsigned long acpi_fill_srat(unsigned long current);
unsigned long acpi_fill_slit(unsigned long current);
unsigned long acpi_fill_ssdt_generator(unsigned long current,
				       const char *oem_table_id);
void acpi_create_ssdt_generator(acpi_header_t *ssdt, const char *oem_table_id);
void acpi_create_fadt(acpi_fadt_t *fadt,acpi_facs_t *facs, void *dsdt);

void update_ssdt(void *ssdt);
void update_ssdtx(void *ssdtx, int i);

/* These can be used by the target port. */
u8 acpi_checksum(u8 *table, u32 length);

void acpi_add_table(acpi_rsdp_t *rsdp, void *table);

int acpi_create_madt_lapic(acpi_madt_lapic_t *lapic, u8 cpu, u8 apic);
int acpi_create_madt_ioapic(acpi_madt_ioapic_t *ioapic, u8 id, u32 addr,
			    u32 gsi_base);
int acpi_create_madt_irqoverride(acpi_madt_irqoverride_t *irqoverride,
				 u8 bus, u8 source, u32 gsirq, u16 flags);
int acpi_create_madt_lapic_nmi(acpi_madt_lapic_nmi_t *lapic_nmi, u8 cpu,
			       u16 flags, u8 lint);
void acpi_create_madt(acpi_madt_t *madt);
unsigned long acpi_create_madt_lapics(unsigned long current);
unsigned long acpi_create_madt_lapic_nmis(unsigned long current, u16 flags,
					  u8 lint);

int acpi_create_srat_lapic(acpi_srat_lapic_t *lapic, u8 node, u8 apic);
int acpi_create_srat_mem(acpi_srat_mem_t *mem, u8 node, u32 basek,u32 sizek,
			 u32 flags);
int acpi_create_mcfg_mmconfig(acpi_mcfg_mmconfig_t *mmconfig, u32 base,
			      u16 seg_nr, u8 start, u8 end);
unsigned long acpi_create_srat_lapics(unsigned long current);
void acpi_create_srat(acpi_srat_t *srat);

void acpi_create_slit(acpi_slit_t *slit);

void acpi_create_hpet(acpi_hpet_t *hpet);

void acpi_create_mcfg(acpi_mcfg_t *mcfg);

void acpi_create_facs(acpi_facs_t *facs);

#if CONFIG_HAVE_ACPI_SLIC
unsigned long acpi_create_slic(unsigned long current);
#endif

void acpi_write_rsdt(acpi_rsdt_t *rsdt);
void acpi_write_xsdt(acpi_xsdt_t *xsdt);
void acpi_write_rsdp(acpi_rsdp_t *rsdp, acpi_rsdt_t *rsdt, acpi_xsdt_t *xsdt);

void acpi_write_hest(acpi_hest_t *hest);
unsigned long acpi_create_hest_error_source(acpi_hest_t *hest, acpi_hest_esd_t *esd, u16 type, void *data, u16 len);
unsigned long acpi_fill_hest(acpi_hest_t *hest);

#if CONFIG_HAVE_ACPI_RESUME
/* 0 = S0, 1 = S1 ...*/
extern u8 acpi_slp_type;

void suspend_resume(void);
void __attribute__((weak)) mainboard_suspend_resume(void);
void *acpi_find_wakeup_vector(void);
void *acpi_get_wakeup_rsdp(void);
void acpi_jump_to_wakeup(void *wakeup_addr);

int acpi_get_sleep_type(void);
#else	/* CONFIG_HAVE_ACPI_RESUME */
#define acpi_slp_type 0
#endif	/* CONFIG_HAVE_ACPI_RESUME */

/* northbridge/amd/amdfam10/amdfam10_acpi.c */
unsigned long acpi_add_ssdt_pstates(acpi_rsdp_t *rsdp, unsigned long current);

/* cpu/intel/speedstep/acpi.c */
void generate_cpu_entries(void);

#else // CONFIG_GENERATE_ACPI_TABLES

#define write_acpi_tables(start) (start)
#define acpi_slp_type 0

#endif	/* CONFIG_GENERATE_ACPI_TABLES */

#if CONFIG_CHROMEOS
void acpi_get_vdat_info(void **vdat_addr, uint32_t *vdat_size);
#endif /* CONFIG_CHROMEOS */

#endif  /* __ASM_ACPI_H */
