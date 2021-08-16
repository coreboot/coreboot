/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef EVENTLOG_H_
#define EVENTLOG_H_

#include <stdint.h>

struct event_header;
struct buffer;

void eventlog_print_event(const struct event_header *event, int count);
int eventlog_init_event(const struct buffer *buf, uint8_t type,
			const void *data, int data_size);

#endif // EVENTLOG_H_
