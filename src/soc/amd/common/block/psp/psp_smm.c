/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <region_file.h>
#include <console/console.h>
#include <amdblocks/psp.h>
#include <soc/iomap.h>
#include <string.h>

#include "psp_def.h"

#define C2P_BUFFER_MAXSIZE 0xc00 /* Core-to-PSP buffer */
#define P2C_BUFFER_MAXSIZE 0xc00 /* PSP-to-core buffer */

/*
 * When sending PSP mailbox commands to the PSP from the SMI handler after the boot done
 * command was sent, the corresponding data buffer needs to be placed in this core to PSP (C2P)
 * buffer.
 */
struct {
	u8 buffer[C2P_BUFFER_MAXSIZE];
} __aligned(32) c2p_buffer;

/*
 * When the PSP sends mailbox commands to the host, it will update the PSP to core (P2C) buffer
 * and then send an SMI to the host to process the request.
 */
struct {
	u8 buffer[P2C_BUFFER_MAXSIZE];
} __aligned(32) p2c_buffer;

/*
 * When sending PSP mailbox commands to the PSP from the SMI handler, the SMM flag needs to be
 * set for the PSP to accept it. Otherwise it should be cleared.
 */
static uint32_t smm_flag;

static void set_smm_flag(void)
{
	smm_flag = 1;
}

static void clear_smm_flag(void)
{
	smm_flag = 0;
}

int psp_notify_smm(void)
{
	msr_t msr;
	int cmd_status;
	struct mbox_cmd_smm_info_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
		.req = {
			.psp_smm_data_region = (uintptr_t)p2c_buffer.buffer,
			.psp_smm_data_length = sizeof(p2c_buffer),
			.psp_mbox_smm_buffer_address = (uintptr_t)c2p_buffer.buffer,
			.psp_mbox_smm_flag_address = (uintptr_t)&smm_flag,
		}
	};

	msr = rdmsr(SMM_ADDR_MSR);
	buffer.req.smm_base = msr.raw;
	msr = rdmsr(SMM_MASK_MSR);
	msr.lo &= 0xffff0000; /* mask SMM_TSEG_VALID and reserved bits */
	buffer.req.smm_mask = msr.raw;

	soc_fill_smm_trig_info(&buffer.req.smm_trig_info);
#if (CONFIG(SOC_AMD_COMMON_BLOCK_PSP_GEN2))
	soc_fill_smm_reg_info(&buffer.req.smm_reg_info);
#endif

	printk(BIOS_DEBUG, "PSP: Notify SMM info... ");

	set_smm_flag();
	cmd_status = send_psp_command(MBOX_BIOS_CMD_SMM_INFO, &buffer);
	clear_smm_flag();

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	return cmd_status;
}

/* Notify PSP the system is going to a sleep state. */
void psp_notify_sx_info(u8 sleep_type)
{
	int cmd_status;
	struct mbox_cmd_sx_info_buffer *buffer;

	/* PSP verifies that this buffer is at the address specified in psp_notify_smm() */
	buffer = (struct mbox_cmd_sx_info_buffer *)c2p_buffer.buffer;
	memset(buffer, 0, sizeof(*buffer));
	buffer->header.size = sizeof(*buffer);

	if (sleep_type > MBOX_BIOS_CMD_SX_INFO_SLEEP_TYPE_MAX) {
		printk(BIOS_ERR, "PSP: BUG: invalid sleep type 0x%x requested\n", sleep_type);
		return;
	}

	printk(BIOS_DEBUG, "PSP: Prepare to enter sleep state %d... ", sleep_type);

	buffer->sleep_type = sleep_type;

	set_smm_flag();
	cmd_status = send_psp_command(MBOX_BIOS_CMD_SX_INFO, buffer);
	clear_smm_flag();

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer->header);
}
