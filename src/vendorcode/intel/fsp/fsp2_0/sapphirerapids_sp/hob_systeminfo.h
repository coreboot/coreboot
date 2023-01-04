/** @file
  System Information HOB Hearder File

  @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 - 2021 Intel Corporation. <BR>

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary    and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.
**/

#ifndef _HOB_SYSTEMINFO_H_
#define _HOB_SYSTEMINFO_H_

#include <fsp/util.h>
#include "SystemInfoHob.h"

typedef enum {
  TypeLcc = 0,
  TypeMcc,
  TypeHcc,
  TypeXcc,
  TypeUcc,
  TypeLccFb,
  TypeZcc,
  TypeX1,
  TypeGrr,
  TypeGnrd,
  TypeChopTypeMax = 0xff
} TYPE_CHOPTYPE_ENUM;

#define FSP_HOB_SYSTEMINFO_GUID { \
	0xf2, 0xa0, 0x50, 0x76, 0x91, 0x0d, 0x0c, 0x4b, \
	0x92, 0x3b, 0xbd, 0xcf, 0x22, 0xd1, 0x64, 0x35  \
}

#endif //_HOB_SYSTEMINFO_H_
