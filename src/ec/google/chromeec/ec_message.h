/*
 * Chromium OS Matrix Keyboard Message Protocol definitions
 *
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _CROS_MESSAGE_H
#define _CROS_MESSAGE_H

/*
 * Command interface between EC and AP, for LPC, I2C and SPI interfaces.
 *
 * This is copied from the Chromium OS Open Source Embedded Controller code.
 */
enum {
	/* The header byte, which follows the preamble */
	MSG_HEADER	= 0xec,

	MSG_HEADER_BYTES	= 3,
	MSG_TRAILER_BYTES	= 2,
	MSG_PROTO_BYTES		= MSG_HEADER_BYTES + MSG_TRAILER_BYTES,

	/* Max length of messages */
	MSG_BYTES		= EC_HOST_PARAM_SIZE + MSG_PROTO_BYTES,
};

#endif /* _CROS_MESSAGE_H */
