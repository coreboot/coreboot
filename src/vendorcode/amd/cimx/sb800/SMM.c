/**
 * @file
 *
 * Southbridge SMM service function
 *
 * Prepare SMM service module for IBV call Southbridge SMI service routine.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */

#include "SBPLATFORM.h"

//
// Declaration of local functions
//

/**
 * Southbridge SMI service module
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sbSmmService (
  IN       AMDSBCFG* pConfig
  )
{
  AMDSBCFG*   pTmp;      //lx-dummy for /W4 build
  pTmp = pConfig;
}

/**
 * softwareSMIservice - Software SMI service
 *
 * @param[in] VOID Southbridge software SMI service ID.
 *
 */
VOID
softwareSMIservice (
  IN       VOID
  )
{
}





