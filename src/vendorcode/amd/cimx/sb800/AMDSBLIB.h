/**
 * @file
 *
 * Southbridge IO access common routine define file
 *
 *
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


//AMDSBLIB Routines

/**
 * SbStall - Delay routine
 *
 *
 *
 * @param[in] uSec
 *
 */
VOID  SbStall (IN UINT32 uSec);

/**
 * SbReset - Generate a reset command
 *
 *
 *
 * @param[in] OpFlag - Dummy
 *
 */
VOID SbReset (IN UINT8 OpFlag);

/**
 * outPort80 - Send data to PORT 80 (debug port)
 *
 *
 *
 * @param[in] pcode - debug code (32 bits)
 *
 */
VOID outPort80 (IN UINT32 pcode);

/**
 * getEfuseStatue - Get Efuse status
 *
 *
 * @param[in] Value - Return Chip strap status
 *
 */
VOID getEfuseStatus (IN VOID* Value);

/**
 * AmdSbDispatcher - Dispatch Southbridge function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
AGESA_STATUS  AmdSbDispatcher (IN VOID *pConfig);

/**
 * AmdSbCopyMem - Memory copy
 *
 * @param[in] pDest - Destance address point
 * @param[in] pSource - Source Address point
 * @param[in] Length - Data length
 *
 */
VOID AmdSbCopyMem (IN VOID* pDest, IN VOID* pSource, IN UINTN Length);
