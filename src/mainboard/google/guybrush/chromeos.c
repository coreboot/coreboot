/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/spi.h>
#include <baseboard/gpio.h>
#include <boardid.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{GPIO_EC_IN_RW, ACTIVE_HIGH, gpio_get(GPIO_EC_IN_RW), "EC in RW"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AL(CROS_WP_GPIO, GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}

void mainboard_spi_fast_speed_override(uint8_t *fast_speed)
{
	uint32_t board_ver = board_id();

	if (board_ver >= CONFIG_OVERRIDE_EFS_SPI_SPEED_MIN_BOARD)
		*fast_speed = CONFIG_OVERRIDE_EFS_SPI_SPEED;
}
