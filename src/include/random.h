/*
 * This file is part of the coreboot project.
 *
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
#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <stdint.h>

/*
 * Generates a 32/64 bit random number respectively.
 * return 0 on success and -1 on error.
 */
int get_random_number_32(uint32_t *rand);
int get_random_number_64(uint64_t *rand);

#endif /* _RANDOM_H_ */
