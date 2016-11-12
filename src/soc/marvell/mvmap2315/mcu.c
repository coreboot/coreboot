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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <console/console.h>
#include <soc/gic.h>
#include <soc/mcu.h>

static u8 mvmap2315_calc_checksum(const void  *data, u32 size)
{
	u8  csum;
	const u8 *bytes = data;
	int i;

	for (i = csum = 0; i < size; i++)
		csum += bytes[i];

	return (~csum) & 0xFF;
}

void mcu_irq(void)
{
	printk(BIOS_DEBUG, "waiting for MCU msg...\n");
	while (!(read32(&mvmap2315_bcm_gicd->ispendr2) &
	      MVMAP2315_MCU_IPC_IRQ))
		;
}

void send_mcu_msg(void *msg, u32 size)
{
	printk(BIOS_DEBUG, "sending msg to MCU...\n");
	write32(&mvmap2315_sp_ipc->wdr0, (u32)msg);
	setbits_le32(&mvmap2315_sp_ipc->isrw, MVMAP2315_IPC_IRQSET_MSGSENT);
}

void *receive_mcu_msg(void)
{
	void *msg;

	printk(BIOS_DEBUG, "receiving msg to MCU...\n");

	write32(&mvmap2315_sp_ipc->dummy, 0xA5A5A5A5);

	switch (read32(&mvmap2315_sp_ipc->iir)) {
	case MSG_READY:
		msg = (void *)(read32(&mvmap2315_sp_ipc->rdr0)
			       + MVMAP2315_MCU_MSG_OFFSET);
		setbits_le32(&mvmap2315_sp_ipc->icr,
			     MVMAP2315_IPC_IRQCLR_MSGREADY);
		setbits_le32(&mvmap2315_sp_ipc->isrw, MVMAP2315_IPC_IRQACK);
		break;
	case MSG_RECEIVED:
		msg = NULL;
		setbits_le32(&mvmap2315_sp_ipc->icr,
			     MVMAP2315_IPC_IRQCLR_MSGRECEIVED);
		break;
	case MSG_READY_RECEIVED:
		msg = (void *)(read32(&mvmap2315_sp_ipc->rdr0)
			       + MVMAP2315_MCU_MSG_OFFSET);
		setbits_le32(&mvmap2315_sp_ipc->icr,
			     MVMAP2315_IPC_IRQCLR_MSGREADY);
		setbits_le32(&mvmap2315_sp_ipc->icr,
			     MVMAP2315_IPC_IRQCLR_MSGRECEIVED);
		setbits_le32(&mvmap2315_sp_ipc->isrw, MVMAP2315_IPC_IRQACK);

		break;
	default:
		msg = NULL;
		write32(&mvmap2315_sp_ipc->icr, read32(&mvmap2315_sp_ipc->iir));
		break;
	}

	return msg;
}

void sned_hash_msg(u8 subcmd)
{
	printk(BIOS_DEBUG, "requesting MCU hash...\n");

	mvmap2315_mcu_msg_buff->protocol_version = 0x3;
	mvmap2315_mcu_msg_buff->cmd_id = 0x2A;
	mvmap2315_mcu_msg_buff->reserved0 = 0x0;
	mvmap2315_mcu_msg_buff->length = 76;
	mvmap2315_mcu_msg_buff->csum = 0;
	mvmap2315_mcu_msg_buff->hash_subcmd = subcmd;
	mvmap2315_mcu_msg_buff->hash_type = 0;
	mvmap2315_mcu_msg_buff->nonce_size = 0;
	mvmap2315_mcu_msg_buff->reserved1 = 0;
	mvmap2315_mcu_msg_buff->offset = 0;
	mvmap2315_mcu_msg_buff->size = 32;
	mvmap2315_mcu_msg_buff->csum = mvmap2315_calc_checksum(
				       (void *)mvmap2315_mcu_msg_buff,
				       sizeof(struct sp_hash_request_msg));

	send_mcu_msg((void *)mvmap2315_mcu_msg_buff,
		     sizeof(struct sp_hash_request_msg));
}

void *receive_hash_msg_respond(void)
{
	struct mcu_hash_msg *mvmap2315_hash_msg_response;

	printk(BIOS_DEBUG, "waiting for MCU hash response msg...\n");

	mcu_irq();

	mvmap2315_hash_msg_response = receive_mcu_msg();

	mcu_irq();

	receive_mcu_msg();

	return mvmap2315_hash_msg_response;
}
