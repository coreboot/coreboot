/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include "gpio.h"

/* FIXME: Example code below */

static void mb_configure_dp1_pwr(bool enable)
{
	gpio_output(GPP_K3, enable);
}

static void mb_configure_dp2_pwr(bool enable)
{
	gpio_output(GPP_K4, enable);
}

static void mb_configure_dp3_pwr(bool enable)
{
	gpio_output(GPP_K5, enable);
}

static void mb_pcie_reset_pch_slots(bool enable)
{
	gpio_output(GPP_K0, enable);
}

static void mb_pcie_reset_cpu_slots(bool enable)
{
	gpio_output(GPP_K1, enable);
}

static void mb_pcie_reset_cnvi_slot(bool enable)
{
	gpio_output(GPP_K2, enable);
}

static void mb_hda_amp_enable(bool enable)
{
	gpio_output(GPP_C19, enable);
}

static void mb_usb31_rp1_pwr_enable(bool enable)
{
	gpio_output(GPP_G0, enable);
}

static void mb_usb31_rp2_pwr_enable(bool enable)
{
	gpio_output(GPP_G1, enable);
}

static void mb_usb31_fp_pwr_enable(bool enable)
{
	gpio_output(GPP_G2, enable);
}

static void mb_usb2_fp1_pwr_enable(bool enable)
{
	gpio_output(GPP_G3, enable);
}

static void mb_usb2_fp2_pwr_enable(bool enable)
{
	gpio_output(GPP_G4, enable);
}

static void mainboard_enable(struct device *dev)
{
	/* FIXME: Do runtime configuration once the board is production ready */
	mb_configure_dp1_pwr(1);
	mb_configure_dp2_pwr(1);
	mb_configure_dp3_pwr(1);
	if (0) {
		mb_pcie_reset_pch_slots(1);
		mb_pcie_reset_cpu_slots(0);
		mb_pcie_reset_cnvi_slot(0);
	}
	mb_hda_amp_enable(1);
	mb_usb31_rp1_pwr_enable(1);
	mb_usb31_rp2_pwr_enable(1);
	mb_usb31_fp_pwr_enable(1);
	mb_usb2_fp1_pwr_enable(1);
	mb_usb2_fp2_pwr_enable(1);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
