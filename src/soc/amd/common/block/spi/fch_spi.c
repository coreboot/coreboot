/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/chip.h>
#include <amdblocks/lpc.h>
#include <amdblocks/psp_efs.h>
#include <amdblocks/spi.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <stdint.h>

static const char *spi_speed_str[8] = {
	"66.66 Mhz",
	"33.33 MHz",
	"22.22 MHz",
	"16.66 MHz",
	"100 MHz",
	"800 KHz",
	"Invalid",
	"Invalid"
};

static const char *read_mode_str[8] = {
	"Normal Read (up to 33M)",
	"Reserved",
	"Dual IO (1-1-2)",
	"Quad IO (1-1-4)",
	"Dual IO (1-2-2)",
	"Quad IO (1-4-4)",
	"Normal Read (up to 66M)",
	"Fast Read"
};

void show_spi_speeds_and_modes(void)
{
	uint16_t val16 = spi_read16(SPI100_SPEED_CONFIG);
	uint32_t val32 = spi_read32(SPI_CNTRL0);

	printk(BIOS_DEBUG, "SPI normal read speed: %s\n",
	       spi_speed_str[DECODE_SPI_NORMAL_SPEED(val16)]);
	printk(BIOS_DEBUG, "SPI fast read speed: %s\n",
	       spi_speed_str[DECODE_SPI_FAST_SPEED(val16)]);
	printk(BIOS_DEBUG, "SPI alt read speed: %s\n",
	       spi_speed_str[DECODE_SPI_ALT_SPEED(val16)]);
	printk(BIOS_DEBUG, "SPI TPM read speed: %s\n",
	       spi_speed_str[DECODE_SPI_TPM_SPEED(val16)]);
	printk(BIOS_DEBUG, "SPI100: %s\n",
	       spi_read16(SPI100_ENABLE) & SPI_USE_SPI100 ? "Enabled" : "Disabled");
	printk(BIOS_DEBUG, "SPI Read Mode: %s\n", read_mode_str[DECODE_SPI_READ_MODE(val32)]);
}

void __weak mainboard_spi_cfg_override(uint8_t *fast_speed, uint8_t *read_mode)
{
	/* No overriding SPI speeds. */
}

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
	spi_write16(SPI100_ENABLE, SPI_USE_SPI100 | spi_read16(SPI100_ENABLE));
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

void fch_spi_config_modes(void)
{
	uint8_t read_mode, fast_speed;
	uint8_t normal_speed = CONFIG_NORMAL_READ_SPI_SPEED;
	uint8_t alt_speed = CONFIG_ALT_SPI_SPEED;
	uint8_t tpm_speed = CONFIG_TPM_SPI_SPEED;

	if (!read_efs_spi_settings(&read_mode, &fast_speed)) {
		read_mode = CONFIG_EFS_SPI_READ_MODE;
		fast_speed = CONFIG_EFS_SPI_SPEED;
	}
	mainboard_spi_cfg_override(&fast_speed, &read_mode);

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
