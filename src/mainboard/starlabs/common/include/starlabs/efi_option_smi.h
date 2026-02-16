/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef STARLABS_EFI_OPTION_SMI_H
#define STARLABS_EFI_OPTION_SMI_H

#include <types.h>

/*
 * ACPI <-> SMM protocol for reading/writing a restricted set of coreboot
 * options in the UEFI variable store.
 *
 * ACPI fills DNVS, then triggers an APMC SMI by writing STARLABS_APMC_CMD
 * to the APM_CNT port (0xB2). SMM reads DNVS, performs the operation, and
 * updates DNVS with status and (for reads) the returned value.
 */

#define STARLABS_APMC_CMD_EFI_OPTION 0xE2

enum starlabs_efiopt_cmd {
	STARLABS_EFIOPT_CMD_GET = 1,
	STARLABS_EFIOPT_CMD_SET = 2,
};

enum starlabs_efiopt_id {
	STARLABS_EFIOPT_ID_FN_LOCK_STATE = 1,
	STARLABS_EFIOPT_ID_TRACKPAD_STATE = 2,
	STARLABS_EFIOPT_ID_KBL_BRIGHTNESS = 3,
	STARLABS_EFIOPT_ID_KBL_STATE = 4,
};

struct starlabs_dnvs_efiopt {
	uint32_t cmd;
	uint32_t id;
	uint32_t value;
	uint32_t status;
} __packed;

#endif /* STARLABS_EFI_OPTION_SMI_H */
