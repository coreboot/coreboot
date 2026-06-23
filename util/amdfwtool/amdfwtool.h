/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _AMD_FW_TOOL_H_
#define _AMD_FW_TOOL_H_

#include <commonlib/bsd/compiler.h>
#include <commonlib/bsd/helpers.h>
#include <openssl/sha.h>
#include <stdint.h>
#include <stdbool.h>

#include <vendorcode/amd/psp/psp_efs.h>
#include <vendorcode/amd/psp/psp_directory.h>
#include <vendorcode/amd/psp/bios_directory.h>
#include <vendorcode/amd/psp/combo_directory.h>
#include <vendorcode/amd/psp/ish.h>

/* An address can be relative to the image/file start but it can also be the address when
 * the image is mapped at 0xff000000. Used to ensure that we only attempt to read within
 * the limits of the file. */
#define SPI_ROM_BASE 0xff000000

#define ERASE_ALIGNMENT 0x1000U
#define TABLE_ALIGNMENT 0x1000U
#define TABLE_GRANULARITY 0x1000U
#define TABLE_L2_SIZE_MAX 0x400U
#define BLOB_ALIGNMENT 0x100U
#define TABLE_ERASE_ALIGNMENT _MAX(TABLE_ALIGNMENT, ERASE_ALIGNMENT)
#define BLOB_ERASE_ALIGNMENT _MAX(BLOB_ALIGNMENT, ERASE_ALIGNMENT)

enum platform {
	PLATFORM_UNKNOWN,
	PLATFORM_MULLINS,
	PLATFORM_CARRIZO,
	PLATFORM_STONEYRIDGE,
	PLATFORM_RAVEN,
	PLATFORM_PICASSO,
	PLATFORM_RENOIR,
	PLATFORM_CEZANNE,
	PLATFORM_MENDOCINO,
	PLATFORM_LUCIENNE,
	PLATFORM_PHOENIX,
	PLATFORM_STRIX,
	PLATFORM_GENOA,
	PLATFORM_KRACKAN2E,
	PLATFORM_STRIXHALO,
	PLATFORM_TURIN,
	PLATFORM_STRIXKRACKAN,
};

#define MAX_PSP_ENTRIES 0xff
#define MAX_BIOS_ENTRIES 0x2f

#define BDT_LVL1 (1 << 0)
#define BDT_LVL2 (1 << 1)
#define BDT_LVL1_AB (1 << 2)
#define BDT_LVL2_AB (1 << 3)
#define BDT_BOTH (BDT_LVL1 | BDT_LVL2)
#define BDT_BOTH_AB (BDT_LVL1_AB | BDT_LVL2_AB)
struct amd_bios_entry {
	enum amd_bios_type type;
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
};

#define PSP_LVL1 (1 << 0)
#define PSP_LVL2 (1 << 1)
#define PSP_LVL1_AB (1 << 2)
#define PSP_LVL2_AB (1 << 3)
#define PSP_BOTH (PSP_LVL1 | PSP_LVL2)
#define PSP_BOTH_AB (PSP_LVL1_AB | PSP_LVL2_AB)

typedef enum _fwid_type {
	FWID_TYPE_FWID = 0,
	FWID_TYPE_UUID,
} fwid_type_t;

#define UUID_LEN_BYTES 16
typedef struct _amd_fw_entry_hash {
	fwid_type_t fwid_type;
	union {
		uint16_t fw_id;
		uint8_t uuid[UUID_LEN_BYTES];
	};
	uint16_t subtype;
	uint32_t sha_len;
	uint8_t sha[SHA384_DIGEST_LENGTH];
} amd_fw_entry_hash;

typedef struct _amd_fw_entry {
	enum amd_fw_type type;
	char *filename;
	uint8_t subprog;
	uint8_t inst;
	uint64_t dest;
	size_t size;
	int level;
	uint64_t other;
	/* If the binary is signed and the tool is invoked to keep the signed binaries separate,
	   then this field is populated with the offset of the concerned PSP binary (relative to
	   BIOS or PSP Directory table). */
	uint64_t addr_signed;
	uint32_t file_size;
	/* Some files that don't have amd_fw_header have to be skipped from hashing. These files
	   include but not limited to: *iKek*, *.tkn, *.stkn */
	bool skip_hashing;
	uint8_t hash_tbl_id;
	fwid_type_t fwid_type;
	uint32_t num_hash_entries;
	amd_fw_entry_hash *hash_entries;
	bool generate_manifest;
} amd_fw_entry;

/* Most PSP binaries, if not all, have the following header format. */
struct amd_fw_header {
	uint8_t reserved_0[20];
	uint32_t fw_size_signed;
	uint8_t reserved_18[24];
	/* 1 if the image is signed, 0 otherwise */
	uint32_t sig_opt;
	uint32_t sig_id;
	uint8_t sig_param[16];
	uint32_t comp_opt;
	uint8_t reserved_4c[4];
	uint32_t uncomp_size;
	uint32_t comp_size;
	/* Starting MDN fw_id is populated instead of fw_type. */
	uint16_t fw_id;
	uint8_t reserved_5a[6];
	uint8_t version[4];
	uint8_t reserved_64[8];
	uint32_t size_total;
	uint8_t reserved_70[12];
	/* Starting MDN fw_id is populated instead of fw_type. fw_type will still be around
	   for backwards compatibility. */
	uint8_t fw_type;
	uint8_t fw_subtype;
	uint8_t fw_subprog;
	uint8_t reserved_7f;
	uint8_t reserved_80[128];
} __packed;

/* Based on the available PSP resources and increasing number of signed PSP binaries,
   AMD recommends to split the hash table into 3 parts for now. */
#define MAX_NUM_HASH_TABLES 3
struct psp_fw_hash_table {
	uint16_t version;
	uint16_t no_of_entries_256;
	uint16_t no_of_entries_384;
	/* The next 2 elements are pointers to arrays of SHA256 and SHA384 entries. */
	/* It does not make sense to store pointers in the CBFS file */
} __packed;

typedef struct _amd_cb_config {
	bool have_whitelist;
	bool unlock_secure;
	bool use_secureos;
	bool load_mp2_fw;
	bool s0i3;
	bool have_mb_spl;
	bool recovery_ab;
	bool recovery_ab_single_copy;
	bool have_apcb_bk;
	enum platform soc_id;

	uint8_t efs_spi_readmode, efs_spi_speed, efs_spi_micron_flag;
	uint32_t body_location, efs_location, ral2_location, rbl2_location;
	uint8_t efs_espi0_config0;
	uint8_t efs_espi0_config1;
	uint8_t efs_espi1_config0;
	uint8_t efs_espi1_config1;
	uint64_t signed_start_addr;
	uint32_t rom_size;
	char *manifest_file;
	const char *signed_output_file;
	char *output, *config;
	int debug;
} amd_cb_config;

#define MAX_ISH_TABLES 4
typedef struct _context {
	char *rom;		/* target buffer, size of flash device */
	uint32_t rom_size;	/* size of flash device */
	uint32_t address_mode;	/* 0:abs address; 1:relative to flash; 2: relative to table 3: relative to partition */
	uint32_t current;	/* pointer within flash & proxy buffer */
	uint32_t current_pointer_saved;
	uint32_t current_table;
	uint32_t current_a_pointer, current_b_pointer, current_l1_pointer;
	void *amd_psp_fw_table_clean;
	void *amd_bios_table_clean;
	struct embedded_firmware *amd_romsig_ptr;
	struct psp_directory_table *pspdir, *pspdir_bak, *pspdir2, *pspdir2_b;
	struct bios_directory_table *biosdir, *biosdir2, *biosdir2_b;
	size_t num_ish_tables;
	struct ish_directory_table *ish_a_dirs[MAX_ISH_TABLES];
	struct ish_directory_table *ish_b_dirs[MAX_ISH_TABLES];
} context;

uint8_t process_config(FILE *config, amd_cb_config *cb_config);
void process_signed_psp_firmwares(const char *signed_rom,
		amd_fw_entry *fw_table,
		uint64_t signed_start_addr,
		enum platform soc_id);
int find_bios_entry(enum amd_bios_type type);

#define EFS_FILE_SUFFIX ".efs"
#define TMP_FILE_SUFFIX ".tmp"
#define BODY_FILE_SUFFIX ".body"
#define RA_FILE_SUFFIX ".ra"
#define RB_FILE_SUFFIX ".rb"

void write_or_fail(int fd, void *ptr, size_t size);
ssize_t read_from_file_to_buf(int fd, void *buf, size_t buf_size);
ssize_t write_from_buf_to_file(int fd, const void *buf, size_t buf_size);
ssize_t write_blob(char *output, void *body_offset, ssize_t body_size, char *suffix);
ssize_t copy_blob(context *ctx, const char *src_file);
#define OK 0

#define LINE_EOF (1)
#define LINE_TOO_LONG (2)

int amdfwtool_getopt(int argc, char *argv[], amd_cb_config *cb_config);


enum platform platform_identify(char *soc_name);
bool platform_needs_ish(enum platform platform_type);
bool platform_is_multi_level(enum platform platform_type);
bool platform_is_second_gen(enum platform platform_type);
bool platform_has_dir_header_v1(enum platform platform_type);
bool platform_has_apob_nv_quirk(enum platform platform_type);
uint32_t platform_get_num_psp_ids(enum platform platform_type);
uint32_t platform_get_psp_id(enum platform platform_type, const unsigned int index);
bool platform_is_initial_alignment_required(enum platform platform_type);
bool platform_has_legacy_ab_recovery(amd_cb_config *cb_config);
uint32_t platform_psb_reserved_size(enum platform platform_type);

#endif	/* _AMD_FW_TOOL_H_ */
