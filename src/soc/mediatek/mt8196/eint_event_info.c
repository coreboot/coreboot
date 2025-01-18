/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <soc/addressmap.h>
#include <soc/eint_event.h>

const struct eint_event_info eint_event[] = {
	{EINT_E_BASE, 75},
	{EINT_S_BASE, 29},
	{EINT_W_BASE, 58},
	{EINT_N_BASE, 47},
	{EINT_C_BASE, 25},
	{},
};
