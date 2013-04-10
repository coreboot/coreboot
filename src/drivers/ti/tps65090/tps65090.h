/*
 * Copyright 2012, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of Google Inc. nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

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
 * @bus		I2C bus number the TPS65090 is on
 * @fet_id	FET ID, value between 1 and 7
 * return	0 on success, non-0 on failure
 */
int tps65090_fet_enable(unsigned int bus, enum fet_id fet_id);

/**
 * Disable FET
 *
 * @bus		I2C bus number the TPS65090 is on
 * @fet_id	FET ID, value between 1 and 7
 * @return	0 on success, non-0 on failure
 */
int tps65090_fet_disable(unsigned int bus, enum fet_id fet_id);

/**
 * Is FET enabled?
 *
 * @bus		I2C bus number the TPS65090 is on
 * @fet_id	FET ID, value between 1 and 7
 * @return	1 enabled, 0 disabled, negative value on failure
 */
int tps65090_fet_is_enabled(unsigned int bus, enum fet_id fet_id);

/**
 * Enable / disable the battery charger
 *
 * @bus		I2C bus number the TPS65090 is on
 * @enable	0 to disable charging, non-zero to enable
 */
int tps65090_set_charge_enable(unsigned int bus, int enable);

/**
 * Check whether we have enabled battery charging
 *
 * @bus		I2C bus number the TPS65090 is on
 * @return 1 if enabled, 0 if disabled
 */
int tps65090_is_charging(unsigned int bus);

/**
 * Return the value of the status register
 *
 * @bus		I2C bus number the TPS65090 is on
 * @return status register value, or -1 on error
 */
int tps65090_get_status(unsigned int bus);

#endif /* __TPS65090_H_ */
