/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#ifndef RAMINIT_H
#define RAMINIT_H

#if 0
#if CONFIG_DIMM_SUPPORT==0x0110
//FBDIMM REG
/* each channel can have 8 fbdimm */
#define DIMM_SOCKETS 8
struct mem_controller {
	u32 node_id;
	device_t f0, f1, f2, f3, f4, f5;
	/* channelA, channelB belong to DCT0,
	 * channelC, channelD belong to DCT1
	 * Each DCT may support one ganged logical FBDIMM ---> 128 bit
	 *			 or a single unganged channel --->64 bit
	 * a DCT can not support 2 unganged channels
	 * two DCTs can not be ganged
	 */
	u8 spd_switch_addr;
	u8 spd_addr[DIMM_SOCKETS*4];
};

#endif
#endif

//#if (CONFIG_DIMM_SUPPORT & 0x00ff)==0x0004
//DDR2 REG and unbuffered : Socket F 1027 and AM3
/* every channel have 4 DDR2 DIMM for socket F
 *		       2 for socket M2/M3
 *		       1 for socket s1g1
 */
#define DIMM_SOCKETS 4
struct mem_controller {
	u32 node_id;
	device_t f0, f1, f2, f3, f4, f5;
	/* channel0 is DCT0 --- channelA
	 * channel1 is DCT1 --- channelB
	 * can be ganged, a single dual-channel DCT ---> 128 bit
	 *	 or unganged a two single-channel DCTs ---> 64bit
	 * When the DCTs are ganged, the writes to DCT1 set of registers
	 * (F2x1XX) are ignored and reads return all 0's
	 * The exception is the DCT phy registers, F2x[1,0]98, F2x[1,0]9C,
	 * and all the associated indexed registers, are still
	 * independently accessiable
	 */
	/* FIXME: I will only support ganged mode for easy support */
	u8 spd_switch_addr;
	u8 spd_addr[DIMM_SOCKETS*2];
};

//#endif


#endif
