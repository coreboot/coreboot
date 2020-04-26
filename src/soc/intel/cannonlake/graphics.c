/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <fsp/util.h>
#include <device/device.h>
#include <drivers/intel/gma/i915_reg.h>
#include <intelblocks/graphics.h>
#include <types.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	return graphics_get_memory_base();
}

void graphics_soc_init(struct device *dev)
{
	uint32_t ddi_buf_ctl;

	/*
	 * Enable DDI-A (eDP) 4-lane operation if the link is not up yet.
	 * This will allow the kernel to use 4-lane eDP links properly
	 * if the VBIOS or GOP driver do not execute.
	 */
	ddi_buf_ctl = graphics_gtt_read(DDI_BUF_CTL_A);
	if (!acpi_is_wakeup_s3() && !(ddi_buf_ctl & DDI_BUF_CTL_ENABLE)) {
		ddi_buf_ctl |= (DDI_A_4_LANES | DDI_INIT_DISPLAY_DETECTED |
				DDI_BUF_IS_IDLE);
		graphics_gtt_write(DDI_BUF_CTL_A, ddi_buf_ctl);
	}
}
