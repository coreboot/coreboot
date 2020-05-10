/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef FSP1_1_CAR_H
#define FSP1_1_CAR_H

#include <fsp/api.h>
#include <stdint.h>

/* Per stage calls from the above two functions. The void * return from
 * cache_as_ram_stage_main() is the stack pointer to use in RAM after
 * exiting cache-as-ram mode. */
void cache_as_ram_stage_main(FSP_INFO_HEADER *fih);
#endif
