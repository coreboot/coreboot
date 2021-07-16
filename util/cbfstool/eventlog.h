/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef EVENTLOG_H_
#define EVENTLOG_H_

struct event_header;

void eventlog_print_event(const struct event_header *event, int count);

#endif // EVENTLOG_H_
