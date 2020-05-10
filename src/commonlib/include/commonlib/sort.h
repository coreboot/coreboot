/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _COMMONLIB_SORT_H_
#define _COMMONLIB_SORT_H_

#include <stddef.h>

typedef enum {
	NUM_ASCENDING,
	NUM_DESCENDING
} sort_order_t;

void bubblesort(int *v, size_t num_entries, sort_order_t order);

#endif /* _COMMONLIB_SORT_H_ */
