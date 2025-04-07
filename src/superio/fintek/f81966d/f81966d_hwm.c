/* SPDX-License-Identifier: GPL-2.0-or-later */

/* Setup only for Fan2
 * TODO: Add support for Fan1 and Fan3
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/hwm5_conf.h>
#include "fintek_internal.h"
#include "chip.h"
#include "f81966d.h"

void f81966d_hwm_init(struct device *dev)
{
	struct resource *res = probe_resource(dev, PNP_IDX_IO0);

	if (!res) {
		printk(BIOS_WARNING, "Super I/O HWM: No HWM resource found.\n");
		return;
	}

	const struct superio_fintek_f81966d_config *reg = dev->chip_info;
	u16 port = res->base;

	/* Use AMD TSI */
	pnp_write_hwm5_index(port, HWM_AMD_TSI_ADDR, reg->hwm_amd_tsi_addr);
	pnp_write_hwm5_index(port, HWM_AMD_TSI_CONTROL_REG, reg->hwm_amd_tsi_control);

	/* Set temp1 sensor to thermistor */
	pnp_write_hwm5_index(port, TEMP_SENS_TYPE_REG, reg->hwm_temp_sens_type);

	/* Select FAN Type */
	pnp_write_hwm5_index(port, HWM_FAN_SEL, reg->hwm_fan_select);

	/* Select FAN Mode*/
	pnp_write_hwm5_index(port, HWM_FAN_MODE, reg->hwm_fan_mode);

	/* Set Boundaries */
	pnp_write_hwm5_index(port, HWM_FAN2_BOUND1, reg->hwm_fan2_bound1);
	pnp_write_hwm5_index(port, HWM_FAN2_BOUND2, reg->hwm_fan2_bound2);
	pnp_write_hwm5_index(port, HWM_FAN2_BOUND3, reg->hwm_fan2_bound3);
	pnp_write_hwm5_index(port, HWM_FAN2_BOUND4, reg->hwm_fan2_bound4);

	/* Set Speed */
	pnp_write_hwm5_index(port, HWM_FAN2_SEG1_SPEED_COUNT, reg->hwm_fan2_seg1_speed);
	pnp_write_hwm5_index(port, HWM_FAN2_SEG2_SPEED_COUNT, reg->hwm_fan2_seg2_speed);
	pnp_write_hwm5_index(port, HWM_FAN2_SEG3_SPEED_COUNT, reg->hwm_fan2_seg3_speed);
	pnp_write_hwm5_index(port, HWM_FAN2_SEG4_SPEED_COUNT, reg->hwm_fan2_seg4_speed);
	pnp_write_hwm5_index(port, HWM_FAN2_SEG5_SPEED_COUNT, reg->hwm_fan2_seg5_speed);

	/* Set Fan control freq */
	pnp_write_hwm5_index(port, HWM_FAN3_CONTROL, reg->hwm_fan3_control);
	pnp_write_hwm5_index(port, HWM_FAN2_TEMP_MAP_SEL, reg->hwm_fan2_temp_map_select);
}
