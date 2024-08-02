/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <types.h>
#include "psp_def.h"

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
