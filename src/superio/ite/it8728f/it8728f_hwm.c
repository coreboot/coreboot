/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include "chip.h"
#include "it8728f_internal.h"

/*
 * FAN controller configuration register index's
 */
#define HWM_MAIN_CTL_REG		 0x13	/* default 0x07 */
#define HWM_CTL_REG			 0x14	/* default 0x40 */
#define HWM_FAN1_CTL_PWM		 0x15	/* default 0x00 */
#define HWM_FAN2_CTL_PWM		 0x16	/* default 0x00 */
#define HWM_FAN3_CTL_PWM		 0x17	/* default 0x00 */
#define HWM_ADC_TEMP_CHAN_EN_REG 0x51	/* default 0x00 */

void it8728f_hwm_ec_init(struct device *dev)
{
	struct superio_ite_it8728f_config *conf = dev->chip_info;
	struct resource *res = find_resource(dev, PNP_IDX_IO0);

	if (!res) {
		printk(BIOS_WARNING, "Super I/O HWM: No HWM resource found.\n");
		return;
	}
	/* I/O port for HWM is at base + 5 */
	u16 port = res->base + 5;

	printk(BIOS_INFO,
		"ITE IT8728F Super I/O HWM: Initializing Hardware Monitor..\n");
	printk(BIOS_DEBUG,
		"ITE IT8728F Super I/O HWM: Base Address at 0x%x\n", port);

	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);

	/* ITE IT8728F HWM (ordered) programming sequence. */

	/* configure fan polarity */
	pnp_write_index(port, HWM_CTL_REG, conf->hwm_ctl_register);

	/* enable fans 1-3 */
	pnp_write_index(port, HWM_MAIN_CTL_REG, conf->hwm_main_ctl_register);

	/* enable termistor temps for temp1-temp3 */
	pnp_write_index(port, HWM_ADC_TEMP_CHAN_EN_REG, conf->hwm_adc_temp_chan_en_reg);

	/* configure which fanX uses which tempY */
	pnp_write_index(port, HWM_FAN1_CTL_PWM, conf->hwm_fan1_ctl_pwm);
	pnp_write_index(port, HWM_FAN2_CTL_PWM, conf->hwm_fan2_ctl_pwm);
	pnp_write_index(port, HWM_FAN3_CTL_PWM, conf->hwm_fan3_ctl_pwm);

	pnp_exit_conf_mode(dev);
}
