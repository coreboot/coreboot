/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 SUSE LINUX AG
 * Copyright (C) 2004 Nick Barker
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>,
 * Raptor Engineering
 * Copyright (C) 2016 Siemens AG
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
 */

/*
 * coreboot ACPI support - headers and defines.
 */

#ifndef __ASM_ACPI_H
#define __ASM_ACPI_H

#define HIGH_MEMORY_SAVE	(CONFIG_RAMTOP - CONFIG_RAMBASE)

/*
 * The type and enable fields are common in ACPI, but the
 * values themselves are hardware implementation defined.
 */
#if IS_ENABLED(CONFIG_ACPI_INTEL_HARDWARE_SLEEP_VALUES)
 #define SLP_EN		(1 << 13)
 #define SLP_TYP_SHIFT	10
 #define SLP_TYP	(7 << SLP_TYP_SHIFT)
 #define  SLP_TYP_S0	0
 #define  SLP_TYP_S1	1
 #define  SLP_TYP_S3	5
 #define  SLP_TYP_S4	6
 #define  SLP_TYP_S5	7
#elif IS_ENABLED(CONFIG_ACPI_AMD_HARDWARE_SLEEP_VALUES)
 #define SLP_EN		(1 << 13)
 #define SLP_TYP_SHIFT	10
 #define SLP_TYP	(7 << SLP_TYP_SHIFT)
 #define  SLP_TYP_S0	0
 #define  SLP_TYP_S1	1
 #define  SLP_TYP_S3	3
 #define  SLP_TYP_S4	4
 #define  SLP_TYP_S5	5
#endif

#if !defined(__ASSEMBLER__) && !defined(__ACPI__) && !defined(__ROMCC__)
#include <stdint.h>
#include <compiler.h>
#include <rules.h>
#include <commonlib/helpers.h>
#include <device/device.h>
#include <uuid.h>
#include <cper.h>

#define RSDP_SIG		"RSD PTR "  /* RSDT pointer signature */
#define ACPI_TABLE_CREATOR	"COREBOOT"  /* Must be exactly 8 bytes long! */
#define OEM_ID			"CORE  "    /* Must be exactly 6 bytes long! */
#define ASLC			"CORE"      /* Must be exactly 4 bytes long! */

/*
 * The assigned ACPI ID for the coreboot project is 'BOOT'
 * http://www.uefi.org/acpi_id_list
 */
#define COREBOOT_ACPI_ID	"BOOT"      /* ACPI ID for coreboot HIDs */

/* List of ACPI HID that use the coreboot ACPI ID */
enum coreboot_acpi_ids {
	COREBOOT_ACPI_ID_CBTABLE	= 0x0000, /* BOOT0000 */
	COREBOOT_ACPI_ID_MAX		= 0xFFFF, /* BOOTFFFF */
};

/* Table 5-30 DESCRIPTION_HEADER Signatures for tables defined by ACPI 6.2a
 *  Additional tables mssing in 5-30:  MADT, RSDP, VFCT, NHLT
 */
enum acpi_tables {
	APIC, BERT, BGRT, CPEP, DSDT, ECDT, EINJ, ERST, FACP, FADT, FACS,
	FPDT, GTDT, HEST, MSCT, MPST, NFIT, OEMX, PCCT, PMTT, PSDT, RASF,
	RSDT, SBST, SDEV, SLIT, SRAT, SSDT, XSDT, BOOT, CSRT, DBG2, DBGP,
	DMAR, DPPT, DRTM, ETDT, HPET, IBFT, IORT, IVRS, LPIT, MCFG, MCHI,
	MSDM, SDEI, SLIC, SPCR, SPMI, STAO, TCPA, TPM2, WAET, WDAT, WDRT,
	WPBT, WSMT, XENV, MADT, RSDP, VFCT, NHLT
};

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
} __packed acpi_rsdp_t;
/* Note: ACPI 1.0 didn't have length, xsdt_address, and ext_checksum. */

/* GAS (Generic Address Structure) */
typedef struct acpi_gen_regaddr {
	u8  space_id;		/* Address space ID */
	u8  bit_width;		/* Register size in bits */
	u8  bit_offset;		/* Register bit offset */
	union {
		u8  resv;		/* Reserved in ACPI 2.0 - 2.0b */
		u8  access_size;	/* Access size in ACPI 2.0c/3.0/4.0/5.0
					 */
	};
	u32 addrl;		/* Register address, low 32 bits */
	u32 addrh;		/* Register address, high 32 bits */
} __packed acpi_addr_t;

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
#define ACPI_ACCESS_SIZE_UNDEFINED	0	/* Undefined (legacy reasons) */
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
} __packed acpi_header_t;

/* A maximum number of 32 ACPI tables ought to be enough for now. */
#define MAX_ACPI_TABLES 32

/* RSDT (Root System Description Table) */
typedef struct acpi_rsdt {
	struct acpi_table_header header;
	u32 entry[MAX_ACPI_TABLES];
} __packed acpi_rsdt_t;

/* XSDT (Extended System Description Table) */
typedef struct acpi_xsdt {
	struct acpi_table_header header;
	u64 entry[MAX_ACPI_TABLES];
} __packed acpi_xsdt_t;

/* HPET timers */
typedef struct acpi_hpet {
	struct acpi_table_header header;
	u32 id;
	struct acpi_gen_regaddr addr;
	u8 number;
	u16 min_tick;
	u8 attributes;
} __packed acpi_hpet_t;

/* MCFG (PCI Express MMIO config space BAR description table) */
typedef struct acpi_mcfg {
	struct acpi_table_header header;
	u8 reserved[8];
} __packed acpi_mcfg_t;

typedef struct acpi_tcpa {
	struct acpi_table_header header;
	u16 platform_class;
	u32 laml;
	u64 lasa;
} __packed acpi_tcpa_t;

typedef struct acpi_mcfg_mmconfig {
	u32 base_address;
	u32 base_reserved;
	u16 pci_segment_group_number;
	u8 start_bus_number;
	u8 end_bus_number;
	u8 reserved[4];
} __packed acpi_mcfg_mmconfig_t;

/* SRAT (System Resource Affinity Table) */
typedef struct acpi_srat {
	struct acpi_table_header header;
	u32 resv;
	u64 resv1;
	/* Followed by static resource allocation structure[n] */
} __packed acpi_srat_t;

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
} __packed acpi_srat_lapic_t;

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
	u32 flags; /* Enable bit 0, hot pluggable bit 1; Non Volatile bit 2,
		    * other bits reserved to 0
		    */
	u32 resv2[2];
} __packed acpi_srat_mem_t;

/* SLIT (System Locality Distance Information Table) */
typedef struct acpi_slit {
	struct acpi_table_header header;
	/* Followed by static resource allocation 8+byte[num*num] */
} __packed acpi_slit_t;

/* MADT (Multiple APIC Description Table) */
typedef struct acpi_madt {
	struct acpi_table_header header;
	u32 lapic_addr;			/* Local APIC address */
	u32 flags;			/* Multiple APIC flags */
} __packed acpi_madt_t;

/* VFCT image header */
struct acpi_vfct_image_hdr {
	u32 PCIBus;
	u32 PCIDevice;
	u32 PCIFunction;
	u16 VendorID;
	u16 DeviceID;
	u16 SSVID;
	u16 SSID;
	u32 Revision;
	u32 ImageLength;
	u8  VbiosContent;	// dummy - copy VBIOS here
} __packed;

/* VFCT (VBIOS Fetch Table) */
struct acpi_vfct {
	struct acpi_table_header header;
	u8  TableUUID[16];
	u32 VBIOSImageOffset;
	u32 Lib1ImageOffset;
	u32 Reserved[4];
	struct acpi_vfct_image_hdr image_hdr;
} __packed;

typedef struct acpi_ivrs_info {
} __packed acpi_ivrs_info_t;

/* IVRS IVHD (I/O Virtualization Hardware Definition Block) Type 10h */
typedef struct acpi_ivrs_ivhd {
	uint8_t type;
	uint8_t flags;
	uint16_t length;
	uint16_t device_id;
	uint16_t capability_offset;
	uint32_t iommu_base_low;
	uint32_t iommu_base_high;
	uint16_t pci_segment_group;
	uint16_t iommu_info;
	uint32_t iommu_feature_info;
	uint8_t entry[0];
} __packed acpi_ivrs_ivhd_t;

/* IVRS (I/O Virtualization Reporting Structure) Type 10h */
typedef struct acpi_ivrs {
	struct acpi_table_header header;
	uint32_t iv_info;
	uint32_t reserved[2];
	struct acpi_ivrs_ivhd ivhd;
} __packed acpi_ivrs_t;

enum dev_scope_type {
	SCOPE_PCI_ENDPOINT = 1,
	SCOPE_PCI_SUB = 2,
	SCOPE_IOAPIC = 3,
	SCOPE_MSI_HPET = 4
};

typedef struct dev_scope {
	u8 type;
	u8 length;
	u8 reserved[2];
	u8 enumeration;
	u8 start_bus;
	struct {
		u8 dev;
		u8 fn;
	} __packed path[0];
} __packed dev_scope_t;

enum dmar_type {
	DMAR_DRHD = 0,
	DMAR_RMRR = 1,
	DMAR_ATSR = 2,
	DMAR_RHSA = 3
};

enum {
	DRHD_INCLUDE_PCI_ALL = 1
};

enum dmar_flags {
	DMAR_INTR_REMAP		= 1,
	DMAR_X2APIC_OPT_OUT	= 2,
};

typedef struct dmar_entry {
	u16 type;
	u16 length;
	u8 flags;
	u8 reserved;
	u16 segment;
	u64 bar;
} __packed dmar_entry_t;

typedef struct dmar_rmrr_entry {
	u16 type;
	u16 length;
	u16 reserved;
	u16 segment;
	u64 bar;
	u64 limit;
} __packed dmar_rmrr_entry_t;

typedef struct dmar_atsr_entry {
	u16 type;
	u16 length;
	u8 flags;
	u8 reserved;
	u16 segment;
} __packed dmar_atsr_entry_t;

/* DMAR (DMA Remapping Reporting Structure) */
typedef struct acpi_dmar {
	struct acpi_table_header header;
	u8 host_address_width;
	u8 flags;
	u8 reserved[10];
	dmar_entry_t structure[0];
} __packed acpi_dmar_t;

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
} __packed acpi_madt_lapic_t;

/* MADT: Local APIC NMI Structure */
typedef struct acpi_madt_lapic_nmi {
	u8 type;			/* Type (4) */
	u8 length;			/* Length in bytes (6) */
	u8 processor_id;		/* ACPI processor ID */
	u16 flags;			/* MPS INTI flags */
	u8 lint;			/* Local APIC LINT# */
} __packed acpi_madt_lapic_nmi_t;

/* MADT: I/O APIC Structure */
typedef struct acpi_madt_ioapic {
	u8 type;			/* Type (1) */
	u8 length;			/* Length in bytes (12) */
	u8 ioapic_id;			/* I/O APIC ID */
	u8 reserved;
	u32 ioapic_addr;		/* I/O APIC address */
	u32 gsi_base;			/* Global system interrupt base */
} __packed acpi_madt_ioapic_t;

/* MADT: Interrupt Source Override Structure */
typedef struct acpi_madt_irqoverride {
	u8 type;			/* Type (2) */
	u8 length;			/* Length in bytes (10) */
	u8 bus;				/* ISA (0) */
	u8 source;			/* Bus-relative int. source (IRQ) */
	u32 gsirq;			/* Global system interrupt */
	u16 flags;			/* MPS INTI flags */
} __packed acpi_madt_irqoverride_t;

#define ACPI_DBG2_PORT_SERIAL			0x8000
#define  ACPI_DBG2_PORT_SERIAL_16550		0x0000
#define  ACPI_DBG2_PORT_SERIAL_16550_DBGP	0x0001
#define  ACPI_DBG2_PORT_SERIAL_ARM_PL011	0x0003
#define  ACPI_DBG2_PORT_SERIAL_ARM_SBSA		0x000e
#define  ACPI_DBG2_PORT_SERIAL_ARM_DDC		0x000f
#define  ACPI_DBG2_PORT_SERIAL_BCM2835		0x0010
#define ACPI_DBG2_PORT_IEEE1394			0x8001
#define  ACPI_DBG2_PORT_IEEE1394_STANDARD	0x0000
#define ACPI_DBG2_PORT_USB			0x8002
#define  ACPI_DBG2_PORT_USB_XHCI		0x0000
#define  ACPI_DBG2_PORT_USB_EHCI		0x0001
#define ACPI_DBG2_PORT_NET			0x8003

/* DBG2: Microsoft Debug Port Table 2 header */
typedef struct acpi_dbg2_header {
	struct acpi_table_header header;
	uint32_t devices_offset;
	uint32_t devices_count;
} __attribute__((packed)) acpi_dbg2_header_t;

/* DBG2: Microsoft Debug Port Table 2 device entry */
typedef struct acpi_dbg2_device {
	uint8_t  revision;
	uint16_t length;
	uint8_t  address_count;
	uint16_t namespace_string_length;
	uint16_t namespace_string_offset;
	uint16_t oem_data_length;
	uint16_t oem_data_offset;
	uint16_t port_type;
	uint16_t port_subtype;
	uint8_t  reserved[2];
	uint16_t base_address_offset;
	uint16_t address_size_offset;
} __attribute__((packed)) acpi_dbg2_device_t;

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
} __packed acpi_fadt_t;

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
#define ACPI_FADT_LEGACY_FREE	0x00	/* No legacy devices (including 8042) */

/* FADT Preferred Power Management Profile */
enum acpi_preferred_pm_profiles {
	PM_UNSPECIFIED		= 0,
	PM_DESKTOP		= 1,
	PM_MOBILE		= 2,
	PM_WORKSTATION		= 3,
	PM_ENTERPRISE_SERVER	= 4,
	PM_SOHO_SERVER		= 5,
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
} __packed acpi_facs_t;

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
} __packed acpi_ecdt_t;

/* HEST (Hardware Error Source Table) */
typedef struct acpi_hest {
	struct acpi_table_header header;
	u32 error_source_count;
	/* error_source_struct(s) */
} __packed acpi_hest_t;

/* Error Source Descriptors */
typedef struct acpi_hest_esd {
	u16 type;
	u16 source_id;
	u16 resv;
	u8 flags;
	u8 enabled;
	u32 prealloc_erecords;		/* The number of error records to
					 * pre-allocate for this error source.
					 */
	u32 max_section_per_record;
} __packed acpi_hest_esd_t;

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
} __packed acpi_hest_hen_t;

/* Generic Error Data Entry (ACPI spec v6.2-A, table 382) */
typedef struct acpi_hest_generic_data {
	guid_t section_type;
	u32 error_severity;
	u16 revision;
	u8 validation_bits;
	u8 flags;
	u32 data_length;
	guid_t fru_id;
	u8 fru_text[20];
	/* error data */
} __packed acpi_hest_generic_data_t;

/* Generic Error Data Entry (ACPI spec v6.2-A, table 382) */
typedef struct acpi_hest_generic_data_v300 {
	guid_t section_type;
	u32 error_severity;
	u16 revision;
	u8 validation_bits;
	u8 flags;		/* see CPER Section Descriptor, Flags field */
	u32 data_length;
	guid_t fru_id;
	u8 fru_text[20];
	cper_timestamp_t timestamp;
	/* error data */
} __packed acpi_hest_generic_data_v300_t;
#define HEST_GENERIC_ENTRY_V300			0x300

/* Both Generic Error Status & Generic Error Data Entry, Error Severity field */
#define ACPI_GENERROR_SEV_RECOVERABLE		0
#define ACPI_GENERROR_SEV_FATAL			1
#define ACPI_GENERROR_SEV_CORRECTED		2
#define ACPI_GENERROR_SEV_NONE			3

/* Generic Error Data Entry, Validation Bits field */
#define ACPI_GENERROR_VALID_FRUID		BIT(0)
#define ACPI_GENERROR_VALID_FRUID_TEXT		BIT(1)
#define ACPI_GENERROR_VALID_TIMESTAMP		BIT(2)

/* Generic Error Status Block (ACPI spec v6.2-A, table 381) */
typedef struct acpi_generic_error_status {
	u32 block_status;
	u32 raw_data_offset;	/* must follow any generic entries */
	u32 raw_data_length;
	u32 data_length;	/* generic data */
	u32 error_severity;
	/* Generic Error Data structures, zero or more entries */
} __packed acpi_generic_error_status_t;

/* Generic Status Block, Block Status values */
#define GENERIC_ERR_STS_UNCORRECTABLE_VALID	BIT(0)
#define GENERIC_ERR_STS_CORRECTABLE_VALID	BIT(1)
#define GENERIC_ERR_STS_MULT_UNCORRECTABLE	BIT(2)
#define GENERIC_ERR_STS_MULT_CORRECTABLE	BIT(3)
#define GENERIC_ERR_STS_ENTRY_COUNT_SHIFT	4
#define GENERIC_ERR_STS_ENTRY_COUNT_MAX		0x3ff
#define GENERIC_ERR_STS_ENTRY_COUNT_MASK	\
					(GENERIC_ERR_STS_ENTRY_COUNT_MAX \
					<< GENERIC_ERR_STS_ENTRY_COUNT_SHIFT)

typedef struct acpi_cstate {
	u8  ctype;
	u16 latency;
	u32 power;
	acpi_addr_t resource;
} __packed acpi_cstate_t;

typedef struct acpi_tstate {
	u32 percent;
	u32 power;
	u32 latency;
	u32 control;
	u32 status;
} __packed acpi_tstate_t;

/* Port types for ACPI _UPC object */
enum acpi_upc_type {
	/* These types are defined in ACPI 6.2 section 9.14 */
	UPC_TYPE_A,
	UPC_TYPE_MINI_AB,
	UPC_TYPE_EXPRESSCARD,
	UPC_TYPE_USB3_A,
	UPC_TYPE_USB3_B,
	UPC_TYPE_USB3_MICRO_B,
	UPC_TYPE_USB3_MICRO_AB,
	UPC_TYPE_USB3_POWER_B,
	UPC_TYPE_C_USB2_ONLY,
	UPC_TYPE_C_USB2_SS_SWITCH,
	UPC_TYPE_C_USB2_SS,
	UPC_TYPE_PROPRIETARY = 0xff,
	/*
	 * The following types are not directly defined in the ACPI
	 * spec but are used by coreboot to identify a USB device type.
	 */
	UPC_TYPE_INTERNAL = 0xff,
	UPC_TYPE_UNUSED,
	UPC_TYPE_HUB
};

unsigned long fw_cfg_acpi_tables(unsigned long start);

/* These are implemented by the target port or north/southbridge. */
unsigned long write_acpi_tables(unsigned long addr);
unsigned long acpi_fill_madt(unsigned long current);
unsigned long acpi_fill_mcfg(unsigned long current);
unsigned long acpi_fill_ivrs_ioapic(acpi_ivrs_t *ivrs, unsigned long current);
void acpi_create_ssdt_generator(acpi_header_t *ssdt, const char *oem_table_id);
void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt);
#if IS_ENABLED(CONFIG_COMMON_FADT)
void acpi_fill_fadt(acpi_fadt_t *fadt);
#endif

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
int acpi_create_srat_mem(acpi_srat_mem_t *mem, u8 node, u32 basek, u32 sizek,
			 u32 flags);
int acpi_create_mcfg_mmconfig(acpi_mcfg_mmconfig_t *mmconfig, u32 base,
			      u16 seg_nr, u8 start, u8 end);
unsigned long acpi_create_srat_lapics(unsigned long current);
void acpi_create_srat(acpi_srat_t *srat,
		      unsigned long (*acpi_fill_srat)(unsigned long current));

void acpi_create_slit(acpi_slit_t *slit,
		      unsigned long (*acpi_fill_slit)(unsigned long current));

void acpi_create_vfct(struct device *device,
		      struct acpi_vfct *vfct,
		      unsigned long (*acpi_fill_vfct)(struct device *device,
				struct acpi_vfct *vfct_struct,
				unsigned long current));

void acpi_create_ivrs(acpi_ivrs_t *ivrs,
		      unsigned long (*acpi_fill_ivrs)(acpi_ivrs_t *ivrs_struct,
		      unsigned long current));

#if ENV_RAMSTAGE && !defined(__SIMPLE_DEVICE__)
void acpi_create_hpet(acpi_hpet_t *hpet);
unsigned long acpi_write_hpet(struct device *device, unsigned long start,
			      acpi_rsdp_t *rsdp);

/* cpu/intel/speedstep/acpi.c */
void generate_cpu_entries(struct device *device);
#endif

void acpi_create_mcfg(acpi_mcfg_t *mcfg);

void acpi_create_facs(acpi_facs_t *facs);

void acpi_create_dbg2(acpi_dbg2_header_t *dbg2_header,
		      int port_type, int port_subtype,
		      acpi_addr_t *address, uint32_t address_size,
		      const char *device_path);

unsigned long acpi_write_dbg2_pci_uart(acpi_rsdp_t *rsdp, unsigned long current,
				       struct device *dev, uint8_t access_size);
void acpi_create_dmar(acpi_dmar_t *dmar, enum dmar_flags flags,
		      unsigned long (*acpi_fill_dmar)(unsigned long));
unsigned long acpi_create_dmar_drhd(unsigned long current, u8 flags,
				    u16 segment, u64 bar);
unsigned long acpi_create_dmar_rmrr(unsigned long current, u16 segment,
				    u64 bar, u64 limit);
unsigned long acpi_create_dmar_atsr(unsigned long current, u8 flags,
				    u16 segment);
void acpi_dmar_drhd_fixup(unsigned long base, unsigned long current);
void acpi_dmar_rmrr_fixup(unsigned long base, unsigned long current);
void acpi_dmar_atsr_fixup(unsigned long base, unsigned long current);
unsigned long acpi_create_dmar_ds_pci_br(unsigned long current,
					   u8 bus, u8 dev, u8 fn);
unsigned long acpi_create_dmar_ds_pci(unsigned long current,
					   u8 bus, u8 dev, u8 fn);
unsigned long acpi_create_dmar_ds_ioapic(unsigned long current,
					      u8 enumeration_id,
					      u8 bus, u8 dev, u8 fn);
unsigned long acpi_create_dmar_ds_msi_hpet(unsigned long current,
						u8 enumeration_id,
						u8 bus, u8 dev, u8 fn);
void acpi_write_hest(acpi_hest_t *hest,
		     unsigned long (*acpi_fill_hest)(acpi_hest_t *hest));

unsigned long acpi_create_hest_error_source(acpi_hest_t *hest,
	acpi_hest_esd_t *esd, u16 type, void *data, u16 len);

/* For ACPI S3 support. */
void acpi_fail_wakeup(void);
void acpi_resume(void *wake_vec);
void acpi_prepare_resume_backup(void);
void mainboard_suspend_resume(void);
void *acpi_find_wakeup_vector(void);

enum {
	ACPI_S0,
	ACPI_S1,
	ACPI_S2,
	ACPI_S3,
	ACPI_S4,
	ACPI_S5,
};

#if IS_ENABLED(CONFIG_ACPI_INTEL_HARDWARE_SLEEP_VALUES) \
		|| IS_ENABLED(CONFIG_ACPI_AMD_HARDWARE_SLEEP_VALUES)
/* Given the provided PM1 control register return the ACPI sleep type. */
static inline int acpi_sleep_from_pm1(uint32_t pm1_cnt)
{
	switch (((pm1_cnt) & SLP_TYP) >> SLP_TYP_SHIFT) {
	case SLP_TYP_S0: return ACPI_S0;
	case SLP_TYP_S1: return ACPI_S1;
	case SLP_TYP_S3: return ACPI_S3;
	case SLP_TYP_S4: return ACPI_S4;
	case SLP_TYP_S5: return ACPI_S5;
	}
	return -1;
}
#endif

/* Returns ACPI_Sx values. */
int acpi_get_sleep_type(void);

/* Read and clear GPE status */
int acpi_get_gpe(int gpe);

static inline int acpi_s3_resume_allowed(void)
{
	return IS_ENABLED(CONFIG_HAVE_ACPI_RESUME);
}

/* Return address in reserved memory where to backup low memory
 * while platform resumes from S3 suspend. Caller is responsible of
 * making a complete copy of the region base..base+size, with
 * parameteres base and size that meet page alignment requirement.
 */
void *acpi_backup_container(uintptr_t base, size_t size);

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)

#ifdef __PRE_RAM__
static inline int acpi_is_wakeup_s3(void)
{
	return (acpi_get_sleep_type() == ACPI_S3);
}
#else
int acpi_is_wakeup(void);
int acpi_is_wakeup_s3(void);
int acpi_is_wakeup_s4(void);
#endif

#else
static inline int acpi_is_wakeup(void) { return 0; }
static inline int acpi_is_wakeup_s3(void) { return 0; }
static inline int acpi_is_wakeup_s4(void) { return 0; }
#endif

static inline uintptr_t acpi_align_current(uintptr_t current)
{
	return ALIGN(current, 16);
}

/* ACPI table revisions should match the revision of the ACPI spec
 * supported. This function keeps the table versions synced. This could
 * be made into a weak function if there is ever a need to override the
 * coreboot default ACPI spec version supported. */
int get_acpi_table_revision(enum acpi_tables table);

#endif  // !defined(__ASSEMBLER__) && !defined(__ACPI__) && !defined(__ROMC__)

#endif  /* __ASM_ACPI_H */
