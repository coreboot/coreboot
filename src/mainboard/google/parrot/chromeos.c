/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/pci_ops.h>
#include <device/device.h>

#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <ec/compal/ene932/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "ec.h"

void fill_lb_gpios(struct lb_gpios *gpios)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);
	u16 gen_pmcon_1 = pci_s_read_config32(dev, GEN_PMCON_1);

	struct lb_gpio chromeos_gpios[] = {
		/* Lid switch GPIO active high (open). */
		{15, ACTIVE_HIGH, get_lid_switch(), "lid"},

		/* Power Button */
		{101, ACTIVE_LOW, (gen_pmcon_1 >> 9) & 1, "power"},

		/* Did we load the VGA Option ROM? */
		/* -1 indicates that this is a pseudo GPIO */
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_lid_switch(void)
{
	return get_gpio(15);
}

int get_write_protect_state(void)
{
	return !get_gpio(70);
}

int get_recovery_mode_switch(void)
{
	u8 gpio = !get_gpio(68);
	/* GPIO68, active low. For Servo support
	 * Treat as active high and let the caller invert if needed. */
	printk(BIOS_DEBUG, "REC MODE GPIO 68: %x\n", gpio);

	return gpio;
}

static int parrot_ec_running_ro(void)
{
	return !get_gpio(68);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AH(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AL(70, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	if (CONFIG(CHROMEOS_NVS) && !parrot_ec_running_ro())
		chromeos_set_ecfw_rw();

	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
