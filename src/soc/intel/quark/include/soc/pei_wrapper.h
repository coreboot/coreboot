/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _PEI_WRAPPER_H_
#define _PEI_WRAPPER_H_

#include <types.h>

#define PEI_VERSION 22

#define ABI_X86 __attribute__((regparm(0)))

typedef void ABI_X86(*tx_byte_func)(unsigned char byte);

struct pei_data {
	uint32_t pei_version;

	int boot_mode;

	/* Data read from flash and passed into MRC */
	const void *saved_data;
	int saved_data_size;

	/* Disable use of saved data (can be set by mainboard) */
	int disable_saved_data;

	/* Data from MRC that should be saved to flash */
	void *data_to_save;
	int data_to_save_size;
} __packed;

#endif /* _PEI_WRAPPER_H_ */
