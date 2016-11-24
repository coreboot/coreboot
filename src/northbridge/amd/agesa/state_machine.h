/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Kyösti Mälkki
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

#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <stdint.h>
#include <AGESA.h>

/* eventlog */
const char *agesa_struct_name(int state);
const char *heap_status_name(int status);
void agesawrapper_trace(AGESA_STATUS ret, AMD_CONFIG_PARAMS *StdHeader, const char *func);

struct sysinfo
{
	int s3resume;
};

void agesa_main(struct sysinfo *cb);
void agesa_postcar(struct sysinfo *cb);

void board_BeforeAgesa(struct sysinfo *cb);
void platform_once(struct sysinfo *cb);

#endif /* _STATE_MACHINE_H_ */
