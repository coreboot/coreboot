/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#define SERVER		0
#define DESKTOP		1
//#define MOBILE	2
#define REV_F		0
#define REV_DR		1
#define REV_FDR		2


/*----------------------------------------------------------------------------
COMMENT OUT ALL BUT 1
----------------------------------------------------------------------------*/
//#define BUILD_VERSION   REV_F		/*BIOS supports rev F only*/
//#define BUILD_VERSION   REV_DR	/*BIOS supports rev 10 only*/
//#define BUILD_VERSION   REV_FDR	/*BIOS supports both rev F and 10*/

/*----------------------------------------------------------------------------
COMMENT OUT ALL BUT 1
----------------------------------------------------------------------------*/
#ifndef SYSTEM_TYPE
#define    SYSTEM_TYPE	    SERVER
//#define    SYSTEM_TYPE     DESKTOP
//#define    SYSTEM_TYPE     MOBILE
#endif

/*----------------------------------------------------------------------------
UPDATE AS NEEDED
----------------------------------------------------------------------------*/
#ifndef MAX_NODES_SUPPORTED
#define MAX_NODES_SUPPORTED		8
#endif

#ifndef MAX_DIMMS_SUPPORTED
#if IS_ENABLED(CONFIG_DIMM_DDR3)
 #define MAX_DIMMS_SUPPORTED		6
#else
 #define MAX_DIMMS_SUPPORTED		8
#endif
#endif

#ifndef MAX_CS_SUPPORTED
#define MAX_CS_SUPPORTED		8
#endif

#ifndef MCT_DIMM_SPARE_NO_WARM
#define MCT_DIMM_SPARE_NO_WARM	0
#endif

#ifndef MEM_MAX_LOAD_FREQ
#if (CONFIG_DIMM_SUPPORT & 0x000F)==0x0005 	/* AMD_FAM10_DDR3 */
 #define MEM_MAX_LOAD_FREQ			933
 #define MEM_MIN_PLATFORM_FREQ_FAM10		400
 #define MEM_MIN_PLATFORM_FREQ_FAM15		333
#else						 /* AMD_FAM10_DDR2 */
 #define MEM_MAX_LOAD_FREQ			400
 #define MEM_MIN_PLATFORM_FREQ_FAM10		200
 /* DDR2 not available on Family 15h */
 #define MEM_MIN_PLATFORM_FREQ_FAM15		0
#endif
#endif

#define MCT_TRNG_KEEPOUT_START		0x00000C00
#define MCT_TRNG_KEEPOUT_END		0x00000CFF
