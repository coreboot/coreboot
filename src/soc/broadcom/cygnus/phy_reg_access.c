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

#include "soc/shmoo_and28/phy_reg_access.h"

uint32 REGRD (uint32 address) {

  volatile unsigned long data;

  data = (* (volatile uint32 *) ( ((uint32)GLOBAL_REG_RBUS_START) | (address)));
  //printf("REGRD %08X=%08X\n", address, data);
  return data;
}

uint32 REGWR (uint32 address, uint32 data) {

  ((* (volatile uint32 *) ( ((uint32)GLOBAL_REG_RBUS_START) | (address))) = data);
  //printf("REGWR %08X=%08X\n", address, data);
//  return SOC_E_NONE;
   return 0;
}
