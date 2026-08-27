/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_EFI_OPTION_H
#define DRIVERS_EFI_OPTION_H

#include <types.h>

enum cb_err efi_option_get_uint(const char *name, uint32_t *value);
enum cb_err efi_option_set_uint(const char *name, uint32_t value);
enum cb_err efi_option_initialize_store(void);

#endif /* DRIVERS_EFI_OPTION_H */
