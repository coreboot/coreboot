/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * IPMI specification:
 * https://www.intel.com/content/dam/www/public/us/en/documents/specification-updates/ipmi-intelligent-platform-mgt-interface-spec-2nd-gen-v2-0-spec-update.pdf
 *
 * LUN seems to be always zero.
 */

#include "ipmi_bt.h"

#include <arch/io.h>
#include <console/console.h>
#include <stdint.h>
#include <string.h>
#include <timer.h>

#include "ipmi_if.h"

/*
 * 11.6.3 BT Host to BMC Buffer (HOST2BMC)
 * "The buffer must be a minimum of 64-bytes deep."  This includes the length byte for
 * convenience of the implementation.
 */
#define MAX_SEND_SIZE    64
/*
 * 11.1 BT Interface-BMC Request Message Format
 * The length field is not considered part of the message on receiving (it's part of framing).
 */
#define MAX_RECEIVE_SIZE 255
/*
 * 11.1 BT Interface-BMC Request Message Format
 * 4 leading bytes form a frame for the message.
 */
#define MAX_PAYLOAD_SIZE (MAX_SEND_SIZE - 4)

#define BT_CTRL_REG  0 // Typical address of BT_CTRL is 0xE4
#define HOST2BMC_REG 1 // Typical address of HOST2BMC is 0xE5
#define BMC2HOST_REG 1 // Typical address of BMC2HOST is 0xE5

/* Bits of BT_CTRL */
#define B_BUSY     BIT(7)
#define H_BUSY     BIT(6)
#define OEM0       BIT(5)
#define SMS_ATN    BIT(4)
#define B2H_ATN    BIT(3)
#define H2B_ATN    BIT(2)
#define CLR_RD_PTR BIT(1)
#define CLR_WR_PTR BIT(0)

static enum cb_err wait_for_control_bits(uint16_t port, uint8_t mask, uint8_t expected)
{
	uint16_t bt_ctrl_port = port + BT_CTRL_REG;
	if (!wait_ms(CONFIG_IPMI_TIMEOUT_MS, (inb(bt_ctrl_port) & mask) == expected)) {
		printk(BIOS_ERR, "%s(0x%04x, 0x%02x, 0x%02x) timeout!\n",
		       __func__, port, mask, expected);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

enum cb_err ipmi_bt_clear(uint16_t port)
{
	uint8_t bt_ctrl;

	/*
	 * First, make sure H_BUSY is set so BMC won't try to write new commands
	 * while we're resetting pointers.
	 */
	outb(H_BUSY, port + BT_CTRL_REG);

	/* If BMC is already in the process of writing, wait until it's done */
	if (wait_for_control_bits(port, B_BUSY, 0) == CB_ERR)
		return CB_ERR;

	bt_ctrl = inb(port + BT_CTRL_REG);

	printk(BIOS_SPEW, "%s(): BT_CTRL = 0x%02x\n", __func__, bt_ctrl);

	/*
	 * Clear all bits which are already set (they are either toggle bits or
	 * write-1-to-clear) and reset buffer pointers. This also clears H_BUSY.
	 */
	outb(bt_ctrl | CLR_RD_PTR | CLR_WR_PTR, port + BT_CTRL_REG);

	return CB_SUCCESS;
}

static enum cb_err ipmi_bt_send(uint16_t port, uint8_t addr, uint8_t cmd,
				const uint8_t *payload, uint8_t payload_len,
				uint8_t seq_num)
{
	uint16_t i;
	uint16_t len;
	uint8_t buf[MAX_SEND_SIZE];

	len = 4 + payload_len;

	/* The length doesn't include the length byte. */
	buf[0] = len - 1;
	buf[1] = addr;
	buf[2] = seq_num;
	buf[3] = cmd;
	memcpy(&buf[4], payload, payload_len);

	/* Wait for BMC to be available and ready for the next command */
	if (wait_for_control_bits(port, B_BUSY | H2B_ATN, 0) == CB_ERR)
		return CB_ERR;

	/* Clear write pointer */
	outb(CLR_WR_PTR, port + BT_CTRL_REG);

	/* Send our message */
	for (i = 0; i < len; ++i)
		outb(buf[i], port + HOST2BMC_REG);

	/* Tell BMC to process the data */
	outb(H2B_ATN, port + BT_CTRL_REG);

	return CB_SUCCESS;
}

static int ipmi_bt_recv(uint16_t port, uint8_t addr, uint8_t cmd,
			uint8_t *response, uint8_t response_len,
			uint8_t seq_num)
{
	uint16_t i;
	uint16_t len;
	uint16_t out_len;
	uint8_t buf[MAX_RECEIVE_SIZE];

	/* Wait for BMC's response */
	if (wait_for_control_bits(port, B2H_ATN, B2H_ATN) == CB_ERR)
		return -1;

	/* Tell BMC that host is busy */
	outb(H_BUSY, port + BT_CTRL_REG);

	/* Acknowledge that response is being processed */
	outb(B2H_ATN, port + BT_CTRL_REG);

	/* Clear read pointer */
	outb(CLR_RD_PTR, port + BT_CTRL_REG);

	/* Receive response */
	len = inb(port + BMC2HOST_REG);
	for (i = 0; i < len; ++i)
		buf[i] = inb(port + BMC2HOST_REG);

	/* Indicate that the host is done working with the buffer */
	outb(H_BUSY, port + BT_CTRL_REG);

	if (len < 3) {
		printk(BIOS_ERR, "IPMI BT response is shorter than 3 bytes: %d\n", len);
		goto error;
	}

	if (buf[0] != addr) {
		printk(BIOS_ERR,
		       "NETFN/LUN field mismatch in IPMI BT response: 0x%02x instead of 0x%02x\n",
		       buf[0], addr);
		goto error;
	}
	if (buf[1] != seq_num) {
		printk(BIOS_ERR,
		       "SEQ field mismatch in IPMI BT response: 0x%02x instead of 0x%02x\n",
		       buf[1], seq_num);
		goto error;
	}
	if (buf[2] != cmd) {
		printk(BIOS_ERR,
		       "CMD field mismatch in IPMI BT response: 0x%02x instead of 0x%02x\n",
		       buf[2], cmd);
		goto error;
	}

	/*
	 * Copy response skipping sequence number to match KCS messages.
	 * Sequence number is really an implementation detail anyway.
	 */
	out_len = MIN(response_len, len - 1);
	if (out_len > 0)
		response[0] = buf[0];
	if (out_len > 1)
		memcpy(&response[1], &buf[2], out_len - 1);

	return out_len;

error:
	printk(BIOS_DEBUG, "  IPMI response length field: 0x%02x\n", len);
	printk(BIOS_DEBUG, "  IPMI NetFn/LUN: 0x%02x\n", addr);
	printk(BIOS_DEBUG, "  IPMI SEQ: 0x%02x\n", seq_num);
	printk(BIOS_DEBUG, "  IPMI command: 0x%02x\n", cmd);
	return -1;
}

int ipmi_message(int port, int netfn, int lun, int cmd,
		 const uint8_t *payload, int payload_len,
		 uint8_t *response, int response_len)
{
	static uint8_t seq_num = 0xff;

	uint8_t addr;

	if (netfn < 0 || netfn > 0x3f) {
		printk(BIOS_ERR, "%s(): NetFn (%d) is not within [0, %d] range\n",
		       __func__, netfn, 0x3f);
		return -1;
	}
	if (lun < 0 || lun > 0x3) {
		printk(BIOS_ERR, "%s(): LUN (%d) is not within [0, %d] range\n",
		       __func__, lun, 0x3);
		return -1;
	}
	if (payload_len < 0 || payload_len > MAX_PAYLOAD_SIZE) {
		printk(BIOS_ERR, "%s(): payload size (%d) is not within [0, %d] range\n",
		       __func__, payload_len, MAX_PAYLOAD_SIZE);
		return -1;
	}

	addr = (netfn << 2) | (lun & 0x3);
	if (ipmi_bt_send(port, addr, cmd, payload, payload_len, ++seq_num) == CB_ERR) {
		printk(BIOS_ERR, "Failed to send IPMI BT command 0x%02x\n", cmd);
		return -1;
	}

	addr = ((netfn + 1) << 2) | (lun & 0x3);
	return ipmi_bt_recv(port, addr, cmd, response, response_len, seq_num);
}
