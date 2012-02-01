/*;********************************************************************************
;
; Copyright (C) 2012 Advanced Micro Devices, Inc.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of
;       its contributors may be used to endorse or promote products derived
;       from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;*********************************************************************************/

#ifndef _AMD_SBLIB_H_
#define _AMD_SBLIB_H_

//SB7xx Family
#define	SB7xx_DEVICE_ID	0x4385
#define	SB700	0x00
#define	SB750	0x01
#define	SB710	0x02

//SB800 Family
#define	SB800	0x10

#define SB_UNKNOWN	0xFF

//SB700 Revision IDs
#define	SB700_A11	0x39
#define	SB700_A12	0x3A
#define	SB700_A13	0x3B
#define	SB700_A14	0x3C

#define	SB_Rev_Sb7xx_A11	0x39
#define	SB_Rev_Sb7xx_A12	0x3A
#define	SB_Rev_Sb7xx_A13	0x3B
#define	SB_Rev_Sb7xx_A14	0x3C


typedef enum {
  Sb_Raid0_1_Capability,                 ///
  Sb_Raid5_Capability,                   ///
  Sb_Ahci_Capability,                   ///
  Sb_Unknown_Capability
} SB_CAPABILITY_ITEM;


typedef enum {
  Sb_Cap_Setting_Auto,
  Sb_Cap_Setting_Enabled,
  Sb_Cap_Setting_Disabled,
  Sb_Cap_Setting_Unknown
} SB_CAPABILITY_SETTING;


#define SB_MODEL_SB700	BIT0
#define SB_MODEL_SB750	BIT1
#define SB_MODEL_SB710	BIT2
#define SB_MODEL_SR5690	BIT3
#define SB_MODEL_UNKNOWN BIT31

typedef struct
{
	UINT32	sbModelMask;
	UINT8	sbRev;
}SB_INFORMATION;


void getSbInformation	(SB_INFORMATION *sbInfo);
SB_CAPABILITY_SETTING getSbCapability	(SB_CAPABILITY_ITEM sbCapabilityItem);
void setSbCapability	(SB_CAPABILITY_ITEM sbCapabilityItem, SB_CAPABILITY_SETTING sbCapSetting);

#endif	//#ifndef _AMD_SBLIB_H_
