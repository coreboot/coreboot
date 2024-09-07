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

#ifndef _FSP_ACPI_HOBS_H_
#define _FSP_ACPI_HOBS_H_

//Typecast HOB pointer to ACPI CXL CEDT table structure
#define FSP_CXL_CEDT_ACPI_HOB_GUID        { 0x5CB7A12A, 0x8B2D, 0x485A, { 0xB7, 0x04, 0xC0, 0x52, 0x49, 0x56, 0x81, 0xE7 } }

//Typecast HOB pointer to RAS_ACPI_PARAM_HOB_DATA;
#define RAS_ACPI_PARAM_HOB_GUID           {0x594dfe5c, 0x7a87, 0x49dc, { 0x8f, 0x33, 0xea, 0x83, 0x4d, 0x6f, 0x18, 0x90 } }

#endif //#ifndef _FSP_ACPI_HOBS_H_
