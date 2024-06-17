/* BPDT version 1.7 support */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_CSE_LAYOUT_H
#define SOC_INTEL_COMMON_CSE_LAYOUT_H

#include <sys/types.h>

enum bpdt_entry_type {
	SMIP = 0,
	CSE_RBE = 1,
	CSE_BUP = 2,
	UCODE = 3,
	IBB = 4,
	S_BPDT = 5,
	OBB = 6,
	CSE_MAIN = 7,
	ISH = 8,
	CSE_IDLM = 9,
	IFP_OVERRIDE = 10,
	UTOK = 11,
	UFS_PHY = 12,
	UFS_GPP = 13,
	PMC = 14,
	IUNIT = 15,
	NVM_CFG = 16,
	UEP = 17,
	OEM_KM = 20,
	PAVP = 22,
	IOM_FW = 23,
	NPHY_FW = 24,
	TBT_FW = 25,
	ICC = 32,

	MAX_SUBPARTS,
};

struct bpdt_header {
	uint32_t signature;		/* BPDT_SIGNATURE */
	uint16_t descriptor_count;
	uint8_t version;		/* Layout 1.7 = 2 */
	uint8_t flags;
	uint32_t checksum;
	uint32_t ifwi_version;
	struct {
		uint16_t major;
		uint16_t minor;
		uint16_t build;
		uint16_t hotfix;
	} fit_tool_version;
} __packed;

struct cse_layout {
	uint8_t rom_bypass[16];
	uint16_t size;
	uint16_t redundancy;
	uint32_t checksum;
	uint32_t data_offset;
	uint32_t data_size;
	uint32_t bp1_offset;
	uint32_t bp1_size;
	uint32_t bp2_offset;
	uint32_t bp2_size;
	uint32_t bp3_offset;
	uint32_t bp3_size;
	uint32_t bp4_offset;
	uint32_t bp4_size;
	uint32_t bp5_offset;
	uint32_t bp5_size;
	uint32_t temp_base_addr;
	uint32_t temp_base_size;
	uint32_t flog_offset;
	uint32_t flog_size;
} __packed;

struct bpdt_entry {
	uint32_t type;
	uint32_t offset;
	uint32_t size;
} __packed;

struct subpart_hdr {
	uint32_t signature;		/* SUBPART_SIGNATURE */
	uint32_t count;
	uint8_t hdr_version;		/* Header version = 2 */
	uint8_t entry_version;		/* Entry version = 1 */
	uint8_t length;
	uint8_t reserved;
	uint8_t name[4];
	uint32_t checksum;
} __packed;

struct subpart_entry {
	uint8_t name[12];
	uint32_t offset_bytes;
	uint32_t length;
	uint32_t rsvd2;
} __packed;

struct subpart_entry_manifest_header {
	uint8_t reserved[36];
	struct {
		uint16_t major;
		uint16_t minor;
		uint16_t build;
		uint16_t hotfix;
	} binary_version;
} __packed;

#define BPDT_HEADER_SZ		sizeof(struct bpdt_header)
#define BPDT_ENTRY_SZ		sizeof(struct bpdt_entry)
#define SUBPART_HEADER_SZ	sizeof(struct subpart_hdr)
#define SUBPART_ENTRY_SZ	sizeof(struct subpart_entry)
#define SUBPART_MANIFEST_HDR_SZ	sizeof(struct subpart_entry_manifest_header)

#endif // SOC_INTEL_COMMON_CSE_LAYOUT_H
