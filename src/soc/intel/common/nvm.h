/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _COMMON_NVM_H_
#define _COMMON_NVM_H_

#include <stddef.h>

/* Determine if area is erased. returns 1 if erased. 0 otherwise. */
int nvm_is_erased(const void *start, size_t size);

/* Erase region according to start and size. Returns < 0 on error else 0. */
int nvm_erase(void *start, size_t size);

/* Write data to NVM. Returns 0 on success < 0 on error.  */
int nvm_write(void *start, const void *data, size_t size);

/* Determine if flash device is write protected */
int nvm_is_write_protected(void);

/* Apply protection to a range of flash */
int nvm_protect(void *start, size_t size);

#endif /* _COMMON_NVM_H_ */
