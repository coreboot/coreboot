/*
 * Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#ifndef __TPS65090_H_
#define __TPS65090_H_

/* I2C device address for TPS65090 PMU */
#define TPS65090_I2C_ADDR	0x48

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
 * @param	fet_id	FET ID, value between 1 and 7
 * @return	0 on success, non-0 on failure
 */
int tps65090_fet_enable(unsigned int fet_id);

/**
 * Disable FET
 *
 * @param	fet_id	FET ID, value between 1 and 7
 * @return	0 on success, non-0 on failure
 */
int tps65090_fet_disable(unsigned int fet_id);

/**
 * Is FET enabled?
 *
 * @param	fet_id	FET ID, value between 1 and 7
 * @return	1 enabled, 0 disabled, negative value on failure
 */
int tps65090_fet_is_enabled(unsigned int fet_id);

/**
 * Enable / disable the battery charger
 *
 * @param enable	0 to disable charging, non-zero to enable
 */
int tps65090_set_charge_enable(int enable);

/**
 * Check whether we have enabled battery charging
 *
 * @return 1 if enabled, 0 if disabled
 */
int tps65090_get_charging(void);

/**
 * Return the value of the status register
 *
 * @return status register value, or -1 on error
 */
int tps65090_get_status(void);

/**
 * Initialize the TPS65090 PMU.
 *
 * @return	0 on success, non-0 on failure
 */
int tps65090_init(void);

#endif /* __TPS65090_H_ */
