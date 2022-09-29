/* SPDX-License-Identifier: GPL-2.0-only */

#include "psp_verstage.h"

#include <amdblocks/acpimmio.h>
#include <amdblocks/espi.h>
#include <amdblocks/i2c.h>
#include <amdblocks/spi.h>
#include <arch/exception.h>
#include <arch/hlt.h>
#include <arch/io.h>
#include <bl_uapp/bl_errorcodes_public.h>
#include <bl_uapp/bl_syscall_public.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/i2c.h>
#include <soc/southbridge.h>
#include <stdint.h>

static void i2c3_set_bar(void *bar)
{
	i2c_set_bar(3, (uintptr_t)bar);
}

static void i2c2_set_bar(void *bar)
{
	i2c_set_bar(2, (uintptr_t)bar);
}

static void espi_set_bar(void *bar)
{
	espi_update_static_bar((uintptr_t)bar);
}

static void iomux_set_bar(void *bar)
{
	acpimmio_iomux = bar;
}

static void misc_set_bar(void *bar)
{
	acpimmio_misc = bar;
}

static void gpio_set_bar(void *bar)
{
	acpimmio_gpio0 = bar;
}

static uintptr_t io_bar;

static void io_set_bar(void *bar)
{
	io_bar = (uintptr_t)bar;
}

u8 io_read8(u16 reg)
{
	return read8p(io_bar + reg);
}

void io_write8(u16 reg, u8 value)
{
	write8p(io_bar + reg, value);
}

static void aoac_set_bar(void *bar)
{
	acpimmio_aoac = bar;
}

static struct {
	const char *name;
	struct {
		enum fch_io_device device;
		uint32_t arg0;
	} args;
	void (*set_bar)(void *bar);
	void *_bar;
} bar_map[] = {
	{"IOMUX", {FCH_IO_DEVICE_IOMUX}, iomux_set_bar},
	{"MISC", {FCH_IO_DEVICE_MISC}, misc_set_bar},
	{"GPIO", {FCH_IO_DEVICE_GPIO}, gpio_set_bar},
	{"IO", {FCH_IO_DEVICE_IOPORT}, io_set_bar},
	{"eSPI", {FCH_IO_DEVICE_ESPI}, espi_set_bar},
	{"I2C2", {FCH_IO_DEVICE_I2C, 2}, i2c2_set_bar},
	{"I2C3", {FCH_IO_DEVICE_I2C, 3}, i2c3_set_bar},
	{"SPI", {FCH_IO_DEVICE_SPI}, spi_set_base},
	{"AOAC", {FCH_IO_DEVICE_AOAC}, aoac_set_bar},
};

uintptr_t *map_spi_rom(void)
{
	uintptr_t *addr = NULL;
	struct spirom_info spi = {0};

	if (svc_get_spi_rom_info(&spi))
		printk(BIOS_DEBUG, "Error getting SPI ROM info.\n");

	if (spi.SpiBiosSmnBase != 0)
		if (svc_map_spi_rom(spi.SpiBiosSmnBase, CONFIG_ROM_SIZE, (void **)&addr))
			printk(BIOS_DEBUG, "Error mapping SPI ROM to address.\n");

	return addr;
}

static uint32_t map_fch_devices(void)
{
	void *bar;
	uint32_t err;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(bar_map); ++i) {
		err = svc_map_fch_dev(bar_map[i].args.device, bar_map[i].args.arg0, 0, &bar);
		if (err) {
			printk(BIOS_ERR, "Failed to map %s: %u\n", bar_map[i].name, err);
			return err;
		}

		bar_map[i]._bar = bar;
		bar_map[i].set_bar(bar);
	}

	return BL_OK;
}

uint32_t unmap_fch_devices(void)
{
	void *bar;
	uint32_t err, rtn = BL_OK;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(bar_map); ++i) {
		bar = bar_map[i]._bar;
		if (!bar)
			continue;

		err = svc_unmap_fch_dev(bar_map[i].args.device, bar);
		if (err) {
			printk(BIOS_ERR, "Failed to unmap %s: %u\n", bar_map[i].name, err);
			rtn = err;
		} else {
			bar_map[i]._bar = NULL;
			bar_map[i].set_bar(NULL);
		}
	}

	return rtn;
}

uint32_t verstage_soc_early_init(void)
{
	return map_fch_devices();
}

void verstage_soc_espi_init(void)
{
	if (!CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI))
		return;
	printk(BIOS_DEBUG, "Setting up espi\n");
	espi_setup();
}

void verstage_soc_i2c_init(void)
{
	printk(BIOS_DEBUG, "Setting up i2c\n");
	i2c_soc_early_init();
}

void verstage_soc_aoac_init(void)
{
	printk(BIOS_DEBUG, "Setting up aoac\n");
	enable_aoac_devices();
}

void verstage_soc_spi_init(void)
{
	printk(BIOS_DEBUG, "Setting up spi\n");
	fch_spi_config_modes();
	show_spi_speeds_and_modes();
}
