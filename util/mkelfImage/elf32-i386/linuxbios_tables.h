#ifndef LINUXBIOS_TABLES_H
#define LINUXBIOS_TABLES_H

#include <stdint.h>

/* The linuxbios table information is for conveying information
 * from the firmware to the loaded OS image.  Primarily this
 * is expected to be information that cannot be discovered by
 * other means, such as quering the hardware directly.
 *
 * All of the information should be Position Independent Data.  
 * That is it should be safe to relocated any of the information
 * without it's meaning/correctnes changing.   For table that
 * can reasonably be used on multiple architectures the data
 * size should be fixed.  This should ease the transition between
 * 32 bit and 64 bit architectures etc.
 *
 * The completeness test for the information in this table is:
 * - Can all of the hardware be detected?
 * - Are the per motherboard constants available?
 * - Is there enough to allow a kernel to run that was written before
 *   a particular motherboard is constructed? (Assuming the kernel
 *   has drivers for all of the hardware but it does not have
 *   assumptions on how the hardware is connected together).
 *
 * With this test it should be straight forward to determine if a
 * table entry is required or not.  This should remove much of the
 * long term compatibility burden as table entries which are
 * irrelevant or have been replaced by better alternatives may be
 * dropped.  Of course it is polite and expidite to include extra
 * table entries and be backwards compatible, but it is not required.
 */


struct lb_header
{
	uint8_t  signature[4]; /* LBIO */
	uint32_t header_bytes;
	uint32_t header_checksum;
	uint32_t table_bytes;
	uint32_t table_checksum;
	uint32_t table_entries;
};

/* Every entry in the boot enviroment list will correspond to a boot
 * info record.  Encoding both type and size.  The type is obviously
 * so you can tell what it is.  The size allows you to skip that
 * boot enviroment record if you don't know what it easy.  This allows
 * forward compatibility with records not yet defined.
 */
struct lb_record {
	uint32_t tag;		/* tag ID */
	uint32_t size;		/* size of record (in bytes) */
};

#define LB_TAG_UNUSED	0x0000

#define LB_TAG_MEMORY	0x0001

struct lb_memory_range {
	uint64_t start;
	uint64_t size;
	uint32_t type;
#define LB_MEM_RAM      1
#define LB_MEM_RESERVED 2
	
};

struct lb_memory {
	uint32_t tag;
	uint32_t size;
	struct lb_memory_range map[0];
};

#define LB_TAG_HWRPB	0x0002
struct lb_hwrpb {
	uint32_t tag;
	uint32_t size;
	uint64_t hwrpb;
};

#endif /* LINUXBIOS_TABLES_H */
