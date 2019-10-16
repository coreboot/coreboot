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

#ifndef FSP1_1_CAR_H
#define FSP1_1_CAR_H

#include <fsp/api.h>
#include <stdint.h>

/* Per stage calls from the above two functions. The void * return from
 * cache_as_ram_stage_main() is the stack pointer to use in RAM after
 * exiting cache-as-ram mode. */
void cache_as_ram_stage_main(FSP_INFO_HEADER *fih);
#endif
