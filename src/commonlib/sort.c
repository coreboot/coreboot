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

#include <commonlib/helpers.h>
#include <commonlib/sort.h>

/* Implement a simple Bubble sort algorithm. Reduce the needed number of
   iterations by taking care of already sorted entries in the list. */
void bubblesort(int *v, size_t num_entries, sort_order_t order)
{
	size_t i, j;
	int swapped;

	/* Make sure there are at least two entries to sort. */
	if (num_entries < 2)
		return;

	for (j = 0; j < num_entries - 1; j++) {
		swapped = 0;
		for (i = 0; i < num_entries - j - 1; i++) {
			switch (order) {
			case NUM_ASCENDING:
				if (v[i] > v[i + 1]) {
					SWAP(v[i], v[i + 1]);
					swapped = 1;
				}
				break;
			case NUM_DESCENDING:
				if (v[i] < v[i + 1]) {
					SWAP(v[i], v[i + 1]);
					swapped = 1;
				}
				break;
			default:
				return;
			}
		}
		if (!swapped)
			break;
	}
}
