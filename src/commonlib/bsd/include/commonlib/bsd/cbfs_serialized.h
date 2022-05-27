/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only */

#ifndef _CBFS_SERIALIZED_H_
#define _CBFS_SERIALIZED_H_

#include <stdint.h>
#include <vb2_sha.h>

enum cbfs_compression {
	CBFS_COMPRESS_NONE	= 0,
	CBFS_COMPRESS_LZMA	= 1,
	CBFS_COMPRESS_LZ4	= 2,
};

enum cbfs_type {
	/* QUERY is an alias for DELETED that can be passed to CBFS APIs to
	   inquire about the type of a file, rather than constrain it. */
	CBFS_TYPE_QUERY		= 0,
	CBFS_TYPE_DELETED	= 0x00000000,
	CBFS_TYPE_NULL		= 0xffffffff,
	CBFS_TYPE_BOOTBLOCK	= 0x01,
	CBFS_TYPE_CBFSHEADER	= 0x02,
	CBFS_TYPE_LEGACY_STAGE	= 0x10,
	CBFS_TYPE_STAGE		= 0x11,
	CBFS_TYPE_SELF		= 0x20,
	CBFS_TYPE_FIT_PAYLOAD	= 0x21,
	CBFS_TYPE_OPTIONROM	= 0x30,
	CBFS_TYPE_BOOTSPLASH	= 0x40,
	CBFS_TYPE_RAW		= 0x50,
	CBFS_TYPE_VSA		= 0x51,
	CBFS_TYPE_MBI		= 0x52,
	CBFS_TYPE_MICROCODE	= 0x53,
	CBFS_TYPE_FSP		= 0x60,
	CBFS_TYPE_MRC		= 0x61,
	CBFS_TYPE_MMA		= 0x62,
	CBFS_TYPE_EFI		= 0x63,
	CBFS_TYPE_STRUCT	= 0x70,
	CBFS_TYPE_CMOS_DEFAULT	= 0xaa,
	CBFS_TYPE_SPD		= 0xab,
	CBFS_TYPE_MRC_CACHE	= 0xac,
	CBFS_TYPE_CMOS_LAYOUT	= 0x01aa,
};

#define CBFS_HEADER_MAGIC  0x4F524243		/* BE: 'ORBC' */
#define CBFS_HEADER_VERSION1 0x31313131		/* BE: '1111' */
#define CBFS_HEADER_VERSION2 0x31313132		/* BE: '1112' */
#define CBFS_HEADER_VERSION  CBFS_HEADER_VERSION2

/* this is the master cbfs header - it must be located somewhere available
 * to bootblock (to load romstage). The last 4 bytes in the image contain its
 * relative offset from the end of the image (as a 32-bit signed integer). */

struct cbfs_header {
	uint32_t magic;
	uint32_t version;
	uint32_t romsize;
	uint32_t bootblocksize;
	uint32_t align; /* fixed to 64 bytes */
	uint32_t offset;
	uint32_t architecture;
	uint32_t pad[1];
} __packed;

/* this used to be flexible, but wasn't ever set to something different. */
#define CBFS_ALIGNMENT 64

/* "Unknown" refers to CBFS headers version 1,
 * before the architecture was defined (i.e., x86 only).
 */
enum cbfs_architecture {
	CBFS_ARCHITECTURE_UNKNOWN  = 0xFFFFFFFF,
	CBFS_ARCHITECTURE_X86      = 0x00000001,
	CBFS_ARCHITECTURE_ARM      = 0x00000010,
	CBFS_ARCHITECTURE_AARCH64  = 0x0000aa64,
	CBFS_ARCHITECTURE_MIPS     = 0x00000100,	/* deprecated */
	CBFS_ARCHITECTURE_RISCV    = 0xc001d0de,
	CBFS_ARCHITECTURE_PPC64    = 0x407570ff,
};

/** This is a component header - every entry in the CBFS
    will have this header.

    This is how the component is arranged in the ROM:

    --------------   <- 0
    component header
    --------------   <- sizeof(struct component)
    component name
    --------------   <- offset
    data
    ...
    --------------   <- offset + len
*/

#define CBFS_FILE_MAGIC "LARCHIVE"
#define CBFS_METADATA_MAX_SIZE 256

struct cbfs_file {
	char magic[8];
	uint32_t len;
	uint32_t type;
	uint32_t attributes_offset;
	uint32_t offset;
	char filename[0];
} __packed;

#if defined __GNUC__ && (__GNUC__ * 100 + __GNUC_MINOR__) >= 406
_Static_assert(sizeof(struct cbfs_file) == 24, "cbfs_file size mismatch");
#endif

/* The common fields of extended cbfs file attributes.
   Attributes are expected to start with tag/len, then append their
   specific fields. */
struct cbfs_file_attribute {
	uint32_t tag;
	/* len covers the whole structure, incl. tag and len */
	uint32_t len;
	uint8_t data[0];
} __packed;

/* All attribute sizes must be divisible by this! */
#define CBFS_ATTRIBUTE_ALIGN 4

/* Depending on how the header was initialized, it may be backed with 0x00 or
 * 0xff. Support both. */
enum cbfs_file_attr_tag {
	CBFS_FILE_ATTR_TAG_UNUSED	= 0,
	CBFS_FILE_ATTR_TAG_UNUSED2	= 0xffffffff,
	CBFS_FILE_ATTR_TAG_COMPRESSION	= 0x42435a4c, /* BE: 'BCZL' */
	CBFS_FILE_ATTR_TAG_HASH		= 0x68736148, /* BE: 'hsaH' */
	CBFS_FILE_ATTR_TAG_POSITION	= 0x42435350, /* BE: 'BCSP' */
	CBFS_FILE_ATTR_TAG_ALIGNMENT	= 0x42434c41, /* BE: 'BCLA' */
	CBFS_FILE_ATTR_TAG_IBB		= 0x32494242, /* BE: '2IBB' */
	CBFS_FILE_ATTR_TAG_PADDING	= 0x47444150, /* BE: 'GNDP' */
	CBFS_FILE_ATTR_TAG_STAGEHEADER	= 0x53746748, /* BE: 'StgH' */
};

struct cbfs_file_attr_compression {
	uint32_t tag;
	uint32_t len;
	/* whole file compression format. 0 if no compression. */
	uint32_t compression;
	uint32_t decompressed_size;
} __packed;

/* Actual size in CBFS may be larger/smaller than struct size! */
struct cbfs_file_attr_hash {
	uint32_t tag;
	uint32_t len;
	struct vb2_hash hash;
} __packed;

struct cbfs_file_attr_position {
	uint32_t tag;
	uint32_t len;
	uint32_t position;
} __packed;

struct cbfs_file_attr_align {
	uint32_t tag;
	uint32_t len;
	uint32_t alignment;
} __packed;

struct cbfs_file_attr_stageheader {
	uint32_t tag;
	uint32_t len;
	uint64_t loadaddr;	/* Memory address to load the code to. */
	uint32_t entry_offset;	/* Offset of entry point from loadaddr. */
	uint32_t memlen;	/* Total length (including BSS) in memory. */
} __packed;


/*** Component sub-headers ***/

/* Following are component sub-headers for the "standard"
   component types */

/** this is the sub-header for payload components.  Payloads
    are loaded by coreboot at the end of the boot process */

struct cbfs_payload_segment {
	uint32_t type;
	uint32_t compression;
	uint32_t offset;
	uint64_t load_addr;
	uint32_t len;
	uint32_t mem_len;
} __packed;

struct cbfs_payload {
	struct cbfs_payload_segment segments;
};

enum cbfs_payload_segment_type {
	PAYLOAD_SEGMENT_CODE   = 0x434F4445,	/* BE: 'CODE' */
	PAYLOAD_SEGMENT_DATA   = 0x44415441,	/* BE: 'DATA' */
	PAYLOAD_SEGMENT_BSS    = 0x42535320,	/* BE: 'BSS ' */
	PAYLOAD_SEGMENT_PARAMS = 0x50415241,	/* BE: 'PARA' */
	PAYLOAD_SEGMENT_ENTRY  = 0x454E5452,	/* BE: 'ENTR' */
};

struct cbfs_optionrom {
	uint32_t compression;
	uint32_t len;
} __packed;

#endif /* _CBFS_SERIALIZED_H_ */
