/** @file
  FSP CPU Data Config Block.

@copyright
  Copyright (c) 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _CPU_CONFIG_FSP_DATA_H_
#define _CPU_CONFIG_FSP_DATA_H_

#pragma pack (push,1)
typedef union {
  struct {
    /**
      Enable or Disable Advanced Encryption Standard (AES) feature.
      For some countries, this should be disabled for legal reasons.
      -    0: Disable
      - <b>1: Enable</b>
    **/
    UINT32 AesEnable           : 1;
    /**
    Processor Early Power On Configuration FCLK setting.
     - <b>0: 800 MHz (ULT/ULX)</b>.
     - <b>1: 1 GHz (DT/Halo)</b>. Not supported on ULT/ULX.
     - 2: 400 MHz.
     - 3: Reserved.
    **/
    UINT32 FClkFrequency       : 2;
    UINT32 EnableRsr           : 1;                 ///< Enable or Disable RSR feature; 0: Disable; <b>1: Enable </b>
    /**
    Policies to obtain CPU temperature.
     - <b>0: ACPI thermal management uses EC reported temperature values</b>.
     - 1: ACPI thermal management uses DTS SMM mechanism to obtain CPU temperature values.
     - 2: ACPI Thermal Management uses EC reported temperature values and DTS SMM is used to handle Out of Spec condition.
    **/
    UINT32 EnableDts           : 2;
    UINT32 SmmbaseSwSmiNumber  : 8;                 ///< Software SMI number for handler to save CPU information in SMRAM.
    /**
      Enable or Disable Virtual Machine Extensions (VMX) feature.
      -    0: Disable
      - <b>1: Enable</b>
    **/
    UINT32 VmxEnable           : 1;
    /**
      Enable or Disable Trusted Execution Technology (TXT) feature.
      -    0: Disable
      - <b>1: Enable</b>
    **/
    UINT32 TxtEnable           : 1;
    UINT32 SkipMpInit          : 1;                 ///< For Fsp only, Silicon Initialization will skip MP Initialization (including BSP) if enabled. For non-FSP, this should always be 0.
    UINT32 RsvdBits            : 15;                ///< Reserved for future use
    UINT32 Reserved;
  } Bits;
  UINT32 Uint32[2];
} CPU_CONFIG_FSP_DATA;
#pragma pack (pop)

#endif // _CPU_CONFIG_FSP_DATA_H_
