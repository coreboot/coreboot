/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/aoac.h>

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
	byte &= (FCH_AOAC_PWR_RST_STATE | FCH_AOAC_RST_CLK_OK_STATE);
	if (byte == (FCH_AOAC_PWR_RST_STATE | FCH_AOAC_RST_CLK_OK_STATE))
		return true;
	else
		return false;
}
