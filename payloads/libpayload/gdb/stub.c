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

#include <gdb.h>
#include <libpayload.h>

struct gdb_state gdb_state;

static u8 reply_buf[2048];
static u8 command_buf[2048];

static struct gdb_message command = {
	.buf = command_buf,
	.used = 0,
	.size = sizeof(command_buf),
};
static struct gdb_message reply = {
	.buf = reply_buf,
	.used = 0,
	.size = sizeof(reply_buf),
};

void gdb_command_loop(u8 signal)
{
	if (gdb_state.resumed) {
		/* We were just running. Send a stop reply. */
		reply.used = 0;
		gdb_message_add_string(&reply, "S");
		gdb_message_encode_bytes(&reply, &signal, 1);
		gdb_send_reply(&reply);

	}
	gdb_state.signal = signal;
	gdb_state.resumed = 0;
	gdb_state.connected = 1;

	while (1) {
		int i;

		gdb_get_command(&command);

		reply.used = 0;
		for (i = 0; i < gdb_command_count; i++) {
			int clen = strlen(gdb_commands[i].str);
			if (!strncmp(gdb_commands[i].str, (char *)command.buf,
				     MIN(clen, command.used))) {
				gdb_commands[i].handler(&command, clen, &reply);
				break;
			}
		}

		/* If we're resuming, we won't send a reply until we stop. */
		if (gdb_state.resumed)
			return;

		gdb_send_reply(&reply);
	}
}

static void gdb_output_write(const void *buffer, size_t count)
{
	if (!gdb_state.resumed) {
		/* Must be a die_if() in GDB (or a bug), so bail out and die. */
		gdb_exit(-1);
		if (CONFIG(LP_VIDEO_CONSOLE))
			video_console_init();
		puts("GDB died, redirecting its last words to the screen:\n");
		console_write(buffer, count);
	} else {
		reply.used = 0;
		reply.buf[reply.used++] = 'O';
		gdb_message_encode_bytes(&reply, buffer, count);
		gdb_send_reply(&reply);
	}
}

static struct console_output_driver gdb_output_driver = {
	.write = &gdb_output_write
};

static void gdb_init(void)
{
	printf("Ready for GDB connection.\n");
	gdb_transport_init();
	gdb_arch_init();
	console_add_output_driver(&gdb_output_driver);
}

void gdb_enter(void)
{
	if (!gdb_state.connected)
		gdb_init();
	gdb_arch_enter();
}

void gdb_exit(s8 exit_status)
{
	if (!gdb_state.connected)
		return;

	reply.used = 0;
	gdb_message_add_string(&reply, "W");
	gdb_message_encode_bytes(&reply, &exit_status, 1);
	gdb_send_reply(&reply);

	console_remove_output_driver(&gdb_output_write);
	gdb_transport_teardown();
	gdb_state.connected = 0;
	printf("Detached from GDB connection.\n");
}

/*
 * This is a check architecture backends can run before entering the GDB command
 * loop during exception handling. If it returns true, GDB was already running
 * and must have caused an exception itself, which may happen if the GDB server
 * tells us to do something stupid (e.g. write to an unmapped address). In that
 * case, all we can do is blindly send a generic error code (since we're not
 * sure which command caused the exception) and continue serving commands. When
 * GDB eventually tells us to resume, we'll return from this function to the
 * architecture backend which will have to do a "super exception return" that
 * returns right back from the original (outermost) exception, "jumping over"
 * all the intermediate exception frames we may have accumulated since. (This is
 * the best we can do because our architecture backends generally don't support
 * "full", unlimited exception reentrancy.)
 */
int gdb_handle_reentrant_exception(void)
{
	if (!gdb_state.connected || gdb_state.resumed)
		return 0;	/* This is not a reentrant exception. */

	static const char error_code[] = "E22";	/* EINVAL? */
	static const struct gdb_message tmp_reply = {
		.buf = (u8 *)error_code,
		.used = sizeof(error_code),
		.size = sizeof(error_code),
	};
	gdb_send_reply(&tmp_reply);
	gdb_command_loop(gdb_state.signal);	/* preserve old signal */
	return 1;
}
