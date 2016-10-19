/*
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 or (at your option) any later version of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include "inteltool.h"

extern volatile uint8_t *mchbar;

static uint32_t read_mchbar32(uint32_t addr)
{
	return *(volatile uint32_t *)(mchbar + addr);
}

static void
print_time(const char *string, unsigned long long time, unsigned long long tCK)
{
	printf(".%s = %lld /* %lld clocks = %.3lf ns */,\n",
	       string, time, time, (time * tCK) / 256.0);
}

static unsigned int
make_spd_time(unsigned long long time, unsigned long long tCK)
{
	return (time * tCK) >> 5;
}

static u16 spd_ddr3_calc_crc(u8 * spd)
{
	int n_crc, i;
	u8 *ptr;
	u16 crc;

	/* Find the number of bytes covered by CRC */
	if (spd[0] & 0x80) {
		n_crc = 117;
	} else {
		n_crc = 126;
	}

	/* Compute the CRC */
	crc = 0;
	ptr = spd;
	while (--n_crc >= 0) {
		crc = crc ^ (int)*ptr++ << 8;
		for (i = 0; i < 8; ++i)
			if (crc & 0x8000) {
				crc = crc << 1 ^ 0x1021;
			} else {
				crc = crc << 1;
			}
	}
	return crc;
}

void ivybridge_dump_timings(const char *dump_spd_file)
{
	u32 mr0[2];
	u32 mr1[2];
	u32 reg;
	unsigned int CAS = 0;
	int tWR = 0, tRFC = 0;
	int tFAW[2], tWTR[2], tCKE[2], tRTP[2], tRRD[2];
	int channel, slot;
	u32 reg_4004_b30[2] = { 0, 0 };
	unsigned int tREFI;
	int rankmap[2];
	u32 mad_dimm[2];
	unsigned int tRCD[2], tXP[2], tXPDLL[2], tRAS[2], tCWL[2], tRP[2],
	    tAONPD[2];
	unsigned int tXSOffset;
	int two_channels = 1;
	struct slot_info {
		unsigned int size_mb;
		unsigned int ranks;
		unsigned int width;
	} slots[2][2];
	unsigned int tCK;
	u8 spd[2][2][256];

	memset(slots, 0, sizeof(slots));

	for (channel = 0; channel < 2; channel++) {
		rankmap[channel] = read_mchbar32(0xc14 + 0x100 * channel) >> 24;
	}

	two_channels = rankmap[0] && rankmap[1];

	mr0[0] = read_mchbar32(0x0004);
	mr1[0] = read_mchbar32(0x0008);
	mr0[1] = read_mchbar32(0x0104);
	mr1[1] = read_mchbar32(0x0108);

	if (mr0[0] != mr0[1] && two_channels)
		printf("MR0 mismatch: %x, %x\n", mr0[0], mr0[1]);
	if (mr1[0] != mr1[1] && two_channels)
		printf("MR1 mismatch: %x, %x\n", mr1[0], mr1[1]);

	reg = read_mchbar32(0x5e00) & ~0x80000000;
	printf(" .tCK = TCK_MHZ%d,\n", 400 * reg / 3);

	tCK = (64 * 10 * 3) / reg;

	if (mr0[0] & 1) {
		CAS = ((mr0[0] >> 4) & 0x7) + 12;
	} else {
		CAS = ((mr0[0] >> 4) & 0x7) + 4;
	}

	for (channel = 0; channel < 2; channel++) {
		mad_dimm[channel] = read_mchbar32(0x5004 + 4 * channel);
	}

	printf(".rankmap = { 0x%x, 0x%x },\n", rankmap[0], rankmap[1]);

	printf(".mad_dimm = { 0x%x, 0x%x },\n", mad_dimm[0], mad_dimm[1]);

	for (channel = 0; channel < 2; channel++)
		if (rankmap[channel]) {
			int ctWR;
			static const u8 mr0_wr_t[12] =
			    { 1, 2, 3, 4, 0, 5, 0, 6, 0, 7, 0, 0 };
			reg = read_mchbar32(0x4004 + 0x400 * channel);

			ctWR = (reg >> 24) & 0x3f;
			if (tWR && ctWR != tWR)
				printf("/* tWR mismatch: %d, %d */\n", tWR,
				       ctWR);
			if (!tWR)
				tWR = ctWR;
			if (((mr0[channel] >> 9) & 7) != mr0_wr_t[tWR - 5])
				printf("/* encoded tWR mismatch: %d, %d */\n",
				       ((mr0[channel] >> 9) & 7),
				       mr0_wr_t[tWR - 5]);
			reg_4004_b30[channel] = reg >> 30;
			tFAW[channel] = (reg >> 16) & 0xff;
			tWTR[channel] = (reg >> 12) & 0xf;
			tCKE[channel] = (reg >> 8) & 0xf;
			tRTP[channel] = (reg >> 4) & 0xf;
			tRRD[channel] = (reg >> 0) & 0xf;

			reg = read_mchbar32(0x4000 + 0x400 * channel);
			tRAS[channel] = reg >> 16;
			tCWL[channel] = (reg >> 12) & 0xf;
			if (CAS != ((reg >> 8) & 0xf))
				printf("/* CAS mismatch: %d, %d. */\n", CAS,
				       (reg >> 8) & 0xf);
			tRP[channel] = (reg >> 4) & 0xf;
			tRCD[channel] = reg & 0xf;

			reg = read_mchbar32(0x400c + channel * 0x400);
			tXPDLL[channel] = reg & 0x1f;
			tXP[channel] = (reg >> 5) & 7;
			tAONPD[channel] = (reg >> 8) & 0xff;
		}
	printf(".mobile = %d,\n", (mr0[0] >> 12) & 1);
	print_time("CAS", CAS, tCK);
	print_time("tWR", tWR, tCK);

	printf(".reg_4004_b30 = { %d, %d },\n", reg_4004_b30[0],
	       reg_4004_b30[1]);
	if (tFAW[0] != tFAW[1] && two_channels)
		printf("/* tFAW mismatch: %d, %d */\n", tFAW[0], tFAW[1]);
	print_time("tFAW", tFAW[0], tCK);
	if (tWTR[0] != tWTR[1] && two_channels)
		printf("/* tWTR mismatch: %d, %d */\n", tWTR[0], tWTR[1]);
	print_time("tWTR", tWTR[0], tCK);
	if (tCKE[0] != tCKE[1] && two_channels)
		printf("/* tCKE mismatch: %d, %d */\n", tCKE[0], tCKE[1]);
	print_time("tCKE", tCKE[0], tCK);
	if (tRTP[0] != tRTP[1] && two_channels)
		printf("/* tRTP mismatch: %d, %d */\n", tRTP[0], tRTP[1]);
	print_time("tRTP", tRTP[0], tCK);
	if (tRRD[0] != tRRD[1] && two_channels)
		printf("/* tRRD mismatch: %d, %d */\n", tRRD[0], tRRD[1]);
	print_time("tRRD", tRRD[0], tCK);

	if (tRAS[0] != tRAS[1] && two_channels)
		printf("/* tRAS mismatch: %d, %d */\n", tRAS[0], tRAS[1]);
	print_time("tRAS", tRAS[0], tCK);

	if (tCWL[0] != tCWL[1] && two_channels)
		printf("/* tCWL mismatch: %d, %d */\n", tCWL[0], tCWL[1]);
	print_time("tCWL", tCWL[0], tCK);

	if (tRP[0] != tRP[1] && two_channels)
		printf("/* tRP mismatch: %d, %d */\n", tRP[0], tRP[1]);
	print_time("tRP", tRP[0], tCK);

	if (tRCD[0] != tRCD[1] && two_channels)
		printf("/* tRCD mismatch: %d, %d */\n", tRCD[0], tRCD[1]);
	print_time("tRCD", tRCD[0], tCK);

	if (tXPDLL[0] != tXPDLL[1] && two_channels)
		printf("/* tXPDLL mismatch: %d, %d */\n", tXPDLL[0], tXPDLL[1]);
	print_time("tXPDLL", tXPDLL[0], tCK);

	if (tXP[0] != tXP[1] && two_channels)
		printf("/* tXP mismatch: %d, %d */\n", tXP[0], tXP[1]);
	print_time("tXP", tXP[0], tCK);

	if (tAONPD[0] != tAONPD[1] && two_channels)
		printf("/* tAONPD mismatch: %d, %d */\n", tAONPD[0], tAONPD[1]);
	print_time("tAONPD", tAONPD[0], tCK);

	reg = read_mchbar32(0x4298);
	if (reg != read_mchbar32(0x4698) && two_channels)
		printf("/* 4298 mismatch: %d, %d */\n", reg,
		       read_mchbar32(0x4698));

	tREFI = reg & 0xffff;
	print_time("tREFI", tREFI, tCK);
	if ((tREFI * 9 / 1024) != (reg >> 25))
		printf("/* tREFI mismatch: %d, %d */\n", tREFI * 9 / 1024,
		       (reg >> 25));
	tRFC = (reg >> 16) & 0x1ff;
	print_time("tRFC", tRFC, tCK);

	reg = read_mchbar32(0x42a4);
	if (reg != read_mchbar32(0x46a4) && two_channels)
		printf("/* 42a4 mismatch: %d, %d */\n", reg,
		       read_mchbar32(0x46a4));

	print_time("tMOD", 8 + ((reg >> 28) & 0xf), tCK);

	tXSOffset = 512 - ((reg >> 16) & 0x3ff);
	print_time("tXSOffset", tXSOffset, tCK);
	if (tXSOffset != ((reg >> 12) & 0xf))
		printf("/* tXSOffset mismatch: %d, %d */\n",
		       tXSOffset, (reg >> 12) & 0xf);
	if (512 != (reg & 0xfff))
		printf("/* tDLLK mismatch: %d, %d */\n", 512, reg & 0xfff);

	reg = read_mchbar32(0x5064);
	printf(".reg5064b0 = 0x%x,\n", reg & 0xfff);
	if ((reg >> 12) != 0x73)
		printf("/* mismatch 0x%x, 0x73.  */\n", reg << 12);

	unsigned int ch0size, ch1size;

	switch (read_mchbar32(0x5000)) {
	case 0x24:
		reg = read_mchbar32(0x5014);
		ch1size = reg >> 24;
		if (((reg >> 16) & 0xff) != 2 * ch1size)
			printf("/* ch1size mismatch: %d, %d*/\n",
			       2 * ch1size, ((ch1size >> 16) & 0xff));
		printf(".channel_size_mb = { ?, %d },\n", ch1size * 256);
		break;
	case 0x21:
		reg = read_mchbar32(0x5014);
		ch0size = reg >> 24;
		if (((reg >> 16) & 0xff) != 2 * ch0size)
			printf("/* ch0size mismatch: %d, %d*/\n",
			       2 * ch0size, ((ch0size >> 16) & 0xff));
		printf(".channel_size_mb = { %d, ? },\n", ch0size * 256);
		break;
	}

	for (channel = 0; channel < 2; channel++) {
		reg = mad_dimm[channel];
		int swap = (reg >> 16) & 1;
		slots[channel][swap].size_mb = (reg & 0xff) * 256;
		slots[channel][swap].ranks = 1 + ((reg >> 17) & 1);
		slots[channel][swap].width = 8 + 8 * ((reg >> 19) & 1);
		slots[channel][!swap].size_mb = ((reg >> 8) & 0xff) * 256;
		slots[channel][!swap].ranks = 1 + ((reg >> 18) & 1);
		slots[channel][!swap].width = 8 + 8 * ((reg >> 20) & 1);
	}
	/* Undetermined: rank mirror, other modes, asr, ext_temp.  */
	memset(spd, 0, sizeof(spd));
	for (channel = 0; channel < 2; channel++)
		for (slot = 0; slot < 2; slot++)
			if (slots[channel][slot].size_mb) {
				printf("/* CH%dS%d: %d MiB  */\n", channel,
				       slot, slots[channel][slot].size_mb);
			}
	for (channel = 0; channel < 2; channel++)
		for (slot = 0; slot < 2; slot++)
			if (slots[channel][slot].size_mb) {
				int capacity_shift;
				unsigned int ras, rc, rfc, faw;
				u16 crc;
				capacity_shift =
				    ffs(slots[channel][slot].size_mb *
					slots[channel][slot].width /
					(slots[channel][slot].ranks * 64)) - 1 -
				    5;

				spd[channel][slot][0] = 0x92;
				spd[channel][slot][1] = 0x11;
				spd[channel][slot][2] = 0xb;	/* DDR3 */
				spd[channel][slot][3] = 3;	/* SODIMM, should we use another type for desktop?  */
				spd[channel][slot][4] = capacity_shift | 0;	/* 8 Banks.  */
				spd[channel][slot][5] = 0;	/* FIXME */
				spd[channel][slot][6] = 0;	/* FIXME */
				spd[channel][slot][7] =
				    ((slots[channel][slot].ranks -
				      1) << 3) | (ffs(slots[channel][slot].
						      width) - 1 - 2);
				spd[channel][slot][8] = 3;	/* Bus width 64b. No ECC yet. */
				spd[channel][slot][9] = 0x52;	/* 2.5ps. FIXME: choose dynamically if needed.  */
				spd[channel][slot][10] = 0x01;
				spd[channel][slot][11] = 0x08;	/* 1/8 ns. FIXME: choose dynamically if needed.  */
				spd[channel][slot][12] = make_spd_time(1, tCK);
				spd[channel][slot][13] = 0;
				spd[channel][slot][14] =
				    (1 << (CAS - 4)) & 0xff;
				spd[channel][slot][15] = (1 << (CAS - 4)) >> 8;
				spd[channel][slot][16] =
				    make_spd_time(CAS, tCK);
				spd[channel][slot][17] =
				    make_spd_time(tWR, tCK);
				spd[channel][slot][18] =
				    make_spd_time(tRCD[channel], tCK);
				spd[channel][slot][19] =
				    make_spd_time(tRRD[channel], tCK);
				spd[channel][slot][20] =
				    make_spd_time(tRP[channel], tCK);
				ras = make_spd_time(tRAS[channel], tCK);
				rc = 0x181;	/* FIXME: should be make_spd_time(tRC, tCK).  */
				spd[channel][slot][22] = ras;
				spd[channel][slot][23] = rc;
				spd[channel][slot][21] =
				    ((ras >> 8) & 0xf) | ((rc >> 4) & 0xf0);
				rfc = make_spd_time(tRFC, tCK);
				spd[channel][slot][24] = rfc;
				spd[channel][slot][25] = rfc >> 8;
				spd[channel][slot][26] =
				    make_spd_time(tWTR[channel], tCK);
				spd[channel][slot][27] =
				    make_spd_time(tRTP[channel], tCK);
				faw = make_spd_time(tFAW[channel], tCK);
				spd[channel][slot][28] = faw >> 8;
				spd[channel][slot][29] = faw;
				spd[channel][slot][30] = 0;	/* FIXME */
				spd[channel][slot][31] = 0;	/* FIXME */
				spd[channel][slot][32] = 0;	/* FIXME */
				spd[channel][slot][33] = 0;	/* FIXME */
				spd[channel][slot][62] = 0x65;	/* Reference card F.  FIXME */
				spd[channel][slot][63] = 0;	/* FIXME */
				crc = spd_ddr3_calc_crc(spd[channel][slot]);
				spd[channel][slot][126] = crc;
				spd[channel][slot][127] = crc >> 8;
			}

	printf("/* SPD matching current mode:  */\n");

	FILE *dump_spd = NULL;

	if (dump_spd_file) {
		dump_spd = fopen (dump_spd_file, "wb");
		if (!dump_spd) {
			fprintf (stderr, "Couldn't open file %s: %s\n", dump_spd_file,
				 strerror (errno));
			exit (1);
		}
	}

	for (channel = 0; channel < 2; channel++)
		for (slot = 0; slot < 2; slot++)
		{
			if (slots[channel][slot].size_mb) {
				int i;

				printf("/* CH%dS%d  */\n", channel, slot);

				for (i = 0; i < 256; i++) {
					if ((i & 0xf) == 0x0)
						printf("%02x: ", i);
					printf("%02x ", spd[channel][slot][i]);
					if ((i & 0xf) == 0xf)
						printf("\n");
				}
				printf("\n");

				if (dump_spd) {
					fwrite(spd[channel][slot], 1, 256, dump_spd);
				}
			} else {
				if (dump_spd) {
					char zero[256];
					memset (zero, 0, 256);
					fwrite(zero, 1, 256, dump_spd);
				}
			}
		}
	if (dump_spd) {
		fclose (dump_spd);
	}

}
