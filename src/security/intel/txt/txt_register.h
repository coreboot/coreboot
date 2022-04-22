/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SECURITY_INTEL_TXT_REGISTER_H_
#define SECURITY_INTEL_TXT_REGISTER_H_

#include <types.h>

/*
 * Document: 315168-016
 * Intel Trusted Execution Technology (Intel TXT)
 * Software Development Guide
 * Chapter B
 */
#define TXT_BASE 0xfed30000UL

#define TXT_STS (TXT_BASE + 0)
#define TXT_ESTS (TXT_BASE + 8)
#define  TXT_ESTS_TXT_RESET_STS (1 << 0)
/*
 * Chapter 6
 * Intel Trusted Execution Technology Lab Handout
 */
#define  TXT_ESTS_WAKE_ERROR_STS (1 << 6)

#define TXT_ERROR (TXT_BASE + 0x30)
#define  ACMERROR_TXT_VALID	(1ul << 31)
#define  ACMERROR_TXT_EXTERNAL	(1ul << 30)

#define  ACMERROR_TXT_PROGRESS_SHIFT	16
#define  ACMERROR_TXT_MINOR_SHIFT	15
#define  ACMERROR_TXT_MAJOR_SHIFT	10
#define  ACMERROR_TXT_CLASS_SHIFT	4
#define  ACMERROR_TXT_TYPE_SHIFT	0

#define  ACMERROR_TXT_PROGRESS_CODE	(0xffull << ACMERROR_TXT_PROGRESS_SHIFT)
#define  ACMERROR_TXT_MINOR_CODE	(0x01ull << ACMERROR_TXT_MINOR_SHIFT)
#define  ACMERROR_TXT_MAJOR_CODE	(0x1full << ACMERROR_TXT_MAJOR_SHIFT)
#define  ACMERROR_TXT_CLASS_CODE	(0x3full << ACMERROR_TXT_CLASS_SHIFT)
#define  ACMERROR_TXT_TYPE_CODE		(0x0full << ACMERROR_TXT_TYPE_SHIFT)

#define  ACMERROR_TXT_AC_MODULE_TYPE_BIOS 0
#define  ACMERROR_TXT_AC_MODULE_TYPE_SINIT 1

#define  TXT_ERROR_MASK (0x3ff << 0)

#define TXT_CMD_RESET (TXT_BASE + 0x38)
#define TXT_CMD_CLOSE_PRIVATE (TXT_BASE + 0x48)

/* Present in Document Number: 315168-016. */
#define TXT_SPAD (TXT_BASE + 0xa0)
#define  ACMSTS_IBB_MEASURED		(1ull << 63)
#define  ACMSTS_VERIFICATION_ERROR	(1ull << 62)
#define  ACMSTS_BG_STARTUP_ERROR	(1ull << 61)	/* CBnT platforms only */
#define  ACMSTS_TXT_DISABLED		(1ull << 60)	/* disabled by FIT type 0xA record */
#define  ACMSTS_BIOS_TRUSTED		(1ull << 59)
#define  ACMSTS_MEM_CLEAR_POWER_DOWN	(1ull << 47)
#define  ACMSTS_TXT_STARTUP_SUCCESS	(1ull << 30)

#define TXT_VER_FSBIF (TXT_BASE + 0x100)
#define  TXT_VER_PRODUCTION_FUSED (1ull << 31)

#define TXT_DIDVID (TXT_BASE + 0x110)

/*
 * Chapter 6
 * Intel Trusted Execution Technology Lab Handout
 */
#define TXT_CAPABILITIES (TXT_BASE + 0x200)
#define  TXT_CAPABILITIES_DPR (1ull << 26)
#define  TXT_CAPABILITIES_PMRC (1ull << 19)

#define TXT_VER_QPIIF (TXT_BASE + 0x200)

#define TXT_SINIT_BASE (TXT_BASE + 0x270)
#define TXT_SINIT_SIZE (TXT_BASE + 0x278)
#define TXT_MLE_JOIN (TXT_BASE + 0x290)

#define TXT_HEAP_BASE (TXT_BASE + 0x300)
#define TXT_HEAP_SIZE (TXT_BASE + 0x308)
/*
 * Chapter 6
 * Intel Trusted Execution Technology Lab Handout
 */
#define TXT_MSEG_BASE (TXT_BASE + 0x310)
#define TXT_MSEG_SIZE (TXT_BASE + 0x318)

/*
 * Chapter 5.4.2.1
 * Intel Trusted Execution Technology Lab Handout
 */
#define TXT_BIOSACM_ERRORCODE (TXT_BASE + 0x328)

#define TXT_DPR (TXT_BASE + 0x330)

#define TXT_ACM_KEY_HASH (TXT_BASE + 0x400)
#define  TXT_ACM_KEY_HASH_LEN 0x4

#define TXT_STS_FTIF (TXT_BASE + 0x800)
#define   TXT_LPC_TPM_PRESENT	0x10000 /* Location of TPM: 001b - LPC TPM */
#define   TXT_SPI_TPM_PRESENT	0x50000 /* Location of TPM: 101b - SPI TPM */
#define   TXT_PTT_PRESENT	0x70000 /* Location of TPM: 111b - PTT present and active */

#define TXT_E2STS (TXT_BASE + 0x8f0)
#define  TXT_E2STS_SECRET_STS (1ull << 1)

/*
 * TCG PC Client Platform TPM Profile (PTP) Specification
 *
 * Note: Only locality 0 registers are publicly accessible.
 */

#define TPM_BASE	0xfed40000UL

#define TPM_ACCESS_REG	(TPM_BASE + 0x00)

/*
 * TXT Memory regions
 * Chapter 5.3
 * Intel Trusted Execution Technology Lab Handout
 */
#define TXT_PRIVATE_SPACE	0xfed20000UL
#define TXT_PUBLIC_SPACE	0xfed30000UL
#define TXT_TPM_DECODE_AREA	0xfed40000UL
#define TXT_RESERVED_SPACE	0xfed50000UL

#define TXT_RESERVED_SPACE_SIZE	0x3ffff

/* ESI flags for GETSEC[ENTERACCS] see  Reference Number: 323372-017 */
#define ACMINPUT_SCLEAN		0
#define ACMINPUT_RESET_TPM_AUXILIARY_INDICIES 2
#define ACMINPUT_NOP		3
#define ACMINPUT_SCHECK		4
#define ACMINPUT_CLEAR_SECRETS	5
#define ACMINPUT_LOCK_CONFIG	6

/*
 * GetSec EAX value.
 * SAFER MODE EXTENSIONS REFERENCE.
 * Intel 64 and IA-32 Architectures Software Developer Manuals Vol 2
 * Order Number:  325383-060US
 */
#define IA32_GETSEC_CAPABILITIES	0
#define IA32_GETSEC_ENTERACCS		2
#define IA32_GETSEC_SENTER		4
#define IA32_GETSEC_SEXIT		5
#define IA32_GETSEC_PARAMETERS		6
#define IA32_GETSEC_SMCTRL		7
#define IA32_GETSEC_WAKEUP		8

#define GETSEC_PARAMS_TXT_EXT_CRTM_SUPPORT (1ul << 5)
#define GETSEC_PARAMS_TXT_EXT_MACHINE_CHECK (1ul << 6)

/* ACM defines */
#define INTEL_ACM_VENDOR 0x00008086

#define ACM_FORMAT_FLAGS_PW 0x00000000
#define ACM_FORMAT_FLAGS_NPW (1 << 14)
#define ACM_FORMAT_FLAGS_DEBUG (1 << 15)

/* Old ACMs are power of two aligned, newer ACMs are not */
#define ACM_FORMAT_SIZE_64KB   (64 * KiB / 4)
#define ACM_FORMAT_SIZE_128KB (128 * KiB / 4)
#define ACM_FORMAT_SIZE_256KB (256 * KiB / 4)

/* MSRs */
#define IA32_MCG_STATUS 0x17a

/* DPR register layout, either in PCI config space or TXT MMIO space */
union dpr_register {
	struct {
		uint32_t lock :  1; /* [ 0.. 0] */
		uint32_t prs  :  1; /* [ 1.. 1] and only present on PCI config */
		uint32_t epm  :  1; /* [ 2.. 2] and only present on PCI config */
		uint32_t      :  1;
		uint32_t size :  8; /* [11.. 4] */
		uint32_t      :  8;
		uint32_t top  : 12; /* [31..20] */
	};
	uint32_t raw;
};

typedef enum {
	CHIPSET_ACM = 2,
} acm_module_type;

typedef enum {
	BIOS = 0,
	SINIT = 1,
} acm_module_sub_type;

/*
 * ACM Header v0.0 without dynamic part
 * Chapter A.1
 * Intel TXT Software Development Guide (Document: 315168-015)
 */
struct __packed acm_header_v0 {
	uint16_t module_type;
	uint16_t module_sub_type;
	uint32_t header_len;
	uint16_t header_version[2];
	uint16_t chipset_id;
	uint16_t flags;
	uint32_t module_vendor;
	uint32_t date;
	uint32_t size;
	uint16_t txt_svn;
	uint16_t se_svn;
	uint32_t code_control;
	uint32_t error_entry_point;
	uint32_t gdt_limit;
	uint32_t gdt_ptr;
	uint32_t seg_sel;
	uint32_t entry_point;
	uint8_t reserved2[64];
	uint32_t key_size;
	uint32_t scratch_size;
	uint8_t rsa2048_pubkey[256];
	uint32_t pub_exp;
	uint8_t rsa2048_sig[256];
	uint32_t scratch[143];
	uint8_t user_area[];
};

struct __packed acm_info_table {
	uint8_t uuid[16];
	uint8_t chipset_acm_type;
	uint8_t version;
	uint16_t length;
	uint32_t chipset_id_list;
	uint32_t os_sinit_data_ver;
	uint32_t min_mle_hdr_ver;
	uint32_t capabilities;
	uint8_t acm_ver;
	uint8_t reserved[3];
};

/*
 * Extended Data Elements
 * Chapter C.1
 * Intel TXT Software Development Guide (Document: 315168-015)
 */
struct __packed txt_extended_data_element_header {
	uint32_t type;
	uint32_t size;
	uint8_t data[0];
};

#define HEAP_EXTDATA_TYPE_END 0
#define HEAP_EXTDATA_TYPE_BIOS_SPEC_VER 1
#define HEAP_EXTDATA_TYPE_ACM 2
#define HEAP_EXTDATA_TYPE_CUSTOM 4

struct __packed txt_bios_spec_ver_element {
	struct txt_extended_data_element_header header;
	uint16_t ver_major;
	uint16_t ver_minor;
	uint16_t ver_revision;
};

/* Used when only the BIOS ACM is included in CBFS */
struct __packed txt_heap_acm_element1 {
	struct txt_extended_data_element_header header;
	uint32_t num_acms; // must greater 0, smaller than 3
	uint64_t acm_addrs[1];
};

/* Used when both BIOS and SINIT ACMs are included in CBFS */
struct __packed txt_heap_acm_element2 {
	struct txt_extended_data_element_header header;
	uint32_t num_acms; // must greater 0, smaller than 3
	uint64_t acm_addrs[2];
};

/*
 * BIOS Data Format
 * Chapter C.2
 * Intel TXT Software Development Guide (Document: 315168-015)
 */
struct __packed txt_biosdataregion {
	uint32_t version;
	uint32_t bios_sinit_size;
	uint64_t lcp_pd_base;
	uint64_t lcp_pd_size;
	uint32_t no_logical_procs;
	uint32_t sinit_flags;
	union {
		uint32_t mle_flags;
		struct {
			uint32_t support_acpi_ppi : 1;
			uint32_t platform_type : 2;
		};
	};
	u8 extended_data_elements[0];
};

void txt_dump_regions(void);
void txt_dump_chipset_info(void);
void txt_dump_acm_info(const struct acm_header_v0 *acm_header);
void txt_dump_getsec_parameters(void);

#endif /* SECURITY_INTEL_TXT_REGISTER_H_ */
