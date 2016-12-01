/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>
#include <console/console.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>

#include "SBPLATFORM.h"
#include <vendorcode/amd/cimx/sb800/ECfan.h>

#define AMD_SB_SPI_TX_LEN	8

static uintptr_t spibar;

static void reset_internal_fifo_pointer(void)
{
	do {
		write8((void *)(spibar + 2),
		       read8((void *)(spibar + 2)) | 0x10);
	} while (read8((void *)(spibar + 0xD)) & 0x7);
}

static void execute_command(void)
{
	write8((void *)(spibar + 2), read8((void *)(spibar + 2)) | 1);

	while ((read8((void *)(spibar + 2)) & 1) &&
	       (read8((void *)(spibar+3)) & 0x80));
}

void spi_init()
{
	device_t dev;

	dev = dev_find_slot(0, PCI_DEVFN(0x14, 3));
	spibar = pci_read_config32(dev, 0xA0) & ~0x1F;
}

unsigned int spi_crop_chunk(unsigned int cmd_len, unsigned int buf_len)
{
	return min(AMD_SB_SPI_TX_LEN - cmd_len, buf_len);
}

int spi_xfer(const struct spi_slave *slave, const void *dout,
		size_t bytesout, void *din, size_t bytesin)
{
	/* First byte is cmd which can not being sent through FIFO. */
	u8 cmd = *(u8 *)dout++;
	u8 readoffby1;
	u8 readwrite;
	size_t count;

	bytesout--;

	/*
	 * Check if this is a write command attempting to transfer more bytes
	 * than the controller can handle. Iterations for writes are not
	 * supported here because each SPI write command needs to be preceded
	 * and followed by other SPI commands, and this sequence is controlled
	 * by the SPI chip driver.
	 */
	if (bytesout > AMD_SB_SPI_TX_LEN) {
		printk(BIOS_DEBUG, "FCH SPI: Too much to write. Does your SPI chip driver use"
		     " spi_crop_chunk()?\n");
		return -1;
	}

	readoffby1 = bytesout ? 0 : 1;

	readwrite = (bytesin + readoffby1) << 4 | bytesout;
	write8((void *)(spibar + 1), readwrite);
	write8((void *)(spibar + 0), cmd);

	reset_internal_fifo_pointer();
	for (count = 0; count < bytesout; count++, dout++) {
		write8((void *)(spibar + 0x0C), *(u8 *)dout);
	}

	reset_internal_fifo_pointer();
	execute_command();

	reset_internal_fifo_pointer();
	/* Skip the bytes we sent. */
	for (count = 0; count < bytesout; count++) {
		cmd = read8((void *)(spibar + 0x0C));
	}

	reset_internal_fifo_pointer();
	for (count = 0; count < bytesin; count++, din++) {
		*(u8 *)din = read8((void *)(spibar + 0x0C));
	}

	return 0;
}

static void ImcSleep(void)
{
	u8	cmd_val = 0x96;		/* Kick off IMC Mailbox command 96 */
	u8	reg0_val = 0;		/* clear response register */
	u8	reg1_val = 0xB4;	/* request ownership flag */

	WriteECmsg (MSG_REG0, AccWidthUint8, &reg0_val);
	WriteECmsg (MSG_REG1, AccWidthUint8, &reg1_val);
	WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &cmd_val);

	WaitForEcLDN9MailboxCmdAck();
}


static void ImcWakeup(void)
{
	u8	cmd_val = 0x96;		/* Kick off IMC Mailbox command 96 */
	u8	reg0_val = 0;		/* clear response register */
	u8	reg1_val = 0xB5;	/* release ownership flag */

	WriteECmsg (MSG_REG0, AccWidthUint8, &reg0_val);
	WriteECmsg (MSG_REG1, AccWidthUint8, &reg1_val);
	WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &cmd_val);

	WaitForEcLDN9MailboxCmdAck();
}

int spi_claim_bus(const struct spi_slave *slave)
{
	/* Nothing is required. */
	return 0;
}

void spi_release_bus(const struct spi_slave *slave)
{
	/* Nothing is required. */
	return;
}

int chipset_volatile_group_begin(const struct spi_flash *flash)
{
	if (!IS_ENABLED(CONFIG_SB800_IMC_FWM))
		return 0;

	ImcSleep();
	return 0;
}

int chipset_volatile_group_end(const struct spi_flash *flash)
{
	if (!IS_ENABLED(CONFIG_SB800_IMC_FWM))
		return 0;

	ImcWakeup();
	return 0;
}

int spi_setup_slave(unsigned int bus, unsigned int cs, struct spi_slave *slave)
{
	slave->bus = bus;
	slave->cs = cs;
	return 0;
}
