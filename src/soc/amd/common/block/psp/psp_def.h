/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_PSP_DEF_H__
#define __AMD_PSP_DEF_H__

#include <types.h>
#include <commonlib/helpers.h>
#include <amdblocks/psp.h>

/* x86 to PSP commands */
#define MBOX_BIOS_CMD_DRAM_INFO			0x01
#define MBOX_BIOS_CMD_SMM_INFO			0x02
#define MBOX_BIOS_CMD_SX_INFO			0x03
#define   MBOX_BIOS_CMD_SX_INFO_SLEEP_TYPE_MAX	0x07
#define MBOX_BIOS_CMD_RSM_INFO			0x04
#define MBOX_BIOS_CMD_PSP_QUERY			0x05
#define MBOX_BIOS_CMD_BOOT_DONE			0x06
#define MBOX_BIOS_CMD_CLEAR_S3_STS		0x07
#define MBOX_BIOS_CMD_S3_DATA_INFO		0x08
#define MBOX_BIOS_CMD_NOP			0x09
#define MBOX_BIOS_CMD_ABORT			0xfe
/* x86 to PSP commands, v1 */
#define MBOX_BIOS_CMD_SMU_FW			0x19
#define MBOX_BIOS_CMD_SMU_FW2			0x1a

/* generic PSP interface status, v1 */
#define PSPV1_STATUS_INITIALIZED	BIT(0)
#define PSPV1_STATUS_ERROR		BIT(1)
#define PSPV1_STATUS_TERMINATED		BIT(2)
#define PSPV1_STATUS_HALT		BIT(3)
#define PSPV1_STATUS_RECOVERY		BIT(4)

/* generic PSP interface status, v2 */
#define PSPV2_STATUS_ERROR		BIT(30)
#define PSPV2_STATUS_RECOVERY		BIT(31)

/* psp_mbox consists of hardware registers beginning at PSPx000070
 *   mbox_command: BIOS->PSP command, cleared by PSP when complete
 *   mbox_status:  BIOS->PSP interface status
 *   cmd_response: pointer to command/response buffer
 */
struct pspv1_mbox {
	u32 mbox_command;
	u32 mbox_status;
	u64 cmd_response; /* definition conflicts w/BKDG but matches agesa */
} __packed;

struct pspv2_mbox {
	union {
		u32 val;
		struct pspv2_mbox_cmd_fields {
			u16 mbox_status;
			u8 mbox_command;
			u32 reserved:6;
			u32 recovery:1;
			u32 ready:1;
		} __packed fields;
	};
	u64 cmd_response;
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

struct smm_req_buffer {
	uint64_t smm_base;		/* TSEG base */
	uint64_t smm_mask;		/* TSEG mask */
	uint64_t psp_smm_data_region;	/* PSP region in SMM space */
	uint64_t psp_smm_data_length;	/* PSP region length in SMM space */
	struct smm_trigger_info smm_trig_info;
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_GEN2)
	struct smm_register_info smm_reg_info;
#endif
	uint64_t psp_mbox_smm_buffer_address;
	uint64_t psp_mbox_smm_flag_address;
} __packed;

struct mbox_cmd_smm_info_buffer {
	struct mbox_buffer_header header;
	struct smm_req_buffer req;
} __attribute__((packed, aligned(32)));

struct mbox_cmd_sx_info_buffer {
	struct mbox_buffer_header header;
	u8 sleep_type;
} __attribute__((packed, aligned(32)));

#define PSP_INIT_TIMEOUT 10000 /* 10 seconds */
#define PSP_CMD_TIMEOUT 1000 /* 1 second */

void psp_print_cmd_status(int cmd_status, struct mbox_buffer_header *header);

/* This command needs to be implemented by the generation specific code. */
int send_psp_command(u32 command, void *buffer);

#endif /* __AMD_PSP_DEF_H__ */
