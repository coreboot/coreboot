/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <boot_device.h>
#include <cpu/x86/smm.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <stdint.h>
#include <smm_call.h>

static struct spi_flash sfg;
static bool sfg_init_done;

__weak int boot_device_spi_cs(void)
{
	return 0;	/* Default to chip select 0 */
}

/* SPI API is used in early stages and as long as Rom Armor isn't active */
static ssize_t spi_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	if (spi_flash_read(&sfg, offset, size, b))
		return -1;

	return size;
}

static ssize_t spi_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	if (spi_flash_write(&sfg, offset, size, b))
		return -1;

	return size;
}

static ssize_t spi_eraseat(const struct region_device *rd,
				size_t offset, size_t size)
{
	if (spi_flash_erase(&sfg, offset, size))
		return -1;

	return size;
}

static const struct region_device_ops spi_ops = {
	.readat = spi_readat,
	.writeat = spi_writeat,
	.eraseat = spi_eraseat,
};

static const struct region_device spi_rw =
	REGION_DEV_INIT(&spi_ops, 0, CONFIG_ROM_SIZE);

static void boot_device_rw_init(void)
{
	const int bus = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS;
	const int cs = boot_device_spi_cs();

	if (sfg_init_done == true)
		return;

	/* Ensure any necessary setup is performed by the drivers. */
	spi_init();

	if (!spi_flash_probe(bus, cs, &sfg))
		sfg_init_done = true;
}

const struct region_device *boot_device_rw(void)
{
	if (ENV_SMM) {
		/* Could return SPI drivers here, but that would increase SMM size.
		 * ROM Armor is enforced right after SMM has been set up, so it's
		 * unlikely that something need R/W access to SPI flash before it
		 * is enforced.
		 */
		if (!psp_get_hsti_state_rom_armor_enforced())
			return NULL;

		return &rom_armor_smm_rw;
	} else if (ENV_RAMSTAGE) {
		/* Probe for the SPI flash device if not already done. */
		if (!psp_get_hsti_state_rom_armor_enforced()) {
			/* ROM Armor not active, can use SPI controller directly */
			boot_device_rw_init();

			if (sfg_init_done != true)
				return NULL;

			return &spi_rw;
		}
		/* ROM Armor active, use APM interface */
		return &rom_armor_apm_call_rw;
	} else {
		/* ROM Armor not active, can use SPI controller directly */
		boot_device_rw_init();

		if (sfg_init_done != true)
			return NULL;

		return &spi_rw;
	}
}

const struct spi_flash *boot_device_spi_flash(void)
{
	if (!psp_get_hsti_state_rom_armor_enforced())
		boot_device_rw_init();

	if (sfg_init_done != true)
		return NULL;

	return &sfg;
}

int boot_device_wp_region(const struct region_device *rd,
			  const enum bootdev_prot_type type)
{
	uint32_t ctrlr_pr;

	if (psp_get_hsti_state_rom_armor_enforced()) {
		printk(BIOS_ERR, "%s: ROM Armor is active. Cannot access SPI flash\n", __func__);
		return -1;
	}

	/* Ensure boot device has been initialized at least once. */
	boot_device_init();

	const struct spi_flash *boot_dev = boot_device_spi_flash();

	if (boot_dev == NULL)
		return -1;

	if (type == MEDIA_WP) {
		if (spi_flash_is_write_protected(boot_dev,
						 region_device_region(rd)) != 1) {
			enum spi_flash_status_reg_lockdown lock =
				SPI_WRITE_PROTECTION_REBOOT;
			if (CONFIG(BOOTMEDIA_SPI_LOCK_REBOOT))
				lock = SPI_WRITE_PROTECTION_REBOOT;
			else if (CONFIG(BOOTMEDIA_SPI_LOCK_PIN))
				lock = SPI_WRITE_PROTECTION_PIN;
			else if (CONFIG(BOOTMEDIA_SPI_LOCK_PERMANENT))
				lock = SPI_WRITE_PROTECTION_PERMANENT;

			return spi_flash_set_write_protected(boot_dev,
						region_device_region(rd), lock);
		}

		/* Already write protected */
		return 0;
	}

	switch (type) {
	case CTRLR_WP:
		ctrlr_pr = WRITE_PROTECT;
		break;
	case CTRLR_RP:
		ctrlr_pr = READ_PROTECT;
		break;
	case CTRLR_RWP:
		ctrlr_pr = READ_WRITE_PROTECT;
		break;
	default:
		return -1;
	}

	return spi_flash_ctrlr_protect_region(boot_dev,
					region_device_region(rd), ctrlr_pr);
}
