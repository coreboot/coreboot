/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <amdblocks/smi.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <device/mmio.h>
#include <types.h>
#include "psp_def.h"

extern struct {
	u8 buffer[P2C_BUFFER_MAXSIZE];
}  __aligned(32) p2c_buffer;

static const uintptr_t p2c_mbox_base = (uintptr_t)&p2c_buffer.buffer;

#define P2C_MBOX_COMMAND_OFFSET		0x00
#define P2C_MBOX_STATUS_OFFSET		0x04
#define P2C_MBOX_BUFFER_OFFSET		0x08

union p2c_mbox_status {
	struct {
		u32 checksum		:  8; /* [ 0.. 7] */
		u32 checksum_en		:  1; /* [ 8.. 8] */
		u32 reserved		: 22; /* [ 9..30] */
		u32 command_ready	:  1; /* [31..31] */
	} __packed fields;
	u32 raw;
};

static u8 rd_bios_mbox_checksum(void)
{
	union p2c_mbox_status status;

	status.raw = read32p(p2c_mbox_base + P2C_MBOX_STATUS_OFFSET);
	return status.fields.checksum;
}

static void wr_bios_mbox_checksum(u8 checksum)
{
	union p2c_mbox_status status;

	status.raw = read32p(p2c_mbox_base + P2C_MBOX_STATUS_OFFSET);
	status.fields.checksum = checksum;
	write32p(p2c_mbox_base + P2C_MBOX_STATUS_OFFSET, status.raw);
}

static bool rd_bios_mbox_checksum_en(void)
{
	union p2c_mbox_status status;

	status.raw = read32p(p2c_mbox_base + P2C_MBOX_STATUS_OFFSET);
	return !!status.fields.checksum_en;
}

static void wr_bios_mbox_ready(bool ready)
{
	union p2c_mbox_status status;

	status.raw = read32p(p2c_mbox_base + P2C_MBOX_STATUS_OFFSET);
	status.fields.command_ready = ready;
	write32p(p2c_mbox_base + P2C_MBOX_STATUS_OFFSET, status.raw);
}

void enable_psp_smi(void)
{
	wr_bios_mbox_ready(true);
}

static void disable_psp_smi(void)
{
	wr_bios_mbox_ready(false);
}

static void clear_psp_command(void)
{
	write32p(p2c_mbox_base + P2C_MBOX_COMMAND_OFFSET, 0);
}

static u32 get_psp_command(void)
{
	return read32p(p2c_mbox_base + P2C_MBOX_COMMAND_OFFSET);
}

static struct mbox_default_buffer *get_psp_command_buffer(void)
{
	return (struct mbox_default_buffer *)(p2c_mbox_base + P2C_MBOX_BUFFER_OFFSET);
}

static uint32_t get_psp_cmd_buffer_length(void)
{
	return read32(&get_psp_command_buffer()->header.size);
}

static u8 calc_psp_buffer_checksum8(void)
{
	const uint8_t *data = (const u8 *)get_psp_command_buffer();
	const size_t size = get_psp_cmd_buffer_length();
	u8 checksum = 0;
	size_t i;

	for (i = 0; i < size; i++)
		checksum += read8(data + i);

	return checksum;
}

static void write_psp_cmd_buffer_status(enum mbox_p2c_status status)
{
	return write32(&get_psp_command_buffer()->header.status, status);
}

static enum mbox_p2c_status check_psp_command(void)
{
	if (rd_bios_mbox_checksum_en() &&
	    calc_psp_buffer_checksum8() != rd_bios_mbox_checksum())
		return MBOX_PSP_CRC_ERROR;

	if (get_psp_cmd_buffer_length() > P2C_BUFFER_MAXSIZE - P2C_MBOX_BUFFER_OFFSET)
		return MBOX_PSP_INVALID_PARAMETER;

	return MBOX_PSP_SUCCESS;
}

static void handle_psp_command(void)
{
	enum mbox_p2c_status status;
	u32 cmd;

	status = check_psp_command();
	if (status != MBOX_PSP_SUCCESS)
		goto out;

	cmd = get_psp_command();

	switch (cmd) {
	default:
		printk(BIOS_ERR, "PSP: Unknown command %d\n", cmd);
		status = MBOX_PSP_UNSUPPORTED;
		break;
	}

out:
	write_psp_cmd_buffer_status(status);

	if (status == MBOX_PSP_SUCCESS && rd_bios_mbox_checksum_en())
		wr_bios_mbox_checksum(calc_psp_buffer_checksum8());
}

/* TODO: check if all wbinvd() calls are necessary */
void psp_smi_handler(void)
{
	disable_psp_smi();

	wbinvd();

	handle_psp_command();

	wbinvd();

	clear_psp_command();
	enable_psp_smi();

	wbinvd();

	reset_psp_smi();
}
