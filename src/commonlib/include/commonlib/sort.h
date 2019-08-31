/*
 * This file is part of the coreboot project.
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
#ifndef _COMMONLIB_SORT_H_
#define _COMMONLIB_SORT_H_

#include <stddef.h>

typedef enum {
	NUM_ASCENDING,
	NUM_DESCENDING
} sort_order_t;

void bubblesort(int *v, size_t num_entries, sort_order_t order);

#endif /* _COMMONLIB_SORT_H_ */
