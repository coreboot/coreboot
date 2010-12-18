/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

typedef struct {
	u16 osys;
	u8 smif;
	u8 prm0;
	u8 prm1;
	u8 scif;
	u8 prm2;
	u8 prm3;
	u8 lckf;
	u8 prm4;
	u8 prm5;
	u32 p80d;
	u8 lids;
	u8 pwrs;
	u8 dbgs;
	u8 linxs;
	u8 rsvd;
	u8 actt;
	u8 psvt;
	u8 tc1v;
	u8 tc2v;
	u8 tspv;
	u8 crtt;
	u8 dtse;
	u8 dts1;
	u8 dts2;
	u8 rsvd2;
	u8 bnum;
	u8 b0sc, b1sc, b2sc;
	u8 b0ss, b1ss, b2ss;
	u8 rsvd3[3];
	u8 apic;
	u8 mpen;
	u8 bten;
	u8 ppcm;
	u8 pcp0;
	u8 pcp1;
	u8 rsvd4[4];
	u8 natp;
	u8 cmap;
	u8 cmbp;
	u8 lptp;
	u8 fdcp;
	u8 rfdv;
	u8 hotk;
	u8 rtcf;
	u8 util;
	u8 acin;
	u8 igds;
	u8 tlst;
	u8 cadl;
	u8 padl;
	u16 cste;
	u16 pste;
	u16 nste;
	u16 sste;
	u8 ndid;
	u32 did1;
	u32 did2;
	u32 did3;
	u32 did4;
	u32 did5;
	u8 rsvd5[0xb];
	u8 brtl;
	u8 odds;
	u8 alse;
	u8 alaf;
	u8 llow;
	u8 lhih;
	u8 rsvd6;
	u8 emae;
	u16 emap;
	u16 emal;
	u8 rsvd7;
	u8 mefe;
	u8 igps;
	u8 rsvd8[2];
	u8 tpmp;
	u8 tpme;
	u8 rsvd9[8];
	u8 gtf0[7];
	u8 gtf2[7];
	u8 idem;
	u8 idet;
	u8 dock;
} global_nvs_t;
