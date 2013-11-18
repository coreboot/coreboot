/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _BAYTRAIL_SPI_H_
#define _BAYTRAIL_SPI_H_

/* These registers live behind SPI_BASE_ADDRESS. */
#define BCR				0xfc
# define SRC_MASK			(0x3 << 2)
# define SRC_CACHE_NO_PREFETCH		(0x0 << 2)
# define SRC_NO_CACHE_NO_PREFETCH	(0x1 << 2)
# define SRC_CACHE_PREFETCH		(0x2 << 2)

#endif /* _BAYTRAIL_SPI_H_ */

