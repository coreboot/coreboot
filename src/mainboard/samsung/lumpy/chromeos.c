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

#include "ec.h"
#include <ec/smsc/mec1308/ec.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		/* Recovery: GPIO42 = CHP3_REC_MODE# */
		{GPIO_REC_MODE, ACTIVE_LOW, !get_recovery_mode_switch(),
		 "presence"},

		{100, ACTIVE_HIGH, get_lid_switch(), "lid"},

		/* Power Button */
		{101, ACTIVE_LOW, get_power_switch(), "power"},

		/* Did we load the VGA Option ROM? */
		/* -1 indicates that this is a pseudo GPIO */
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return get_gpio(GPIO_SPI_WP);
}

int get_recovery_mode_switch(void)
{
	return !get_gpio(GPIO_REC_MODE);
}

int get_lid_switch(void)
{
	return ec_read(0x83) & 1;
}

int get_power_switch(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);
	u16 gen_pmcon_1 = pci_s_read_config32(dev, GEN_PMCON_1);
	return (gen_pmcon_1 >> 9) & 1;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(GPIO_REC_MODE, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_SPI_WP, CROS_GPIO_DEVICE_NAME),
};

DECLARE_CROS_GPIOS(cros_gpios);
