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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>
#include <spi-generic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>

#if defined (CONFIG_HUDSON_IMC_FWM)
#include "FchPlatform.h"

static int bus_claimed = 0;
#endif

static u32 spibar;

static void reset_internal_fifo_pointer(void)
{
	do {
		write8(spibar + 2, read8(spibar + 2) | 0x10);
	} while (read8(spibar + 0xD) & 0x7);
}

static void execute_command(void)
{
	write8(spibar + 2, read8(spibar + 2) | 1);

	while ((read8(spibar + 2) & 1) && (read8(spibar+3) & 0x80));
}

void spi_init()
{
	device_t dev;

	dev = dev_find_slot(0, PCI_DEVFN(0x14, 3));
	spibar = pci_read_config32(dev, 0xA0) & ~0x1F;
}

int spi_xfer(struct spi_slave *slave, const void *dout,
		unsigned int bitsout, void *din, unsigned int bitsin)
{
	/* First byte is cmd which can not being sent through FIFO. */
	u8 cmd = *(u8 *)dout++;
	u8 readoffby1;
#if !CONFIG_SOUTHBRIDGE_AMD_AGESA_YANGTZE
	u8 readwrite;
#endif
	u8 bytesout, bytesin;
	u8 count;

	bitsout -= 8;
	bytesout = bitsout / 8;
	bytesin  = bitsin / 8;

	readoffby1 = bytesout ? 0 : 1;

#if CONFIG_SOUTHBRIDGE_AMD_AGESA_YANGTZE
	write8(spibar + 0x1E, 5);
	write8(spibar + 0x1F, bytesout); /* SpiExtRegIndx [5] - TxByteCount */
	write8(spibar + 0x1E, 6);
	write8(spibar + 0x1F, bytesin);  /* SpiExtRegIndx [6] - RxByteCount */
#else
	readwrite = (bytesin + readoffby1) << 4 | bytesout;
	write8(spibar + 1, readwrite);
#endif
	write8(spibar + 0, cmd);

	reset_internal_fifo_pointer();
	for (count = 0; count < bytesout; count++, dout++) {
		write8(spibar + 0x0C, *(u8 *)dout);
	}

	reset_internal_fifo_pointer();
	execute_command();

	reset_internal_fifo_pointer();
	/* Skip the bytes we sent. */
	for (count = 0; count < bytesout; count++) {
		cmd = read8(spibar + 0x0C);
	}

	reset_internal_fifo_pointer();
	for (count = 0; count < bytesin; count++, din++) {
		*(u8 *)din = read8(spibar + 0x0C);
	}

	return 0;
}
int spi_claim_bus(struct spi_slave *slave)
{
#if defined (CONFIG_HUDSON_IMC_FWM)

	if (slave->rw == SPI_WRITE_FLAG) {
		bus_claimed++;
		if (bus_claimed == 1)
			ImcSleep(NULL);
	}
#endif

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
#if defined (CONFIG_HUDSON_IMC_FWM)

	if (slave->rw == SPI_WRITE_FLAG)  {
		bus_claimed--;
		if (bus_claimed <= 0) {
			bus_claimed = 0;
			ImcWakeup(NULL);
		}
	}
#endif
}

void spi_cs_activate(struct spi_slave *slave)
{
}

void spi_cs_deactivate(struct spi_slave *slave)
{
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)
{
	struct spi_slave *slave = malloc(sizeof(*slave));

	if (!slave) {
		return NULL;
	}

	memset(slave, 0, sizeof(*slave));

	return slave;
}
