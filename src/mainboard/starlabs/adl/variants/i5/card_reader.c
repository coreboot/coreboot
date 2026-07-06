/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <device/device.h>
#include <device/i2c_bus.h>
#include <option.h>
#include <static.h>

static bool has_dedicated_card_reader(void)
{
	struct device *mxc_accel = DEV_PTR(mxc6655);

	return i2c_dev_detect(i2c_busdev(mxc_accel), mxc_accel->path.i2c.device);
}

static void update_card_reader_acpi(void *unused)
{
	(void)unused;

	if (get_uint_option("card_reader", 0) == 0) {
		DEV_PTR(hub_card_reader)->enabled = 0;
		DEV_PTR(usb2_port4)->enabled = 0;
		return;
	}

	if (has_dedicated_card_reader())
		DEV_PTR(hub_card_reader)->enabled = 0;
	else
		DEV_PTR(usb2_port4)->enabled = 0;
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, update_card_reader_acpi, NULL);
