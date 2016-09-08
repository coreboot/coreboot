/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __SOC_MARVELL_MVMAP2315_MCU_H__
#define __SOC_MARVELL_MVMAP2315_MCU_H__

#include <stdint.h>

#include <soc/addressmap.h>
#include <types.h>

#define MVMAP2315_MCU_MSG_OFFSET			0xCE000000

enum {
	GET_HASH = 0x0,
	ABORT_HASH = 0x1,
	START_HASH = 0x2,
	SYNC_HASH = 0x3,
};

enum {
	ABORT = 0x0,
	FINISHED = 0x1,
	BUSY = 0x2
};

enum {
	MSG_READY = 1,
	MSG_RECEIVED = 8,
	MSG_READY_RECEIVED = 9,
};

struct sp_hash_request_msg {
	u8 protocol_version;
	u8 csum;
	u16 cmd_id;
	u8 cmd_version;
	u8 reserved0;
	u16 length;
	u8 hash_subcmd;
	u8 hash_type;
	u8 nonce_size;
	u8 reserved1;
	u32 offset;
	u32 size;
	u8 nonce_data[64];
};

check_member(sp_hash_request_msg, nonce_data, 0x14);
static struct sp_hash_request_msg * const mvmap2315_mcu_msg_buff
					= (void *)MVMAP2315_MCU_MSG_BUFF_BASE;

struct mcu_hash_msg {
	u8 protocol_version;
	u8 csum;
	u16 result;
	u16 length;
	u8 status;
	u8 type;
	u8 digest_size;
	u8 reserved0;
	u32 offset;
	u32 size;
	u8 digest_data[64];
};

struct mcu_pwr_status_msg {
	u8 protocol_version;
	u8 csum;
	u16 cmd_id;
	u8 cmd_version;
	u8 reserved;
	u16 length;
	u8 status;
};

#define MVMAP2315_IPC_IRQSET_MSGSENT			BIT(0)
#define MVMAP2315_IPC_IRQCLR_MSGREADY			BIT(0)
#define MVMAP2315_IPC_IRQCLR_MSGRECEIVED		BIT(3)
#define MVMAP2315_IPC_IRQACK				BIT(3)
#define MVMAP2315_MCU_IPC_IRQ				BIT(6)
struct mvmap2315_ipc_regs {
	u32 isrr;
	u32 wdr0;
	u32 wdr1;
	u32 isrw;
	u32 icr;
	u32 iir;
	u32 rdr0;
	u32 rdr1;
	u32 maj_mid_rev;
	u32 cfg_rev;
	u32 dummy;
};

check_member(mvmap2315_ipc_regs, dummy, 0x28);
static struct mvmap2315_ipc_regs * const mvmap2315_sp_ipc
					= (void *)MVMAP2315_SP_IPC_BASE;

void mcu_irq(void);
void send_mcu_msg(void *msg, u32 size);
void *receive_mcu_msg(void);
void sned_hash_msg(u8 subcmd);
void *receive_hash_msg_respond(void);

#endif /* __SOC_MARVELL_MVMAP2315_MCU_H__ */
