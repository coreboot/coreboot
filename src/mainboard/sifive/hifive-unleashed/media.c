/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <cbfs.h>
#include <symbols.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/spi.h>
#include <soc/clock.h>
#include <console/console.h>
#include <spi_sdcard.h>

/* follow is the FSBL boot device defined by ZSBL of sifive
 * FSBL replaced by bootblock of coreboot
 * MSEL_SPInx1 -> test if boot from memory-mapped on SPIn
 * MSEL_SPInx4 -> test if boot from memory-mapped on QPIn
 * MSEL_SPInSD -> test if boot from sdcard mount on SPIn */
#define MSEL_SPI0x1(m)	(((m) == 5) || ((m) == 14))
#define MSEL_SPI0x4(m)	(((m) == 6) || ((m) == 10) || ((m) == 15))
#define MSEL_SPI1x1(m)	((m) == 12)
#define MSEL_SPI1x4(m)	(((m) == 7) || ((m) == 13))
#define MSEL_SPI1SD(m)	((m) ==  8)
#define MSEL_SPI2x1(m)	((m) ==  9)
#define MSEL_SPI2SD(m)	((m) == 11)

static struct spi_sdcard card;

/* At 0x20000000: A 256MiB long memory-mapped view of the flash at QSPI0 */
static struct mem_region_device spi_mdev =
	MEM_REGION_DEV_RO_INIT((void *)0x20000000, CONFIG_ROM_SIZE);

static ssize_t unleashed_sd_readat(const struct region_device *rdev, void *dest,
					size_t offset, size_t count)
{
	spi_sdcard_read(&card, dest, offset, count);
	return count;
}

static const struct region_device_ops unleashed_sd_ops = {
	.mmap   = mmap_helper_rdev_mmap,
	.munmap = mmap_helper_rdev_munmap,
	.readat = unleashed_sd_readat,
};

static struct mmap_helper_region_device sd_mdev =
	MMAP_HELPER_DEV_INIT(&unleashed_sd_ops, 0, CONFIG_ROM_SIZE, &cbfs_cache);

const struct region_device *boot_device_ro(void)
{
	uint32_t m = read32((uint32_t *)FU540_MSEL);
	if (MSEL_SPI0x1(m) || MSEL_SPI0x4(m))
		return &spi_mdev.rdev;
	if (MSEL_SPI2SD(m))
		return &sd_mdev.rdev;
	die("Wrong configuration of MSEL\n");
	return NULL;
}

static const struct fu540_spi_mmap_config spi_mmap_config = {
	.cmd_en = 1,
	.addr_len = 4,
	.pad_cnt = 6,
	.cmd_proto = FU540_SPI_PROTO_S,
	.addr_proto = FU540_SPI_PROTO_Q,
	.data_proto = FU540_SPI_PROTO_Q,
	.cmd_code = 0xec,
	.pad_code = 0
};

void boot_device_init(void)
{
	uint32_t m = read32((uint32_t *)FU540_MSEL);
	if (MSEL_SPI0x1(m) || MSEL_SPI0x4(m)) {
		struct spi_slave slave;
		/* initialize spi controller */
		spi_setup_slave(0, 0, &slave);
		/* map flash to memory space */
		fu540_spi_mmap(&slave, &spi_mmap_config);
		return;
	}
	if (MSEL_SPI2SD(m)) {
		spi_sdcard_init(&card, 2, 0);
		return;
	}
	die("Wrong configuration of MSEL\n");
}
