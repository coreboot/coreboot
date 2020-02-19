/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <harcuvar_boardid.h>
#include <hsio.h>
#include <soc/fiamux.h>

size_t mainboard_get_hsio_config(BL_HSIO_INFORMATION **p_hsio_config)
{
    uint32_t boardid = board_id();
    size_t num;
    switch (boardid) {
    case BoardIdHarcuvar:
        num = ARRAY_SIZE(harcuvar_hsio_config);
        (*p_hsio_config) = (BL_HSIO_INFORMATION *)harcuvar_hsio_config;
        break;
    default:
        num = 0;
        (*p_hsio_config) = NULL;
        break;
    }
    return num;
}
