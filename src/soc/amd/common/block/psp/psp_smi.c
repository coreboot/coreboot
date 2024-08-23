/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <amdblocks/smi.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <device/mmio.h>
#include <types.h>
#include "psp_def.h"
#include "psp_smi_flash.h"

/* PSP to x86 commands */
#define MBOX_PSP_CMD_SPI_INFO		0x83
#define MBOX_PSP_CMD_SPI_READ		0x84
#define MBOX_PSP_CMD_SPI_WRITE		0x85
#define MBOX_PSP_CMD_SPI_ERASE		0x86

extern struct {
	uint8_t buffer[P2C_BUFFER_MAXSIZE];
}  __aligned(32) p2c_buffer;

static const uintptr_t p2c_mbox_base = (uintptr_t)&p2c_buffer.buffer;

#define P2C_MBOX_COMMAND_OFFSET		0x00
#define P2C_MBOX_STATUS_OFFSET		0x04
#define P2C_MBOX_BUFFER_OFFSET		0x08

union p2c_mbox_status {
	struct {
		uint32_t checksum	:  8; /* [ 0.. 7] */
		uint32_t checksum_en	:  1; /* [ 8.. 8] */
		uint32_t reserved	: 22; /* [ 9..30] */
		uint32_t command_ready	:  1; /* [31..31] */
	} __packed fields;
	uint32_t raw;
};

static uint8_t rd_bios_mbox_checksum(void)
{
	union p2c_mbox_status status;

	status.raw = read32p(p2c_mbox_base + P2C_MBOX_STATUS_OFFSET);
	return status.fields.checksum;
}

static void wr_bios_mbox_checksum(uint8_t checksum)
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

static uint32_t get_psp_command(void)
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

static uint8_t calc_psp_buffer_checksum8(void)
{
	const uint8_t *data = (const uint8_t *)get_psp_command_buffer();
	const size_t size = get_psp_cmd_buffer_length();
	uint8_t checksum = 0;
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
	uint32_t cmd;
	struct mbox_default_buffer *const buffer = get_psp_command_buffer();

	status = check_psp_command();
	if (status != MBOX_PSP_SUCCESS)
		goto out;

	cmd = get_psp_command();

	switch (cmd) {
	case MBOX_PSP_CMD_SPI_INFO:
		status = psp_smi_spi_get_info(buffer);
		break;
	case MBOX_PSP_CMD_SPI_READ:
		status = psp_smi_spi_read(buffer);
		break;
	case MBOX_PSP_CMD_SPI_WRITE:
		status = psp_smi_spi_write(buffer);
		break;
	case MBOX_PSP_CMD_SPI_ERASE:
		status = psp_smi_spi_erase(buffer);
		break;
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
