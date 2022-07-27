/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_SMBUS_DEF_H
#define DEVICE_SMBUS_DEF_H

#include <types.h>

/* Error results are negative success is >= 0 */
#define SMBUS_ERROR			CB_ERR
#define SMBUS_WAIT_UNTIL_READY_TIMEOUT	CB_I2C_BUSY
#define SMBUS_WAIT_UNTIL_DONE_TIMEOUT	CB_I2C_TIMEOUT
#define SMBUS_WAIT_UNTIL_ACTIVE_TIMEOUT	CB_I2C_NO_DEVICE

#endif /* DEVICE_SMBUS_DEF_H */
