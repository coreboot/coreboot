/* SPDX-License-Identifier: BSD-3-Clause or GPL-2.0-only */

#ifndef __TPS65090_H_
#define __TPS65090_H_

/* I2C device address for TPS65090 PMU */
#define TPS65090_I2C_ADDR	0x48

/* TPS65090 FET control registers */
enum fet_id {
	FET1_CTRL = 0x0f,
	FET2_CTRL,
	FET3_CTRL,
	FET4_CTRL,
	FET5_CTRL,
	FET6_CTRL,
	FET7_CTRL,
};

enum {
	/* Status register fields */
	TPS65090_ST1_OTC	= 1 << 0,
	TPS65090_ST1_OCC	= 1 << 1,
	TPS65090_ST1_STATE_SHIFT = 4,
	TPS65090_ST1_STATE_MASK	= 0xf << TPS65090_ST1_STATE_SHIFT,
};

/* FET errors */
enum {
	FET_ERR_COMMS		= -1,	/* FET comms error */
	FET_ERR_NOT_READY	= -2,	/* FET is not yet ready - retry */
};

/**
 * Enable FET
 *
 * @param bus		I2C bus number the TPS65090 is on
 * @param fet_id	FET ID, value between 1 and 7
 * return	0 on success, non-0 on failure
 */
int tps65090_fet_enable(unsigned int bus, enum fet_id fet_id);

/**
 * Disable FET
 *
 * @param bus		I2C bus number the TPS65090 is on
 * @param fet_id	FET ID, value between 1 and 7
 * @return	0 on success, non-0 on failure
 */
int tps65090_fet_disable(unsigned int bus, enum fet_id fet_id);

/**
 * Is FET enabled?
 *
 * @param bus		I2C bus number the TPS65090 is on
 * @param fet_id	FET ID, value between 1 and 7
 * @return	1 enabled, 0 disabled, negative value on failure
 */
int tps65090_fet_is_enabled(unsigned int bus, enum fet_id fet_id);

/**
 * Enable / disable the battery charger
 *
 * @param bus		I2C bus number the TPS65090 is on
 * @param enable	0 to disable charging, non-zero to enable
 */
int tps65090_set_charge_enable(unsigned int bus, int enable);

/**
 * Check whether we have enabled battery charging
 *
 * @param bus		I2C bus number the TPS65090 is on
 * @return 1 if enabled, 0 if disabled
 */
int tps65090_is_charging(unsigned int bus);

/**
 * Return the value of the status register
 *
 * @param bus		I2C bus number the TPS65090 is on
 * @return status register value, or -1 on error
 */
int tps65090_get_status(unsigned int bus);

#endif /* __TPS65090_H_ */
