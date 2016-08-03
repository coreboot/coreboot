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
union CPU_CONFIG_FSP_DATA {
  struct {
    /**
      Enable or Disable Advanced Encryption Standard (AES) feature.
      For some countries, this should be disabled for legal reasons.
      -    0: Disable
      - <b>1: Enable</b>
    **/
    uint32_t AesEnable           : 1;
    /**
    Processor Early Power On Configuration FCLK setting.
     - <b>0: 800 MHz (ULT/ULX)</b>.
     - <b>1: 1 GHz (DT/Halo)</b>. Not supported on ULT/ULX.
     - 2: 400 MHz.
     - 3: Reserved.
    **/
    uint32_t FClkFrequency       : 2;
    uint32_t EnableRsr           : 1;                 ///< Enable or Disable RSR feature; 0: Disable; <b>1: Enable </b>
    /**
    Policies to obtain CPU temperature.
     - <b>0: ACPI thermal management uses EC reported temperature values</b>.
     - 1: ACPI thermal management uses DTS SMM mechanism to obtain CPU temperature values.
     - 2: ACPI Thermal Management uses EC reported temperature values and DTS SMM is used to handle Out of Spec condition.
    **/
    uint32_t EnableDts           : 2;
    uint32_t SmmbaseSwSmiNumber  : 8;                 ///< Software SMI number for handler to save CPU information in SMRAM.
    /**
      Enable or Disable Virtual Machine Extensions (VMX) feature.
      -    0: Disable
      - <b>1: Enable</b>
    **/
    uint32_t VmxEnable           : 1;
    /**
      Enable or Disable Trusted Execution Technology (TXT) feature.
      -    0: Disable
      - <b>1: Enable</b>
    **/
    uint32_t TxtEnable           : 1;
    uint32_t SkipMpInit          : 1;                 ///< For Fsp only, Silicon Initialization will skip MP Initialization (including BSP) if enabled. For non-FSP, this should always be 0.
    uint32_t RsvdBits            : 15;                ///< Reserved for future use
    uint64_t MicrocodePatchAddress;     ///< Pointer to microcode patch that is suitable for this processor.
  } Bits;
  uint32_t Uint32[3];
};

#endif // _CPU_CONFIG_FSP_DATA_H_
