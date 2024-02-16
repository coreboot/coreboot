/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is basically drivers/spi/cbfs_spi.cb but the boot device is not known at build time
 */

#include <boot_device.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/mmio.h>
#include <lib.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/spi.h>
#include <spi_flash.h>
#include <spi_sdcard.h>
#include <stdint.h>
#include <symbols.h>
#include <timer.h>

// There are two different means of accessing the SPI flash.
// They are controlled by the fctrl register.
// 1. memory mapped access
// 2. FIFO based access

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

#define MSEL_ACCESS_METHOD_MMAP(m) ((m == 5) || (m == 6) || (m == 7) || (m == 10) || (m == 13))

// probably something for devicetree
struct fu740_spi_config fu740_spi_configs[] = {
	{ 0 },
	{
		.freq = 10*1000*1000,
	},
	{ 0 },
};

static struct spi_sdcard spi2_sdcard; //TODO initialize SPI2
//static struct spi_flash spi0_flash;
//static struct spi_flash spi1_flash;
static struct spi_flash spi_flash;
static bool spi_flash_init_done;

static ssize_t sd_readat(const struct region_device *rdev, void *dest,
					size_t offset, size_t count)
{
	spi_sdcard_read(&spi2_sdcard, dest, offset, count);
	return count;
}

static ssize_t spi_readat(const struct region_device *rd, void *b, size_t offset, size_t size)
{
	if (spi_flash_read(&spi_flash, offset, size, b))
		return -1;
	return size;
}

static ssize_t spi_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	if (spi_flash_write(&spi_flash, offset, size, b))
		return -1;
	return size;
}

static ssize_t spi_eraseat(const struct region_device *rd, size_t offset, size_t size)
{
	if (spi_flash_erase(&spi_flash, offset, size))
		return -1;
	return size;
}

static const struct region_device_ops sd_ops = {
	.mmap   = mmap_helper_rdev_mmap,
	.munmap = mmap_helper_rdev_munmap,
	.readat = sd_readat,
};

static const struct region_device_ops spi_ops = {
	.mmap = mmap_helper_rdev_mmap,
	.munmap = mmap_helper_rdev_munmap,
	.readat = spi_readat,
	.writeat = spi_writeat,
	.eraseat = spi_eraseat,
};

//TODO using postram cache means that all spi transitions are transferred to actual memory.
// Native memory mapping obviously doesnt have that problem. That can however only happen if
// memory has been initialized before accessing the boot device. So no CBFS access before boot.
//static struct mem_pool mem_cbfs_cache =
//       MEM_POOL_INIT(_postram_cbfs_cache, REGION_SIZE(postram_cbfs_cache), CONFIG_CBFS_CACHE_ALIGN);
//static struct mmap_helper_region_device spi_mdev =
//       MMAP_HELPER_DEV_INIT(&spi_ops, 0, CONFIG_ROM_SIZE, &mem_cbfs_cache);

static struct mmap_helper_region_device spi_mdev =
	MMAP_HELPER_DEV_INIT(&spi_ops, 0, CONFIG_ROM_SIZE, &cbfs_cache);

static struct mmap_helper_region_device sd_mdev =
	MMAP_HELPER_DEV_INIT(&sd_ops, 0, CONFIG_ROM_SIZE, &cbfs_cache);

void boot_device_init(void)
{
	printk(BIOS_DEBUG, "%s 0\n", __func__);
	if (spi_flash_init_done == true)
		return;

	uint32_t m = read32((uint32_t *)FU740_MSEL) & 0xf;
	printk(BIOS_DEBUG, "%s MSEL %#x\n", __func__, m);

	// We have not yet found a way to reliably program the
	// on-board SPI part (sifive neglected to put a diode on vcc ...)
	// Once we work that out, we can test this code.
	// It is left here as a hint of what needs to be done.
	// Pass the information of the flash read operation to the spi controller
	if (MSEL_SPI0x4(m)) {
		die("SPI0x4 is not supported yet");
		//config.ffmt_config.data_proto = FU740_SPI_PROTO_Q;
		//config.ffmt_config.cmd_code = 0x6B; // Quad output read
		//.cmd_code = 0xec,
		//.pad_cnt = 6,
	}
	if (MSEL_SPI1x4(m)) {
		printk(BIOS_DEBUG, "%s SPI1x4 1\n", __func__);
		fu740_spi_configs[1].ffmt_config.data_proto = FU740_SPI_PROTO_Q;
		fu740_spi_configs[1].ffmt_config.cmd_code = 0x6b; // quad
		if (spi_flash_probe(1, 0, &spi_flash)) {
			printk(BIOS_EMERG, "SPI1x4 failed to init SPI flash\n");
			return;
		}
		if (fu740_spi_setup(&spi_flash.spi) == -1) {
			printk(BIOS_EMERG, "SPI1x4 failed to configure mmap for SPI flash\n");
		}
		printk(BIOS_DEBUG, "%s SPI1x4 2\n", __func__);
	} else if (MSEL_SPI1x1(m)) {
		printk(BIOS_DEBUG, "%s 1\n", __func__);
		fu740_spi_configs[1].ffmt_config.data_proto = FU740_SPI_PROTO_S;
		fu740_spi_configs[1].ffmt_config.cmd_code = 0x03; // Normal read
		//TODO SPI1x1 is always using programmed IO (FIFO)? (memory map did not work...)
		if (spi_flash_probe(1, 0, &spi_flash)) {
			printk(BIOS_EMERG, "failed to init SPI flash\n");
			return;
		}
		printk(BIOS_DEBUG, "%s 2\n", __func__);
	} else if (MSEL_SPI2SD(m)) {
		spi_sdcard_init(&spi2_sdcard, 2, 0);
	} else {
		printk(BIOS_EMERG, "MSEL: %#02x: ", m);
		die("unsupported configuration of MSEL\n");
	}

	spi_flash_init_done = true;
}

/* Return the CBFS boot device. */
const struct region_device *boot_device_ro(void)
{
	if (spi_flash_init_done != true)
		return NULL;

	uint32_t m = read32((uint32_t *)FU740_MSEL);

	if (MSEL_SPI2SD(m) || MSEL_SPI1SD(m))
		return &sd_mdev.rdev;
	return &spi_mdev.rdev;
}

//const struct spi_flash *boot_device_spi_flash(void)
//{
//	boot_device_init();
//
//	if (spi_flash_init_done != true)
//		return NULL;
//
//	return &spi_flash;
//}
