/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * coreboot ACPI support - headers and defines.
 */

#ifndef __ACPI_ACPI_H__
#define __ACPI_ACPI_H__

/*
 * The type and enable fields are common in ACPI, but the
 * values themselves are hardware implementation defined.
 */
#if CONFIG(ACPI_INTEL_HARDWARE_SLEEP_VALUES)
 #define SLP_EN		(1 << 13)
 #define SLP_TYP_SHIFT	10
 #define SLP_TYP	(7 << SLP_TYP_SHIFT)
 #define  SLP_TYP_S0	0
 #define  SLP_TYP_S1	1
 #define  SLP_TYP_S3	5
 #define  SLP_TYP_S4	6
 #define  SLP_TYP_S5	7
#elif CONFIG(ACPI_AMD_HARDWARE_SLEEP_VALUES)
 #define SLP_EN		(1 << 13)
 #define SLP_TYP_SHIFT	10
 #define SLP_TYP	(7 << SLP_TYP_SHIFT)
 #define  SLP_TYP_S0	0
 #define  SLP_TYP_S1	1
 #define  SLP_TYP_S3	3
 #define  SLP_TYP_S4	4
 #define  SLP_TYP_S5	5
#endif

#define ACPI_TABLE_CREATOR	"COREBOOT"  /* Must be exactly 8 bytes long! */
#define OEM_ID			"COREv4"    /* Must be exactly 6 bytes long! */
#define ACPI_DSDT_REV_1		0x01        /* DSDT revision: ACPI v1 */
#define ACPI_DSDT_REV_2		0x02        /* DSDT revision: ACPI v2.0 and greater */

#if !defined(__ASSEMBLER__) && !defined(__ACPI__)
#include <commonlib/helpers.h>
#include <device/device.h>
#include <uuid.h>
#include <cper.h>
#include <romstage_handoff.h>
#include <types.h>

enum acpi_device_sleep_states {
	ACPI_DEVICE_SLEEP_D0		= 0,
	ACPI_DEVICE_SLEEP_D1		= 1,
	ACPI_DEVICE_SLEEP_D2		= 2,
	ACPI_DEVICE_SLEEP_D3		= 3,
	ACPI_DEVICE_SLEEP_D3_HOT	= ACPI_DEVICE_SLEEP_D3,
	ACPI_DEVICE_SLEEP_D3_COLD	= 4,
};

#define RSDP_SIG		"RSD PTR "  /* RSDT pointer signature */
#define ASLC			"CORE"      /* Must be exactly 4 bytes long! */

#define ACPI_NAME_BUFFER_SIZE	5 /* 4 chars + 1 NUL */

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

enum acpi_tables {
	/* Tables defined by ACPI and used by coreboot */
	BERT, CEDT, DBG2, DMAR, DSDT, EINJ, FACS, FADT, HEST, HMAT, HPET, IVRS,
	MADT, MCFG, RSDP, RSDT, SLIT, SRAT, SSDT, TCPA, TPM2, XSDT, ECDT, LPIT,
	/* Additional proprietary tables used by coreboot */
	VFCT, NHLT, SPMI, CRAT
};

/* RSDP (Root System Description Pointer) */
typedef struct acpi_rsdp {
	char  signature[8];	/* RSDP signature */
	u8    checksum;		/* Checksum of the first 20 bytes */
	char  oem_id[6];	/* OEM ID */
	u8    revision;		/* RSDP revision */
	u32   rsdt_address;	/* Physical address of RSDT (32 bits) */
	u32   length;		/* Total RSDP length (incl. extended part) */
	u64   xsdt_address;	/* Physical address of XSDT (64 bits) */
	u8    ext_checksum;	/* Checksum of the whole table */
	u8    reserved[3];
} __packed acpi_rsdp_t;

/* GAS (Generic Address Structure) */
typedef struct acpi_gen_regaddr {
	u8  space_id;		/* Address space ID */
	u8  bit_width;		/* Register size in bits */
	u8  bit_offset;		/* Register bit offset */
	u8  access_size;	/* Access size since ACPI 2.0c */
	u32 addrl;		/* Register address, low 32 bits */
	u32 addrh;		/* Register address, high 32 bits */
} __packed acpi_addr_t;

#define ACPI_ADDRESS_SPACE_MEMORY		0	/* System memory */
#define ACPI_ADDRESS_SPACE_IO			1	/* System I/O */
#define ACPI_ADDRESS_SPACE_PCI			2	/* PCI config space */
#define ACPI_ADDRESS_SPACE_EC			3	/* Embedded controller */
#define ACPI_ADDRESS_SPACE_SMBUS		4	/* SMBus */
#define ACPI_ADDRESS_SPACE_CMOS			5	/* SystemCMOS */
#define ACPI_ADDRESS_SPACE_PCI_BAR_TARGET	6	/* PciBarTarget */
#define ACPI_ADDRESS_SPACE_IPMI			7	/* IPMI */
#define ACPI_ADDRESS_SPACE_GENERAL_PURPOSE_IO	8	/* GeneralPurposeIO */
#define ACPI_ADDRESS_SPACE_GENERIC_SERIAL_BUS	9	/* GenericSerialBus  */
#define ACPI_ADDRESS_SPACE_PCC			0x0A	/* Platform Comm. Channel */
#define ACPI_ADDRESS_SPACE_FIXED		0x7f	/* Functional fixed hardware */
#define  ACPI_FFIXEDHW_VENDOR_INTEL		1	/* Intel */
#define  ACPI_FFIXEDHW_CLASS_HLT		0	/* C1 Halt */
#define  ACPI_FFIXEDHW_CLASS_IO_HLT		1	/* C1 I/O then Halt */
#define  ACPI_FFIXEDHW_CLASS_MWAIT		2	/* MWAIT Native C-state */
#define  ACPI_FFIXEDHW_FLAG_HW_COORD		1	/* Hardware Coordination bit */
#define  ACPI_FFIXEDHW_FLAG_BM_STS		2	/* BM_STS avoidance bit */
/* 0x80-0xbf: Reserved */
/* 0xc0-0xff: OEM defined */

/* Access size definitions for Generic address structure */
#define ACPI_ACCESS_SIZE_UNDEFINED	0	/* Undefined (legacy reasons) */
#define ACPI_ACCESS_SIZE_BYTE_ACCESS	1
#define ACPI_ACCESS_SIZE_WORD_ACCESS	2
#define ACPI_ACCESS_SIZE_DWORD_ACCESS	3
#define ACPI_ACCESS_SIZE_QWORD_ACCESS	4

/* Macros for common resource types */
#define ACPI_REG_MSR(address, offset, width) \
	(acpi_addr_t){ \
		.space_id    = ACPI_ADDRESS_SPACE_FIXED, \
		.access_size = ACPI_ACCESS_SIZE_QWORD_ACCESS, \
		.addrl       = address, \
		.bit_offset  = offset, \
		.bit_width   = width, \
	}

#define ACPI_REG_UNSUPPORTED	(acpi_addr_t){0}

/* Common ACPI HIDs */
#define ACPI_HID_FDC "PNP0700"
#define ACPI_HID_KEYBOARD "PNP0303"
#define ACPI_HID_MOUSE "PNP0F03"
#define ACPI_HID_COM "PNP0501"
#define ACPI_HID_LPT "PNP0400"
#define ACPI_HID_PNP "PNP0C02"
#define ACPI_HID_CONTAINER "PNP0A05"

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
	acpi_header_t header;
	u32 entry[MAX_ACPI_TABLES];
} __packed acpi_rsdt_t;

/* XSDT (Extended System Description Table) */
typedef struct acpi_xsdt {
	acpi_header_t header;
	u64 entry[MAX_ACPI_TABLES];
} __packed acpi_xsdt_t;

/* HPET timers */
typedef struct acpi_hpet {
	acpi_header_t header;
	u32 id;
	acpi_addr_t addr;
	u8 number;
	u16 min_tick;
	u8 attributes;
} __packed acpi_hpet_t;

/* MCFG (PCI Express MMIO config space BAR description table) */
typedef struct acpi_mcfg {
	acpi_header_t header;
	u8 reserved[8];
} __packed acpi_mcfg_t;

typedef struct acpi_tcpa {
	acpi_header_t header;
	u16 platform_class;
	u32 laml;
	u64 lasa;
} __packed acpi_tcpa_t;

typedef struct acpi_tpm2 {
	acpi_header_t header;
	u16 platform_class;
	u8  reserved[2];
	u64 control_area;
	u32 start_method;
	u8  msp[12];
	u32 laml;
	u64 lasa;
} __packed acpi_tpm2_t;

typedef struct acpi_mcfg_mmconfig {
	u32 base_address;
	u32 base_reserved;
	u16 pci_segment_group_number;
	u8 start_bus_number;
	u8 end_bus_number;
	u8 reserved[4];
} __packed acpi_mcfg_mmconfig_t;

/*
 * CEDT (CXL Early Discovery Table)
 * CXL spec 2.0 section 9.14.1
 */
typedef struct acpi_cedt {
	acpi_header_t header;
	/* Followed by CEDT structures[n] */
} __packed acpi_cedt_t;

#define ACPI_CEDT_STRUCTURE_CHBS 0
#define ACPI_CEDT_STRUCTURE_CFMWS 1

#define ACPI_CEDT_CHBS_CXL_VER_1_1 0x00
#define ACPI_CEDT_CHBS_CXL_VER_2_0 0x01

/* CHBS: CXL Host Bridge Structure */
typedef struct acpi_cedt_chbs {
	u8 type; /* Always 0, other values reserved */
	u8 resv1;
	u16 length; /* Length in bytes (32) */
	u32 uid;    /* CXL Host Bridge Unique ID */
	u32 cxl_ver;
	u32 resv2;
	/*
	 * For CXL 1.1, the base is Downstream Port Root Complex Resource Block;
	 * For CXL 2.0, the base is CXL Host Bridge Component Registers.
	 */
	u64 base;
	u64 len;
} __packed acpi_cedt_chbs_t;

#define ACPI_CEDT_CFMWS_RESTRICTION_TYPE_2_MEM (1 << 0)
#define ACPI_CEDT_CFMWS_RESTRICTION_TYPE_3_MEM (1 << 1)
#define ACPI_CEDT_CFMWS_RESTRICTION_VOLATIL    (1 << 2)
#define ACPI_CEDT_CFMWS_RESTRICTION_PERSISTENT (1 << 3)
#define ACPI_CEDT_CFMWS_RESTRICTION_FIXED      (1 << 4)

/* CFMWS: CXL Fixed Memory Window Structure */
typedef struct acpi_cedt_cfmws {
	u8 type; /* Type (0) */
	u8 resv1;
	u16 length; /* Length in bytes (32) */
	u32 resv2;
	u64 base_hpa;		  /* Base of the HPA range, 256MB aligned */
	u64 window_size;	  /* Number of bytes this window represents */
	u8 eniw;		  /* Encoded Number of Interleave Ways */
	u8 interleave_arithmetic; /* Standard Modulo arithmetic (0) */
	u16 resv3;
	u32 hbig; /* Host Bridge Interleave Granularity */
	u16 restriction;
	u16 qtg_id;
	u32 interleave_target[]; /* Interleave Target List */
} __packed acpi_cedt_cfmws_t;

/*
 * HMAT (Heterogeneous Memory Attribute Table)
 * ACPI spec 6.4 section 5.2.27
 */
typedef struct acpi_hmat {
	acpi_header_t header;
	u32 resv;
	/* Followed by HMAT table structure[n] */
} __packed acpi_hmat_t;

/* HMAT: Memory Proximity Domain Attributes structure */
typedef struct acpi_hmat_mpda {
	u16 type;			/* Type (0) */
	u16 resv;
	u32 length;			/* Length in bytes (40) */
	u16 flags;
	u16 resv1;
	u32 proximity_domain_initiator;
	u32 proximity_domain_memory;
	u32 resv2;
	u64 resv3;
	u64 resv4;
} __packed acpi_hmat_mpda_t;

/* HMAT: System Locality Latency and Bandwidth Information structure */
typedef struct acpi_hmat_sllbi {
	u16 type;			/* Type (1) */
	u16 resv;
	u32 length;			/* Length in bytes */
	u8 flags;
	u8 data_type;
	/*
	 * Transfer size defined as a 5-biased power of 2 exponent,
	 * when the bandwidth/latency value is achieved.
	 */
	u8 min_transfer_size;
	u8 resv1;
	u32 num_initiator_domains;
	u32 num_target_domains;
	u32 resv2;
	u64 entry_base_unit;
	/* Followed by initiator proximity domain list */
	/* Followed by target proximity domain list */
	/* Followed by latency / bandwidth values */
} __packed acpi_hmat_sllbi_t;

/* HMAT: Memory Side Cache Information structure */
typedef struct acpi_hmat_msci {
	u16 type;			/* Type (2) */
	u16 resv;
	u32 length;			/* Length in bytes */
	u32 domain;			/* Proximity domain for the memory */
	u32 resv1;
	u64 cache_size;
	/* Describes level, associativity, write policy, cache line size */
	u32 cache_attributes;
	u16 resv2;
	/*
	 * Number of SMBIOS handlers that contribute to the
	 * memory side cache physical devices
	 */
	u16 num_handlers;
	/* Followed by SMBIOS handlers*/
} __packed acpi_hmat_msci_t;

/* SRAT (System Resource Affinity Table) */
typedef struct acpi_srat {
	acpi_header_t header;
	u32 resv;
	u64 resv1;
	/* Followed by static resource allocation structure[n] */
} __packed acpi_srat_t;

#define ACPI_SRAT_STRUCTURE_LAPIC 0
#define ACPI_SRAT_STRUCTURE_MEM   1
#define ACPI_SRAT_STRUCTURE_GIA   5

/* SRAT: Processor Local APIC/SAPIC Affinity Structure */
typedef struct acpi_srat_lapic {
	u8 type;			/* Type (0) */
	u8 length;			/* Length in bytes (16) */
	u8 proximity_domain_7_0;	/* Proximity domain bits[7:0] */
	u8 apic_id;			/* Local APIC ID */
	u32 flags; /* Enable bit 0 = 1, other bits reserved to 0 */
	u8 local_sapic_eid;		/* Local SAPIC EID */
	u8 proximity_domain_31_8[3];	/* Proximity domain bits[31:8] */
	u32 clock_domain;		/* _CDM Clock Domain */
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

/* SRAT: Generic Initiator Affinity Structure (ACPI spec 6.4 section 5.2.16.6) */
typedef struct acpi_srat_gia {
	u8 type;		/* Type (5) */
	u8 length;		/* Length in bytes (32) */
	u8 resv;
	u8 dev_handle_type;	/* Device handle type */
	u32 proximity_domain;	/*Proximity domain */
	u8 dev_handle[16];	/* Device handle */
	u32 flags;
	u32 resv1;
} __packed acpi_srat_gia_t;

#define ACPI_SRAT_GIA_DEV_HANDLE_ACPI 0
#define ACPI_SRAT_GIA_DEV_HANDLE_PCI  1

/* SLIT (System Locality Distance Information Table) */
typedef struct acpi_slit {
	acpi_header_t header;
	/* Followed by static resource allocation 8+byte[num*num] */
} __packed acpi_slit_t;

/* MADT (Multiple APIC Description Table) */
typedef struct acpi_madt {
	acpi_header_t header;
	u32 lapic_addr;			/* Local APIC address */
	u32 flags;			/* Multiple APIC flags */
} __packed acpi_madt_t;

/*
 * LPIT (Low Power Idle Table)
 * Conforms to "Intel Low Power S0 Idle" specification, rev 002 from July 2017.
 */
typedef struct acpi_lpit {
	acpi_header_t header;
} __packed acpi_lpit_t;

/* LPIT: LPI descriptor flags */
typedef struct acpi_lpi_flags {
	uint32_t disabled		:  1;
	uint32_t counter_not_available	:  1;
	uint32_t reserved		: 30;
} __packed acpi_lpi_desc_flags_t;

/* LPIT: LPI descriptor types */
enum acpi_lpi_desc_type {
	ACPI_LPI_DESC_TYPE_NATIVE_CSTATE = 0x00,
	/* type >= 1 reserved */
};

/* LPIT: LPI descriptor header */
typedef struct acpi_lpi_desc_hdr {
	uint32_t type;
	uint32_t length;
	uint16_t uid;
	uint16_t reserved;
} __packed acpi_lpi_desc_hdr_t;

#define ACPI_LPIT_CTR_FREQ_TSC 0

/* LPIT: Native C-state instruction based LPI structure */
typedef struct acpi_lpi_desc_ncst {
	acpi_lpi_desc_hdr_t header;
	acpi_lpi_desc_flags_t flags;
	acpi_addr_t entry_trigger;	/* Entry trigger C-state */
	uint32_t min_residency;		/* Minimum residency or "break-even" in microseconds */
	uint32_t max_latency;		/* Worst case exit latency in microseconds */
	acpi_addr_t residency_counter;
	uint64_t counter_frequency;	/* Frequency in cycles per second - 0 means TSC freq */
} __packed acpi_lpi_desc_ncst_t;

/* VFCT image header */
typedef struct acpi_vfct_image_hdr {
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
} __packed acpi_vfct_image_hdr_t;

/* VFCT (VBIOS Fetch Table) */
typedef struct acpi_vfct {
	acpi_header_t header;
	u8  TableUUID[16];
	u32 VBIOSImageOffset;
	u32 Lib1ImageOffset;
	u32 Reserved[4];
	acpi_vfct_image_hdr_t image_hdr;
} __packed acpi_vfct_t;

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
	acpi_header_t header;
	uint32_t iv_info;
	uint32_t reserved[2];
	struct acpi_ivrs_ivhd ivhd;
} __packed acpi_ivrs_t;

/* CRAT (Component Resource Affinity Table Structure) */
struct acpi_crat_header {
	acpi_header_t header;
	uint32_t total_entries;
	uint16_t num_nodes;
	uint8_t reserved[6];
} __packed;

/* IVHD Type 11h IOMMU Attributes */
typedef struct ivhd11_iommu_attr {
	uint32_t reserved1 : 13;
	uint32_t perf_counters : 4;
	uint32_t perf_counter_banks : 6;
	uint32_t msi_num_ppr : 5;
	uint32_t reserved2 : 4;
} __packed ivhd11_iommu_attr_t;

/* IVRS IVHD (I/O Virtualization Hardware Definition Block) Type 11h */
typedef struct acpi_ivrs_ivhd_11 {
	uint8_t type;
	uint8_t flags;
	uint16_t length;
	uint16_t device_id;
	uint16_t capability_offset;
	uint32_t iommu_base_low;
	uint32_t iommu_base_high;
	uint16_t pci_segment_group;
	uint16_t iommu_info;
	struct ivhd11_iommu_attr iommu_attributes;
	uint32_t efr_reg_image_low;
	uint32_t efr_reg_image_high;
	uint32_t reserved[2];
	uint8_t entry[0];
} __packed acpi_ivrs_ivhd11_t;

enum dev_scope_type {
	SCOPE_PCI_ENDPOINT = 1,
	SCOPE_PCI_SUB = 2,
	SCOPE_IOAPIC = 3,
	SCOPE_MSI_HPET = 4,
	SCOPE_ACPI_NAMESPACE_DEVICE = 5
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
	DMAR_RHSA = 3,
	DMAR_ANDD = 4,
	DMAR_SATC = 5
};

enum {
	DRHD_INCLUDE_PCI_ALL = 1
};

enum {
	ATC_REQUIRED = 1
};

enum dmar_flags {
	DMAR_INTR_REMAP			= 1 << 0,
	DMAR_X2APIC_OPT_OUT		= 1 << 1,
	DMA_CTRL_PLATFORM_OPT_IN_FLAG	= 1 << 2,
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

typedef struct dmar_rhsa_entry {
	u16 type;
	u16 length;
	u32 reserved;
	u64 base_address;
	u32 proximity_domain;
} __packed dmar_rhsa_entry_t;

typedef struct dmar_andd_entry {
	u16 type;
	u16 length;
	u8 reserved[3];
	u8 device_number;
	u8 device_name[];
} __packed dmar_andd_entry_t;

typedef struct dmar_satc_entry {
	u16 type;
	u16 length;
	u8 flags;
	u8 reserved;
	u16 segment_number;
} __packed dmar_satc_entry_t;

/* DMAR (DMA Remapping Reporting Structure) */
typedef struct acpi_dmar {
	acpi_header_t header;
	u8 host_address_width;
	u8 flags;
	u8 reserved[10];
	dmar_entry_t structure[0];
} __packed acpi_dmar_t;

/* MADT: APIC Structure Types */
enum acpi_apic_types {
	LOCAL_APIC,			/* Processor local APIC */
	IO_APIC,			/* I/O APIC */
	IRQ_SOURCE_OVERRIDE,		/* Interrupt source override */
	NMI_TYPE,			/* NMI source */
	LOCAL_APIC_NMI,			/* Local APIC NMI */
	LAPIC_ADDRESS_OVERRIDE,		/* Local APIC address override */
	IO_SAPIC,			/* I/O SAPIC */
	LOCAL_SAPIC,			/* Local SAPIC */
	PLATFORM_IRQ_SOURCES,		/* Platform interrupt sources */
	LOCAL_X2APIC,			/* Processor local x2APIC */
	LOCAL_X2APIC_NMI,		/* Local x2APIC NMI */
	GICC,				/* GIC CPU Interface */
	GICD,				/* GIC Distributor */
	GIC_MSI_FRAME,			/* GIC MSI Frame */
	GICR,				/* GIC Redistributor */
	GIC_ITS,			/* Interrupt Translation Service */
	/* 0x10-0x7f: Reserved */
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

#define ACPI_MADT_LAPIC_NMI_ALL_PROCESSORS	0xff
#define ACPI_MADT_LX2APIC_NMI_ALL_PROCESSORS	((u32)-1)

/* MADT: I/O APIC Structure */
typedef struct acpi_madt_ioapic {
	u8 type;			/* Type (1) */
	u8 length;			/* Length in bytes (12) */
	u8 ioapic_id;			/* I/O APIC ID */
	u8 reserved;
	u32 ioapic_addr;		/* I/O APIC address */
	u32 gsi_base;			/* Global system interrupt base */
} __packed acpi_madt_ioapic_t;

#define MP_IRQ_POLARITY_DEFAULT		0x0
#define MP_IRQ_POLARITY_HIGH		0x1
#define MP_IRQ_POLARITY_LOW		0x3
#define MP_IRQ_POLARITY_MASK		0x3
#define MP_IRQ_TRIGGER_DEFAULT		0x0
#define MP_IRQ_TRIGGER_EDGE		0x4
#define MP_IRQ_TRIGGER_LEVEL		0xc
#define MP_IRQ_TRIGGER_MASK		0xc

/* MADT: Interrupt Source Override Structure */
typedef struct acpi_madt_irqoverride {
	u8 type;			/* Type (2) */
	u8 length;			/* Length in bytes (10) */
	u8 bus;				/* ISA (0) */
	u8 source;			/* Bus-relative int. source (IRQ) */
	u32 gsirq;			/* Global system interrupt */
	u16 flags;			/* MPS INTI flags */
} __packed acpi_madt_irqoverride_t;

/* MADT: Processor Local x2APIC Structure */
typedef struct acpi_madt_lx2apic {
	u8 type;			/* Type (9) */
	u8 length;			/* Length in bytes (16) */
	u16 reserved;
	u32 x2apic_id;			/* Local x2APIC ID */
	u32 flags;			/* Same as Local APIC flags */
	u32 processor_id;		/* ACPI processor ID */
} __packed acpi_madt_lx2apic_t;

/* MADT: Processor Local x2APIC NMI Structure */
typedef struct acpi_madt_lx2apic_nmi {
	u8 type;			/* Type (10) */
	u8 length;			/* Length in bytes (12) */
	u16 flags;			/* Same as MPS INTI flags */
	u32 processor_id;		/* ACPI processor ID */
	u8 lint;			/* Local APIC LINT# */
	u8 reserved[3];
} __packed acpi_madt_lx2apic_nmi_t;

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
	acpi_header_t header;
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
	acpi_header_t header;
	u32 firmware_ctrl;
	u32 dsdt;
	u8 reserved;	/* Should be 0 */
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
	acpi_addr_t reset_reg;
	u8 reset_value;
	u16 ARM_boot_arch;	/* Must be zero if ACPI Revision <= 5.0 */
	u8 FADT_MinorVersion;	/* Must be zero if ACPI Revision <= 5.0 */
	u32 x_firmware_ctl_l;
	u32 x_firmware_ctl_h;
	u32 x_dsdt_l;
	u32 x_dsdt_h;
	acpi_addr_t x_pm1a_evt_blk;
	acpi_addr_t x_pm1b_evt_blk;
	acpi_addr_t x_pm1a_cnt_blk;
	acpi_addr_t x_pm1b_cnt_blk;
	acpi_addr_t x_pm2_cnt_blk;
	acpi_addr_t x_pm_tmr_blk;
	acpi_addr_t x_gpe0_blk;
	acpi_addr_t x_gpe1_blk;
	/* Revision 5 */
	acpi_addr_t sleep_control_reg;
	acpi_addr_t sleep_status_reg;
	/* Revision 6 */
	u64 hypervisor_vendor_identity;
} __packed acpi_fadt_t;

/* FADT TABLE Revision values */
#define ACPI_FADT_REV_ACPI_1	1
#define ACPI_FADT_REV_ACPI_2	3
#define ACPI_FADT_REV_ACPI_3	4
#define ACPI_FADT_REV_ACPI_4	4
#define ACPI_FADT_REV_ACPI_5	5
#define ACPI_FADT_REV_ACPI_6	6

/* FADT Minor Version value:
 *  Bits 0-3: minor version
 *  Bits 4-7: Errata
 *   value of 1 means this is compatible with Errata A,
 *   value of 2 would be compatible with Errata B, and so on
 * Version 6.3 Errata A would be: (1 << 4) | 3
 */
#define ACPI_FADT_MINOR_VERSION_0	0 /* coreboot currently use this version */

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
/* bits 22-31: reserved since ACPI 5.0 */

/* FADT Boot Architecture Flags */
#define ACPI_FADT_LEGACY_DEVICES	(1 << 0)
#define ACPI_FADT_8042			(1 << 1)
#define ACPI_FADT_VGA_NOT_PRESENT	(1 << 2)
#define ACPI_FADT_MSI_NOT_SUPPORTED	(1 << 3)
#define ACPI_FADT_NO_PCIE_ASPM_CONTROL	(1 << 4)
#define ACPI_FADT_NO_CMOS_RTC		(1 << 5)
#define ACPI_FADT_LEGACY_FREE	0x00	/* No legacy devices (including 8042) */

/* FADT ARM Boot Architecture Flags */
#define ACPI_FADT_ARM_PSCI_COMPLIANT	(1 << 0)
#define ACPI_FADT_ARM_PSCI_USE_HVC	(1 << 1)
/* bits 2-16: reserved since ACPI 5.1 */

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
	PM_TABLET		= 8,	/* ACPI 5.0 & greater */
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
	u8 version;				/* FACS version */
	u8 resv1[3];				/* This value is 0 */
	u32 ospm_flags;				/* 64BIT_WAKE_F */
	u8 resv2[24];				/* This value is 0 */
} __packed acpi_facs_t;

/* FACS flags */
#define ACPI_FACS_S4BIOS_F	(1 << 0)
#define ACPI_FACS_64BIT_WAKE_F	(1 << 1)
/* Bits 31..2: reserved */

/* ECDT (Embedded Controller Boot Resources Table) */
typedef struct acpi_ecdt {
	acpi_header_t header;
	acpi_addr_t ec_control;	/* EC control register */
	acpi_addr_t ec_data;	/* EC data register */
	u32 uid;				/* UID */
	u8 gpe_bit;				/* GPE bit */
	u8 ec_id[];				/* EC ID  */
} __packed acpi_ecdt_t;

/* HEST (Hardware Error Source Table) */
typedef struct acpi_hest {
	acpi_header_t header;
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

/* BERT (Boot Error Record Table) */
typedef struct acpi_bert {
	acpi_header_t header;
	u32 region_length;
	u64 error_region;
} __packed acpi_bert_t;

/* Generic Error Data Entry */
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

/* Generic Error Data Entry v300 */
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

/*
 * Generic Error Status Block
 *
 * If there is a raw data section at the end of the generic error status block after the
 * zero or more generic error data entries, raw_data_length indicates the length of the raw
 * section and raw_data_offset is the offset of the beginning of the raw data section from
 * the start of the acpi_generic_error_status block it is contained in. So if raw_data_length
 * is non-zero, raw_data_offset must be at least sizeof(acpi_generic_error_status_t).
 */
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

struct acpi_sw_pstate {
	u32 core_freq;
	u32 power;
	u32 transition_latency;
	u32 bus_master_latency;
	u32 control_value;
	u32 status_value;
} __packed;

struct acpi_xpss_sw_pstate {
	u64 core_freq;
	u64 power;
	u64 transition_latency;
	u64 bus_master_latency;
	u64 control_value;
	u64 status_value;
	u64 control_mask;
	u64 status_mask;
} __packed;

typedef struct acpi_tstate {
	u32 percent;
	u32 power;
	u32 latency;
	u32 control;
	u32 status;
} __packed acpi_tstate_t;

enum acpi_lpi_state_flags {
	ACPI_LPI_STATE_DISABLED = 0,
	ACPI_LPI_STATE_ENABLED
};

/* Low Power Idle State */
struct acpi_lpi_state {
	u32 min_residency_us;
	u32 worst_case_wakeup_latency_us;
	u32 flags;
	u32 arch_context_lost_flags;
	u32 residency_counter_frequency_hz;
	u32 enabled_parent_state;
	acpi_addr_t entry_method;
	acpi_addr_t residency_counter_register;
	acpi_addr_t usage_counter_register;
	const char *state_name;
};

/* Port types for ACPI _UPC object */
enum acpi_upc_type {
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

enum acpi_ipmi_interface_type {
	IPMI_INTERFACE_RESERVED = 0,
	IPMI_INTERFACE_KCS,
	IPMI_INTERFACE_SMIC,
	IPMI_INTERFACE_BT,
	IPMI_INTERFACE_SSIF,
};

#define ACPI_IPMI_PCI_DEVICE_FLAG	(1 << 0)
#define ACPI_IPMI_INT_TYPE_SCI		(1 << 0)
#define ACPI_IPMI_INT_TYPE_APIC		(1 << 1)

/* ACPI IPMI 2.0 */
struct acpi_spmi {
	acpi_header_t header;
	u8 interface_type;
	u8 reserved;
	u16 specification_revision;
	u8 interrupt_type;
	u8 gpe;
	u8 reserved2;
	u8 pci_device_flag;

	u32 global_system_interrupt;
	acpi_addr_t base_address;
	union {
		struct {
			u8 pci_segment_group;
			u8 pci_bus;
			u8 pci_device;
			u8 pci_function;
		};
		u8 uid[4];
	};
	u8 reserved3;
} __packed;

/* EINJ APEI Standard Definitions */
/* EINJ Error Types
   Refer to the ACPI spec, EINJ section, for more info on bit definitions
*/
#define ACPI_EINJ_CPU_CE		(1 << 0)
#define ACPI_EINJ_CPU_UCE		(1 << 1)
#define ACPI_EINJ_CPU_UCE_FATAL		(1 << 2)
#define ACPI_EINJ_MEM_CE		(1 << 3)
#define ACPI_EINJ_MEM_UCE		(1 << 4)
#define ACPI_EINJ_MEM_UCE_FATAL		(1 << 5)
#define ACPI_EINJ_PCIE_CE		(1 << 6)
#define ACPI_EINJ_PCIE_UCE_NON_FATAL	(1 << 7)
#define ACPI_EINJ_PCIE_UCE_FATAL	(1 << 8)
#define ACPI_EINJ_PLATFORM_CE		(1 << 9)
#define ACPI_EINJ_PLATFORM_UCE		(1 << 10)
#define ACPI_EINJ_PLATFORM_UCE_FATAL	(1 << 11)
#define ACPI_EINJ_VENDOR_DEFINED	(1 << 31)
#define ACPI_EINJ_DEFAULT_CAP		(ACPI_EINJ_MEM_CE | ACPI_EINJ_MEM_UCE | \
					ACPI_EINJ_PCIE_CE | ACPI_EINJ_PCIE_UCE_FATAL)

/* EINJ actions */
#define ACTION_COUNT			9
#define BEGIN_INJECT_OP			0x00
#define GET_TRIGGER_ACTION_TABLE	0x01
#define SET_ERROR_TYPE			0x02
#define GET_ERROR_TYPE			0x03
#define END_INJECT_OP			0x04
#define EXECUTE_INJECT_OP		0x05
#define CHECK_BUSY_STATUS		0x06
#define GET_CMD_STATUS			0x07
#define SET_ERROR_TYPE_WITH_ADDRESS	0x08
#define TRIGGER_ERROR			0xFF

/* EINJ Instructions */
#define READ_REGISTER			0x00
#define READ_REGISTER_VALUE		0x01
#define WRITE_REGISTER			0x02
#define WRITE_REGISTER_VALUE		0x03
#define NO_OP				0x04

/* EINJ (Error Injection Table) */
typedef struct acpi_gen_regaddr1 {
	u8  space_id;		/* Address space ID */
	u8  bit_width;		/* Register size in bits */
	u8  bit_offset;		/* Register bit offset */
	u8  access_size;	/* Access size since ACPI 2.0c */
	u64 addr;		/* Register address */
} __packed acpi_addr64_t;

/* Instruction entry */
typedef struct acpi_einj_action_table {
	u8 action;
	u8 instruction;
	u16 flags;
	acpi_addr64_t reg;
	u64 value;
	u64 mask;
} __packed acpi_einj_action_table_t;

typedef struct acpi_injection_header {
	u32 einj_header_size;
	u32 flags;
	u32 entry_count;
} __packed acpi_injection_header_t;

typedef struct acpi_einj_trigger_table {
	u32 header_size;
	u32 revision;
	u32 table_size;
	u32 entry_count;
	acpi_einj_action_table_t trigger_action[1];
} __packed acpi_einj_trigger_table_t;

typedef struct set_error_type {
	u32 errtype;
	u32 vendorerrortype;
	u32 flags;
	u32 apicid;
	u64 memaddr;
	u64 memrange;
	u32 pciesbdf;
} __packed set_error_type_t;

#define EINJ_PARAM_NUM 6
typedef struct acpi_einj_smi {
	u64 op_state;
	u64 err_inject[EINJ_PARAM_NUM];
	u64 trigger_action_table;
	u64 err_inj_cap;
	u64 op_status;
	u64 cmd_sts;
	u64 einj_addr;
	u64 einj_addr_msk;
	set_error_type_t setaddrtable;
	u64 reserved[50];
} __packed acpi_einj_smi_t;

/* EINJ Flags */
#define EINJ_DEF_TRIGGER_PORT	0xb2
#define FLAG_PRESERVE		0x01
#define FLAG_IGNORE		0x00

/* EINJ Registers */
#define EINJ_REG_MEMORY(address) \
	{ \
	.space_id = ACPI_ADDRESS_SPACE_MEMORY, \
	.bit_width = 64, \
	.bit_offset = 0, \
	.access_size = ACPI_ACCESS_SIZE_QWORD_ACCESS, \
	.addr = address}

#define EINJ_REG_IO() \
	{ \
	.space_id = ACPI_ADDRESS_SPACE_IO, \
	.bit_width = 0x10, \
	.bit_offset = 0, \
	.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS, \
	.addr = EINJ_DEF_TRIGGER_PORT} /* HW dependent code can override this also */

typedef struct acpi_einj {
	acpi_header_t header;
	acpi_injection_header_t inj_header;
	acpi_einj_action_table_t action_table[ACTION_COUNT];
} __packed acpi_einj_t;


uintptr_t get_coreboot_rsdp(void);
void acpi_create_einj(acpi_einj_t *einj, uintptr_t addr, u8 actions);

unsigned long fw_cfg_acpi_tables(unsigned long start);

/* These are implemented by the target port or north/southbridge. */
void preload_acpi_dsdt(void);
unsigned long write_acpi_tables(unsigned long addr);
unsigned long acpi_fill_madt(unsigned long current);
unsigned long acpi_fill_ivrs_ioapic(acpi_ivrs_t *ivrs, unsigned long current);
void acpi_create_ssdt_generator(acpi_header_t *ssdt, const char *oem_table_id);
void acpi_write_bert(acpi_bert_t *bert, uintptr_t region, size_t length);
void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt);

void soc_lpi_get_constraints(void *unused);

void acpi_fill_fadt(acpi_fadt_t *fadt);
void arch_fill_fadt(acpi_fadt_t *fadt);
void soc_fill_fadt(acpi_fadt_t *fadt);
void mainboard_fill_fadt(acpi_fadt_t *fadt);

void acpi_fill_gnvs(void);
void acpi_fill_cnvs(void);

unsigned long acpi_fill_lpit(unsigned long current);

/* These can be used by the target port. */
u8 acpi_checksum(u8 *table, u32 length);

void acpi_add_table(acpi_rsdp_t *rsdp, void *table);

/* Create CXL Early Discovery Table */
void acpi_create_cedt(acpi_cedt_t *cedt,
	unsigned long (*acpi_fill_cedt)(unsigned long current));
/* Create a CXL Host Bridge Structure for CEDT */
int acpi_create_cedt_chbs(acpi_cedt_chbs_t *chbs, u32 uid, u32 cxl_ver, u64 base);
/* Create a CXL Fixed Memory Window Structure for CEDT */
int acpi_create_cedt_cfmws(acpi_cedt_cfmws_t *cfmws, u64 base_hpa, u64 window_size,
	u8 eniw, u32 hbig, u16 restriction, u16 qtg_id, const u32 *interleave_target);

int acpi_create_madt_lapic(acpi_madt_lapic_t *lapic, u8 cpu, u8 apic);
int acpi_create_madt_ioapic(acpi_madt_ioapic_t *ioapic, u8 id, u32 addr,
			    u32 gsi_base);
int acpi_create_madt_ioapic_from_hw(acpi_madt_ioapic_t *ioapic, u32 addr);
int acpi_create_madt_irqoverride(acpi_madt_irqoverride_t *irqoverride,
				 u8 bus, u8 source, u32 gsirq, u16 flags);
int acpi_create_madt_lapic_nmi(acpi_madt_lapic_nmi_t *lapic_nmi, u8 cpu,
			       u16 flags, u8 lint);
void acpi_create_madt(acpi_madt_t *madt);
unsigned long acpi_create_madt_lapics(unsigned long current);
unsigned long acpi_create_madt_lapics_with_nmis(unsigned long current);
int acpi_create_madt_lx2apic(acpi_madt_lx2apic_t *lapic, u32 cpu, u32 apic);
int acpi_create_madt_lx2apic_nmi(acpi_madt_lx2apic_nmi_t *lapic_nmi, u32 cpu,
				 u16 flags, u8 lint);
int acpi_create_srat_lapic(acpi_srat_lapic_t *lapic, u8 node, u8 apic);
int acpi_create_srat_mem(acpi_srat_mem_t *mem, u8 node, u32 basek, u32 sizek,
			 u32 flags);
/*
 * Given the Generic Initiator device's BDF, the proximity domain's ID
 * and flag, create Generic Initiator Affinity structure in SRAT.
 */
int acpi_create_srat_gia_pci(acpi_srat_gia_t *gia, u32 proximity_domain,
		u16 seg, u8 bus, u8 dev, u8 func, u32 flags);
int acpi_create_mcfg_mmconfig(acpi_mcfg_mmconfig_t *mmconfig, u32 base,
			      u16 seg_nr, u8 start, u8 end);
unsigned long acpi_create_srat_lapics(unsigned long current);
void acpi_create_srat(acpi_srat_t *srat,
		      unsigned long (*acpi_fill_srat)(unsigned long current));

void acpi_create_slit(acpi_slit_t *slit,
		      unsigned long (*acpi_fill_slit)(unsigned long current));

/*
 * Create a Memory Proximity Domain Attributes structure for HMAT,
 * given proximity domain for the attached initiaor, and
 * proximimity domain for the memory.
 */
int acpi_create_hmat_mpda(acpi_hmat_mpda_t *mpda, u32 initiator, u32 memory);
/* Create Heterogeneous Memory Attribute Table */
void acpi_create_hmat(acpi_hmat_t *hmat,
		      unsigned long (*acpi_fill_hmat)(unsigned long current));

void acpi_create_vfct(const struct device *device,
		      acpi_vfct_t *vfct,
		      unsigned long (*acpi_fill_vfct)(const struct device *device,
				acpi_vfct_t *vfct_struct,
				unsigned long current));

void acpi_create_ipmi(const struct device *device,
		      struct acpi_spmi *spmi,
		      const u16 ipmi_revision,
		      const acpi_addr_t *addr,
		      const enum acpi_ipmi_interface_type type,
		      const s8 gpe_interrupt,
		      const u32 apic_interrupt,
		      const u32 uid);

void acpi_create_ivrs(acpi_ivrs_t *ivrs,
		      unsigned long (*acpi_fill_ivrs)(acpi_ivrs_t *ivrs_struct,
		      unsigned long current));

void acpi_create_crat(struct acpi_crat_header *crat,
		      unsigned long (*acpi_fill_crat)(struct acpi_crat_header *crat_struct,
		      unsigned long current));

void acpi_create_hpet(acpi_hpet_t *hpet);
unsigned long acpi_write_hpet(const struct device *device, unsigned long start,
			      acpi_rsdp_t *rsdp);

/* cpu/intel/speedstep/acpi.c */
void generate_cpu_entries(const struct device *device);

void acpi_create_mcfg(acpi_mcfg_t *mcfg);

void acpi_create_facs(acpi_facs_t *facs);

void acpi_create_dbg2(acpi_dbg2_header_t *dbg2_header,
		      int port_type, int port_subtype,
		      acpi_addr_t *address, uint32_t address_size,
		      const char *device_path);

unsigned long acpi_write_dbg2_pci_uart(acpi_rsdp_t *rsdp, unsigned long current,
				const struct device *dev, uint8_t access_size);
void acpi_create_dmar(acpi_dmar_t *dmar, enum dmar_flags flags,
		      unsigned long (*acpi_fill_dmar)(unsigned long));
unsigned long acpi_create_dmar_drhd(unsigned long current, u8 flags,
				    u16 segment, u64 bar);
unsigned long acpi_create_dmar_rmrr(unsigned long current, u16 segment,
				    u64 bar, u64 limit);
unsigned long acpi_create_dmar_atsr(unsigned long current, u8 flags,
				    u16 segment);
unsigned long acpi_create_dmar_rhsa(unsigned long current, u64 base_addr,
				    u32 proximity_domain);
unsigned long acpi_create_dmar_andd(unsigned long current, u8 device_number,
				    const char *device_name);
unsigned long acpi_create_dmar_satc(unsigned long current, u8 flags,
				    u16 segment);
void acpi_dmar_drhd_fixup(unsigned long base, unsigned long current);
void acpi_dmar_rmrr_fixup(unsigned long base, unsigned long current);
void acpi_dmar_atsr_fixup(unsigned long base, unsigned long current);
void acpi_dmar_satc_fixup(unsigned long base, unsigned long current);
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

void acpi_create_lpit(acpi_lpit_t *lpit);
unsigned long acpi_create_lpi_desc_ncst(acpi_lpi_desc_ncst_t *lpi_desc, uint16_t uid);

/* chipsets that select ACPI_BERT must implement this function */
enum cb_err acpi_soc_get_bert_region(void **region, size_t *length);

/* For ACPI S3 support. */
void __noreturn acpi_resume(void *wake_vec);
void mainboard_suspend_resume(void);
void *acpi_find_wakeup_vector(void);

/* ACPI_Sn assignments are defined to always equal the sleep state numbers */
enum {
	ACPI_S0 = 0,
	ACPI_S1 = 1,
	ACPI_S2 = 2,
	ACPI_S3 = 3,
	ACPI_S4 = 4,
	ACPI_S5 = 5,
};

#if CONFIG(ACPI_INTEL_HARDWARE_SLEEP_VALUES) \
		|| CONFIG(ACPI_AMD_HARDWARE_SLEEP_VALUES)
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

uint8_t acpi_get_preferred_pm_profile(void);

/* Returns ACPI_Sx values. */
int acpi_get_sleep_type(void);

/* Read and clear GPE status */
int acpi_get_gpe(int gpe);

/* Once we enter payload, is SMI handler installed and capable of
   responding to APM_CNT Advanced Power Management Control commands. */
static inline int permanent_smi_handler(void)
{
	return CONFIG(HAVE_SMI_HANDLER);
}

static inline int acpi_s3_resume_allowed(void)
{
	return CONFIG(HAVE_ACPI_RESUME);
}

static inline int acpi_is_wakeup_s3(void)
{
	if (!acpi_s3_resume_allowed())
		return 0;

	if (ENV_ROMSTAGE_OR_BEFORE)
		return (acpi_get_sleep_type() == ACPI_S3);

	return romstage_handoff_is_resume();
}

static inline uintptr_t acpi_align_current(uintptr_t current)
{
	return ALIGN_UP(current, 16);
}

/* ACPI table revisions should match the revision of the ACPI spec
 * supported. This function keeps the table versions synced. This could
 * be made into a weak function if there is ever a need to override the
 * coreboot default ACPI spec version supported. */
int get_acpi_table_revision(enum acpi_tables table);
u8 get_acpi_fadt_minor_version(void);

#endif  // !defined(__ASSEMBLER__) && !defined(__ACPI__)

#endif  /* __ACPI_ACPI_H__ */
