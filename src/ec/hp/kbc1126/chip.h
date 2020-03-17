/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _EC_HP_KBC1126_CHIP_H
#define _EC_HP_KBC1126_CHIP_H

struct ec_hp_kbc1126_config
{
	u16 ec_data_port;
	u16 ec_cmd_port;
	u8 ec_ctrl_reg;
	u8 ec_fan_ctrl_value;
};

#endif /* _EC_HP_KBC1126_CHIP_H */
