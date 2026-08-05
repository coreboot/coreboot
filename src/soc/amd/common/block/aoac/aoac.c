/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/aoac.h>
#include <delay.h>

/* This initiates the power on sequence, but doesn't wait for the device to be powered on. */
void power_on_aoac_device(unsigned int dev)
{
	uint8_t byte = aoac_read8(AOAC_DEV_D3_CTL(dev));
	byte |= FCH_AOAC_PWR_ON_DEV;
	byte &= ~FCH_AOAC_TARGET_DEVICE_STATE;
	byte |= FCH_AOAC_D0_INITIALIZED;
	aoac_write8(AOAC_DEV_D3_CTL(dev), byte);
}

void power_off_aoac_device(unsigned int dev)
{
	uint8_t byte = aoac_read8(AOAC_DEV_D3_CTL(dev));
	byte &= ~FCH_AOAC_PWR_ON_DEV;
	aoac_write8(AOAC_DEV_D3_CTL(dev), byte);
}

bool is_aoac_device_enabled(unsigned int dev)
{
	uint8_t byte = aoac_read8(AOAC_DEV_D3_STATE(dev));
	byte &= (FCH_AOAC_PWR_RST_STATE | FCH_AOAC_REF_CLK_OK_STATE);
	if (byte == (FCH_AOAC_PWR_RST_STATE | FCH_AOAC_REF_CLK_OK_STATE))
		return true;
	else
		return false;
}

void wait_for_aoac_enabled(unsigned int dev)
{
	while (!is_aoac_device_enabled(dev))
		udelay(100);
}

void enable_aoac_devices(void)
{
	size_t i, num;
	const unsigned int *devs = soc_get_aoac_devices(&num);

	for (i = 0; i < num; i++)
		power_on_aoac_device(devs[i]);

	/* Wait for AOAC devices to indicate power and clock OK */
	for (i = 0; i < num; i++)
		wait_for_aoac_enabled(devs[i]);
}
