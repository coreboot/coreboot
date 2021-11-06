/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define GPIO_SPI_WP	24
#define GPIO_REC_MODE	42

#define FLAG_SPI_WP	0
#define FLAG_REC_MODE	1

#include "ec.h"
#include <ec/smsc/mec1308/ec.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);
	u16 gen_pmcon_1 = pci_s_read_config32(dev, GEN_PMCON_1);
	u8 lid = ec_read(0x83);

	struct lb_gpio chromeos_gpios[] = {
		/* Recovery: GPIO42 = CHP3_REC_MODE# */
		{GPIO_REC_MODE, ACTIVE_LOW, !get_recovery_mode_switch(),
		 "presence"},

		{100, ACTIVE_HIGH, lid & 1, "lid"},

		/* Power Button */
		{101, ACTIVE_LOW, (gen_pmcon_1 >> 9) & 1, "power"},

		/* Did we load the VGA Option ROM? */
		/* -1 indicates that this is a pseudo GPIO */
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
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

	/* Write Protect: GPIO24 = KBC3_SPI_WP#, active high */
	if (get_gpio(GPIO_SPI_WP))
		flags |= (1 << FLAG_SPI_WP);
	/* Recovery: GPIO42 = CHP3_REC_MODE#, active low */
	if (!get_gpio(GPIO_REC_MODE))
		flags |= (1 << FLAG_REC_MODE);

	pci_s_write_config32(dev, SATA_SP, flags);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(GPIO_REC_MODE, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_SPI_WP, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	if (CONFIG(CHROMEOS_NVS) && ec_read(0xcb))
		chromeos_set_ecfw_rw();

	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
