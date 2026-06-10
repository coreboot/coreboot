/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _AMD_PSP_PSP_DIRECTORY_H_
#define  _AMD_PSP_PSP_DIRECTORY_H_

#include <commonlib/bsd/compiler.h>
#include <stdint.h>

enum amd_addr_mode {
	AMD_ADDR_PHYSICAL = 0,	/* Physical address */
	AMD_ADDR_REL_BIOS,	/* Relative to beginning of image */
	AMD_ADDR_REL_TAB,	/* Relative to table */
	AMD_ADDR_REL_SLOT,	/* Relative to slot */
	AMD_ADDR_AUTO,
};

/*
 * AMD PSP directory structures.
 * Document #55758.
 */
struct psp_directory_header {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	union {
		uint32_t additional_info;
		struct {
			uint32_t dir_size:10;		/* in 4K blocks */
			uint32_t spi_block_size:4;
			uint32_t base_addr:15;		/* [26:12] of directory base addr */
			uint32_t address_mode:2;	/* directory address mode */
			uint32_t version:1;		/* Always 0 */
		} __packed additional_info_fields;
		struct {
			uint32_t dir_size:16;		/* in 4K blocks */
			uint32_t spi_block_size:4;	/* 4K << (1 << value) */
			uint32_t dir_hdr_size:4;	/* in 1K blocks */
			uint32_t address_mode:2;	/* directory address mode */
			uint32_t reserved:5;
			uint32_t version:1;		/* Always 1 */
		} __packed additional_info_fields_v1;
	};
} __packed __aligned(16);

struct psp_directory_entry {
	uint8_t type;
	uint8_t subprog;
	union {
		uint16_t rsvd;
		struct {
			uint8_t rom_id:2;
			uint8_t writable:1;
			uint8_t inst:4;
			uint8_t rsvd_1:1;
			uint8_t rsvd_2:8;
		} __packed;
	};
	uint32_t size;
	uint64_t addr:62; /* or a value in some cases */
	uint64_t address_mode:2;
} __packed;

struct psp_directory_table {
	struct psp_directory_header header;
	struct psp_directory_entry entries[];
} __packed __aligned(16);

#define PSP_COOKIE 0x50535024		/* 'PSP$' */
#define PSPL2_COOKIE 0x324c5024		/* '2LP$' */

enum amd_fw_type {
	AMD_FW_PSP_PUBKEY = 0x00,
	AMD_FW_PSP_BOOTLOADER = 0x01,
	AMD_FW_PSP_SECURED_OS = 0x02,
	AMD_FW_PSP_RECOVERY = 0x03,
	AMD_FW_PSP_NVRAM = 0x04,
	AMD_FW_PSP_RTM_PUBKEY = 0x05,
	AMD_FW_PSP_SMU_FIRMWARE = 0x08,
	AMD_FW_PSP_SECURED_DEBUG = 0x09,
	AMD_FW_PSP_ABL_PUBKEY = 0x0a,
	AMD_FW_PSP_FUSE_CHAIN = 0x0b,
	AMD_FW_PSP_TRUSTLETS = 0x0c,
	AMD_FW_PSP_TRUSTLETKEY = 0x0d,
	AMD_FW_PSP_SMU_FIRMWARE2 = 0x12,
	AMD_FW_PSP_DEBUG_UNLOCK = 0x13,
	AMD_FW_PSP_TEEIPKEY = 0x15,
	AMD_FW_PSP_SEV_DRIVER = 0x1a,
	AMD_FW_PSP_BOOT_DRIVER = 0x1b,
	AMD_FW_PSP_SOC_DRIVER = 0x1c,
	AMD_FW_PSP_DEBUG_DRIVER = 0x1d,
	AMD_FW_PSP_INTERFACE_DRIVER = 0x1f,
	AMD_FW_PSP_HW_IPCFG = 0x20,
	AMD_FW_PSP_WRAPPED_IKEK = 0x21,
	AMD_FW_PSP_TOKEN_UNLOCK = 0x22,
	AMD_FW_PSP_SEC_GASKET = 0x24,
	AMD_FW_PSP_MP2_FW = 0x25,
	AMD_FW_PSP_DRIVER_ENTRIES = 0x28,
	AMD_FW_PSP_KVM_IMAGE = 0x29,
	AMD_FW_PSP_MP5 = 0x2a,
	AMD_FW_PSP_S0I3_DRIVER = 0x2d,
	AMD_FW_PSP_ABL0 = 0x30,
	AMD_FW_PSP_ABL1 = 0x31,
	AMD_FW_PSP_ABL2 = 0x32,
	AMD_FW_PSP_ABL3 = 0x33,
	AMD_FW_PSP_ABL4 = 0x34,
	AMD_FW_PSP_ABL5 = 0x35,
	AMD_FW_PSP_ABL6 = 0x36,
	AMD_FW_PSP_ABL7 = 0x37,
	AMD_FW_PSP_SEV_DATA = 0x38,
	AMD_FW_PSP_SEV_CODE = 0x39,
	AMD_FW_PSP_WHITELIST = 0x3a,
	AMD_FW_PSP_VBIOS_BTLOADER = 0x3c,
	AMD_FW_PSP_L2_PTR = 0x40,
	AMD_FW_PSP_DXIO = 0x42,
	AMD_FW_PSP_USB_PHY = 0x44,
	AMD_FW_PSP_TOS_SEC_POLICY = 0x45,
	AMD_FW_PSP_DRTM_TA = 0x47,
	AMD_FW_PSP_RECOVERYAB_A = 0x48,
	AMD_FW_PSP_RECOVERYAB_B = 0x4A,
	AMD_FW_PSP_BIOS_TABLE = 0x49,
	AMD_FW_PSP_KEYDB_BL = 0x50,
	AMD_FW_PSP_KEYDB_TOS = 0x51,
	AMD_FW_PSP_VERSTAGE = 0x52,
	AMD_FW_PSP_VERSTAGE_SIG = 0x53,
	AMD_FW_PSP_RPMC_NVRAM = 0x54,
	AMD_FW_PSP_SPL = 0x55,
	AMD_FW_PSP_DMCU_ERAM = 0x58,
	AMD_FW_PSP_DMCU_ISR = 0x59,
	AMD_FW_PSP_MSMU = 0x5a,
	AMD_FW_PSP_SPIROM_CFG = 0x5c,
	AMD_FW_PSP_MPIO = 0x5d,
	AMD_FW_PSP_TPMLITE = 0x5f, /* family 17h & 19h */
	AMD_FW_PSP_SMUSCS = 0x5f, /* family 15h & 16h */
	AMD_FW_PSP_RAS_DRIVER = 0x64,
	AMD_FW_PSP_RAS_TA = 0x65,
	AMD_FW_PSP_FHP_DRIVER = 0x67,
	AMD_FW_PSP_SPDM_DRIVER = 0x68,
	AMD_FW_PSP_DPE_DRIVER = 0x69,
	AMD_FW_PSP_AB_NVRAM = 0x6e, /* PSP_AB_NVRAM on V2000A, FSDL driver on other SoCs */
	AMD_FW_PSP_DMCUB = 0x71,
	AMD_FW_PSP_BOOTLOADER_AB = 0x73,
	AMD_FW_PSP_RIB = 0x76,
	AMD_FW_PSP_AMF_SRAM = 0x85,
	AMD_FW_PSP_AMF_DRAM = 0x86,
	AMD_FW_PSP_MFD_MPM = 0x87,
	AMD_FW_PSP_AMF_WLAN = 0x88,
	AMD_FW_PSP_AMF_MFD = 0x89,
	AMD_FW_PSP_MPDMA_TF = 0x8c,
	AMD_FW_PSP_TA_IKEK = 0x8d,
	AMD_FW_PSP_SFDR = 0x8e,
	AMD_FW_PSP_MPCCX = 0x90,
	AMD_FW_PSP_GMI3_PHY = 0x91,
	AMD_FW_PSP_MPDMA_PM = 0x92,
	AMD_FW_PSP_LSDMA = 0x94,
	AMD_FW_PSP_C20_MP = 0x95,
	AMD_FW_PSP_FCFG_TABLE = 0x98,
	AMD_FW_PSP_MINIMSMU = 0x9a,
	AMD_FW_PSP_GFXIMU_0 = 0x9b,
	AMD_FW_PSP_GFXIMU_1 = 0x9c,
	AMD_FW_PSP_SRAM_FW_EXT = 0x9d,
	AMD_FW_PSP_TOS_WHITELIST = 0x9f,
	AMD_FW_PSP_UMSMU = 0xa2,
	AMD_FW_PSP_S3IMG = 0xa0,
	AMD_FW_PSP_USBDP = 0xa4,
	AMD_FW_PSP_USBSS = 0xa5,
	AMD_FW_PSP_USB4 = 0xa6,
	AMD_FW_PSP_IMC = 0x200,	/* Large enough to be larger than the top BHD entry type. */
	AMD_FW_PSP_GEC,
	AMD_FW_PSP_XHCI,
	AMD_FW_PSP_INVALID,		/* Real last one to detect the last entry in table. */
};

#endif  /* _AMD_PSP_PSP_DIRECTORY_H_ */
