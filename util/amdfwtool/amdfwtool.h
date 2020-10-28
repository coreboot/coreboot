/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _AMD_FW_TOOL_H_
#define _AMD_FW_TOOL_H_

#if defined(__GLIBC__)
typedef unsigned long long int uint64_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
#endif


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
	AMD_WRAPPED_IKEK = 0x21,
	AMD_TOKEN_UNLOCK = 0x22,
	AMD_SEC_GASKET = 0x24,
	AMD_MP2_FW = 0x25,
	AMD_DRIVER_ENTRIES = 0x28,
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
	AMD_FW_L2_PTR = 0x40,
	AMD_FW_PSP_VERSTAGE = 0x52,
	AMD_FW_VERSTAGE_SIG = 0x53,
	AMD_FW_IMC = 0x200,	/* Large enough to be larger than the top BHD entry type. */
	AMD_FW_GEC,
	AMD_FW_XHCI,
	AMD_FW_INVALID,		/* Real last one to detect the last entry in table. */
	AMD_FW_SKIP		/* This is for non-applicable options. */
} amd_fw_type;

typedef enum _amd_bios_type {
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


#define BDT_LVL1 0x1
#define BDT_LVL2 0x2
#define BDT_BOTH (BDT_LVL1 | BDT_LVL2)
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


#define PSP_LVL1 0x1
#define PSP_LVL2 0x2
#define PSP_BOTH (PSP_LVL1 | PSP_LVL2)
typedef struct _amd_fw_entry {
	amd_fw_type type;
	char *filename;
	uint8_t subprog;
	int level;
	uint64_t other;
} amd_fw_entry;

typedef struct _amd_cb_config {
	uint8_t have_whitelist;
	uint8_t unlock_secure;
	uint8_t use_secureos;
	uint8_t load_mp2_fw;
	uint8_t s0i3;
} amd_cb_config;

void register_fw_fuse(char *str);
uint8_t process_config(FILE *config, amd_cb_config *cb_config, uint8_t print_deps);

#define OK 0

#define LINE_EOF (1)
#define LINE_TOO_LONG (2)


#endif	/* _AMD_FW_TOOL_H_ */
