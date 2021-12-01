/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_CLEVO_IT5570E_CHIP_H
#define EC_CLEVO_IT5570E_CHIP_H

#define IT5570E_FAN_CURVE_LEN	4	/* Number of fan curve points */
#define IT5570E_MAX_FAN_CNT	4	/* Maximum number of configurable fans */

enum ec_clevo_it5570e_fan_mode {
	FAN_MODE_AUTO = 0,
	FAN_MODE_CUSTOM,
};

struct ec_clevo_it5570e_fan_curve {
	uint8_t temperature[IT5570E_FAN_CURVE_LEN];
	uint8_t speed[IT5570E_FAN_CURVE_LEN];
};

struct ec_clevo_it5570e_config {
	uint8_t pl2_on_battery;
	enum ec_clevo_it5570e_fan_mode fan_mode;
	struct ec_clevo_it5570e_fan_curve fan_curves[IT5570E_MAX_FAN_CNT];
};

typedef struct ec_clevo_it5570e_config ec_config_t;

#endif /* EC_CLEVO_IT5570E_CHIP_H */
