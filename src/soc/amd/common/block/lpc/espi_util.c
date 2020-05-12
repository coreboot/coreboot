/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/chip.h>
#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <arch/mmio.h>
#include <console/console.h>
#include <soc/pci_devs.h>
#include <stddef.h>
#include <stdint.h>
#include <types.h>

static uintptr_t espi_bar;

void espi_update_static_bar(uintptr_t bar)
{
	espi_bar = bar;
}

static uintptr_t espi_get_bar(void)
{
	uintptr_t espi_spi_base;

	if (espi_bar)
		return espi_bar;

	espi_spi_base = lpc_get_spibase();
	espi_update_static_bar(espi_spi_base + ESPI_OFFSET_FROM_BAR);

	return espi_bar;
}

static uint32_t espi_read32(int reg)
{
	return read32((void *)(espi_get_bar() + reg));
}

static void espi_write32(int reg, uint32_t val)
{
	write32((void *)(espi_get_bar() + reg), val);
}

static uint16_t espi_read16(int reg)
{
	return read16((void *)(espi_get_bar() + reg));
}

static void espi_write16(int reg, uint16_t val)
{
	write16((void *)(espi_get_bar() + reg), val);
}

static uint8_t espi_read8(int reg)
{
	return read8((void *)(espi_get_bar() + reg));
}

static void espi_write8(int reg, uint8_t val)
{
	write8((void *)(espi_get_bar() + reg), val);
}

static void espi_enable_decode(int decode_en)
{
	uint32_t val;

	val = espi_read32(ESPI_DECODE);
	val |= decode_en;
	espi_write32(ESPI_DECODE, val);
}

static bool espi_is_decode_enabled(int decode)
{
	uint32_t val;

	val = espi_read32(ESPI_DECODE);
	return !!(val & decode);
}

static int espi_find_io_window(uint16_t win_base)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(ESPI_DECODE_IO_RANGE_EN(i)))
			continue;

		if (espi_read16(ESPI_IO_RANGE_BASE(i)) == win_base)
			return i;
	}

	return -1;
}

static int espi_get_unused_io_window(void)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(ESPI_DECODE_IO_RANGE_EN(i)))
			return i;
	}

	return -1;
}

/*
 * Returns decode enable bits for standard IO port addresses. If port address is not supported
 * by standard decode or if the size of window is not 1, then it returns -1.
 */
static int espi_std_io_decode(uint16_t base, size_t size)
{
	int ret = -1;

	if (size != 1)
		return ret;

	switch (base) {
	case 0x80:
		ret = ESPI_DECODE_IO_0x80_EN;
		break;
	case 0x60:
	case 0x64:
		ret = ESPI_DECODE_IO_0X60_0X64_EN;
		break;
	case 0x2e:
	case 0x2f:
		ret = ESPI_DECODE_IO_0X2E_0X2F_EN;
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

static size_t espi_get_io_window_size(int idx)
{
	return espi_read8(ESPI_IO_RANGE_SIZE(idx)) + 1;
}

static void espi_write_io_window(int idx, uint16_t base, size_t size)
{
	espi_write16(ESPI_IO_RANGE_BASE(idx), base);
	espi_write8(ESPI_IO_RANGE_SIZE(idx), size - 1);
}

static int espi_open_generic_io_window(uint16_t base, size_t size)
{
	size_t win_size;
	int idx;

	for (; size; size -= win_size, base += win_size) {
		win_size = MIN(size, ESPI_GENERIC_IO_MAX_WIN_SIZE);

		idx = espi_find_io_window(base);
		if (idx != -1) {
			size_t curr_size = espi_get_io_window_size(idx);

			if (curr_size > win_size) {
				printk(BIOS_INFO, "eSPI window already configured to be larger than requested! ");
				printk(BIOS_INFO, "Base: 0x%x, Requested size: 0x%zx, Actual size: 0x%zx\n",
				       base, win_size, curr_size);
			} else if (curr_size < win_size) {
				espi_write_io_window(idx, base, win_size);
				printk(BIOS_INFO, "eSPI window at base: 0x%x resized from 0x%zx to 0x%zx\n",
				       base, curr_size, win_size);
			}

			continue;
		}

		idx = espi_get_unused_io_window();
		if (idx == -1) {
			printk(BIOS_ERR, "Cannot open IO window base %x size %zx\n", base,
			       size);
			printk(BIOS_ERR, "ERROR: No more available IO windows!\n");
			return -1;
		}

		espi_write_io_window(idx, base, win_size);
		espi_enable_decode(ESPI_DECODE_IO_RANGE_EN(idx));
	}

	return 0;
}

int espi_open_io_window(uint16_t base, size_t size)
{
	int std_io;

	std_io = espi_std_io_decode(base, size);
	if (std_io != -1) {
		espi_enable_decode(std_io);
		return 0;
	}

	return espi_open_generic_io_window(base, size);
}

static int espi_find_mmio_window(uint32_t win_base)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_MMIO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(ESPI_DECODE_MMIO_RANGE_EN(i)))
			continue;

		if (espi_read32(ESPI_MMIO_RANGE_BASE(i)) == win_base)
			return i;
	}

	return -1;
}

static int espi_get_unused_mmio_window(void)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_MMIO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(ESPI_DECODE_MMIO_RANGE_EN(i)))
			return i;
	}

	return -1;

}

static size_t espi_get_mmio_window_size(int idx)
{
	return espi_read16(ESPI_MMIO_RANGE_SIZE(idx)) + 1;
}

static void espi_write_mmio_window(int idx, uint32_t base, size_t size)
{
	espi_write32(ESPI_MMIO_RANGE_BASE(idx), base);
	espi_write16(ESPI_MMIO_RANGE_SIZE(idx), size - 1);
}

int espi_open_mmio_window(uint32_t base, size_t size)
{
	size_t win_size;
	int idx;

	for (; size; size -= win_size, base += win_size) {
		win_size = MIN(size, ESPI_GENERIC_MMIO_MAX_WIN_SIZE);

		idx = espi_find_mmio_window(base);
		if (idx != -1) {
			size_t curr_size = espi_get_mmio_window_size(idx);

			if (curr_size > win_size) {
				printk(BIOS_INFO, "eSPI window already configured to be larger than requested! ");
				printk(BIOS_INFO, "Base: 0x%x, Requested size: 0x%zx, Actual size: 0x%zx\n",
				       base, win_size, curr_size);
			} else if (curr_size < win_size) {
				espi_write_mmio_window(idx, base, win_size);
				printk(BIOS_INFO, "eSPI window at base: 0x%x resized from 0x%zx to 0x%zx\n",
				       base, curr_size, win_size);
			}

			continue;
		}

		idx = espi_get_unused_mmio_window();
		if (idx == -1) {
			printk(BIOS_ERR, "Cannot open IO window base %x size %zx\n", base,
			       size);
			printk(BIOS_ERR, "ERROR: No more available MMIO windows!\n");
			return -1;
		}

		espi_write_mmio_window(idx, base, win_size);
		espi_enable_decode(ESPI_DECODE_MMIO_RANGE_EN(idx));
	}

	return 0;
}

static const struct espi_config *espi_get_config(void)
{
	const struct soc_amd_common_config *soc_cfg = soc_get_common_config();

	if (!soc_cfg)
		die("Common config structure is NULL!\n");

	return &soc_cfg->espi_config;
}

void espi_configure_decodes(void)
{
	int i;
	const struct espi_config *cfg = espi_get_config();

	espi_enable_decode(cfg->std_io_decode_bitmap);

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (cfg->generic_io_range[i].size == 0)
			continue;
		espi_open_generic_io_window(cfg->generic_io_range[i].base,
					    cfg->generic_io_range[i].size);
	}
}
