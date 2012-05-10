/**
 * @file
 *
 * AMD IDS Routines
 *
 * Contains AMD AGESA IDS Translation
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision  $   @e \$Date 2008-04-07 15:08:45 -0500 (Mon, 07 Apr 2008) $
 */
/*****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
 * 
 *
 ***************************************************************************/
#ifndef _IDS_F10_ALLSERVICE_H_
#define _IDS_F10_ALLSERVICE_H_
#ifdef __IDS_EXTENDED__
  #include IDS_EXT_INCLUDE_F10 (IdsIntF10AllService)
#endif

/// enum for ids scrub option
typedef enum {
  IDS_F10_SCRUB_DIS = 0, ///< 0x00 Disable sequential scrubbing

  IDS_F10_SCRUB_RESERVED,     ///< 0x01 reserved
  IDS_F10_SCRUB_40NS,     ///< 0x02 40ns
  IDS_F10_SCRUB_80NS,     ///< 0x03 80ns
  IDS_F10_SCRUB_160NS,     ///< 0x04 160ns
  IDS_F10_SCRUB_320NS,     ///< 0x05 320ns
  IDS_F10_SCRUB_640NS,     ///< 0x06 640ns
  IDS_F10_SCRUB_1_28US,     ///< 0x07 1.28 us
  IDS_F10_SCRUB_2_56US,     ///< 0x08 2.56 us
  IDS_F10_SCRUB_5_12US,     ///< 0x09 5.12 us
  IDS_F10_SCRUB_10_2US,     ///< 0x0A 10.2 us
  IDS_F10_SCRUB_20_5US,     ///< 0x0B 20.5 us
  IDS_F10_SCRUB_41_0US,     ///< 0x0C 41.0 us
  IDS_F10_SCRUB_81_9US,     ///< 0x0D 81.9 us
  IDS_F10_SCRUB_163_8US,     ///< 0x0E 163.8 us
  IDS_F10_SCRUB_327_7US,     ///< 0x0F 327.7 us
  IDS_F10_SCRUB_655_4US,     ///< 0x10 655.4 us
  IDS_F10_SCRUB_1_31MS,     ///< 0x11 1.31 ms
  IDS_F10_SCRUB_2_62MS,     ///< 0x12 2.62 ms
  IDS_F10_SCRUB_5_24MS,     ///< 0x13 5.24 ms
  IDS_F10_SCRUB_10_49MS,     ///< 0x14 10.49 ms
  IDS_F10_SCRUB_20_97MS,     ///< 0x15 20.97 ms
  IDS_F10_SCRUB_42MS,     ///< 0x16 42ms
  IDS_F10_SCRUB_84MS,     ///< 0x17 84ms

  IDS_F10_SCRUB_AUTO = 0x1F       ///< Auto
} IDS_SCRUB_OPTIONS;
#endif

