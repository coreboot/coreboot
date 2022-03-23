/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/mmio.h>
#include <console/console.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <stddef.h>

#include <Proc/Fch/FchPlatform.h>

#define SPI_REG_OPCODE		0x0
#define SPI_REG_CNTRL01		0x1
#define SPI_REG_CNTRL02		0x2
 #define CNTRL02_FIFO_RESET	(1 << 4)
 #define CNTRL02_EXEC_OPCODE	(1 << 0)
#define SPI_REG_CNTRL03		0x3
 #define CNTRL03_SPIBUSY	(1 << 7)
#define SPI_REG_FIFO		0xc
#define SPI_REG_CNTRL11		0xd
 #define CNTRL11_FIFOPTR_MASK	0x07

#if CONFIG(SOUTHBRIDGE_AMD_AGESA_YANGTZE)
#define AMD_SB_SPI_TX_LEN	64
#else
#define AMD_SB_SPI_TX_LEN	8
#endif

static uintptr_t spibar;

static inline uint8_t spi_read(uint8_t reg)
{
	return read8((void *)(spibar + reg));
}

static inline void spi_write(uint8_t reg, uint8_t val)
{
	write8((void *)(spibar + reg), val);
}

static void reset_internal_fifo_pointer(void)
{
	uint8_t reg8;

	do {
		reg8 = spi_read(SPI_REG_CNTRL02);
		reg8 |= CNTRL02_FIFO_RESET;
		spi_write(SPI_REG_CNTRL02, reg8);
	} while (spi_read(SPI_REG_CNTRL11) & CNTRL11_FIFOPTR_MASK);
}

static void execute_command(void)
{
	uint8_t reg8;

	reg8 = spi_read(SPI_REG_CNTRL02);
	reg8 |= CNTRL02_EXEC_OPCODE;
	spi_write(SPI_REG_CNTRL02, reg8);

	while ((spi_read(SPI_REG_CNTRL02) & CNTRL02_EXEC_OPCODE) &&
	       (spi_read(SPI_REG_CNTRL03) & CNTRL03_SPIBUSY));
}

void spi_init(void)
{
	struct device *dev;

	dev = pcidev_on_root(0x14, 3);
	spibar = pci_read_config32(dev, 0xA0) & ~0x1F;
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
		size_t bytesout, void *din, size_t bytesin)
{
	/* First byte is cmd which can not be sent through FIFO. */
	u8 cmd = *(u8 *)dout++;
	u8 readoffby1;
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

	if (CONFIG(SOUTHBRIDGE_AMD_AGESA_YANGTZE)) {
		spi_write(0x1E, 5);
		spi_write(0x1F, bytesout); /* SpiExtRegIndx [5] - TxByteCount */
		spi_write(0x1E, 6);
		spi_write(0x1F, bytesin);  /* SpiExtRegIndx [6] - RxByteCount */
	} else {
		u8 readwrite = (bytesin + readoffby1) << 4 | bytesout;
		spi_write(SPI_REG_CNTRL01, readwrite);
	}
	spi_write(SPI_REG_OPCODE, cmd);

	reset_internal_fifo_pointer();
	for (count = 0; count < bytesout; count++, dout++) {
		spi_write(SPI_REG_FIFO, *(uint8_t *)dout);
	}

	reset_internal_fifo_pointer();
	execute_command();

	reset_internal_fifo_pointer();
	/* Skip the bytes we sent. */
	for (count = 0; count < bytesout; count++) {
		spi_read(SPI_REG_FIFO);
	}

	for (count = 0; count < bytesin; count++, din++) {
		*(uint8_t *)din = spi_read(SPI_REG_FIFO);
	}

	return 0;
}

int chipset_volatile_group_begin(const struct spi_flash *flash)
{
	if (!CONFIG(HUDSON_IMC_FWM))
		return 0;

	ImcSleep(NULL);
	return 0;
}

int chipset_volatile_group_end(const struct spi_flash *flash)
{
	if (!CONFIG(HUDSON_IMC_FWM))
		return 0;

	ImcWakeup(NULL);
	return 0;
}

static int xfer_vectors(const struct spi_slave *slave,
			struct spi_op vectors[], size_t count)
{
	return spi_flash_vector_helper(slave, vectors, count, spi_ctrlr_xfer);
}

static const struct spi_ctrlr spi_ctrlr = {
	.xfer_vector = xfer_vectors,
	.max_xfer_size = AMD_SB_SPI_TX_LEN,
	.flags = SPI_CNTRLR_DEDUCT_CMD_LEN,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = 0,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
