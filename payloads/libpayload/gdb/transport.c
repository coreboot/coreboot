/*
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <endian.h>
#include <gdb.h>
#include <libpayload.h>

/* MMIO word size is not standardized, but *usually* 32 (even on ARM64) */
typedef u32 mmio_word_t;

static const int timeout_us = 100 * 1000;
static const char output_overrun[] = "GDB output buffer overrun (try "
				     "increasing reply.size)!\n";

/* Serial-specific glue code... add more transport layers here when desired. */

static void gdb_raw_putchar(u8 c)
{
	serial_putchar(c);
}

static int gdb_raw_getchar(void)
{
	u64 start = timer_us(0);

	while (!serial_havechar())
		if (timer_us(start) > timeout_us)
			return -1;

	return serial_getchar();
}

void gdb_transport_init(void)
{
	console_remove_output_driver(serial_putchar);
}

void gdb_transport_teardown(void)
{
	serial_console_init();
}

/* Hex digit character <-> number conversion (illegal chars undefined!). */

static u8 from_hex(unsigned char c)
{
	static const s8 values[] = {
		-1, 10, 11, 12, 13, 14, 15, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		 0,  1,  2,  3,  4,  5,  6,  7,
		 8,  9, -1, -1, -1, -1, -1, -1,
	};

	return values[c & 0x1f];
}

static char to_hex(u8 v)
{
	static const char digits[] = "0123456789abcdef";

	return digits[v & 0xf];
}

/* Message encode/decode functions (must access whole aligned words for MMIO) */

void gdb_message_encode_bytes(struct gdb_message *message, const void *data,
			      int length)
{
	die_if(message->used + length * 2 > message->size, output_overrun);
	const mmio_word_t *aligned =
		(mmio_word_t *)ALIGN_DOWN((uintptr_t)data, sizeof(*aligned));
	mmio_word_t word = be32toh(readl(aligned++));
	while (length--) {
		u8 byte = (word >> ((((void *)aligned - data) - 1) * 8));
		message->buf[message->used++] = to_hex(byte >> 4);
		message->buf[message->used++] = to_hex(byte & 0xf);
		if (length && ++data == (void *)aligned)
			word = be32toh(readl(aligned++));
	}
}

void gdb_message_decode_bytes(const struct gdb_message *message, int offset,
			      void *data, int length)
{
	die_if(offset + 2 * length > message->used, "Decode overrun in GDB "
		"message: %.*s", message->used, message->buf);
	mmio_word_t *aligned =
		(mmio_word_t *)ALIGN_DOWN((uintptr_t)data, sizeof(*aligned));
	int shift = ((void *)(aligned + 1) - data) * 8;
	mmio_word_t word = be32toh(readl(aligned)) >> shift;
	while (length--) {
		word <<= 8;
		word |= from_hex(message->buf[offset++]) << 4;
		word |= from_hex(message->buf[offset++]);
		if (++data - (void *)aligned == sizeof(*aligned))
			writel(htobe32(word), aligned++);
	}
	if (data != (void *)aligned) {
		shift = ((void *)(aligned + 1) - data) * 8;
		clrsetbits_be32(aligned, ~((1 << shift) - 1), word << shift);
	}
}

void gdb_message_encode_zero_bytes(struct gdb_message *message, int length)
{
	die_if(message->used + length * 2 > message->size, output_overrun);
	memset(message->buf + message->used, '0', length * 2);
	message->used += length * 2;
}

void gdb_message_add_string(struct gdb_message *message, const char *string)
{
	message->used += strlcpy((char *)message->buf + message->used,
			     string, message->size - message->used);

	/* Check >= instead of > to account for strlcpy's trailing '\0'. */
	die_if(message->used >= message->size, output_overrun);
}

void gdb_message_encode_int(struct gdb_message *message, uintptr_t val)
{
	int length = sizeof(uintptr_t) * 2 - __builtin_clz(val) / 4;
	die_if(message->used + length > message->size, output_overrun);
	while (length--)
		message->buf[message->used++] =
			to_hex((val >> length * 4) & 0xf);
}

uintptr_t gdb_message_decode_int(const struct gdb_message *message, int offset,
				 int length)
{
	uintptr_t val = 0;

	die_if(length > sizeof(uintptr_t) * 2, "GDB decoding invalid number: "
	       "%.*s", message->used, message->buf);

	while (length--) {
		val <<= 4;
		val |= from_hex(message->buf[offset++]);
	}

	return val;
}

/* Like strtok/strsep: writes back offset argument, returns original offset. */
int gdb_message_tokenize(const struct gdb_message *message, int *offset)
{
	int token = *offset;
	while (!strchr(",;:", message->buf[(*offset)++]))
		die_if(*offset >= message->used, "Undelimited token in GDB "
				"message at offset %d: %.*s",
				token, message->used, message->buf);
	return token;
}

/* High-level send/receive functions. */

void gdb_get_command(struct gdb_message *command)
{
	enum command_state {
		STATE_WAITING,
		STATE_COMMAND,
		STATE_CHECKSUM0,
		STATE_CHECKSUM1,
	};

	u8 checksum = 0;
	u8 running_checksum = 0;
	enum command_state state = STATE_WAITING;

	while (1) {
		int c = gdb_raw_getchar();
		if (c < 0) {
			/*
			 * Timeout waiting for a byte. Reset the
			 * state machine.
			 */
			state = STATE_WAITING;
			continue;
		}

		switch (state) {
		case STATE_WAITING:
			if (c == '$') {
				running_checksum = 0;
				command->used = 0;
				state = STATE_COMMAND;
			}
			break;
		case STATE_COMMAND:
			if (c == '#') {
				state = STATE_CHECKSUM0;
				break;
			}
			die_if(command->used >= command->size, "GDB input buf"
			       "fer overrun (try increasing command.size)!\n");
			command->buf[command->used++] = c;
			running_checksum += c;
			break;
		case STATE_CHECKSUM0:
			checksum = from_hex(c) << 4;
			state = STATE_CHECKSUM1;
			break;
		case STATE_CHECKSUM1:
			checksum += from_hex(c);
			if (running_checksum == checksum) {
				gdb_raw_putchar('+');
				return;
			} else {
				state = STATE_WAITING;
				gdb_raw_putchar('-');
			}
			break;
		}
	}
}

void gdb_send_reply(const struct gdb_message *reply)
{
	int i;
	int retries = 1 * 1000 * 1000 / timeout_us;
	u8 checksum = 0;

	for (i = 0; i < reply->used; i++)
		checksum += reply->buf[i];

	do {
		gdb_raw_putchar('$');
		for (i = 0; i < reply->used; i++)
			gdb_raw_putchar(reply->buf[i]);
		gdb_raw_putchar('#');
		gdb_raw_putchar(to_hex(checksum >> 4));
		gdb_raw_putchar(to_hex(checksum & 0xf));
	} while (gdb_raw_getchar() != '+' && retries--);
}
