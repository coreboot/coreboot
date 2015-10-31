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

#include <arch/cache.h>
#include <gdb.h>
#include <libpayload.h>

static void gdb_get_last_signal(struct gdb_message *command,
				int offset, struct gdb_message *reply)
{
	gdb_message_add_string(reply, "S");
	gdb_message_encode_bytes(reply, &gdb_state.signal, 1);
}

static void gdb_read_general_registers(struct gdb_message *command,
				       int offset, struct gdb_message *reply)
{
	gdb_arch_encode_regs(reply);
}

static void gdb_write_general_registers(struct gdb_message *command,
					int offset, struct gdb_message *reply)
{
	gdb_arch_decode_regs(offset, command);
	gdb_message_add_string(reply, "OK");
}

static void gdb_read_memory(struct gdb_message *command,
			    int offset, struct gdb_message *reply)
{
	int tok = gdb_message_tokenize(command, &offset);
	uintptr_t addr = gdb_message_decode_int(command, tok, offset - 1 - tok);
	size_t length = gdb_message_decode_int(command, offset,
					       command->used - offset);

	gdb_message_encode_bytes(reply, (void *)addr, length);
}

static void gdb_write_memory(struct gdb_message *command,
			     int offset, struct gdb_message *reply)
{
	int tok = gdb_message_tokenize(command, &offset);
	uintptr_t addr = gdb_message_decode_int(command, tok, offset - 1 - tok);
	tok = gdb_message_tokenize(command, &offset);
	size_t length = gdb_message_decode_int(command, tok, offset - 1 - tok);

	die_if(length * 2 != command->used - offset, "Invalid length field in "
	       "GDB command: %.*s", command->used, command->buf);

	gdb_message_decode_bytes(command, offset, (void *)addr, length);
	cache_sync_instructions();
	gdb_message_add_string(reply, "OK");
}

static void gdb_continue(struct gdb_message *command,
			 int offset, struct gdb_message *reply)
{
	/* Disable single step if it's still on. */
	gdb_arch_set_single_step(0);

	/* No need to support the extension that passes in new EIP/PC. */
	if (command->used > offset)
		gdb_message_add_string(reply, "E00");
	else
		gdb_state.resumed = 1;
}

static void gdb_single_step(struct gdb_message *command,
			    int offset, struct gdb_message *reply)
{
	if (command->used > offset || gdb_arch_set_single_step(1))
		gdb_message_add_string(reply, "E00");
	else
		gdb_state.resumed = 1;
}

struct gdb_command gdb_commands[] = {
	{ "?", &gdb_get_last_signal },
	{ "g", &gdb_read_general_registers },
	{ "G", &gdb_write_general_registers },
	{ "m", &gdb_read_memory },
	{ "M", &gdb_write_memory },
	{ "c", &gdb_continue },
	{ "s", &gdb_single_step }
};
const int gdb_command_count = ARRAY_SIZE(gdb_commands);
