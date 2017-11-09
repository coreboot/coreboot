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
#include <arch/early_variables.h>
#include <timer.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <soc/southbridge.h>
#include <soc/pci_devs.h>
#include <soc/imc.h>

static uintptr_t spibar CAR_GLOBAL;

static uintptr_t get_spibase(void)
{
	return *(uintptr_t *)car_get_var_ptr(&spibar);
}

static void set_spibar(uintptr_t base)
{
	*(uintptr_t *)car_get_var_ptr(&spibar) = base;
}

static inline uint8_t spi_read8(uint8_t reg)
{
	return read8((void *)(get_spibase() + reg));
}

static inline uint32_t spi_read32(uint8_t reg)
{
	return read32((void *)(get_spibase() + reg));
}

static inline void spi_write8(uint8_t reg, uint8_t val)
{
	write8((void *)(get_spibase() + reg), val);
}

static inline void spi_write32(uint8_t reg, uint32_t val)
{
	write32((void *)(get_spibase() + reg), val);
}

static int reset_internal_fifo_pointer(void)
{
	uint8_t reg;
	const uint32_t timeout_ms = 500;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, timeout_ms);

	do {
		reg = spi_read8(SPI_REG_CNTRL02);
		reg |= SPI_FIFO_PTR_CLR02;
		spi_write8(SPI_REG_CNTRL02, reg);
		/* wait for ptr=0 */
		if (!(spi_read8(SPI_CNTRL11) & (SPI_FIFO_PTR_MASK11)))
			return 0;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_DEBUG, "FCH SPI Error: FIFO reset failed\n");
	return -1;
}

static int execute_command(void)
{
	uint32_t reg;
	const uint32_t timeout_ms = 500;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, timeout_ms);

	reg = spi_read32(SPI_CNTRL0);
	reg |= EXEC_OPCODE;
	spi_write32(SPI_CNTRL0, reg);

	do {
		if (!(spi_read32(SPI_CNTRL0) & (EXEC_OPCODE | SPI_BUSY)))
			return 0;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_DEBUG, "FCH SPI Error: Timeout executing command\n");
	return -1;
}

void spi_init(void)
{
	uintptr_t bar;

	bar = pci_read_config32(SOC_LPC_DEV, SPIROM_BASE_ADDRESS_REGISTER);
	bar = ALIGN_DOWN(bar, 64);
	set_spibar(bar);
}

static int do_command(uint8_t cmd, const void *dout,
		size_t bytesout, void *din, size_t *bytesin)
{
	size_t count;
	size_t max_in = MIN(*bytesin, SPI_FIFO_DEPTH);

	spi_write8(SPI_EXT_INDEX, SPI_TX_BYTE_COUNT);
	spi_write8(SPI_EXT_DATA, bytesout);
	spi_write8(SPI_EXT_INDEX, SPI_RX_BYTE_COUNT);
	spi_write8(SPI_EXT_DATA, max_in);
	spi_write8(SPI_CNTRL0, cmd);

	if (reset_internal_fifo_pointer())
		return -1;
	for (count = 0; count < bytesout; count++, dout++)
		spi_write8(SPI_CNTRL1, *(uint8_t *)dout);

	if (execute_command())
		return -1;

	if (reset_internal_fifo_pointer())
		return -1;
	for (count = 0; count < bytesout; count++)
		spi_read8(SPI_CNTRL1); /* skip the bytes we sent */

	for (count = 0; count < max_in; count++, din++)
		*(uint8_t *)din = spi_read8(SPI_CNTRL1);

	*bytesin -= max_in;
	return 0;
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
		size_t bytesout, void *din, size_t bytesin)
{
	uint8_t cmd;

	/* First byte is cmd which cannot be sent through FIFO */
	cmd = *(uint8_t *)dout++;
	bytesout--;

	/*
	 * Check if this is a write command attempting to transfer more bytes
	 * than the controller can handle.  Iterations for writes are not
	 * supported here because each SPI write command needs to be preceded
	 * and followed by other SPI commands, and this sequence is controlled
	 * by the SPI chip driver.
	 */
	if (bytesout > SPI_FIFO_DEPTH) {
		printk(BIOS_DEBUG, "FCH SPI: Too much to write. Does your SPI"
				" chip driver use spi_crop_chunk()?\n");
		return -1;
	}

	do {
		if (do_command(cmd, dout, bytesout, din, &bytesin))
			return -1;
	} while (bytesin);

	return 0;
}

int chipset_volatile_group_begin(const struct spi_flash *flash)
{
	if (IS_ENABLED(CONFIG_STONEYRIDGE_IMC_FWM))
		imc_sleep();
	return 0;
}

int chipset_volatile_group_end(const struct spi_flash *flash)
{
	if (IS_ENABLED(CONFIG_STONEYRIDGE_IMC_FWM))
		imc_wakeup();
	return 0;
}

static const struct spi_ctrlr spi_ctrlr = {
	.xfer = spi_ctrlr_xfer,
	.xfer_vector = spi_xfer_two_vectors,
	.max_xfer_size = SPI_FIFO_DEPTH,
	.deduct_cmd_len = true,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = 0,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
