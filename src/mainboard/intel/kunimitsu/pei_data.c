/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <stdint.h>
#include <string.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include "boardid.h"
#include "spd/spd.h"

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	mainboard_fill_dq_map_data(&pei_data->dq_map);
	mainboard_fill_dqs_map_data(&pei_data->dqs_map);
	mainboard_fill_rcomp_res_data(&pei_data->RcompResistor);
	mainboard_fill_rcomp_strength_data(&pei_data->RcompTarget);
}
