/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <ppc.h>

unsigned long get_timer_freq(void)
{
    return 100000000 / 4;
}

