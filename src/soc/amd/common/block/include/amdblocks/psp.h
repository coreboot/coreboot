/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __AMD_PSP_H__
#define __AMD_PSP_H__

/* Get the mailbox base address - specific to family of device. */
struct psp_mbox *soc_get_mbox_address(void);

/* BIOS-to-PSP functions return 0 if successful, else negative value */
#define PSPSTS_SUCCESS      0
#define PSPSTS_NOBASE       1
#define PSPSTS_HALTED       2
#define PSPSTS_RECOVERY     3
#define PSPSTS_SEND_ERROR   4
#define PSPSTS_INIT_TIMEOUT 5
#define PSPSTS_CMD_TIMEOUT  6
/* other error codes */
#define PSPSTS_UNSUPPORTED  7
#define PSPSTS_INVALID_NAME 8
#define PSPSTS_INVALID_BLOB 9

int psp_notify_dram(void);

/*
 * type: identical to the corresponding PSP command, e.g. pass
 *       MBOX_BIOS_CMD_SMU_FW2 to load SMU FW2 blob.
 * name: cbfs file name
 */
enum psp_blob_type {
	BLOB_SMU_FW,
	BLOB_SMU_FW2,
};

int psp_load_named_blob(enum psp_blob_type type, const char *name);

#endif /* __AMD_PSP_H__ */
