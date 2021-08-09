/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/chip.h>
#include <amdblocks/lpc.h>
#include <amdblocks/psp_efs.h>
#include <amdblocks/spi.h>
#include <arch/mmio.h>
#include <console/console.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <stdint.h>

static uint8_t lower_speed(uint8_t speed1, uint8_t speed2)
{
	uint8_t speeds[] = {SPI_SPEED_800K, SPI_SPEED_16M, SPI_SPEED_22M,
			    SPI_SPEED_33M,  SPI_SPEED_66M, SPI_SPEED_100M};

	for (int i = 0; i < ARRAY_SIZE(speeds); i++) {
		if (speed1 == speeds[i])
			return speed1;
		if (speed2 == speeds[i])
			return speed2;
	}

	/* Fall back to 16MHz if we got invalid speed values */
	return SPI_SPEED_16M;
}

static void fch_spi_set_spi100(uint8_t norm, uint8_t fast, uint8_t alt, uint8_t tpm)
{
	spi_write16(SPI100_SPEED_CONFIG, SPI_SPEED_CFG(norm, fast, alt, tpm));
	spi_write16(SPI100_ENABLE, SPI_USE_SPI100);
}

static void fch_spi_configure_4dw_burst(void)
{
	uint16_t val = spi_read16(SPI100_HOST_PREF_CONFIG);

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_4DW_BURST))
		val |= SPI_RD4DW_EN_HOST;
	else
		val &= ~SPI_RD4DW_EN_HOST;

	spi_write16(SPI100_HOST_PREF_CONFIG, val);
}

static void fch_spi_set_read_mode(u32 mode)
{
	uint32_t val = spi_read32(SPI_CNTRL0) & ~SPI_READ_MODE_MASK;

	spi_write32(SPI_CNTRL0, val | SPI_READ_MODE(mode));
}

static void fch_spi_config_modes(void)
{
	uint8_t read_mode, fast_speed;
	uint8_t normal_speed = CONFIG_NORMAL_READ_SPI_SPEED;
	uint8_t alt_speed = CONFIG_ALT_SPI_SPEED;
	uint8_t tpm_speed = CONFIG_TPM_SPI_SPEED;

	if (!read_efs_spi_settings(&read_mode, &fast_speed)) {
		read_mode = CONFIG_EFS_SPI_READ_MODE;
		fast_speed = CONFIG_EFS_SPI_SPEED;
	}

	if (fast_speed != CONFIG_EFS_SPI_SPEED) {
		normal_speed = lower_speed(normal_speed, fast_speed);
		tpm_speed = lower_speed(tpm_speed, fast_speed);
		alt_speed = lower_speed(alt_speed, fast_speed);
	}

	fch_spi_set_read_mode((u32)read_mode);
	fch_spi_set_spi100(normal_speed, fast_speed, alt_speed, tpm_speed);
}

void fch_spi_early_init(void)
{
	lpc_enable_spi_rom(SPI_ROM_ENABLE);
	lpc_enable_spi_prefetch();
	fch_spi_configure_4dw_burst();
	fch_spi_config_modes();
}
