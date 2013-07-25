/* $NoKeywords:$ */
/**
 * @file
 *
 * Common Return routines.
 *
 * Routines which do nothing, returning a result (preferably some version of zero) which
 * is consistent with "do nothing" or "default".  Useful for function pointer tables.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Common
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ***************************************************************************
*
*/

#ifndef _COMMON_RETURNS_H_
#define _COMMON_RETURNS_H_


/**
* Return True
*
* @retval True    Default case, no special action
*/
BOOLEAN
CommonReturnTrue ( VOID );

/**
* Return False.
*
* @retval FALSE    Default case, no special action
*/
BOOLEAN
CommonReturnFalse ( VOID );

/**
 * Return (UINT8)zero.
 *
 *
 * @retval zero    None, or only case zero.
 */
UINT8
CommonReturnZero8 ( VOID );

/**
 * Return (UINT32)zero.
 *
 *
 * @retval zero    None, or only case zero.
 */
UINT32
CommonReturnZero32 ( VOID );

/**
 * Return (UINT64)zero.
 *
 *
 * @retval zero    None, or only case zero.
 */
UINT64
CommonReturnZero64 ( VOID );

/**
 * Return (UINT8)one.
 *
 *
 * @retval one     None, or only case one.
 */
UINT8
CommonReturnOne8 ( VOID );

/**
 * Return (UINT32)one.
 *
 *
 * @retval one     None, or only case one.
 */
UINT32
CommonReturnOne32 ( VOID );

/**
 * Return (UINT64)one.
 *
 *
 * @retval one     None, or only case one.
 */
UINT64
CommonReturnOne64 ( VOID );

/**
 * Return NULL
 *
 * @retval NULL    pointer to nothing
 */
VOID *
CommonReturnNULL ( VOID );

/**
* Return AGESA_SUCCESS.
*
* @retval AGESA_SUCCESS Success.
*/
AGESA_STATUS
CommonReturnAgesaSuccess ( VOID );

/**
* Return AGESA_ERROR.
*
* @retval AGESA_ERROR Error.
*/
AGESA_STATUS
CommonReturnAgesaError ( VOID );

/**
 * Do Nothing.
 *
 */
VOID
CommonVoid ( VOID );

/**
 * ASSERT if this routine is called.
 *
 */
VOID
CommonAssert ( VOID );

#endif // _COMMON_RETURNS_H_
