/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DASHARO_EC_H
#define DASHARO_EC_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Send a command to the EC.  request_data/request_size are the request payload,
 * request_data can be NULL if request_size is 0.  reply_data/reply_size are
 * the reply payload, reply_data can be NULL if reply_size is 0.
 */
bool dasharo_ec_cmd(uint8_t cmd, const uint8_t *request_data,
	uint8_t request_size, uint8_t *reply_data, uint8_t reply_size);

#endif
