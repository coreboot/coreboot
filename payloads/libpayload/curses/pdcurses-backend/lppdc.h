#include <libpayload-config.h>
#include <curses.h>
#include <curspriv.h>

extern int curses_flags;
extern unsigned char *pdc_atrtab;
extern short curstoreal[16], realtocurs[16];

#define F_ENABLE_CONSOLE 1
#define F_ENABLE_SERIAL 2
