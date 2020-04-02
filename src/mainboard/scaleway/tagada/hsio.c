/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <hsio.h>
#include <soc/fiamux.h>

size_t mainboard_get_hsio_config(BL_HSIO_INFORMATION **p_hsio_config)
{
    size_t num;
    num = ARRAY_SIZE(tagada_hsio_config);
    (*p_hsio_config) = (BL_HSIO_INFORMATION *)tagada_hsio_config;
    return num;
}
