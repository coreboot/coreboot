/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2007-2009 coresystems GmbH
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

/* SPDs for DDR2 SDRAM */
#define SPD_MEM_TYPE	2
	#define SPD_MEM_TYPE_SDRAM_DDR	0x07
	#define SPD_MEM_TYPE_SDRAM_DDR2	0x08

#define SPD_DIMM_TYPE	20	/* x  bit0 or bit4 =1 mean registered*/
	#define SPD_DIMM_TYPE_RDIMM	(1<<0)
	#define SPD_DIMM_TYPE_UDIMM	(1<<1)
	#define SPD_DIMM_TYPE_SODIMM	(1<<2)
	#define SPD_DIMM_TYPE_uDIMM	(1<<3)
	#define SPD_DIMM_TYPE_mRDIMM	(1<<4)
	#define SPD_DIMM_TYPE_mUDIMM	(1<<5)

#define SPD_MOD_ATTRIB	21
	#define SPD_MOD_ATTRIB_DIFCK	0x20
	#define SPD_MOD_ATTRIB_REGADC	0x11   /* x */
	#define SPD_MOD_ATTRIB_PROBE	0x40

#define SPD_DEV_ATTRIB	22  /* Device attributes --- general */
#define SPD_DIMM_CONF_TYPE	11
	#define SPD_DIMM_CONF_TYPE_ECC		0x02
	#define SPD_DIMM_CONF_TYPE_ADDR_PARITY	0x04	 /* ? */

#define SPD_CAS_LAT_MIN_X_1	23
#define SPD_CAS_LAT_MAX_X_1	24
#define SPD_CAS_LAT_MIN_X_2	25
#define SPD_CAS_LAT_MAX_X_2	26

#define SPD_BURST_LENGTHS	16
	#define SPD_BURST_LENGTHS_4	(1<<2)
	#define SPD_BURST_LENGTHS_8	(1<<3)

#define SPD_ROW_NUM	3	/* Number of Row addresses */
#define SPD_COL_NUM	4	/* Number of Column addresses */
#define SPD_BANK_NUM	17	/* SDRAM Device attributes - Number of Banks on
				 SDRAM device, it could be 0x4, 0x8, so address
				 lines for that would be 2, and 3 */

/* Number of Ranks bit [2:0], Package (bit4, 1=stack, 0=planr), Height bit[7:5] */
#define SPD_MOD_ATTRIB_RANK	5
	#define SPD_MOD_ATTRIB_RANK_NUM_SHIFT	0
	#define SPD_MOD_ATTRIB_RANK_NUM_MASK	0x07
		#define SPD_MOD_ATTRIB_RANK_NUM_BASE	1
		#define SPD_MOD_ATTRIB_RANK_NUM_MIN	1
		#define SPD_MOD_ATTRIB_RANK_NUM_MAX	8

#define SPD_RANK_SIZE	31	/* Only one bit is set */
	#define SPD_RANK_SIZE_1GB	(1<<0)
	#define SPD_RANK_SIZE_2GB	(1<<1)
	#define SPD_RANK_SIZE_4GB	(1<<2)
	#define SPD_RANK_SIZE_8GB	(1<<3)
	#define SPD_RANK_SIZE_16GB	(1<<4)
	#define SPD_RANK_SIZE_128MB	(1<<5)
	#define SPD_RANK_SIZE_256MB	(1<<6)
	#define SPD_RANK_SIZE_512MB	(1<<7)

#define SPD_DATA_WIDTH		6	/* valid value 0, 32, 33, 36, 64, 72, 80, 128, 144, 254, 255 */
#define SPD_PRI_WIDTH		13	/* Primary SDRAM Width, it could be 0x08 or 0x10 */
#define SPD_ERR_WIDTH		14	/* Error Checking SDRAM Width, it could be 0x08 or 0x10 */

#define SPD_CAS_LAT	18	/* SDRAM Device Attributes -- CAS Latency */
	#define SPD_CAS_LAT_2	(1<<2)
	#define SPD_CAS_LAT_3	(1<<3)
	#define SPD_CAS_LAT_4	(1<<4)
	#define SPD_CAS_LAT_5	(1<<5)
	#define SPD_CAS_LAT_6	(1<<6)

#define SPD_TRP	27  /* bit [7:2] = 1-63 ns, bit [1:0] 0.25ns+, final value ((val>>2) + (val & 3) * 0.25)ns */
#define SPD_TRRD	28
#define SPD_TRCD	29
#define SPD_TRAS	30
#define SPD_TWR	36	/* x */
#define SPD_TWTR	37	/* x */
#define SPD_TRTP	38	/* x */

#define SPD_EX_TRC_TRFC 40
#define SPD_TRC	41	/* add byte 0x40 bit [3:1] , so final val41+ table[((val40>>1) & 0x7)]  ... table[]={0, 0.25, 0.33, 0.5, 0.75, 0, 0}*/
#define SPD_TRFC	42	/* add byte 0x40 bit [6:4] , so final val42+ table[((val40>>4) & 0x7)] + (val40 & 1)*256*/

#define SPD_TREF	12
