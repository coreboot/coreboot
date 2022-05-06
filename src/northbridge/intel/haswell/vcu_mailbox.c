/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/vcu_mailbox.h>
#include <types.h>

/*
 * This is a library for the VCU (Validation Control Unit) mailbox. This
 * mailbox is primarily used to adjust some magic PCIe tuning parameters.
 *
 * There are two revisions of the VCU mailbox. Rev1 is specific to Haswell
 * stepping A0, and all other steppings use Rev2. Haswell stepping A0 CPUs
 * are early Engineering Samples with undocumented errata, and most likely
 * need special microcode updates to boot. Thus, the code does not support
 * VCU mailbox Rev1, because no one should need it anymore.
 */

#define VCU_MAILBOX_INTERFACE	0x6c00
#define VCU_MAILBOX_DATA	0x6c04

#define VCU_RUN_BUSY		(1U << 31)

enum vcu_opcode {
	VCU_INVALID_OPCODE		= 0x00,
	VCU_OPCODE_READ_VCU_API_VER_ID	= 0x01,
	VCU_OPCODE_OPEN_SEQ		= 0x02,
	VCU_OPCODE_CLOSE_SEQ		= 0x03,
	VCU_OPCODE_READ_DATA		= 0x07,
	VCU_OPCODE_WRITE_DATA		= 0x08,
	VCU_OPCODE_READ_CSR		= 0x13,
	VCU_OPCODE_WRITE_CSR		= 0x14,
	VCU_OPCODE_READ_MMIO		= 0x15,
	VCU_OPCODE_WRITE_MMIO		= 0x16,
};

enum vcu_sequence {
	SEQ_ID_READ_CSR		= 0x1,
	SEQ_ID_WRITE_CSR	= 0x2,
	SEQ_ID_READ_MMIO	= 0x3,
	SEQ_ID_WRITE_MMIO	= 0x4,
};

#define VCU_RESPONSE_MASK		0xffff
#define VCU_RESPONSE_SUCCESS		0x40
#define VCU_RESPONSE_BUSY		0x80
#define VCU_RESPONSE_THREAD_UNAVAILABLE	0x82
#define VCU_RESPONSE_ILLEGAL		0x90

/* FIXME: Use timer API */
static void send_vcu_command(const enum vcu_opcode opcode, const uint32_t data)
{
	if (opcode == VCU_INVALID_OPCODE)
		return;

	for (unsigned int i = 0; i < 10; i++) {
		mchbar_write32(VCU_MAILBOX_DATA, data);
		mchbar_write32(VCU_MAILBOX_INTERFACE, opcode | VCU_RUN_BUSY);
		uint32_t vcu_interface;
		for (unsigned int j = 0; j < 100; j++) {
			vcu_interface = mchbar_read32(VCU_MAILBOX_INTERFACE);
			if (!(vcu_interface & VCU_RUN_BUSY))
				break;

			udelay(10);
		}
		if (vcu_interface & VCU_RUN_BUSY)
			continue;

		if ((vcu_interface & VCU_RESPONSE_MASK) == VCU_RESPONSE_SUCCESS)
			return;
	}
	printk(BIOS_ERR, "VCU: Failed to send command\n");
}

static enum vcu_opcode get_register_opcode(enum vcu_sequence seq)
{
	switch (seq) {
	case SEQ_ID_READ_CSR:
		return VCU_OPCODE_READ_CSR;
	case SEQ_ID_WRITE_CSR:
		return VCU_OPCODE_WRITE_CSR;
	case SEQ_ID_READ_MMIO:
		return VCU_OPCODE_READ_MMIO;
	case SEQ_ID_WRITE_MMIO:
		return VCU_OPCODE_WRITE_MMIO;
	default:
		BUG();
		return VCU_INVALID_OPCODE;
	}
}

static enum vcu_opcode get_data_opcode(enum vcu_sequence seq)
{
	switch (seq) {
	case SEQ_ID_READ_CSR:
	case SEQ_ID_READ_MMIO:
		return VCU_OPCODE_READ_DATA;
	case SEQ_ID_WRITE_CSR:
	case SEQ_ID_WRITE_MMIO:
		return VCU_OPCODE_WRITE_DATA;
	default:
		BUG();
		return VCU_INVALID_OPCODE;
	}
}

static uint32_t send_vcu_sequence(uint32_t addr, enum vcu_sequence seq, uint32_t wr_data)
{
	send_vcu_command(VCU_OPCODE_OPEN_SEQ, seq);

	send_vcu_command(get_register_opcode(seq), addr);

	send_vcu_command(get_data_opcode(seq), wr_data);

	const uint32_t rd_data = mchbar_read32(VCU_MAILBOX_DATA);

	send_vcu_command(VCU_OPCODE_CLOSE_SEQ, seq);

	return rd_data;
}

#define VCU_WRITE_IGNORED	0

uint32_t vcu_read_csr(uint32_t addr)
{
	return send_vcu_sequence(addr, SEQ_ID_READ_CSR, VCU_WRITE_IGNORED);
}

void vcu_write_csr(uint32_t addr, uint32_t data)
{
	send_vcu_sequence(addr, SEQ_ID_WRITE_CSR, data);
}

void vcu_update_csr(uint32_t addr, uint32_t andvalue, uint32_t orvalue)
{
	vcu_write_csr(addr, (vcu_read_csr(addr) & andvalue) | orvalue);
}

uint32_t vcu_read_mmio(uint32_t addr)
{
	return send_vcu_sequence(addr, SEQ_ID_READ_MMIO, VCU_WRITE_IGNORED);
}

void vcu_write_mmio(uint32_t addr, uint32_t data)
{
	send_vcu_sequence(addr, SEQ_ID_WRITE_MMIO, data);
}

void vcu_update_mmio(uint32_t addr, uint32_t andvalue, uint32_t orvalue)
{
	vcu_write_mmio(addr, (vcu_read_mmio(addr) & andvalue) | orvalue);
}
