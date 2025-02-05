/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _UX_LOCALES_H_
#define _UX_LOCALES_H_

#include <types.h>

enum ux_locale_msg {
	UX_LOCALE_MSG_MEMORY_TRAINING,
	UX_LOCALE_MSG_NUM,
};

/* Unmap the preram_locales if it has been mapped. No-op otherwise. */
void ux_locales_unmap(void);

/*
 * Get the localized text for a given message ID as per `enum ux_locale_msg`.
 * This function will try to read the language ID from vboot API, and search the
 * corresponding translation from CBFS preram_locales.
 */
const char *ux_locales_get_text(enum ux_locale_msg msg_id);

#endif  // _UX_LOCALES_H_
