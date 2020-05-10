/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __EFI_WRAPPER_H__
#define __EFI_WRAPPER_H__

#define EFI_WRAPPER_VER 2

/* Provide generic x86 calling conventions. */
#define ABI_X86 __attribute((regparm(0)))

/* Errors returned by the EFI wrapper. */
enum efi_wrapper_error {
	INVALID_VER = -1,
};

struct efi_wrapper_params {
	/* Mainboard Inputs */
	int version;

	void ABI_X86 (*console_out)(unsigned char byte);

	unsigned int tsc_ticks_per_microsecond;
} __packed;

typedef int ABI_X86 (*efi_wrapper_entry_t)(struct efi_wrapper_params *);
#endif
