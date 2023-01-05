/** @file
 *
 * This file is _NOT_ automatically generated in coreboot!
 *
 */

#ifndef __FSPSUPD_H__
#define __FSPSUPD_H__

#include <FspUpd.h>

typedef struct __packed {
	/** Offset 0x0020**/	uint32_t                    vbios_buffer;
	/** Offset 0x0024**/	uint64_t                    gop_reserved;
	/** Offset 0x002C**/	uint32_t                    reserved1;
	/** Offset 0x0030**/	uint16_t                    UpdTerminator;
} FSP_S_CONFIG;

/** Fsp S UPD Configuration
**/
typedef struct __packed {
	/** Offset 0x0000**/	FSP_UPD_HEADER              FspUpdHeader;
	/** Offset 0x0020**/	FSP_S_CONFIG                FspsConfig;
} FSPS_UPD;

#endif
