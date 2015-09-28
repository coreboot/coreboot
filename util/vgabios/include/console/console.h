#ifndef _CONSOLE_CONSOLE_H
#define _CONSOLE_CONSOLE_H
#define CONFIG_X86EMU_DEBUG 1

int printk(int msg_level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#endif
