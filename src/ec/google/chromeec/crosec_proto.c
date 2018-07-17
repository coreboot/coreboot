/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <compiler.h>
#include <console/console.h>
#include <delay.h>
#include <stdint.h>
#include <string.h>
#include "ec.h"
#include "ec_commands.h"
#include "ec_message.h"

/* Common utilities */
void *__weak crosec_get_buffer(size_t size, int req)
{
	printk(BIOS_DEBUG, "crosec_get_buffer() implementation required.\n");
	return NULL;
}

/* Dumps EC command / response data into debug output.
 *
 * @param name	Message prefix name.
 * @param cmd	Command code, or -1 to ignore cmd message.
 * @param data	Data buffer to print.
 * @param len	Length of data.
 */
static void cros_ec_dump_data(const char *name, int cmd, const uint8_t *data,
			      int len)
{
	int i;

	printk(BIOS_DEBUG, "%s: ", name);
	if (cmd != -1)
		printk(BIOS_DEBUG, "cmd=%#x: ", cmd);
	for (i = 0; i < len; i++)
		printk(BIOS_DEBUG, "%02x ", data[i]);
	printk(BIOS_DEBUG, "\n");
}

/* Calculate a simple 8-bit checksum of a data block
 *
 * @param data	Data block to checksum
 * @param size	Size of data block in bytes
 * @return checksum value (0 to 255)
 */
static int cros_ec_calc_checksum(const uint8_t *data, int size)
{
	int csum, i;

	for (i = csum = 0; i < size; i++)
		csum += data[i];
	return csum & 0xff;
}

/* Standard Chrome EC protocol, version 3 */

struct ec_command_v3 {
	struct ec_host_request header;
	uint8_t data[MSG_BYTES];
};

struct ec_response_v3 {
	struct ec_host_response header;
	uint8_t data[MSG_BYTES];
};

/**
 * Create a request packet for protocol version 3.
 *
 * @param cec_command	Command description.
 * @param cmd		Packed command bit stream.
 * @return packet size in bytes, or <0 if error.
 */
static int create_proto3_request(const struct chromeec_command *cec_command,
				 struct ec_command_v3 *cmd)
{
	struct ec_host_request *rq = &cmd->header;
	int out_bytes = cec_command->cmd_size_in + sizeof(*rq);

	/* Fail if output size is too big */
	if (out_bytes > sizeof(*cmd)) {
		printk(BIOS_ERR, "%s: Cannot send %d bytes\n", __func__,
		       cec_command->cmd_size_in);
		return -EC_RES_REQUEST_TRUNCATED;
	}

	/* Fill in request packet */
	rq->struct_version = EC_HOST_REQUEST_VERSION;
	rq->checksum = 0;
	rq->command = cec_command->cmd_code;
	rq->command_version = cec_command->cmd_version;
	rq->reserved = 0;
	rq->data_len = cec_command->cmd_size_in;

	/* Copy data after header */
	memcpy(cmd->data, cec_command->cmd_data_in, cec_command->cmd_size_in);

	/* Write checksum field so the entire packet sums to 0 */
	rq->checksum = (uint8_t)(-cros_ec_calc_checksum(
			(const uint8_t*)cmd, out_bytes));

	cros_ec_dump_data("out", rq->command, (const uint8_t *)cmd, out_bytes);

	/* Return size of request packet */
	return out_bytes;
}

/**
 * Prepare the device to receive a protocol version 3 response.
 *
 * @param cec_command	Command description.
 * @param resp		Response buffer.
 * @return maximum expected number of bytes in response, or <0 if error.
 */
static int prepare_proto3_response_buffer(
		const struct chromeec_command *cec_command,
		struct ec_response_v3 *resp)
{
	int in_bytes = cec_command->cmd_size_out + sizeof(resp->header);

	/* Fail if input size is too big */
	if (in_bytes > sizeof(*resp)) {
		printk(BIOS_ERR, "%s: Cannot receive %d bytes\n", __func__,
		       cec_command->cmd_size_out);
		return -EC_RES_RESPONSE_TOO_BIG;
	}

	/* Return expected size of response packet */
	return in_bytes;
}

/**
 * Handle a protocol version 3 response packet.
 *
 * The packet must already be stored in the response buffer.
 *
 * @param resp		Response buffer.
 * @param cec_command	Command structure to receive valid response.
 * @return number of bytes of response data, or <0 if error
 */
static int handle_proto3_response(struct ec_response_v3 *resp,
				  struct chromeec_command *cec_command)
{
	struct ec_host_response *rs = &resp->header;
	int in_bytes;
	int csum;

	cros_ec_dump_data("in-header", -1, (const uint8_t*)rs, sizeof(*rs));

	/* Check input data */
	if (rs->struct_version != EC_HOST_RESPONSE_VERSION) {
		printk(BIOS_ERR, "%s: EC response version mismatch\n", __func__);
		return -EC_RES_INVALID_RESPONSE;
	}

	if (rs->reserved) {
		printk(BIOS_ERR, "%s: EC response reserved != 0\n", __func__);
		return -EC_RES_INVALID_RESPONSE;
	}

	if (rs->data_len > sizeof(resp->data) ||
	    rs->data_len > cec_command->cmd_size_out) {
		printk(BIOS_ERR, "%s: EC returned too much data\n", __func__);
		return -EC_RES_RESPONSE_TOO_BIG;
	}

	cros_ec_dump_data("in-data", -1, resp->data, rs->data_len);

	/* Update in_bytes to actual data size */
	in_bytes = sizeof(*rs) + rs->data_len;

	/* Verify checksum */
	csum = cros_ec_calc_checksum((const uint8_t *)resp, in_bytes);
	if (csum) {
		printk(BIOS_ERR, "%s: EC response checksum invalid: 0x%02x\n",
		       __func__, csum);
		return -EC_RES_INVALID_CHECKSUM;
	}

	/* Return raw response. */
	cec_command->cmd_code = rs->result;
	cec_command->cmd_size_out = rs->data_len;
	memcpy(cec_command->cmd_data_out, resp->data, rs->data_len);

	/* Return error result, if any */
	if (rs->result) {
		printk(BIOS_ERR, "%s: EC response with error code: %d\n",
		       __func__, rs->result);
		return -(int)rs->result;
	}

	return rs->data_len;
}

static int send_command_proto3(struct chromeec_command *cec_command,
			       crosec_io_t crosec_io, void *context)
{
	int out_bytes, in_bytes;
	int rv;
	struct ec_command_v3 *cmd;
	struct ec_response_v3 *resp;

	if ((cmd = crosec_get_buffer(sizeof(*cmd), 1)) == NULL)
		return -EC_RES_ERROR;
	if ((resp = crosec_get_buffer(sizeof(*resp), 0)) == NULL)
		return -EC_RES_ERROR;

	/* Create request packet */
	out_bytes = create_proto3_request(cec_command, cmd);
	if (out_bytes < 0) {
		return out_bytes;
	}

	/* Prepare response buffer */
	in_bytes = prepare_proto3_response_buffer(cec_command, resp);
	if (in_bytes < 0) {
		return in_bytes;
	}

	rv = crosec_io(out_bytes, in_bytes, context);
	if (rv != 0) {
		printk(BIOS_ERR, "%s: failed to complete I/O: Err = %#x.\n",
		       __func__, rv >= 0 ? rv : -rv);
		return -EC_RES_ERROR;
	}

	/* Process the response */
	return handle_proto3_response(resp, cec_command);
}

static int crosec_command_proto_v3(struct chromeec_command *cec_command,
				   crosec_io_t crosec_io, void *context)
{
	int rv = send_command_proto3(cec_command, crosec_io, context);
	if (rv < 0) {
		cec_command->cmd_code = rv;
		return 1;
	}
	return 0;
}

int crosec_command_proto(struct chromeec_command *cec_command,
			 crosec_io_t crosec_io, void *context)
{
	// TODO(hungte) Detect and fallback to v2 if we need.
	return crosec_command_proto_v3(cec_command, crosec_io, context);
}
