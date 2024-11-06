/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _FSP_INFO_HEADER_H_
#define _FSP_INFO_HEADER_H_

#if CONFIG_UDK_VERSION == CONFIG_UDK_2017_VERSION
#include <vendorcode/intel/edk2/UDK2017/IntelFsp2Pkg/Include/Guid/FspHeaderFile.h>
#elif CONFIG_UDK_VERSION == CONFIG_UDK_202005_VERSION
#include <vendorcode/intel/edk2/edk2-stable202005/IntelFsp2Pkg/Include/Guid/FspHeaderFile.h>
#elif CONFIG_UDK_VERSION == CONFIG_UDK_202111_VERSION
#include <vendorcode/intel/edk2/edk2-stable202111/IntelFsp2Pkg/Include/Guid/FspHeaderFile.h>
#elif CONFIG_UDK_VERSION == CONFIG_UDK_202302_VERSION
#include <vendorcode/intel/edk2/edk2-stable202302/IntelFsp2Pkg/Include/Guid/FspHeaderFile.h>
#elif CONFIG_UDK_VERSION == CONFIG_UDK_202305_VERSION
#include <vendorcode/intel/edk2/edk2-stable202305/IntelFsp2Pkg/Include/Guid/FspHeaderFile.h>
#else
#error "Unknown UDK_VESION!"
#endif

#endif /* _FSP_INFO_HEADER_H_ */
