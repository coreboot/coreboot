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

#include <commonlib/region.h>

/* Determine if flash device is write protected */
int nvm_is_write_protected(void);

/* Apply protection to a range of flash */
int nvm_protect(const struct region *region);

#endif /* _COMMON_NVM_H_ */
