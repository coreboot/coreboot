/*
 * This file is part of the coreboot project.
 * Constants that are mainboard-defined and do not belong in Kconfig. 
 * We really do not want this stuff to be visible -- it will make it appear that they can be 
 * changed, and they can not. 
 * 
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* we are unsure of these */
#define DIMM_SOCKETS 4
#define NODE_NUMS 16
#define CPU_SOCKET_TYPE SOCKET_AM2
/* these have been checked against v2 */
#define MEM_TRAIN_SEQ 1
#define HW_MEM_HOLE_SIZE_AUTO_INC 1
#define HW_MEM_HOLE_SIZEK 0x100000
#define HT_CHAIN_UNITID_BASE 0
#define HT_CHAIN_END_UNITID_BASE 0x6
#define SB_HT_CHAIN_ON_BUS0 2
#define SB_HT_CHAIN_UNITID_OFFSET_ONLY 0
#define ENABLE_APIC_EXT_ID 0
#define LIFT_BSP_APIC_ID 1
#define K8_SET_FIDVID 1
/* MSR FIDVID_CTL and FIDVID_STATUS are shared by cores, 
 * so may don't need to do twice */
#define K8_SET_FIDVID_CORE0_ONLY 1

/* architecture stuff which ought to be set "somewhere" "SOMEHOW" */
/* preferably by asking the CPU, not be a DEFINE! */
#define CPU_ADDR_BITS 40

/* you don't want to set this in Kconfig or dts either. It's dangerous! */
#define CONFIG_AP_CODE_IN_CAR 0
/* MCP55 is another fun challenge */
#define MCP55_NUM 1
#define MCP55_DEVN_BASE	HT_CHAIN_UNITID_BASE
