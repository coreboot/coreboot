/* SPDX-License-Identifier: GPL-2.0-only or BSD-3-Clause */

#ifndef _CBFS_H_
#define _CBFS_H_

#include "coreboot_tables.h"

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

/** These are standard values for the known compression
    algorithms that coreboot knows about for stages and
    payloads.  Of course, other CBFS users can use whatever
    values they want, as long as they understand them. */

#define CBFS_COMPRESS_NONE  0
#define CBFS_COMPRESS_LZMA  1
#define CBFS_COMPRESS_LZ4   2

/** These are standard component types for well known
    components (i.e - those that coreboot needs to consume.
    Users are welcome to use any other value for their
    components */

#define CBFS_TYPE_STAGE		0x10
#define CBFS_TYPE_SELF		0x20
#define CBFS_TYPE_FIT_PAYLOAD	0x21
#define CBFS_TYPE_OPTIONROM	0x30
#define CBFS_TYPE_BOOTSPLASH	0x40
#define CBFS_TYPE_RAW		0x50
#define CBFS_TYPE_VSA		0x51
#define CBFS_TYPE_MBI		0x52
#define CBFS_TYPE_MICROCODE	0x53
#define CBFS_COMPONENT_CMOS_DEFAULT 0xaa
#define CBFS_COMPONENT_CMOS_LAYOUT 0x01aa


/** this is the master cbfs header - it need to be
    located somewhere in the bootblock.  Where it
    actually lives is up to coreboot. A pointer to
    this header will live at 0xFFFFFFFc, so we can
    easily find it. */

#define CBFS_HEADER_MAGIC  0x4F524243
#define CBFS_HEADPTR_ADDR 0xFFFFFFFc
#define CBFS_HEADER_VERSION1 0x31313131

struct cbfs_header {
	u32 magic;
        u32 version;
        u32 romsize;
        u32 bootblocksize;
	u32 align;
	u32 offset;
	u32 pad[2];
} __attribute__((packed));

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

struct cbfs_file {
	char magic[8];
	u32 len;
	u32 type;
	u32 checksum;
	u32 offset;
} __attribute__((packed));

/*** Component sub-headers ***/

/* Following are component sub-headers for the "standard"
   component types */

/** This is the sub-header for stage components.  Stages are
    loaded by coreboot during the normal boot process */

struct cbfs_stage {
	u32 compression;  /** Compression type */
	u64 entry;  /** entry point */
	u64 load;   /** Where to load in memory */
	u32 len;          /** length of data to load */
	u32 memlen;	   /** total length of object in memory */
} __attribute__((packed));

/** this is the sub-header for payload components.  Payloads
    are loaded by coreboot at the end of the boot process */

struct cbfs_payload_segment {
	u32 type;
	u32 compression;
	u32 offset;
	u64 load_addr;
	u32 len;
	u32 mem_len;
} __attribute__((packed));

struct cbfs_payload {
	struct cbfs_payload_segment segments;
};

#define PAYLOAD_SEGMENT_CODE   0x45444F43
#define PAYLOAD_SEGMENT_DATA   0x41544144
#define PAYLOAD_SEGMENT_BSS    0x20535342
#define PAYLOAD_SEGMENT_PARAMS 0x41524150
#define PAYLOAD_SEGMENT_ENTRY  0x52544E45

struct cbfs_optionrom {
	u32 compression;
	u32 len;
} __attribute__((packed));

#define CBFS_NAME(_c) (((char *) (_c)) + sizeof(struct cbfs_file))
#define CBFS_SUBHEADER(_p) ( (void *) ((((u8 *) (_p)) + ntohl((_p)->offset))) )

void * cbfs_get_file(const char *name);
struct cbfs_file *cbfs_find(const char *name);
void *cbfs_find_file(const char *name, unsigned int type, unsigned int *len);

void open_cbfs(const char *filename);
#endif
