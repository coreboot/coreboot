/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef EC_GOOGLE_WILCO_EC_H
#define EC_GOOGLE_WILCO_EC_H

#include <stddef.h>
#include <stdint.h>

/* Different supported message types */
enum wilco_ec_msg_type {
	WILCO_EC_MSG_RAW,		/* Raw message, do not skip any data */
	WILCO_EC_MSG_DEFAULT,		/* Skip 1 byte of response data */
	WILCO_EC_MSG_NO_RESPONSE,	/* EC does not respond to command */
};

/**
 * wilco_ec_mailbox
 *
 * Send a command request to the EC mailbox and receive the response.
 *
 * @type:          Mailbox message type, see enum above
 * @command:       Command to execute
 * @request_data:  Request data buffer
 * @request_size:  Number of bytes in request data buffer (max 32)
 * @response_data: Response data buffer
 * @response_size: Number of bytes in response data buffer (max 32)
 *
 * @return number of bytes received, negative error code on failure
 */
int wilco_ec_mailbox(enum wilco_ec_msg_type type, uint8_t command,
		     const void *request_data, size_t request_size,
		     void *response_data, size_t response_size);

#endif /* EC_GOOGLE_WILCO_EC_H */
