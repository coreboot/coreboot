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

#include <hsio.h>
#include <soc/fiamux.h>

size_t mainboard_get_hsio_config(BL_HSIO_INFORMATION **p_hsio_config)
{
    size_t num;
    num = ARRAY_SIZE(tagada_hsio_config);
    (*p_hsio_config) = (BL_HSIO_INFORMATION *)tagada_hsio_config;
    return num;
}
