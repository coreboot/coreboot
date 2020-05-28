/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <ec/google/common/mec.h>
#include <string.h>
#include <timer.h>
#include <types.h>

#include "ec.h"

/* Mailbox ID */
#define EC_MAILBOX_ID			0x00f0

/* Version of mailbox interface */
#define EC_MAILBOX_VERSION		0

/* Command to start mailbox transaction */
#define EC_MAILBOX_START_COMMAND	0xda

/* Version of EC protocol */
#define EC_MAILBOX_PROTO_VERSION	3

/* Max number of bytes in protocol data payload */
#define EC_MAILBOX_DATA_SIZE		32

/* Number of header bytes to be counted as data bytes */
#define EC_MAILBOX_DATA_EXTRA		2

/* Maximum timeout */
#define EC_MAILBOX_TIMEOUT_MS		MSECS_PER_SEC

/* EC response flags */
#define EC_CMDR_DATA		BIT(0)	/* Data ready for host to read */
#define EC_CMDR_PENDING		BIT(1)	/* Write pending to EC */
#define EC_CMDR_BUSY		BIT(2)	/* EC is busy processing a command */
#define EC_CMDR_CMD		BIT(3)	/* Last host write was a command */

/* Request to EC */
struct wilco_ec_request {
	uint8_t struct_version;		/* version (=3) */
	uint8_t checksum;		/* sum of all bytes must be 0 */
	uint16_t mailbox_id;		/* mailbox identifier */
	uint8_t mailbox_version;	/* mailbox version (=0) */
	uint8_t reserved1;		/* unused (=0) */
	uint16_t data_size;		/* length (data + 2 bytes of header) */
	uint8_t command;		/* mailbox command */
	uint8_t reserved2;		/* unused (=0) */
} __packed;

/* Response from EC */
struct wilco_ec_response {
	uint8_t struct_version;		/* version (=3) */
	uint8_t checksum;		/* sum of all bytes must be 0 */
	uint16_t result;		/* result code */
	uint16_t data_size;		/* length of data buffer (always 32) */
	uint8_t reserved[3];		/* unused (=0) */
	uint8_t data[EC_MAILBOX_DATA_SIZE];
} __packed;

struct wilco_ec_message {
	uint8_t command;		/* mailbox command code */
	uint8_t result;			/* request result */
	size_t request_size;		/* bytes to send to the EC */
	size_t response_size;		/* bytes expected from the EC */
	enum wilco_ec_msg_type type;	/* message type */
	/*
	 * This data buffer will contain the request data when passed to
	 * wilco_ec_message() and will contain the response data on return.
	 */
	uint8_t data[EC_MAILBOX_DATA_SIZE];
};

static bool wilco_ec_response_timed_out(void)
{
	uint8_t mask = EC_CMDR_PENDING | EC_CMDR_BUSY;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, EC_MAILBOX_TIMEOUT_MS);

	while (inb(CONFIG_EC_BASE_HOST_COMMAND) & mask) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "%s: Command timeout\n", __func__);
			return true; /* Timed out */
		}
		mdelay(1);
	}

	return false; /* Did not time out */
}

static uint8_t wilco_ec_checksum(void *data, size_t size)
{
	uint8_t *data_bytes = (uint8_t *)data;
	uint8_t checksum = 0;
	size_t i;

	for (i = 0; i < size; i++)
		checksum += data_bytes[i];

	return checksum;
}

static void wilco_ec_prepare(struct wilco_ec_message *msg,
			     struct wilco_ec_request *rq)
{
	memset(rq, 0, sizeof(*rq));

	/* Fill in request packet */
	rq->struct_version = EC_MAILBOX_PROTO_VERSION;
	rq->mailbox_id = EC_MAILBOX_ID;
	rq->mailbox_version = EC_MAILBOX_VERSION;
	rq->data_size = msg->request_size + EC_MAILBOX_DATA_EXTRA;
	rq->command = msg->command;

	/* Checksum header and data */
	rq->checksum = wilco_ec_checksum(rq, sizeof(*rq));
	rq->checksum += wilco_ec_checksum(msg->data, msg->request_size);
	rq->checksum = -rq->checksum;
}

static int wilco_ec_transfer(struct wilco_ec_message *msg)
{
	struct wilco_ec_request rq;
	struct wilco_ec_response rs;
	uint8_t checksum;
	size_t skip_size;

	/* Prepare request packet */
	wilco_ec_prepare(msg, &rq);

	/* Write request header */
	mec_io_bytes(MEC_IO_WRITE, CONFIG_EC_BASE_PACKET, 0, &rq, sizeof(rq));

	/* Write request data */
	mec_io_bytes(MEC_IO_WRITE, CONFIG_EC_BASE_PACKET, sizeof(rq),
		     msg->data, msg->request_size);

	/* Start the command */
	outb(EC_MAILBOX_START_COMMAND, CONFIG_EC_BASE_HOST_COMMAND);

	/* Some commands will put the EC into a state where it cannot respond */
	if (msg->type == WILCO_EC_MSG_NO_RESPONSE) {
		printk(BIOS_DEBUG, "%s: EC does not respond to this command\n",
		       __func__);
		return 0;
	}

	/* Wait for it to complete */
	if (wilco_ec_response_timed_out()) {
		printk(BIOS_ERR, "%s: response timed out\n", __func__);
		return -1;
	}

	/* Check result */
	msg->result = inb(CONFIG_EC_BASE_HOST_DATA);
	if (msg->result != 0) {
		printk(BIOS_ERR, "%s: bad response: 0x%02x\n",
		       __func__, msg->result);
		return -1;
	}

	/* Read back response */
	checksum = mec_io_bytes(MEC_IO_READ, CONFIG_EC_BASE_PACKET, 0,
				&rs, sizeof(rs));
	if (checksum) {
		printk(BIOS_ERR, "%s: bad checksum %02x\n", __func__, checksum);
		return -1;
	}
	msg->result = rs.result;

	/* EC always returns EC_MAILBOX_DATA_SIZE bytes */
	if (rs.data_size > EC_MAILBOX_DATA_SIZE) {
		printk(BIOS_ERR, "%s: packet too long (%d bytes, expected %d)",
		       __func__, rs.data_size, EC_MAILBOX_DATA_SIZE);
		return -1;
	}

	/* Skip response data bytes as requested */
	skip_size = (msg->type == WILCO_EC_MSG_DEFAULT) ? 1 : 0;

	if (msg->response_size > rs.data_size - skip_size) {
		printk(BIOS_ERR, "%s: data too short (%lu bytes, expected %zu)",
		       __func__, rs.data_size - skip_size, msg->response_size);
		return -1;
	}

	memcpy(msg->data, rs.data + skip_size, msg->response_size);

	/* Return actual amount of data received */
	return msg->response_size;
}

int wilco_ec_mailbox(enum wilco_ec_msg_type type, uint8_t command,
		     const void *request_data, size_t request_size,
		     void *response_data, size_t response_size)
{
	struct wilco_ec_message msg = {
		.command = command,
		.request_size = request_size,
		.response_size = response_size,
		.type = type,
	};
	int ret;

	if (request_size > EC_MAILBOX_DATA_SIZE) {
		printk(BIOS_ERR, "%s: provided request data too large: %zu\n",
		       __func__, request_size);
		return -1;
	}
	if (response_size > EC_MAILBOX_DATA_SIZE) {
		printk(BIOS_ERR, "%s: expected response data too large: %zu\n",
		       __func__, response_size);
		return -1;
	}
	if (request_size && !request_data) {
		printk(BIOS_ERR, "%s: request data missing\n", __func__);
		return -1;
	}
	if (response_size && !response_data) {
		printk(BIOS_ERR, "%s: request data missing\n", __func__);
		return -1;
	}

	/* Copy request data if present */
	if (request_size)
		memcpy(msg.data, request_data, request_size);

	/* Do the EC transfer */
	ret = wilco_ec_transfer(&msg);

	/* Copy response data if present */
	if (ret > 0 && response_size)
		memcpy(response_data, msg.data, response_size);

	/* Return error if message result is non-zero */
	if (ret >= 0 && msg.result)
		ret = -1;

	return ret;
}
