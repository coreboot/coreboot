/*
* Copyright (C) 2015 Broadcom Corporation
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/


#ifndef __SOC_BROADCOM_CYGNUS_CONFIG_H__
#define __SOC_BROADCOM_CYGNUS_CONFIG_H__

#include <stdint.h>
#include <string.h>
#include <soc/halapis/ddr_regs.h>

/* DDR shmoo Parameters */
#define SDI_INTERFACE_BITWIDTH	16
#define SDI_NUM_COLUMNS			1024
#define SDI_NUM_BANKS			8

#ifdef DDR3_SIZE_512MB
#define SDI_NUM_ROWS			32768
#else
#define SDI_NUM_ROWS			65536
#endif

/* Idle count (in units of 1024 cycles) before auto entering self-refresh  */
#define DDR_AUTO_SELF_REFRESH_IDLE_COUNT	16

#endif /* __SOC_BROADCOM_CYGNUS_CONFIG_H__ */
