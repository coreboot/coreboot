/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include "chip.h"
#include "fintek_internal.h"

/*
 * The Fintek F71869AD Super I/O Hardware Monitor permits the configuration of
 * three fans individually, where fan1 is typically taken as the CPU fan. Each
 * fan is controlled by the relation:
 *
 * Tfan? = Tnow + (Ta - Tb)*Ct
 *
 * Parameters in this relation are specified in the devicetree.cb.
 */

/*
 * Register CR01 ~ CR03 -> Configuration Registers
 * Register CR0A ~ CR0F -> PECI/TSI Control Register
 * Register CR10 ~ CR37 -> Voltage Setting Register
 * Register CR40 ~ CR4E -> PECI 3.0 Command and Register
 * Register CR60 ~ CR8E -> Temperature Setting Register
 * Register CR90 ~ CRDF -> Fan Control Setting Register
 */
#define HWM_SMBUS_ADDR             0x08
#define HWM_SMBUS_CONTROL_REG      0x0A
#define HWM_FAN_TYPE_SEL_REG       0x94
#define HWM_FAN1_TEMP_ADJ_RATE_REG 0x95
#define HWM_FAN_MODE_SEL_REG       0x96
#define HWM_FAN_FAULT_TIME_REG     0x9F /* bit7 FAN_PROG_SEL */
#define HWM_FAN1_IDX_RPM_MODE      0xA3
#define HWM_FAN1_SEG1_SPEED_COUNT  0xAA
#define HWM_FAN1_SEG2_SPEED_COUNT  0xAB
#define HWM_FAN1_SEG3_SPEED_COUNT  0xAC
#define HWM_FAN1_TEMP_MAP_SEL      0xAF
#define HWM_TEMP_SENSOR_TYPE       0x6B

/* note: multifunc registers need to be tweaked before here */
void f71869ad_hwm_init(struct device *dev)
{
	const struct superio_fintek_f71869ad_config *conf = dev->chip_info;
	struct resource *res = probe_resource(dev, PNP_IDX_IO0);

	if (!res) {
		printk(BIOS_WARNING, "Super I/O HWM: No HWM resource found.\n");
		return;
	}
	u16 port = res->base; /* data-sheet default base = 0x229 */

	printk(BIOS_INFO,
		"Fintek F71869AD Super I/O HWM: Initializing Hardware Monitor..\n");
	printk(BIOS_DEBUG,
		"Fintek F71869AD Super I/O HWM: Base Address at 0x%x\n", port);

	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);

	/* Fintek F71869AD HWM (ordered) programming sequence. */

	/* SMBus Address p.53 */
	pnp_write_index(port, HWM_SMBUS_ADDR, conf->hwm_smbus_address);
	/* Configure pins 57/58 as PECI_REQ#/PECI (AMD_TSI) p.54 */
	pnp_write_index(port, HWM_SMBUS_CONTROL_REG, conf->hwm_smbus_control_reg);
	/* Tfan1 = Tnow + (Ta - Tb)*Ct where, */
	/* FAN1_TEMP_SEL_DIG, FAN1_TEMP_SEL (Tnow) set to come from CR7Ah p.73 */
	pnp_write_index(port, HWM_FAN1_TEMP_MAP_SEL, conf->hwm_fan1_temp_map_sel);
	/* set FAN_PROG_SEL = 1 */
	pnp_write_index(port, HWM_FAN_FAULT_TIME_REG, 0x8a);
	/* FAN1_BASE_TEMP (Tb) set when FAN_PROG_SEL = 1, p.64-65 */
	pnp_write_index(port, HWM_FAN_TYPE_SEL_REG, conf->hwm_fan_type_sel_reg);
	/* set TFAN1_ADJ_SEL (Ta) p.67 to use CR7Ah p.61 */
	pnp_write_index(port, HWM_FAN_MODE_SEL_REG, conf->hwm_fan_mode_sel_reg);
	/* TFAN1_ADJ_{UP,DOWN}_RATE (Ct = 1/4 up & down) in 0x95 when FAN_PROG_SEL =
		1, p.88 */
	pnp_write_index(port, HWM_FAN1_TEMP_ADJ_RATE_REG, conf->hwm_fan1_temp_adj_rate_reg);
	/* set FAN_PROG_SEL = 0 */
	pnp_write_index(port, HWM_FAN_FAULT_TIME_REG, 0x0a);
	/* FAN1 RPM mode p.70 */
	pnp_write_index(port, HWM_FAN1_IDX_RPM_MODE, conf->hwm_fan1_idx_rpm_mode);
	/* FAN1 Segment X Speed Count */
	pnp_write_index(port, HWM_FAN1_SEG1_SPEED_COUNT, conf->hwm_fan1_seg1_speed_count);
	pnp_write_index(port, HWM_FAN1_SEG2_SPEED_COUNT, conf->hwm_fan1_seg2_speed_count);
	pnp_write_index(port, HWM_FAN1_SEG3_SPEED_COUNT, conf->hwm_fan1_seg3_speed_count);
	/* Temperature sensor type */
	pnp_write_index(port, HWM_TEMP_SENSOR_TYPE, conf->hwm_temp_sensor_type);

	pnp_exit_conf_mode(dev);
}
