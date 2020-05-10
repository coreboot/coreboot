/* SPDX-License-Identifier: GPL-2.0-only */

#include <spi-generic.h>
#include <spi_flash.h>
#include <console/spi.h>

static struct spi_slave slave;

void spiconsole_init(void) {
	spi_init();
	spi_setup_slave(0, 0, &slave);
	return;
}

/*
 * The EM100 'hyper terminal' specification defines a header of 9 characters.
 * Because of this, devices with a spi_crop_chunk of less than 10 characters
 * can't be supported by this standard.
 *
 * To add support in romstage, the static struct here and the ones used by
 * spi_xfer will need to be modified - removed, or mapped into cbmem.
 *
 * Because the Dediprog software expects strings, not single characters, and
 * because of the header overhead, this builds up a buffer to send.
 */
void spiconsole_tx_byte(unsigned char c) {
	static struct em100_msg msg = {
		.header.spi_command = EM100_DEDICATED_CMD,
		.header.em100_command = EM100_UFIFO_CMD,
		.header.msg_signature = EM100_MSG_SIGNATURE,
		.header.msg_type = EM100_MSG_ASCII,
		.header.msg_length = 0
	};

	/* Verify the spi buffer is big enough to send even a single byte */
	if (spi_crop_chunk(&slave, 0, MAX_MSG_LENGTH) <
			sizeof(struct em100_msg_header) + 1)
		return;

	msg.data[msg.header.msg_length] = c;
	msg.header.msg_length++;

	/* Send the data on newline or when the max spi length is reached */
	if (c == '\n' || (sizeof(struct em100_msg_header) +
			msg.header.msg_length == spi_crop_chunk(&slave, 0,
			MAX_MSG_LENGTH))) {
		spi_xfer(&slave, &msg, sizeof(struct em100_msg_header) +
				msg.header.msg_length, NULL, 0);

		msg.header.msg_length = 0;
	}

	return;
}
