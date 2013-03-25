/* Public Domain Curses */
/* This file is BSD licensed, Copyright 2011 secunet AG */

#include "lppdc.h"
#include <curses.h>
#include <libpayload.h>

int curses_flags = F_ENABLE_SERIAL | F_ENABLE_CONSOLE;

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));

#ifdef CONFIG_SPEAKER
    speaker_tone(1760, 500); /* 1760 == note A6 */
#endif
}

void PDC_napms(int ms)
{
    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    mdelay(ms);
}

const char *PDC_sysname(void)
{
    return "LIBPAYLOAD";
}

void curses_enable_serial(int enable)
{
    curses_flags = (curses_flags & ~F_ENABLE_SERIAL) | (enable * F_ENABLE_SERIAL);
}

void curses_enable_vga(int enable)
{
    curses_flags = (curses_flags & ~F_ENABLE_CONSOLE) | (enable * F_ENABLE_CONSOLE);
}

int curses_serial_enabled(void)
{
    return !!(curses_flags & F_ENABLE_SERIAL);
}

int curses_vga_enabled(void)
{
    return !!(curses_flags & F_ENABLE_CONSOLE);
}

