/*
 * UEFI PEI wrapper
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Google Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL GOOGLE INC BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PEI_DATA_H_
#define _PEI_DATA_H_

#include <types.h>

#define PEI_VERSION 22

#define ABI_X86 __attribute__((regparm(0)))

typedef void ABI_X86(*tx_byte_func)(unsigned char byte);

struct pei_data {
	uint32_t pei_version;

	int boot_mode;
	int ec_present;

	/* Console output function */
	tx_byte_func tx_byte;

	/*
	 * DIMM SPD data for memory down configurations
	 * [CHANNEL][SLOT][SPD]
	 */
	uint8_t spd_data[2][2][512];

	/*
	 * LPDDR3 DQ byte map
	 * [CHANNEL][ITERATION][2]
	 *
	 * Maps which PI clocks are used by what LPDDR DQ Bytes (from CPU side)
	 * DQByteMap[0] - ClkDQByteMap:
	 * - If clock is per rank, program to [0xFF, 0xFF]
	 * - If clock is shared by 2 ranks, program to [0xFF, 0] or [0, 0xFF]
	 * - If clock is shared by 2 ranks but does not go to all bytes,
	 *   Entry[i] defines which DQ bytes Group i services
	 * DQByteMap[1] - CmdNDQByteMap: [0] is CmdN/CAA and [1] is CmdN/CAB
	 * DQByteMap[2] - CmdSDQByteMap: [0] is CmdS/CAA and [1] is CmdS/CAB
	 * DQByteMap[3] - CkeDQByteMap : [0] is CKE /CAA and [1] is CKE /CAB
	 *                For DDR, DQByteMap[3:1] = [0xFF, 0]
	 * DQByteMap[4] - CtlDQByteMap : Always program to [0xFF, 0]
	 *                since we have 1 CTL / rank
	 * DQByteMap[5] - CmdVDQByteMap: Always program to [0xFF, 0]
	 *                since we have 1 CA Vref
	 */
	uint8_t dq_map[2][12];

	/*
	 * LPDDR3 Map from CPU DQS pins to SDRAM DQS pins
	 * [CHANNEL][MAX_BYTES]
	 */
	uint8_t dqs_map[2][8];
	uint16_t RcompResistor[3];
	uint16_t RcompTarget[5];
	/* Data read from flash and passed into MRC */
	const void *saved_data;
	int saved_data_size;

	/* Disable use of saved data (can be set by mainboard) */
	int disable_saved_data;

	/* Data from MRC that should be saved to flash */
	void *data_to_save;
	int data_to_save_size;
	int mem_cfg_id;
} __attribute__((packed));

typedef struct pei_data PEI_DATA;

#endif /* _PEI_DATA_H_ */
