/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <soc/early_init.h>
#include <stdlib.h>
#include <string.h>

static struct early_init_data *find_early_init(void)
{
	assert(sizeof(struct early_init_data) <= REGION_SIZE(early_init_data));
	return (struct early_init_data *)_early_init_data;
}

void early_init_clear(void)
{
	struct early_init_data *data = find_early_init();

	if (!data)
		return;

	memset(data, 0, sizeof(*data));
}

void early_init_save_time(enum early_init_type init_type)
{
	struct early_init_data *data = find_early_init();

	if (!data)
		return;

	timer_monotonic_get(&data->init_time[init_type]);
}

uint64_t early_init_get_elapsed_time_us(enum early_init_type init_type)
{
	struct early_init_data *data = find_early_init();
	struct mono_time cur_time;

	if (!data)
		return 0;

	memset(&cur_time, 0, sizeof(cur_time));

	/* If early init data was never saved */
	if (!memcmp(&data->init_time[init_type], &cur_time, sizeof(cur_time)))
		return 0;

	timer_monotonic_get(&cur_time);

	return mono_time_diff_microseconds(&data->init_time[init_type],
					   &cur_time);
}
