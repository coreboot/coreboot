/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _FIT_H_
#define _FIT_H_

#include <vendorcode/intel/edk2/uefi_2.4/uefi_types.h>

#define FIT_TABLE_SIGNATURE		SIGNATURE_64('_', 'F', 'I', 'T', '_', ' ', ' ', ' ')

#define	FIT_TABLE_TYPE_HEADER		0x0
#define	FIT_TABLE_TYPE_TXE_SECURE_BOOT	0x10
#define	FIT_ENTRY_SUB_TYPE_TXE_HASH	0x2
#define	FIT_ENTRY_SUB_TYPE_BOOT_POLICY	0x3
#define	FIT_ENTRY_SUB_TYPE_IBBL_HASH	0x7
#define	FIT_ENTRY_SUB_TYPE_IBB_HASH	0x8

struct firmware_interface_table_entry {
	uint64_t	address;
	uint8_t		size[3];
	uint8_t		sub_type;
	uint16_t	version;
	uint8_t		type:		7;
	uint8_t		cv:		1;
	uint8_t		chk_sum;
};

#endif /* _FIT_H_ */
