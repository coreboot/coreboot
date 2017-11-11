/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#include <AGESA.h>
#include <FchPlatform.h>
#include <baseboard/variants.h>
#include <soc/smi.h>
#include <soc/southbridge.h>
#include <stdlib.h>

const GPIO_CONTROL __attribute__((weak)) agesa_board_gpios[] = {

};

/*
 * GPE setup table must match ACPI GPE ASL
 *  { gevent, gpe, direction, level }
 */
static const struct sci_source gpe_table[] = {


};

const __attribute__((weak)) struct sci_source *get_gpe_table(size_t *num)
{
	*num = ARRAY_SIZE(gpe_table);
	return gpe_table;
}
