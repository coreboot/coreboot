/*
 *  blantantly copied from linux/kernel/printk.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 */

#ifndef lint
static char rcsid[] = "$Id$";
#endif

//typedef void * va_list;

#include <stdarg.h>

static char buf[1024];

/* printk's without a loglevel use this.. */
#define DEFAULT_MESSAGE_LOGLEVEL 4 /* KERN_WARNING */

/* We show everything that is MORE important than this.. */
#define MINIMUM_CONSOLE_LOGLEVEL 1 /* Minimum loglevel we let people use */

#ifndef DEFAULT_CONSOLE_LOGLEVEL
#define DEFAULT_CONSOLE_LOGLEVEL 8 /* anything MORE serious than KERN_WARNING */
#endif

/* Keep together for sysctl support */

int console_loglevel = DEFAULT_CONSOLE_LOGLEVEL;
int default_message_loglevel = DEFAULT_MESSAGE_LOGLEVEL;
int minimum_console_loglevel = MINIMUM_CONSOLE_LOGLEVEL;
int default_console_loglevel = DEFAULT_CONSOLE_LOGLEVEL;

void display(char*);
extern int vsprintf(char *buf, const char *, va_list);

int printk(const char *fmt, ...)
{
	va_list args;
	int i;
	char *p;
	int msg_level;

	va_start(args, fmt);
	i = vsprintf(buf, fmt, args); /* hopefully i < sizeof(buf)-4 */
	va_end(args);
	p = buf;
	if (
		p[0] == '<' &&
		p[1] > '0' && 
		p[1] <= '9' &&
		p[2] == '>'
	) {
		msg_level = p[1] - '0';
		p +=3;
	} else {
		msg_level = default_message_loglevel;
	}
	if (msg_level < console_loglevel) {
		display(p);
	}
	return i;
}

