/* SPDX-License-Identifier: GPL-2.0-only */

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
