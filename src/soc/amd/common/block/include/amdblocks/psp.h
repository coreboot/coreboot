/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __AMD_PSP_H__
#define __AMD_PSP_H__

#include <amdblocks/agesawrapper.h>
#include <soc/pci_devs.h>
#include <stdint.h>
#include <compiler.h>

/* x86 to PSP commands */
#define MBOX_BIOS_CMD_DRAM_INFO    0x01
#define MBOX_BIOS_CMD_SMM_INFO     0x02
#define MBOX_BIOS_CMD_SX_INFO      0x03
#define MBOX_BIOS_CMD_RSM_INFO     0x04
#define MBOX_BIOS_CMD_PSP_QUERY    0x05
#define MBOX_BIOS_CMD_BOOT_DONE    0x06
#define MBOX_BIOS_CMD_CLEAR_S3_STS 0x07
#define MBOX_BIOS_CMD_S3_DATA_INFO 0x08
#define MBOX_BIOS_CMD_NOP          0x09
#define MBOX_BIOS_CMD_SMU_FW       0x19
#define MBOX_BIOS_CMD_SMU_FW2      0x1a
#define MBOX_BIOS_CMD_ABORT        0xfe

/* generic PSP interface status */
#define STATUS_INITIALIZED         0x1
#define STATUS_ERROR               0x2
#define STATUS_TERMINATED          0x4
#define STATUS_HALT                0x8
#define STATUS_RECOVERY            0x10

/* psp_mbox consists of hardware registers beginning at PSPx000070
 *   mbox_command: BIOS->PSP command, cleared by PSP when complete
 *   mbox_status:  BIOS->PSP interface status
 *   cmd_response: pointer to command/response buffer
 */
struct psp_mbox {
	u32 mbox_command;
	u32 mbox_status;
	u64 cmd_response; /* definition conflicts w/BKDG but matches agesa */
} __packed;

/* command/response format, BIOS builds this in memory
 *   mbox_buffer_header: generic header
 *   mbox_buffer:        command-specific buffer format
 *
 * AMD reference code aligns and pads all buffers to 32 bytes.
 */
struct mbox_buffer_header {
	u32 size;	/* total size of buffer */
	u32 status;	/* command status, filled by PSP if applicable */
} __packed;

/*
 * command-specific buffer definitions:  see NDA document #54267
 * The following commands need a buffer definition if they are to be used.
 * All other commands will work with the default buffer.
 * MBOX_BIOS_CMD_SMM_INFO		MBOX_BIOS_CMD_PSP_QUERY
 * MBOX_BIOS_CMD_SX_INFO		MBOX_BIOS_CMD_S3_DATA_INFO
 * MBOX_BIOS_CMD_RSM_INFO
 */

struct mbox_default_buffer {	/* command-response buffer unused by command */
	struct mbox_buffer_header header;
} __attribute__((packed, aligned(32)));

/* send_psp_command() error codes */
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

#define PSP_INIT_TIMEOUT 10000 /* 10 seconds */
#define PSP_CMD_TIMEOUT 1000 /* 1 second */

/* BIOS-to-PSP functions return 0 if successful, else negative value */

int psp_notify_dram(void);

/*
 * type: identical to the corresponding PSP command, e.g. pass
 *       MBOX_BIOS_CMD_SMU_FW2 to load SMU FW2 blob.
 * name: cbfs file name
 */
int psp_load_named_blob(int type, const char *name);

#endif /* __AMD_PSP_H__ */
