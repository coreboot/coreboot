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

#ifndef _GDB_H_
#define _GDB_H_

#include <stdint.h>

struct gdb_message
{
	u8 *buf;
	int used;
	const int size;
};

struct gdb_state
{
	u8 signal;
	u8 resumed	: 1;
	u8 connected	: 1;
};
extern struct gdb_state gdb_state;

typedef void (*gdb_command_handler)(struct gdb_message *command,
				    int offset, struct gdb_message *reply);
struct gdb_command
{
	const char *str;
	gdb_command_handler handler;
};
extern struct gdb_command gdb_commands[];
extern const int gdb_command_count;

/* arch/gdb.c */

void gdb_arch_init(void);
void gdb_arch_enter(void);

int gdb_arch_set_single_step(int on);

void gdb_arch_encode_regs(struct gdb_message *message);
void gdb_arch_decode_regs(int offset, struct gdb_message *message);

/* gdb/transport.c */

void gdb_transport_init(void);
void gdb_transport_teardown(void);

void gdb_message_encode_bytes(struct gdb_message *message, const void *data,
			      int length);
void gdb_message_decode_bytes(const struct gdb_message *message, int offset,
			      void *data, int length);
void gdb_message_encode_zero_bytes(struct gdb_message *message, int length);

void gdb_message_add_string(struct gdb_message *message, const char *string);

void gdb_message_encode_int(struct gdb_message *message, uintptr_t val);
uintptr_t gdb_message_decode_int(const struct gdb_message *message, int offset,
				 int length);

int gdb_message_tokenize(const struct gdb_message *message, int *offset);

void gdb_get_command(struct gdb_message *command);
void gdb_send_reply(const struct gdb_message *reply);

/* gdb/stub.c */

void gdb_command_loop(uint8_t signal);

enum {
	GDB_SIG0 = 0, /* Signal 0 */
	GDB_SIGHUP = 1, /* Hangup */
	GDB_SIGINT = 2, /* Interrupt */
	GDB_SIGQUIT = 3, /* Quit */
	GDB_SIGILL = 4, /* Illegal instruction */
	GDB_SIGTRAP = 5, /* Trace/breakpoint trap */
	GDB_SIGABRT = 6, /* Aborted */
	GDB_SIGEMT = 7, /* Emulation trap */
	GDB_SIGFPE = 8, /* Arithmetic exception */
	GDB_SIGKILL = 9, /* Killed */
	GDB_SIGBUS = 10, /* Bus error */
	GDB_SIGSEGV = 11, /* Segmentation fault */
	GDB_SIGSYS = 12, /* Bad system call */
	GDB_SIGPIPE = 13, /* Broken pipe */
	GDB_SIGALRM = 14, /* Alarm clock */
	GDB_SIGTERM = 15, /* Terminated */
	GDB_SIGURG = 16, /* Urgent I/O condition */
	GDB_SIGSTOP = 17, /* Stopped (signal) */
	GDB_SIGTSTP = 18, /* Stopped (user) */
	GDB_SIGCONT = 19, /* Continued */
	GDB_SIGCHLD = 20, /* Child status changed */
	GDB_SIGTTIN = 21, /* Stopped (ttyinput) */
	GDB_SIGTTOU = 22, /* Stopped (ttyoutput) */
	GDB_SIGIO = 23, /* I/O possible */
	GDB_SIGXCPU = 24, /* CPU time limit exceeded */
	GDB_SIGXFSZ = 25, /* File size limit exceeded */
	GDB_SIGVTALRM = 26, /* Virtual timer expired */
	GDB_SIGPROF = 27, /* Profiling timer expired */
	GDB_SIGWINCH = 28, /* Window size changed */
	GDB_SIGLOST = 29, /* Resource lost */
	GDB_SIGUSR1 = 30, /* User defined signal1 */
	GDB_SUGUSR2 = 31, /* User defined signal2 */
	GDB_SIGPWR = 32, /* Powerfail/restart */
	GDB_SIGPOLL = 33, /* Pollable event occurred */
	GDB_SIGWIND = 34, /* SIGWIND */
	GDB_SIGPHONE = 35, /* SIGPHONE */
	GDB_SIGWAITING = 36, /* Process's LWPs are blocked */
	GDB_SIGLWP = 37, /* Signal LWP */
	GDB_SIGDANGER = 38, /* Swap space dangerously low */
	GDB_SIGGRANT = 39, /* Monitor mode granted */
	GDB_SIGRETRACT = 40, /* Need to relinquish monitor mode */
	GDB_SIGMSG = 41, /* Monitor mode data available */
	GDB_SIGSOUND = 42, /* Sound completed */
	GDB_SIGSAK = 43, /* Secure attention */
	GDB_SIGPRIO = 44, /* SIGPRIO */

	GDB_SIG33 = 45, /* Real-timeevent 33 */
	GDB_SIG34 = 46, /* Real-timeevent 34 */
	GDB_SIG35 = 47, /* Real-timeevent 35 */
	GDB_SIG36 = 48, /* Real-timeevent 36 */
	GDB_SIG37 = 49, /* Real-timeevent 37 */
	GDB_SIG38 = 50, /* Real-timeevent 38 */
	GDB_SIG39 = 51, /* Real-timeevent 39 */
	GDB_SIG40 = 52, /* Real-timeevent 40 */
	GDB_SIG41 = 53, /* Real-timeevent 41 */
	GDB_SIG42 = 54, /* Real-timeevent 42 */
	GDB_SIG43 = 55, /* Real-timeevent 43 */
	GDB_SIG44 = 56, /* Real-timeevent 44 */
	GDB_SIG45 = 57, /* Real-timeevent 45 */
	GDB_SIG46 = 58, /* Real-timeevent 46 */
	GDB_SIG47 = 59, /* Real-timeevent 47 */
	GDB_SIG48 = 60, /* Real-timeevent 48 */
	GDB_SIG49 = 61, /* Real-timeevent 49 */
	GDB_SIG50 = 62, /* Real-timeevent 50 */
	GDB_SIG51 = 63, /* Real-timeevent 51 */
	GDB_SIG52 = 64, /* Real-timeevent 52 */
	GDB_SIG53 = 65, /* Real-timeevent 53 */
	GDB_SIG54 = 66, /* Real-timeevent 54 */
	GDB_SIG55 = 67, /* Real-timeevent 55 */
	GDB_SIG56 = 68, /* Real-timeevent 56 */
	GDB_SIG57 = 69, /* Real-timeevent 57 */
	GDB_SIG58 = 70, /* Real-timeevent 58 */
	GDB_SIG59 = 71, /* Real-timeevent 59 */
	GDB_SIG60 = 72, /* Real-timeevent 60 */
	GDB_SIG61 = 73, /* Real-timeevent 61 */
	GDB_SIG62 = 74, /* Real-timeevent 62 */
	GDB_SIG63 = 75, /* Real-timeevent 63 */
	GDB_SIGCANCEL = 76, /* LWP internal signal */
	GDB_SIG32 = 77, /* Real-timeevent 32 */
	GDB_SIG64 = 78, /* Real-timeevent 64 */
	GDB_SIG65 = 79, /* Real-timeevent 65 */
	GDB_SIG66 = 80, /* Real-timeevent 66 */
	GDB_SIG67 = 81, /* Real-timeevent 67 */
	GDB_SIG68 = 82, /* Real-timeevent 68 */
	GDB_SIG69 = 83, /* Real-timeevent 69 */
	GDB_SIG70 = 84, /* Real-timeevent 70 */
	GDB_SIG71 = 85, /* Real-timeevent 71 */
	GDB_SIG72 = 86, /* Real-timeevent 72 */
	GDB_SIG73 = 87, /* Real-timeevent 73 */
	GDB_SIG74 = 88, /* Real-timeevent 74 */
	GDB_SIG75 = 89, /* Real-timeevent 75 */
	GDB_SIG76 = 90, /* Real-timeevent 76 */
	GDB_SIG77 = 91, /* Real-timeevent 77 */
	GDB_SIG78 = 92, /* Real-timeevent 78 */
	GDB_SIG79 = 93, /* Real-timeevent 79 */
	GDB_SIG80 = 94, /* Real-timeevent 80 */
	GDB_SIG81 = 95, /* Real-timeevent 81 */
	GDB_SIG82 = 96, /* Real-timeevent 82 */
	GDB_SIG83 = 97, /* Real-timeevent 83 */
	GDB_SIG84 = 98, /* Real-timeevent 84 */
	GDB_SIG85 = 99, /* Real-timeevent 85 */
	GDB_SIG86 = 100, /* Real-timeevent 86 */
	GDB_SIG87 = 101, /* Real-timeevent 87 */
	GDB_SIG88 = 102, /* Real-timeevent 88 */
	GDB_SIG89 = 103, /* Real-timeevent 89 */
	GDB_SIG90 = 104, /* Real-timeevent 90 */
	GDB_SIG91 = 105, /* Real-timeevent 91 */
	GDB_SIG92 = 106, /* Real-timeevent 92 */
	GDB_SIG93 = 107, /* Real-timeevent 93 */
	GDB_SIG94 = 108, /* Real-timeevent 94 */
	GDB_SIG95 = 109, /* Real-timeevent 95 */
	GDB_SIG96 = 110, /* Real-timeevent 96 */
	GDB_SIG97 = 111, /* Real-timeevent 97 */
	GDB_SIG98 = 112, /* Real-timeevent 98 */
	GDB_SIG99 = 113, /* Real-timeevent 99 */
	GDB_SIG100 = 114, /* Real-timeevent 100 */
	GDB_SIG101 = 115, /* Real-timeevent 101 */
	GDB_SIG102 = 116, /* Real-timeevent 102 */
	GDB_SIG103 = 117, /* Real-timeevent 103 */
	GDB_SIG104 = 118, /* Real-timeevent 104 */
	GDB_SIG105 = 119, /* Real-timeevent 105 */
	GDB_SIG106 = 120, /* Real-timeevent 106 */
	GDB_SIG107 = 121, /* Real-timeevent 107 */
	GDB_SIG108 = 122, /* Real-timeevent 108 */
	GDB_SIG109 = 123, /* Real-timeevent 109 */
	GDB_SIG110 = 124, /* Real-timeevent 110 */
	GDB_SIG111 = 125, /* Real-timeevent 111 */
	GDB_SIG112 = 126, /* Real-timeevent 112 */
	GDB_SIG113 = 127, /* Real-timeevent 113 */
	GDB_SIG114 = 128, /* Real-timeevent 114 */
	GDB_SIG115 = 129, /* Real-timeevent 115 */
	GDB_SIG116 = 130, /* Real-timeevent 116 */
	GDB_SIG117 = 131, /* Real-timeevent 117 */
	GDB_SIG118 = 132, /* Real-timeevent 118 */
	GDB_SIG119 = 133, /* Real-timeevent 119 */
	GDB_SIG120 = 134, /* Real-timeevent 120 */
	GDB_SIG121 = 135, /* Real-timeevent 121 */
	GDB_SIG122 = 136, /* Real-timeevent 122 */
	GDB_SIG123 = 137, /* Real-timeevent 123 */
	GDB_SIG124 = 138, /* Real-timeevent 124 */
	GDB_SIG125 = 139, /* Real-timeevent 125 */
	GDB_SIG126 = 140, /* Real-timeevent 126 */
	GDB_SIG127 = 141, /* Real-timeevent 127 */
	GDB_SIGINFO = 142, /* Information request */
	GDB_UNKNOWN1 = 43, /* Unknownsignal */
	GDB_DEFAULT = 144, /* error:defaultsignal */
	/* Machexceptions */
	GDB_EXC_BAD_ACCESS = 145, /* Could not access memory */
	GDB_EXC_BAD_INSTRCTION = 146, /* Illegal instruction/operand */
	GDB_EXC_ARITHMETIC = 147, /* Arithmetic exception */
	GDB_EXC_EMULATION = 148, /* Emulation instruction */
	GDB_EXC_SOFTWARE = 149, /* Software generated exception */
	GDB_EXC_BREAKPOINT = 150, /* Breakpoint */
};

#endif	/* _GDB_H_ */
