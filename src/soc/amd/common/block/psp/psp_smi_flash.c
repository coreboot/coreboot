/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <types.h>
#include "psp_def.h"

enum psp_spi_id_type {
	SMI_TARGET_NVRAM		= 0,
	SMI_TARGET_RPMC_NVRAM		= 5,
};

struct pspv2_spi_info_request {
	u64 target_nv_id;
	u64 lba;
	u64 block_size;
	u64 num_blocks;
} __packed;

struct mbox_pspv2_cmd_spi_info {
	struct mbox_buffer_header header;
	struct pspv2_spi_info_request req;
} __packed;

struct pspv2_spi_read_write_request {
	u64 target_nv_id;
	u64 lba;
	u64 offset;
	u64 num_bytes;
	u8 buffer[];
} __packed;

struct mbox_pspv2_cmd_spi_read_write {
	struct mbox_buffer_header header;
	struct pspv2_spi_read_write_request req;
} __packed;

struct pspv2_spi_erase_request {
	u64 target_nv_id;
	u64 lba;
	u64 num_blocks;
} __packed;

struct mbox_pspv2_cmd_spi_erase {
	struct mbox_buffer_header header;
	struct pspv2_spi_erase_request req;
} __packed;

enum mbox_p2c_status psp_smi_spi_get_info(struct mbox_default_buffer *buffer)
{
	printk(BIOS_SPEW, "PSP: SPI info request\n");

	return MBOX_PSP_UNSUPPORTED;
}

enum mbox_p2c_status psp_smi_spi_read(struct mbox_default_buffer *buffer)
{
	printk(BIOS_SPEW, "PSP: SPI read request\n");

	return MBOX_PSP_UNSUPPORTED;
}

enum mbox_p2c_status psp_smi_spi_write(struct mbox_default_buffer *buffer)
{
	printk(BIOS_SPEW, "PSP: SPI write request\n");

	return MBOX_PSP_UNSUPPORTED;
}

enum mbox_p2c_status psp_smi_spi_erase(struct mbox_default_buffer *buffer)
{
	printk(BIOS_SPEW, "PSP: SPI erase request\n");

	return MBOX_PSP_UNSUPPORTED;
}
