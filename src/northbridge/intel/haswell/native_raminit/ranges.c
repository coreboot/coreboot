/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <types.h>

#include "ranges.h"

void linear_record_pass(
	struct linear_train_data *const data,
	const bool pass,
	const int32_t value,
	const int32_t start,
	const int32_t step)
{
	/* If this is the first time, initialize all values */
	if (value == start) {
		/*
		 * If value passed, create a zero-length region for the current value,
		 * which may be extended as long as the successive values are passing.
		 *
		 * Otherwise, create a zero-length range for the preceding value. This
		 * range cannot be extended by other passing values, which is desired.
		 */
		data->current.start = start - (pass ? 0 : step);
		data->current.end   = data->current.start;
		data->largest       = data->current;
	} else if (pass) {
		/* If this pass is not contiguous, it belongs to a new region */
		if (data->current.end != (value - step))
			data->current.start = value;

		/* Update end of current region */
		data->current.end = value;

		/* Update largest region */
		if (range_width(data->current) > range_width(data->largest))
			data->largest = data->current;
	}
}

void phase_record_pass(
	struct phase_train_data *const data,
	const bool pass,
	const int32_t value,
	const int32_t start,
	const int32_t step)
{
	/* If this is the first time, initialize all values */
	if (value == start) {
		/*
		 * If value passed, create a zero-length region for the current value,
		 * which may be extended as long as the successive values are passing.
		 *
		 * Otherwise, create a zero-length range for the preceding value. This
		 * range cannot be extended by other passing values, which is desired.
		 */
		data->current.start = start - (pass ? 0 : step);
		data->current.end   = data->current.start;
		data->largest       = data->current;
		data->initial       = data->current;
		return;
	}
	if (!pass)
		return;

	/* Update initial region */
	if (data->initial.end == (value - step))
		data->initial.end = value;

	/* If this pass is not contiguous, it belongs to a new region */
	if (data->current.end != (value - step))
		data->current.start = value;

	/* Update end of current region */
	data->current.end = value;

	/* Update largest region */
	if (range_width(data->current) > range_width(data->largest))
		data->largest = data->current;
}

void phase_append_initial_to_current(
	struct phase_train_data *const data,
	const int32_t start,
	const int32_t step)
{
	/* If initial region is valid and does not overlap, append it */
	if (data->initial.start == start && data->initial.end != data->current.end)
		data->current.end += step + range_width(data->initial);

	/* Update largest region */
	if (range_width(data->current) > range_width(data->largest))
		data->largest = data->current;
}

void phase_append_current_to_initial(
	struct phase_train_data *const data,
	const int32_t start,
	const int32_t step)
{
	/* If initial region is valid and does not overlap, append it */
	if (data->initial.start == start && data->initial.end != data->current.end) {
		data->initial.start -= (step + range_width(data->current));
		data->current = data->initial;
	}

	/* Update largest region */
	if (range_width(data->current) > range_width(data->largest))
		data->largest = data->current;
}
