/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _NB_PLATFORM_H_
#define _NB_PLATFORM_H_

#define SERIAL_OUT_SUPPORT //enable serial output
#define CIMX_DEBUG

#ifdef  CIMX_DEBUG
#define CIMX_TRACE_SUPPORT
#define CIMX_ASSERT_SUPPORT
#endif

#ifdef  CIMX_TRACE_SUPPORT
	#define CIMX_INIT_TRACE(Arguments)
	#if CONFIG_REDIRECT_NBCIMX_TRACE_TO_SERIAL
		#define TRACE_DATA(Ptr, Level) BIOS_DEBUG //always enable
		#define CIMX_TRACE(Argument) do {do_printk Argument;} while (0)
	#else
		#define TRACE_DATA(Ptr, Level)
		#define CIMX_TRACE(Argument)
	#endif
#else
	#define CIMX_TRACE(Argument)
	#define CIMX_INIT_TRACE(Arguments)
#endif

#ifdef CIMX_ASSERT_SUPPORT
	#ifdef ASSERT
		#undef ASSERT
		#define ASSERT CIMX_ASSERT
	#endif
	#ifdef CIMX_TRACE_SUPPORT
		#define CIMX_ASSERT(x)  if(!(x)) {\
			LibAmdTraceDebug (CIMX_TRACE_ALL, (CHAR8 *)"ASSERT !!! "__FILE__" - line %d\n", __LINE__); \
			/*__asm {jmp $}; */\
		}
	//#define IDS_HDT_CONSOLE(s, args...) do_printk(BIOS_DEBUG, s, ##args)
	#else
		#define CIMX_ASSERT(x) if(!(x)) {\
			/*__asm {jmp $}; */\
		}
	#endif
#else
	#define CIMX_ASSERT(x)
#endif

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

//#define STALL(Ptr, TimeUs, Flag) LibAmdSbStall(TimeUs)
#define STALL(Ptr, TimeUs, Flag) LibAmdSbStall(TimeUs, Ptr)

#ifdef  B2_IMAGE
#define REPORT_EVENT(Class, Info, Param1, Param2, Param3, Param4, CfgPtr) LibNbEventLog(Class, Info, Param1, Param2, Param3, Param4, CfgPtr)
#else
#define REPORT_EVENT(Class, Info, Param1, Param2, Param3, Param4, CfgPtr)
#endif



// CIMX configuration parameters
//#define CIMX_B2_IMAGE_BASE_ADDRESS      0xFFF40000
/**
 * PCIEX_BASE_ADDRESS - Define PCIE base address
 *
 * @param[Option]     MOVE_PCIEBAR_TO_F0000000 Set PCIe base address to 0xF7000000
 */
#ifdef  MOVE_PCIEBAR_TO_F0000000
#define PCIEX_BASE_ADDRESS           0xF7000000
#else
#define PCIEX_BASE_ADDRESS           CONFIG_MMCONF_BASE_ADDRESS
#endif



#define CIMX_S3_SAVE 1
#include <cpu/amd/common/cbtypes.h>
#include <console/console.h>

#include "amd.h" //cimx typedef
#include <amdlib.h>
#include "amdAcpiLib.h"
#include "amdAcpiMadt.h"
#include "amdAcpiIvrs.h"
#include "amdSbLib.h"
#include "nbPcie.h"

//must put before the nbType.h
#include "platform_cfg.h" /*platform dependented configuration */
#include "nbType.h"

#include "nbLib.h"
#include "nbDef.h"
#include "nbInit.h"
#include "nbHtInit.h"
#include "nbIommu.h"
#include "nbEventLog.h"
#include "nbRegisters.h"
#include "nbPcieAspm.h"
#include "nbPcieLinkWidth.h"
#include "nbPcieHotplug.h"
#include "nbPciePortRemap.h"
#include "nbPcieWorkarounds.h"
#include "nbPcieCplBuffers.h"
#include "nbPciePllControl.h"
#include "nbMiscInit.h"
#include "nbIoApic.h"
#include "nbPcieSb.h"
#include "nbRecovery.h"
#include "nbMaskedMemoryInit.h"


#define FIX_PTR_ADDR(x, y) x

#define TRACE_ALWAYS  0xffffffff

#define AmdNbDispatcher NULL

#define CIMX_TRACE_ALL    0xFFFFFFFF
#define CIMX_NBPOR_TRACE  0xFFFFFFFF
#define CIMX_NBHT_TRACE   0xFFFFFFFF
#define CIMX_NBPCIE_TRACE 0xFFFFFFFF
#define CIMX_NB_TRACE     0xFFFFFFFF
#define CIMX_NBPCIE_MISC  0xFFFFFFFF

#endif

