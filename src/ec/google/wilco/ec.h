/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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

/**
 * wilco_ec_send
 *
 * Send a basic EC command with a one byte parameter with no
 * returned data;
 *
 * @command: Command to execute
 * @param:   Command parameter to send
 *
 * @return negative error code on failure
 */
static inline int wilco_ec_send(uint8_t command, uint8_t param)
{
	return wilco_ec_mailbox(WILCO_EC_MSG_DEFAULT, command,
				&param, sizeof(param), NULL, 0);
}

/**
 * wilco_ec_send_noargs
 *
 * Send a basic EC command with no parameters and no returned data.
 *
 * @command: Command to execute
 *
 * @return negative error code on failure
 */
static inline int wilco_ec_send_noargs(uint8_t command)
{
	return wilco_ec_mailbox(WILCO_EC_MSG_DEFAULT, command,
				NULL, 0, NULL, 0);
}

/**
 * wilco_ec_sendrecv
 *
 * Send a basic EC command with a one byte parameter, ignoring the
 * first byte of returned data to match the common behavior.
 * The maximum response size is 31 due to the ignored byte.
 *
 * @command: Command to execute
 * @param:   Command parameter to send
 * @data:    Response data buffer
 * @size:    Number of bytes in response data buffer (max 31)
 *
 * @return number of bytes received, negative error code on failure
 */
static inline int wilco_ec_sendrecv(uint8_t command, uint8_t param,
				    void *data, size_t size)
{
	return wilco_ec_mailbox(WILCO_EC_MSG_DEFAULT, command,
				&param, sizeof(param), data, size);
}

/**
 * wilco_ec_sendrecv_noargs
 *
 * Send a basic EC command with no parameters, ignoring the
 * first byte of returned data to match the common behavior.
 * The maximum response size is 31 due to the ignored byte.
 *
 * @command: Command to execute
 * @data:    Response data buffer
 * @size:    Number of bytes in response data buffer (max 31)
 *
 * @return number of bytes received, negative error code on failure
 */
static inline int wilco_ec_sendrecv_noargs(uint8_t command,
					   void *data, size_t size)
{
	return wilco_ec_mailbox(WILCO_EC_MSG_DEFAULT, command,
				NULL, 0, data, size);
}

#endif /* EC_GOOGLE_WILCO_EC_H */
