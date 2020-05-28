/* SPDX-License-Identifier: GPL-2.0-only */

#include <timestamp.h>

/* Stubs */
void timestamp_add_now(enum timestamp_id id)
{
}

void timestamp_add(enum timestamp_id id, uint64_t ts)
{
}

uint64_t timestamp_get(void)
{
	return 0;
}
