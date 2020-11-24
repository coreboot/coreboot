/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PICASSO_PSP_TRANSFER_H
#define AMD_PICASSO_PSP_TRANSFER_H

# if (CONFIG_CMOS_RECOVERY_BYTE != 0)
#  define CMOS_RECOVERY_BYTE CONFIG_CMOS_RECOVERY_BYTE
# elif CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)
#  error "Must set CONFIG_CMOS_RECOVERY_BYTE"
# endif

#define CMOS_RECOVERY_MAGIC_VAL	0x96

#define TRANSFER_INFO_SIZE		64
#define TIMESTAMP_BUFFER_SIZE		0x200

#define TRANSFER_MAGIC_VAL		0x50544953

/* Area for things that would cause errors in a linker script */
#if !defined(__ASSEMBLER__)
#include <stdint.h>

struct transfer_info_struct {
	uint32_t	magic_val;		/* Identifier */
	uint32_t	struct_bytes;		/* Size of this structure */
	uint32_t	buffer_size;		/* Size of the transfer buffer area */

	/* Offsets from start of transfer buffer */
	uint32_t	workbuf_offset;
	uint32_t	console_offset;
	uint32_t	timestamp_offset;
	uint32_t	fmap_offset;

	uint32_t	unused1[5];

	/* Fields reserved for the PSP */
	uint64_t	timestamp;		/* Offset 0x30 */
	uint32_t	psp_unused;		/* Offset 0x38 */
	uint32_t	psp_info;		/* Offset 0x3C */
};

_Static_assert(sizeof(struct transfer_info_struct) == TRANSFER_INFO_SIZE, \
		"TRANSFER_INFO_SIZE is incorrect");

/* Make sure the PSP transferred information over to x86 side. */
void verify_psp_transfer_buf(void);
/* Display the transfer block's PSP_info data */
void show_psp_transfer_info(void);
/* Called by bootblock_c_entry in the VBOOT_STARTS_BEFORE_BOOTBLOCK case */
void boot_with_psp_timestamp(uint64_t base_timestamp);

#endif

#endif	/* AMD_PICASSO_PSP_TRANSFER_H */
