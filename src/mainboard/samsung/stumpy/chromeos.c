/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "onboard.h"

#define FLAG_SPI_WP	0
#define FLAG_REC_MODE	1

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		/* Recovery: GPIO42 = CHP3_REC_MODE# */
		{GPIO_REC_MODE, ACTIVE_LOW, !get_recovery_mode_switch(),
		 "presence"},

		/* Hard code the lid switch GPIO to open. */
		{100, ACTIVE_HIGH, 1, "lid"},

		/* Power Button */
		{101, ACTIVE_LOW, get_power_switch(), "power"},

		/* Did we load the VGA Option ROM? */
		/* -1 indicates that this is a pseudo GPIO */
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

static bool raw_write_protect_state(void)
{
	return get_gpio(GPIO_SPI_WP);
}

static bool raw_recovery_mode_switch(void)
{
	return !get_gpio(GPIO_REC_MODE);
}

int get_power_switch(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);
	u16 gen_pmcon_1 = pci_s_read_config32(dev, GEN_PMCON_1);
	return (gen_pmcon_1 >> 9) & 1;
}

int get_write_protect_state(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 2);
	return (pci_s_read_config32(dev, SATA_SP) >> FLAG_SPI_WP) & 1;
}

int get_recovery_mode_switch(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 2);
	return (pci_s_read_config32(dev, SATA_SP) >> FLAG_REC_MODE) & 1;
}

void init_bootmode_straps(void)
{
	u32 flags = 0;
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 2);

	/* Write Protect: GPIO68 = CHP3_SPI_WP, active high */
	if (raw_write_protect_state())
		flags |= (1 << FLAG_SPI_WP);

	/* Recovery: GPIO42 = CHP3_REC_MODE#, active low */
	if (raw_recovery_mode_switch())
		flags |= (1 << FLAG_REC_MODE);

	pci_s_write_config32(dev, SATA_SP, flags);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(GPIO_REC_MODE, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_SPI_WP, CROS_GPIO_DEVICE_NAME),
};

const struct cros_gpio *variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
