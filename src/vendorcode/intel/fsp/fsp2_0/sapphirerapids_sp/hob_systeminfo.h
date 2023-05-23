/** @file

Copyright (c) 2019-2023, Intel Corporation. All rights reserved.<BR>

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

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
