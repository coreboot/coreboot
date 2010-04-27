#ifndef _LINUX_UNIFORM_BOOT_H
#define _LINUX_UNIFORM_BOOT_H

/* The uniform boot environment information is restricted to
 * hardware information.  In particular for a simple enough machine
 * all of the environment information should be able to reside in
 * a rom and not need to be moved.  This information is the
 * information a trivial boot room can pass to linux to let it
 * run the hardware.
 *
 * Also all of the information should be Position Independent Data.
 * That is it should be safe to relocated any of the information
 * without it's meaning/correctnes changing. The exception is the
 * uniform_boot_header with it's two pointers arg & env.
 *
 * The addresses in the arg & env pointers must be physical
 * addresses. A physical address is an address you put in the page
 * table.
 *
 * The Command line is for user policy.  Things like the default
 * root device.
 *
 */

struct uniform_boot_header
{
	unsigned long header_bytes;
	unsigned long header_checksum;
	unsigned long arg;
	unsigned long arg_bytes;
	unsigned long env;
	unsigned long env_bytes;
};

/* Every entry in the boot enviroment list will correspond to a boot
 * info record.  Encoding both type and size.  The type is obviously
 * so you can tell what it is.  The size allows you to skip that
 * boot enviroment record if you don't know what it easy.  This allows
 * forward compatibility with records not yet defined.
 */
struct ube_record {
	unsigned long tag;		/* tag ID */
	unsigned long size;		/* size of record (in bytes) */
	unsigned long data[0];		/* data */
};


#define UBE_TAG_MEMORY	0x0001

struct ube_memory_range {
	unsigned long long start;
	unsigned long long size;
	unsigned long type;
#define UBE_MEM_RAM      1
#define UBE_MEM_RESERVED 2
#define UBE_MEM_ACPI     3
#define UBE_MEM_NVS      4

};

struct ube_memory {
	unsigned long tag;
	unsigned long size;
	struct ube_memory_range map[0];
};

#endif /* _LINUX_UNIFORM_BOOT_H */
