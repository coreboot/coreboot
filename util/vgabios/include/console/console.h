/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CONSOLE_CONSOLE_H
#define _CONSOLE_CONSOLE_H

int printk(int msg_level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#endif
