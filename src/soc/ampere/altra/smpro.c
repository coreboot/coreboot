/* SPDX-License-Identifier: GPL-2.0-only */

#include "smpro.h"
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/addressmap.h>

/*
 * Register access through the SMpro (System Control Processor) doorbell
 * mailbox, for registers the normal world can't access directly (e.g. the
 * PCIe host bridge registers), and messages to the other SMpro/PMpro
 * doorbells. Port of edk2-platforms AmpereAltraPkg Library/MailboxInterfaceLib
 * and SystemFirmwareInterfaceLib (MailboxMsgRegisterRead/Write), socket 0
 * only.
 */

#define DB_IN_REG		0x00
#define DB_DIN0_REG		0x04
#define DB_DIN1_REG		0x08
#define DB_OUT_REG		0x10
#define DB_DOUT0_REG		0x14
#define DB_DOUT1_REG		0x18
#define DB_STATUS_REG		0x20
#define DB_STATUS_MASK_REG	0x24

#define DB_STATUS_AVAIL		(1 << 16)
#define DB_STATUS_ACK		(1 << 0)

#define POLL_INTERVAL_US	1000
#define POLL_COUNT		(10 * 1000 * 1000 / POLL_INTERVAL_US)

/* SMpro doorbell channel 0 */
#define SMPRO_DB0		AC01_SMPRO_DB_BASE

/* Debug message: type 0 in bits 31:28, subtype in 27:24, address bits 47:32 in 15:0 */
#define MSG_REGISTER_READ	0x01
#define MSG_REGISTER_WRITE	0x02
#define DEBUG_MSG(subtype, addr) (((subtype) << 24) | (((addr) >> 32) & 0xffff))

int ac01_mailbox_write(uintptr_t db, uint32_t data, uint32_t ext0, uint32_t ext1)
{
	if (read32p(db + DB_STATUS_REG) & DB_STATUS_ACK)
		write32p(db + DB_STATUS_REG, DB_STATUS_ACK);

	write32p(db + DB_DOUT0_REG, ext0);
	write32p(db + DB_DOUT1_REG, ext1);
	write32p(db + DB_OUT_REG, data);

	for (unsigned int i = 0; !(read32p(db + DB_STATUS_REG) & DB_STATUS_ACK); i++) {
		if (i == POLL_COUNT)
			return -1;
		udelay(POLL_INTERVAL_US);
	}
	write32p(db + DB_STATUS_REG, DB_STATUS_ACK);

	return 0;
}

void ac01_mailbox_unmask_interrupt(uintptr_t db)
{
	write32p(db + DB_STATUS_MASK_REG, ~DB_STATUS_AVAIL);
}

static int mailbox_read(uint32_t *data, uint32_t *ext0)
{
	for (unsigned int i = 0; !(read32p(SMPRO_DB0 + DB_STATUS_REG) & DB_STATUS_AVAIL); i++) {
		if (i == POLL_COUNT)
			return -1;
		udelay(POLL_INTERVAL_US);
	}

	*ext0 = read32p(SMPRO_DB0 + DB_DIN0_REG);
	read32p(SMPRO_DB0 + DB_DIN1_REG);
	*data = read32p(SMPRO_DB0 + DB_IN_REG);
	write32p(SMPRO_DB0 + DB_STATUS_REG, DB_STATUS_AVAIL);

	return 0;
}

int smpro_reg_read(uint64_t addr, uint32_t *val)
{
	uint32_t data, ext0;

	if (ac01_mailbox_write(SMPRO_DB0, DEBUG_MSG(MSG_REGISTER_READ, addr), (uint32_t)addr, 0) ||
	    mailbox_read(&data, &ext0)) {
		printk(BIOS_ERR, "SMpro: reading register 0x%llx timed out\n", addr);
		return -1;
	}

	if ((data & 0xff00) == 0) {
		printk(BIOS_ERR, "SMpro: reading register 0x%llx is not supported\n", addr);
		return -1;
	}

	*val = ext0;
	return 0;
}

int smpro_reg_write(uint64_t addr, uint32_t val)
{
	if (ac01_mailbox_write(SMPRO_DB0, DEBUG_MSG(MSG_REGISTER_WRITE, addr), (uint32_t)addr, val)) {
		printk(BIOS_ERR, "SMpro: writing register 0x%llx timed out\n", addr);
		return -1;
	}

	return 0;
}
