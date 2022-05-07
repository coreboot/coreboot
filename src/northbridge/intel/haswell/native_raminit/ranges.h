/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef HASWELL_RAMINIT_RANGES_H
#define HASWELL_RAMINIT_RANGES_H

#include <types.h>

/*
 * Many algorithms shmoo some parameter to determine the largest passing
 * range. Provide a common implementation to avoid redundant boilerplate.
 */
struct passing_range {
	int32_t start;
	int32_t end;
};

/* Structure for linear parameters, such as roundtrip delays */
struct linear_train_data {
	struct passing_range current;
	struct passing_range largest;
};

/*
 * Phase ranges are "circular": the first and last indices are contiguous.
 * To correctly determine the largest passing range, one has to combine
 * the initial range and the current range when processing the last index.
 */
struct phase_train_data {
	struct passing_range initial;
	struct passing_range current;
	struct passing_range largest;
};

static inline int32_t range_width(const struct passing_range range)
{
	return range.end - range.start;
}

static inline int32_t range_center(const struct passing_range range)
{
	return range.start + range_width(range) / 2;
}

void linear_record_pass(
	struct linear_train_data *data,
	bool pass,
	int32_t value,
	int32_t start,
	int32_t step);

void phase_record_pass(
	struct phase_train_data *data,
	bool pass,
	int32_t value,
	int32_t start,
	int32_t step);

void phase_append_initial_to_current(
	struct phase_train_data *data,
	int32_t start,
	int32_t step);

void phase_append_current_to_initial(
	struct phase_train_data *data,
	int32_t start,
	int32_t step);

#endif
