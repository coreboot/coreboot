/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _AMD_FW_TOOL_H_
#define _AMD_FW_TOOL_H_

#include <commonlib/bsd/compiler.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum _amd_fw_type {
	AMD_FW_PSP_PUBKEY = 0,
	AMD_FW_PSP_BOOTLOADER = 1,
	AMD_FW_PSP_SMU_FIRMWARE = 8,
	AMD_FW_PSP_RECOVERY = 3,
	AMD_FW_PSP_RTM_PUBKEY = 5,
	AMD_FW_PSP_SECURED_OS = 2,
	AMD_FW_PSP_NVRAM = 4,
	AMD_FW_PSP_SECURED_DEBUG = 9,
	AMD_FW_PSP_TRUSTLETS = 12,
	AMD_FW_PSP_TRUSTLETKEY = 13,
	AMD_FW_PSP_SMU_FIRMWARE2 = 18,
	AMD_PSP_FUSE_CHAIN = 11,
	AMD_FW_PSP_SMUSCS = 95,
	AMD_DEBUG_UNLOCK = 0x13,
	AMD_HW_IPCFG = 0x20,
	AMD_WRAPPED_IKEK = 0x21,
	AMD_TOKEN_UNLOCK = 0x22,
	AMD_SEC_GASKET = 0x24,
	AMD_MP2_FW = 0x25,
	AMD_DRIVER_ENTRIES = 0x28,
	AMD_FW_KVM_IMAGE = 0x29,
	AMD_S0I3_DRIVER = 0x2d,
	AMD_ABL0 = 0x30,
	AMD_ABL1 = 0x31,
	AMD_ABL2 = 0x32,
	AMD_ABL3 = 0x33,
	AMD_ABL4 = 0x34,
	AMD_ABL5 = 0x35,
	AMD_ABL6 = 0x36,
	AMD_ABL7 = 0x37,
	AMD_FW_PSP_WHITELIST = 0x3a,
	AMD_VBIOS_BTLOADER = 0x3c,
	AMD_FW_L2_PTR = 0x40,
	AMD_FW_USB_PHY = 0x44,
	AMD_FW_TOS_SEC_POLICY = 0x45,
	AMD_FW_DRTM_TA = 0x47,
	AMD_FW_RECOVERYAB_A = 0x48,
	AMD_FW_RECOVERYAB_B = 0x4A,
	AMD_FW_BIOS_TABLE = 0x49,
	AMD_FW_KEYDB_BL = 0x50,
	AMD_FW_KEYDB_TOS = 0x51,
	AMD_FW_PSP_VERSTAGE = 0x52,
	AMD_FW_VERSTAGE_SIG = 0x53,
	AMD_RPMC_NVRAM = 0x54,
	AMD_FW_SPL = 0x55,
	AMD_FW_DMCU_ERAM = 0x58,
	AMD_FW_DMCU_ISR = 0x59,
	AMD_FW_MSMU = 0x5a,
	AMD_FW_SPIROM_CFG = 0x5c,
	AMD_FW_DMCUB = 0x71,
	AMD_FW_PSP_BOOTLOADER_AB = 0x73,
	AMD_TA_IKEK = 0x8d,
	AMD_FW_IMC = 0x200,	/* Large enough to be larger than the top BHD entry type. */
	AMD_FW_GEC,
	AMD_FW_XHCI,
	AMD_FW_INVALID,		/* Real last one to detect the last entry in table. */
	AMD_FW_SKIP		/* This is for non-applicable options. */
} amd_fw_type;

typedef enum _amd_bios_type {
	AMD_BIOS_RTM_PUBKEY = 0x05,
	AMD_BIOS_SIG = 0x07,
	AMD_BIOS_APCB = 0x60,
	AMD_BIOS_APOB = 0x61,
	AMD_BIOS_BIN = 0x62,
	AMD_BIOS_APOB_NV = 0x63,
	AMD_BIOS_PMUI = 0x64,
	AMD_BIOS_PMUD = 0x65,
	AMD_BIOS_UCODE = 0x66,
	AMD_BIOS_APCB_BK = 0x68,
	AMD_BIOS_MP2_CFG = 0x6a,
	AMD_BIOS_PSP_SHARED_MEM = 0x6b,
	AMD_BIOS_L2_PTR =  0x70,
	AMD_BIOS_INVALID,
	AMD_BIOS_SKIP
} amd_bios_type;

typedef enum _amd_addr_mode {
	AMD_ADDR_PHYSICAL = 0,	/* Physical address */
	AMD_ADDR_REL_BIOS,	/* Relative to beginning of image */
	AMD_ADDR_REL_TAB,	/* Relative to table */
	AMD_ADDR_REL_SLOT,	/* Relative to slot */
} amd_addr_mode;

struct second_gen_efs { /* todo: expand for Server products */
	int gen:1; /* Client products only use bit 0 */
	int reserved:31;
} __attribute__((packed));

#define EFS_SECOND_GEN 0
#define EFS_BEFORE_SECOND_GEN 1

typedef struct _embedded_firmware {
	uint32_t signature; /* 0x55aa55aa */
	uint32_t imc_entry;
	uint32_t gec_entry;
	uint32_t xhci_entry;
	uint32_t psp_directory;
	union {
		uint32_t new_psp_directory;
		uint32_t combo_psp_directory;
	};
	uint32_t bios0_entry; /* todo: add way to select correct entry */
	uint32_t bios1_entry;
	uint32_t bios2_entry;
	struct second_gen_efs efs_gen;
	uint32_t bios3_entry;
	uint32_t reserved_2Ch;
	uint32_t promontory_fw_ptr;
	uint32_t lp_promontory_fw_ptr;
	uint32_t reserved_38h;
	uint32_t reserved_3Ch;
	uint8_t spi_readmode_f15_mod_60_6f;
	uint8_t fast_speed_new_f15_mod_60_6f;
	uint8_t reserved_42h;
	uint8_t spi_readmode_f17_mod_00_2f;
	uint8_t spi_fastspeed_f17_mod_00_2f;
	uint8_t qpr_dummy_cycle_f17_mod_00_2f;
	uint8_t reserved_46h;
	uint8_t spi_readmode_f17_mod_30_3f;
	uint8_t spi_fastspeed_f17_mod_30_3f;
	uint8_t micron_detect_f17_mod_30_3f;
	uint8_t reserved_4Ah;
	uint8_t reserved_4Bh;
	uint32_t reserved_4Ch;
} __attribute__((packed, aligned(16))) embedded_firmware;

typedef struct _psp_directory_header {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	union {
		uint32_t additional_info;
		struct {
			uint32_t dir_size:10;
			uint32_t spi_block_size:4;
			uint32_t base_addr:15;
			uint32_t address_mode:2;
			uint32_t not_used:1;
		} __attribute__((packed)) additional_info_fields;
	};
} __attribute__((packed, aligned(16))) psp_directory_header;

typedef struct _psp_directory_entry {
	uint8_t type;
	uint8_t subprog;
	uint16_t rsvd;
	uint32_t size;
	uint64_t addr:62; /* or a value in some cases */
	uint64_t address_mode:2;
} __attribute__((packed)) psp_directory_entry;

typedef struct _psp_directory_table {
	psp_directory_header header;
	psp_directory_entry entries[];
} __attribute__((packed, aligned(16))) psp_directory_table;

#define MAX_PSP_ENTRIES 0x2f

typedef struct _psp_combo_header {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	uint32_t lookup;
	uint64_t reserved[2];
} __attribute__((packed, aligned(16))) psp_combo_header;

typedef struct _psp_combo_entry {
	uint32_t id_sel;
	uint32_t id;
	uint64_t lvl2_addr;
} __attribute__((packed)) psp_combo_entry;

typedef struct _psp_combo_directory {
	psp_combo_header header;
	psp_combo_entry entries[];
} __attribute__((packed, aligned(16))) psp_combo_directory;

#define MAX_COMBO_ENTRIES 1

typedef struct _bios_directory_hdr {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	union {
		uint32_t additional_info;
		struct {
			uint32_t dir_size:10;
			uint32_t spi_block_size:4;
			uint32_t base_addr:15;
			uint32_t address_mode:2;
			uint32_t not_used:1;
		} __attribute__((packed)) additional_info_fields;
	};
} __attribute__((packed, aligned(16))) bios_directory_hdr;

typedef struct _bios_directory_entry {
	uint8_t type;
	uint8_t region_type;
	int reset:1;
	int copy:1;
	int ro:1;
	int compressed:1;
	int inst:4;
	uint8_t subprog; /* b[7:3] reserved */
	uint32_t size;
	uint64_t source:62;
	uint64_t address_mode:2;
	uint64_t dest;
} __attribute__((packed)) bios_directory_entry;

typedef struct _bios_directory_table {
	bios_directory_hdr header;
	bios_directory_entry entries[];
} bios_directory_table;

#define MAX_BIOS_ENTRIES 0x2f

#define BDT_LVL1 (1 << 0)
#define BDT_LVL2 (1 << 1)
#define BDT_LVL1_AB (1 << 2)
#define BDT_LVL2_AB (1 << 3)
#define BDT_BOTH (BDT_LVL1 | BDT_LVL2)
#define BDT_BOTH_AB (BDT_LVL1_AB | BDT_LVL2_AB)
typedef struct _amd_bios_entry {
	amd_bios_type type;
	char *filename;
	int subpr;
	int region_type;
	int reset;
	int copy;
	int ro;
	int zlib;
	int inst;
	uint64_t src;
	uint64_t dest;
	size_t size;
	int level;
} amd_bios_entry;

typedef struct _ish_directory_table {
	uint32_t checksum;
	uint32_t boot_priority;
	uint32_t update_retry_count;
	uint8_t  glitch_retry_count;
	uint8_t  glitch_higherbits_reserved[3];
	uint32_t pl2_location;
	uint32_t psp_id;
	uint32_t slot_max_size;
	uint32_t reserved;
} __attribute__((packed)) ish_directory_table;

#define EMBEDDED_FW_SIGNATURE 0x55aa55aa
#define PSP_COOKIE 0x50535024		/* 'PSP$' */
#define PSPL2_COOKIE 0x324c5024		/* '2LP$' */
#define PSP2_COOKIE 0x50535032		/* 'PSP2' */
#define BHD_COOKIE 0x44484224		/* 'DHB$ */
#define BHDL2_COOKIE 0x324c4224		/* '2LB$ */

#define PSP_LVL1 (1 << 0)
#define PSP_LVL2 (1 << 1)
#define PSP_LVL1_AB (1 << 2)
#define PSP_LVL2_AB (1 << 3)
#define PSP_BOTH (PSP_LVL1 | PSP_LVL2)
#define PSP_BOTH_AB (PSP_LVL1_AB | PSP_LVL2_AB)
typedef struct _amd_fw_entry {
	amd_fw_type type;
	char *filename;
	uint8_t subprog;
	uint64_t dest;
	size_t size;
	int level;
	uint64_t other;
} amd_fw_entry;

typedef struct _amd_cb_config {
	bool have_whitelist;
	bool unlock_secure;
	bool use_secureos;
	bool load_mp2_fw;
	bool multi_level;
	bool s0i3;
	bool second_gen;
	bool have_mb_spl;
	bool recovery_ab;
	bool recovery_ab_single_copy;
	bool need_ish;
	bool use_combo;
} amd_cb_config;

void register_fw_fuse(char *str);
uint8_t process_config(FILE *config, amd_cb_config *cb_config, uint8_t print_deps);

#define OK 0

#define LINE_EOF (1)
#define LINE_TOO_LONG (2)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#endif	/* _AMD_FW_TOOL_H_ */
