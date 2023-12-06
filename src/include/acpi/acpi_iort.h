/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_IORT_H__
#define __ACPI_IORT_H__

#include <acpi/acpi.h>
#include <types.h>

/* IORT - IO Remapping Table revision 6
 * Document number: ARM DEN 0049E.e, Sep 2022
 */
typedef struct acpi_table_iort {
	acpi_header_t header;	/* Common ACPI table header */
	u32 node_count;
	u32 node_offset;
	u32 reserved;
} __packed acpi_iort_t;

/*
 * IORT subtables
 */
typedef struct acpi_iort_node {
	u8 type;
	u16 length;
	u8 revision;
	u32 identifier;
	u32 mapping_count;
	u32 mapping_offset;
	char node_data[];
} __packed acpi_iort_node_t;

/* Values for subtable Type above */
enum acpi_iort_node_type {
	ACPI_IORT_NODE_ITS_GROUP = 0x00,
	ACPI_IORT_NODE_NAMED_COMPONENT = 0x01,
	ACPI_IORT_NODE_PCI_ROOT_COMPLEX = 0x02,
	ACPI_IORT_NODE_SMMU = 0x03,
	ACPI_IORT_NODE_SMMU_V3 = 0x04,
	ACPI_IORT_NODE_PMCG = 0x05,
	ACPI_IORT_NODE_RMR = 0x06,
};

/* ITS Group revision 1 */
typedef struct acpi_iort_its_group {
	u32 its_count;
	u32 identifiers[];	/* GIC ITS identifier array */
} __packed acpi_iort_its_group_t;

/* SMMUv3 revision 5 */
typedef struct acpi_iort_smmu_v3 {
	u64 base_address;	/* SMMUv3 base address */
	u32 flags;
	u32 reserved;
	u64 vatos_address;
	u32 model;
	u32 event_gsiv;
	u32 pri_gsiv;
	u32 gerr_gsiv;
	u32 sync_gsiv;
	u32 pxm;
	u32 id_mapping_index;
} __packed acpi_iort_smmu_v3_t;

/* Masks for Flags field above */
#define ACPI_IORT_SMMU_V3_COHACC_OVERRIDE   (1)
#define ACPI_IORT_SMMU_V3_HTTU_OVERRIDE     (3<<1)
#define ACPI_IORT_SMMU_V3_PXM_VALID         (1<<3)
#define ACPI_IORT_SMMU_V3_DEVICEID_VALID    (1<<4)

typedef struct acpi_iort_id_mapping {
	u32 input_base;		/* Lowest value in input range */
	u32 id_count;		/* Number of IDs */
	u32 output_base;	/* Lowest value in output range */
	u32 output_reference;	/* A reference to the output node */
	u32 flags;
} __packed acpi_iort_id_mapping_t;

/* Masks for Flags field above for IORT subtable */
#define ACPI_IORT_ID_SINGLE_MAPPING (1)

/* Named Component revision 4 */
typedef struct acpi_iort_named_component {
	u32 node_flags;
	u64 memory_properties;	/* Memory access properties */
	u8 memory_address_limit;	/* Memory address size limit */
	char device_name[];	/* Path of namespace object */
} __packed acpi_iort_named_component_t;

/* Masks for Flags field above */
#define ACPI_IORT_NC_STALL_SUPPORTED    (1)
#define ACPI_IORT_NC_PASID_BITS         (31<<1)

typedef struct acpi_iort_root_complex {
	u64 memory_properties;	/* Memory access properties */
	u32 ats_attribute;
	u32 pci_segment_number;
	u8 memory_address_limit;/* Memory address size limit */
	u16 pasid_capabilities;	/* PASID Capabilities */
	u8 reserved;		/* Reserved, must be zero */
	u32 flags;		/* Flags */
} __packed acpi_iort_root_complex_t;

/* Masks for ats_attribute field above */
#define ACPI_IORT_ATS_SUPPORTED         (1)	/* The root complex ATS support */
#define ACPI_IORT_PRI_SUPPORTED         (1<<1)	/* The root complex PRI support */
#define ACPI_IORT_PASID_FWD_SUPPORTED   (1<<2)	/* The root complex PASID forward support */

/* Masks for pasid_capabilities field above */
#define ACPI_IORT_PASID_MAX_WIDTH       (0x1F)	/* Bits 0-4 */

unsigned long acpi_soc_fill_iort(acpi_iort_t *iort, unsigned long current);

/* ITS Group */
unsigned long acpi_iort_its_entry(unsigned long current, acpi_iort_t *iort, acpi_iort_node_t **its,
			u32 its_count, u32 *identifiers);

/* SMMUv3 */
unsigned long acpi_iort_smmuv3_entry(unsigned long current, acpi_iort_t *iort,
				     acpi_iort_node_t **smmu_v3, u64 base, u32 flags);

/* ID mapping */
unsigned long acpi_iort_id_map_entry(unsigned long current, acpi_iort_node_t *node, u32 input_base,
				     u32 id_count, u32 output_base, u32 output_reference,
				     u32 flags);

/* Named Component */
unsigned long acpi_iort_nc_entry(unsigned long current, acpi_iort_t *iort, acpi_iort_node_t **nc,
				 u32 node_flags, u64 memory_properties, u32 memory_address_limit,
				 char *device_name);
/* Root Complex */
unsigned long acpi_iort_rc_entry(unsigned long current, acpi_iort_t *node, acpi_iort_node_t **rc,
				 u64 memory_properties, u32 ats_attribute, u32 pci_segment_number,
				 u8 memory_address_limit, u16 pasid_capabilities);

#endif
