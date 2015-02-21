/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <device/device.h>
#include "w83793.h"
#include <device/smbus.h>
#include "chip.h"

static int w83793_fan_limit(struct device *dev, int fan, uint16_t limit)
{
	return smbus_write_byte(dev, 0x90 + fan * 2, limit >> 8) ||
		smbus_write_byte(dev, 0x91 + fan * 2, limit & 0xff);
}

static int w83793_bank(struct device *dev, int bank)
{
	return smbus_write_byte(dev, 0, bank);
}

static int w83793_td_level(struct device *dev, int fan, const char *level)
{
	fan *= 0x10;

	smbus_write_byte(dev, 0x30 + fan, level[0]);
	smbus_write_byte(dev, 0x31 + fan, level[1]);
	smbus_write_byte(dev, 0x32 + fan, level[2]);
	smbus_write_byte(dev, 0x33 + fan, level[3]);
	smbus_write_byte(dev, 0x34 + fan, level[4]);
	smbus_write_byte(dev, 0x35 + fan, level[5]);
	smbus_write_byte(dev, 0x36 + fan, level[6]);
	return 0;
}

static int w83793_tr_level(struct device *dev, int fan, const char *level)
{
	fan *= 0x10;

	smbus_write_byte(dev, 0x70 + fan, level[0]);
	smbus_write_byte(dev, 0x71 + fan, level[1]);
	smbus_write_byte(dev, 0x72 + fan, level[2]);
	smbus_write_byte(dev, 0x73 + fan, level[3]);
	smbus_write_byte(dev, 0x74 + fan, level[4]);
	smbus_write_byte(dev, 0x75 + fan, level[5]);
	smbus_write_byte(dev, 0x76 + fan, level[6]);
	return 0;
}


static int w83793_td_fan_level(struct device *dev, int fan, const char *level)
{
	fan *= 0x10;

	smbus_write_byte(dev, 0x38 + fan, level[0]);
	smbus_write_byte(dev, 0x39 + fan, level[1]);
	smbus_write_byte(dev, 0x3a + fan, level[2]);
	smbus_write_byte(dev, 0x3b + fan, level[3]);
	smbus_write_byte(dev, 0x3c + fan, level[4]);
	smbus_write_byte(dev, 0x3d + fan, level[5]);
	smbus_write_byte(dev, 0x3e + fan, level[6]);
	return 0;
}

static int w83793_tr_fan_level(struct device *dev, int fan, const char *level)
{
	fan *= 0x10;

	smbus_write_byte(dev, 0x78 + fan, level[0]);
	smbus_write_byte(dev, 0x79 + fan, level[1]);
	smbus_write_byte(dev, 0x7a + fan, level[2]);
	smbus_write_byte(dev, 0x7b + fan, level[3]);
	smbus_write_byte(dev, 0x7c + fan, level[4]);
	smbus_write_byte(dev, 0x7d + fan, level[5]);
	smbus_write_byte(dev, 0x7e + fan, level[6]);
	return 0;
}

static uint8_t millivolts_to_limit_value_type1(int millivolts)
{
	/* Datasheet v1.4 page 64 (VCoreA, VCoreB, Vtt Limit) */
	return ((millivolts * 125) / 1000);
}

static uint8_t millivolts_to_limit_value_type2(int millivolts)
{
	/* Datasheet v1.4 page 64 (VSEN1, VEN2, VSEN3) */
	return ((millivolts * 625) / 10000);
}

static uint8_t millivolts_to_limit_value_type3(int millivolts)
{
	/* Datasheet v1.4 page 64 (5VDD, 5VSB) */
	return ((((millivolts * 10) + 1500) * 417) / 10000);
}

static uint8_t fan_pct_to_cfg_val(uint8_t percent)
{
	uint8_t cfg = (((unsigned int)percent * 10000) / 15873);
	if (cfg > 0x3f)
		cfg = 0x3f;
	return cfg;
}

static void w83793_init(struct device *dev)
{
	struct drivers_i2c_w83793_config *config = dev->chip_info;
	uint16_t id;
	int i;

	if (w83793_bank(dev, 0))
		printk(BIOS_ERR, "%s: failed\n", __func__);

	if (!config)
		return;

	id = smbus_read_byte(dev, 0x0d);
	w83793_bank(dev, 0x80);
	id |= smbus_read_byte(dev, 0x0d) << 8;
	printk(BIOS_ERR, "ID: %04x\n", id);

	/* reset configuration */
	smbus_write_byte(dev, 0x40, 0x80);
	smbus_write_byte(dev, 0x51, 0x06);

	/* Multi function control */
	smbus_write_byte(dev, 0x58, config->mfc);

	/* FANIN_Ctrl */
	smbus_write_byte(dev, 0x5c, config->fanin);

	/* FANIN_Sel */
	smbus_write_byte(dev, 0x5d, config->fanin_sel);

	/* Temperature source */
	smbus_write_byte(dev, 0x5e, config->td_mode_select);
	/* TR monitor enable */
	smbus_write_byte(dev, 0x5f, config->tr_enable);
	/* PECI Agent configuration */
	smbus_write_byte(dev, 0xd0, config->peci_agent_conf);
	/* TCase */
	smbus_write_byte(dev, 0xd1, config->tcase0);
	smbus_write_byte(dev, 0xd2, config->tcase1);
	smbus_write_byte(dev, 0xd3, config->tcase2);
	smbus_write_byte(dev, 0xd4, config->tcase3);
	/* PECI Reportstyle */
	smbus_write_byte(dev, 0xd5, 0x00);

	/* Voltage high/low limits */
	smbus_write_byte(dev, 0x60, millivolts_to_limit_value_type1(config->vcorea_high_limit_mv));
	smbus_write_byte(dev, 0x61, millivolts_to_limit_value_type1(config->vcorea_low_limit_mv));
	smbus_write_byte(dev, 0x62, millivolts_to_limit_value_type1(config->vcoreb_high_limit_mv));
	smbus_write_byte(dev, 0x63, millivolts_to_limit_value_type1(config->vcoreb_low_limit_mv));
	smbus_write_byte(dev, 0x64, millivolts_to_limit_value_type1(config->vtt_high_limit_mv));
	smbus_write_byte(dev, 0x65, millivolts_to_limit_value_type1(config->vtt_low_limit_mv));
	smbus_write_byte(dev, 0x6a, millivolts_to_limit_value_type2(config->vsen1_high_limit_mv));
	smbus_write_byte(dev, 0x6b, millivolts_to_limit_value_type2(config->vsen1_low_limit_mv));
	smbus_write_byte(dev, 0x6c, millivolts_to_limit_value_type2(config->vsen2_high_limit_mv));
	smbus_write_byte(dev, 0x6d, millivolts_to_limit_value_type2(config->vsen2_low_limit_mv));
	smbus_write_byte(dev, 0x6e, millivolts_to_limit_value_type2(config->vsen3_high_limit_mv));
	smbus_write_byte(dev, 0x6f, millivolts_to_limit_value_type2(config->vsen3_low_limit_mv));
	smbus_write_byte(dev, 0x70, millivolts_to_limit_value_type1(config->vsen4_high_limit_mv));
	smbus_write_byte(dev, 0x71, millivolts_to_limit_value_type1(config->vsen4_low_limit_mv));
	smbus_write_byte(dev, 0x72, millivolts_to_limit_value_type3(config->vdd_high_limit_mv));
	smbus_write_byte(dev, 0x73, millivolts_to_limit_value_type3(config->vdd_low_limit_mv));
	smbus_write_byte(dev, 0x74, millivolts_to_limit_value_type3(config->vsb_high_limit_mv));
	smbus_write_byte(dev, 0x75, millivolts_to_limit_value_type3(config->vsb_low_limit_mv));
	smbus_write_byte(dev, 0x76, millivolts_to_limit_value_type2(config->vbat_high_limit_mv));
	smbus_write_byte(dev, 0x77, millivolts_to_limit_value_type2(config->vbat_low_limit_mv));

	/* Temperature high/low limits */
	smbus_write_byte(dev, 0x78, config->td1_critical_temperature);
	smbus_write_byte(dev, 0x79, config->td1_critical_hysteresis);
	smbus_write_byte(dev, 0x7a, config->td1_warning_temperature);
	smbus_write_byte(dev, 0x7b, config->td1_warning_hysteresis);
	smbus_write_byte(dev, 0x7c, config->td2_critical_temperature);
	smbus_write_byte(dev, 0x7d, config->td2_critical_hysteresis);
	smbus_write_byte(dev, 0x7e, config->td2_warning_temperature);
	smbus_write_byte(dev, 0x7f, config->td2_warning_hysteresis);
	smbus_write_byte(dev, 0x80, config->td3_critical_temperature);
	smbus_write_byte(dev, 0x81, config->td3_critical_hysteresis);
	smbus_write_byte(dev, 0x82, config->td3_warning_temperature);
	smbus_write_byte(dev, 0x83, config->td3_warning_hysteresis);
	smbus_write_byte(dev, 0x84, config->td4_critical_temperature);
	smbus_write_byte(dev, 0x85, config->td4_critical_hysteresis);
	smbus_write_byte(dev, 0x86, config->td4_warning_temperature);
	smbus_write_byte(dev, 0x87, config->td4_warning_hysteresis);
	smbus_write_byte(dev, 0x88, config->tr1_critical_temperature);
	smbus_write_byte(dev, 0x89, config->tr1_critical_hysteresis);
	smbus_write_byte(dev, 0x8a, config->tr1_warning_temperature);
	smbus_write_byte(dev, 0x8b, config->tr1_warning_hysteresis);
	smbus_write_byte(dev, 0x8c, config->tr2_critical_temperature);
	smbus_write_byte(dev, 0x8d, config->tr2_critical_hysteresis);
	smbus_write_byte(dev, 0x8e, config->tr2_warning_temperature);
	smbus_write_byte(dev, 0x8f, config->tr2_warning_hysteresis);

	/* Set minimum FAN speeds before alarms will be set */
	for (i = 0; i < config->first_valid_fan_number; i++)
		w83793_fan_limit(dev, i, 0x0);
	for (i = config->first_valid_fan_number; i < 12; i++)
		w83793_fan_limit(dev, i, 0x768);

	/* Fan output style control */
	smbus_write_byte(dev, 0xb0, config->fanctrl1);
	smbus_write_byte(dev, 0xb1, config->fanctrl2);

	/* FAN Uptime */
	smbus_write_byte(dev, 0xc3, 0x02);

	/* FAN downtime */
	smbus_write_byte(dev, 0xc4, 0x03);

	/* ALL FAN critical temperature */
	smbus_write_byte(dev, 0xc5, config->critical_temperature);

	/* Temperature offset */
	smbus_write_byte(dev, 0xa8, 0xf9);
	smbus_write_byte(dev, 0xa9, 0xf9);
	smbus_write_byte(dev, 0xaa, 0xf9);
	smbus_write_byte(dev, 0xab, 0xf9);

	/* Default FAN speed */
	smbus_write_byte(dev, 0xb2, fan_pct_to_cfg_val(config->default_speed));

	/* Manual FAN speeds */
	smbus_write_byte(dev, 0xb3, fan_pct_to_cfg_val(config->fan1_duty));
	smbus_write_byte(dev, 0xb4, fan_pct_to_cfg_val(config->fan2_duty));
	smbus_write_byte(dev, 0xb5, fan_pct_to_cfg_val(config->fan3_duty));
	smbus_write_byte(dev, 0xb6, fan_pct_to_cfg_val(config->fan4_duty));
	smbus_write_byte(dev, 0xb7, fan_pct_to_cfg_val(config->fan5_duty));
	smbus_write_byte(dev, 0xb8, fan_pct_to_cfg_val(config->fan6_duty));
	smbus_write_byte(dev, 0xb9, fan_pct_to_cfg_val(config->fan7_duty));
	smbus_write_byte(dev, 0xba, fan_pct_to_cfg_val(config->fan8_duty));

	/* BANK 2 */
	smbus_write_byte(dev, 0x00, 0x02);

	/* TD FAN select */
	smbus_write_byte(dev, 0x01, config->td1_fan_select);
	smbus_write_byte(dev, 0x02, config->td2_fan_select);
	smbus_write_byte(dev, 0x03, config->td3_fan_select);
	smbus_write_byte(dev, 0x04, config->td4_fan_select);

	smbus_write_byte(dev, 0x05, config->tr1_fan_select);
	smbus_write_byte(dev, 0x06, config->tr2_fan_select);

	/* FAN control mode */
	smbus_write_byte(dev, 0x07, 0x00);

	/* hysteresis tolerance */
	smbus_write_byte(dev, 0x08, 0xaa);
	smbus_write_byte(dev, 0x09, 0xaa);

	/* FanNonStop */
	smbus_write_byte(dev, 0x18, fan_pct_to_cfg_val(config->fan1_nonstop));
	smbus_write_byte(dev, 0x19, fan_pct_to_cfg_val(config->fan2_nonstop));
	smbus_write_byte(dev, 0x1a, fan_pct_to_cfg_val(config->fan3_nonstop));
	smbus_write_byte(dev, 0x1b, fan_pct_to_cfg_val(config->fan4_nonstop));
	smbus_write_byte(dev, 0x1c, fan_pct_to_cfg_val(config->fan5_nonstop));
	smbus_write_byte(dev, 0x1d, fan_pct_to_cfg_val(config->fan6_nonstop));
	smbus_write_byte(dev, 0x1e, fan_pct_to_cfg_val(config->fan7_nonstop));
	smbus_write_byte(dev, 0x1f, fan_pct_to_cfg_val(config->fan8_nonstop));

	/* FanStart */
	smbus_write_byte(dev, 0x20, 0x08);
	smbus_write_byte(dev, 0x21, 0x08);
	smbus_write_byte(dev, 0x22, 0x08);
	smbus_write_byte(dev, 0x23, 0x08);
	smbus_write_byte(dev, 0x24, 0x08);
	smbus_write_byte(dev, 0x25, 0x08);
	smbus_write_byte(dev, 0x26, 0x08);
	smbus_write_byte(dev, 0x27, 0x08);

	for (i = 0; i < 4; i++)
		w83793_td_level(dev, i, (const char[]){ 0x32, 0x32, 0x32, 0x32, 0x37, 0x41, 0x4b });

	for (i = 0; i < 2; i++)
		w83793_tr_level(dev, i, (const char[]){ 0x1e, 0x23, 0x28, 0x2d, 0x32, 0x37, 0x3c });

	for (i = 0; i < 4; i++)
		w83793_td_fan_level(dev, i, (const char[]){ 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x26 });

	for (i = 0; i < 2; i++)
		w83793_tr_fan_level(dev, i, (const char[]){ 0x08, 0x0c, 0x10, 0x18, 0x20, 0x30, 0x38 });


	smbus_write_byte(dev, 0x00, 0x00);

	/* start monitoring operation */
	smbus_write_byte(dev, 0x40, 0x09);

}

static struct device_operations w83793_operations = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = w83793_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &w83793_operations;
}

struct chip_operations drivers_i2c_w83793_ops = {
	CHIP_NAME("Nuvoton W83793 Hardware Monitor")
	.enable_dev = enable_dev,
};
