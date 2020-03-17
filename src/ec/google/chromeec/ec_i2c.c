/*
 * This file is part of the coreboot project.
 *
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

#include <console/console.h>
#include <device/i2c_simple.h>
#include <stdint.h>
#include <string.h>

#include "ec.h"
#include "ec_commands.h"

#if CONFIG(EC_GOOGLE_CHROMEEC_I2C_PROTO3)

#define PROTO3_FRAMING_BYTES sizeof(uint32_t)
/* Just use the LPC host packet size to size the buffer. */
#define PROTO3_MAX_PACKET_SIZE 268

struct proto3_i2c_buf {
	uint8_t framing_bytes[PROTO3_FRAMING_BYTES];
	uint8_t data[PROTO3_MAX_PACKET_SIZE];
} __attribute__((aligned(sizeof(uint32_t))));

static struct proto3_i2c_buf req_buf;
static struct proto3_i2c_buf resp_buf;

enum {
	CMD_INDEX,
	RESP_INDEX,
	SEGS_PER_CMD,
};

struct i2c_ec {
	int bus;
	struct i2c_msg segs[SEGS_PER_CMD];
};

static struct i2c_ec ec_dev = {
	.bus = CONFIG_EC_GOOGLE_CHROMEEC_I2C_BUS,
	.segs[CMD_INDEX] = {
		.flags = 0,
		.slave = CONFIG_EC_GOOGLE_CHROMEEC_I2C_CHIP,
		/* Framing byte to be transferred prior to request. */
		.buf = &req_buf.framing_bytes[3],
	},
	.segs[RESP_INDEX] = {
		.flags = I2C_M_RD,
		.slave = CONFIG_EC_GOOGLE_CHROMEEC_I2C_CHIP,
		/* return code and total length before full response. */
		.buf = &resp_buf.framing_bytes[2],
	},
};

void *crosec_get_buffer(size_t size, int req)
{
	struct proto3_i2c_buf *ib;

	if (size > PROTO3_MAX_PACKET_SIZE) {
		printk(BIOS_DEBUG, "Proto v3 buffer request too large: %zu!\n",
			size);
		return NULL;
	}

	if (req)
		ib = &req_buf;
	else
		ib = &resp_buf;

	return &ib->data[0];
}

static int crosec_i2c_io(size_t req_size, size_t resp_size, void *context)
{
	struct i2c_ec *ec = context;
	uint8_t ret_code;
	size_t resp_len;

	if (req_size > PROTO3_MAX_PACKET_SIZE ||
		resp_size > PROTO3_MAX_PACKET_SIZE)
		return -1;

	/* Place the framing byte and set size accordingly. */
	ec->segs[CMD_INDEX].len = req_size + 1;
	ec->segs[CMD_INDEX].buf[0] = EC_COMMAND_PROTOCOL_3;
	/* Return code and length returned prior to packet data. */
	ec->segs[RESP_INDEX].len = resp_size + 2;

	if (i2c_transfer(ec->bus, ec->segs, ARRAY_SIZE(ec->segs)) != 0) {
		printk(BIOS_ERR, "%s: Cannot complete read from i2c-%d:%#x\n",
		       __func__, ec->bus, ec->segs[0].slave);
		return -1;
	}

	ret_code = ec->segs[RESP_INDEX].buf[0];
	resp_len = ec->segs[RESP_INDEX].buf[1];

	if (ret_code != 0) {
		printk(BIOS_ERR, "EC command returned 0x%x\n", ret_code);
		return -1;
	}

	if (resp_len > resp_size) {
		printk(BIOS_ERR, "Response length mismatch %zu vs %zu\n",
			resp_len, resp_size);
		return -1;
	}

	return 0;
}

int google_chromeec_command(struct chromeec_command *cec_command)
{
	return crosec_command_proto(cec_command, crosec_i2c_io, &ec_dev);
}

#else /* CONFIG_EC_GOOGLE_CHROMEEC_I2C_PROTO3 */

/* Command (host->device) format for I2C:
 *  uint8_t version, cmd, len, data[len], checksum;
 *
 * Response (device->host) format for I2C:
 *  uint8_t response, len, data[len], checksum;
 *
 * Note the location of checksum is different from LPC protocol.
 *
 * The length is 8 bit so maximum data size is 0xff.
 * Any I2C command should fit in 0xff + 4 bytes, and max response length
 * is 0xff + 3 bytes.
 */
#define MAX_I2C_DATA_SIZE		(0xff)

typedef struct {
	uint8_t version;
	uint8_t command;
	uint8_t length;
	uint8_t data[MAX_I2C_DATA_SIZE + 1];
} EcCommandI2c;

typedef struct {
	uint8_t response;
	uint8_t length;
	uint8_t data[MAX_I2C_DATA_SIZE + 1];
} EcResponseI2c;

static inline void i2c_dump(int bus, int chip, const uint8_t *data, size_t size)
{
#ifdef TRACE_CHROMEEC
	printk(BIOS_INFO, "i2c: bus=%d, chip=%#x, size=%d, data: ", bus, chip,
	       size);
	while (size-- > 0) {
		printk(BIOS_INFO, "%02X ", *data++);
	}
	printk(BIOS_INFO, "\n");
#endif
}

static int ec_verify_checksum(const EcResponseI2c *resp)
{
	size_t size = sizeof(*resp) - sizeof(resp->data) + resp->length;
	uint8_t calculated = google_chromeec_calc_checksum(
			(const uint8_t *)resp, size);
	uint8_t received = resp->data[resp->length];
	if (calculated != received) {
		printk(BIOS_ERR, "%s: Unmatch (rx: %#02x, calc: %#02x)\n",
		       __func__, received, calculated);
		return 0;
	}
	return 1;
}

static void ec_fill_checksum(EcCommandI2c *cmd)
{
	size_t size = sizeof(*cmd) - sizeof(cmd->data) + cmd->length;
	cmd->data[cmd->length] = google_chromeec_calc_checksum(
			(const uint8_t *)cmd, size);
}

int google_chromeec_command(struct chromeec_command *cec_command)
{
	EcCommandI2c cmd;
	EcResponseI2c resp;
	int bus = CONFIG_EC_GOOGLE_CHROMEEC_I2C_BUS;
	int chip = CONFIG_EC_GOOGLE_CHROMEEC_I2C_CHIP;
	size_t size_i2c_cmd = (sizeof(cmd) - sizeof(cmd.data) +
			       cec_command->cmd_size_in + 1),
	       size_i2c_resp = (sizeof(resp) - sizeof(resp.data) +
				cec_command->cmd_size_out + 1);

	if (cec_command->cmd_size_in > MAX_I2C_DATA_SIZE ||
	    cec_command->cmd_size_out > MAX_I2C_DATA_SIZE) {
		printk(BIOS_ERR, "%s: Command data size too large (%d,%d)\n",
		       __func__, cec_command->cmd_size_in,
		       cec_command->cmd_size_out);
		cec_command->cmd_code = EC_RES_INVALID_PARAM;
		return 1;
	}

	/* Construct command. */
	cmd.version = EC_CMD_VERSION0 + cec_command->cmd_version;
	cmd.command = cec_command->cmd_code;
	cmd.length = cec_command->cmd_size_in;
	memcpy(cmd.data, cec_command->cmd_data_in, cmd.length);
	ec_fill_checksum(&cmd);

	/* Start I2C communication */
	i2c_dump(bus, chip, (const uint8_t *)&cmd, size_i2c_cmd);
	if (i2c_write_raw(bus, chip, (uint8_t *)&cmd, size_i2c_cmd) != 0) {
		printk(BIOS_ERR, "%s: Cannot complete write to i2c-%d:%#x\n",
		       __func__, bus, chip);
		cec_command->cmd_code = EC_RES_ERROR;
		return 1;
	}
	if (i2c_read_raw(bus, chip, (uint8_t *)&resp, size_i2c_resp) != 0) {
		printk(BIOS_ERR, "%s: Cannot complete read from i2c-%d:%#x\n",
		       __func__, bus, chip);
		cec_command->cmd_code = EC_RES_ERROR;
		return 1;
	}

	/* Verify and return response */
	cec_command->cmd_code = resp.response;
	if (resp.response != EC_RES_SUCCESS) {
		printk(BIOS_DEBUG, "%s: Received bad result code %d\n",
		       __func__, (int)resp.response);
		return 1;
	}
	if (resp.length > cec_command->cmd_size_out) {
		printk(BIOS_ERR, "%s: Received len %#02x too large\n",
		       __func__, (int)resp.length);
		cec_command->cmd_code = EC_RES_INVALID_RESPONSE;
		return 1;
	}
	if (!ec_verify_checksum(&resp)) {
		cec_command->cmd_code = EC_RES_INVALID_CHECKSUM;
		return 1;
	}
	cec_command->cmd_size_out = resp.length;
	memcpy(cec_command->cmd_data_out, resp.data, resp.length);
	return 0;
}

#endif /* CONFIG_EC_GOOGLE_CHROMEEC_I2C_PROTO3 */

u8 google_chromeec_get_event(void)
{
	printk(BIOS_ERR, "%s: Not supported.\n", __func__);
	return 0;
}
