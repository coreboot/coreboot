/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _UX_LOCALES_H_
#define _UX_LOCALES_H_

#include <types.h>

/* Unmap the preram_locales if it has been mapped. No-op otherwise. */
void ux_locales_unmap(void);

/*
 * Get the localized text for a given string name.
 * This function will try to read the language ID from vboot API, and search the
 * corresponding translation from CBFS preram_locales.
 */
const char *ux_locales_get_text(const char *name);

#endif  // _UX_LOCALES_H_
