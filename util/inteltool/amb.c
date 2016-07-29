/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2012 Sven Schnelle <svens@stackframe.org>
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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "inteltool.h"

#define AMB_CONFIG_SPACE_SIZE 0x20000

#define AMB_ADDR(base, fn, reg) (base | ((fn & 7) << 8) | ((reg & 0xff)))

static uint32_t amb_read_config32(volatile void *base, int fn, int reg)
{
	return *(uint32_t *)(AMB_ADDR((intptr_t)base, fn, reg));
}

static void amb_printreg32(volatile void *base, int fn, int reg,
			   const char *name, int printzero)
{
	uint32_t val = amb_read_config32(base, fn, reg);
	if (!val && !printzero)
		return;
	printf("%d:%2.2x %-16.16s: 0x%08x\n", fn, reg, name, val);
}

static uint16_t amb_read_config16(volatile void *base, int fn, int reg)
{
	return *(uint16_t *)(AMB_ADDR((intptr_t)base, fn, reg));
}

static void amb_printreg16(volatile void *base, int fn, int reg,
			   const char *name, int printzero)
{
	uint16_t val = amb_read_config16(base, fn, reg);
	if (!val && !printzero)
		return;
	printf("%d:%2.2x %-16.16s: 0x%04x\n", fn, reg, name, val);

}

static uint8_t amb_read_config8(volatile void *base, int fn, int reg)
{
	return *(uint8_t *)(AMB_ADDR((intptr_t)base, fn, reg));
}

static void amb_printreg8(volatile void *base, int fn, int reg,
			  const char *name, int printzero)
{
	uint8_t val = amb_read_config8(base, fn, reg);
	if (!val && !printzero)
		return;
	printf("%d:%2.2x %-16.16s: 0x%02x\n", fn, reg, name, val);

}

static void amb_printreg24(volatile void *base, int fn, int reg,
			   const char *name, int printzero)
{
	uint32_t val;

	if (reg & 1) {
		val = amb_read_config8(base, fn, reg) |
			(amb_read_config16(base, fn, reg + 1) << 8);
	} else {
		val = amb_read_config16(base, fn, reg) |
			(amb_read_config8(base, fn, reg + 2) << 16);
	}

	if (!val && !printzero)
		return;

	printf("%d:%2.2x %-16.16s: 0x%06x\n", fn, reg, name, val);
}


struct amb_register {
	int fn;
	int offset;
	const char *name;
	void (*printfunc)(volatile void *, int, int, const char *, int);
	int width;
} amb_registers[] = {
	{ 0, 0x00, "VID", NULL, 2 },
	{ 0, 0x02, "DID", NULL, 2 },
	{ 0, 0x08, "RID", NULL, 1 },
	{ 0, 0x09, "CCR", NULL, 3 },
	{ 0, 0x0e, "HDR", NULL, 1 },
	{ 1, 0x40, "FBDS0", NULL, 1 },
	{ 1, 0x41, "FBDS1", NULL, 1 },
	{ 1, 0x42, "FBDS2", NULL, 1 },
	{ 1, 0x43, "FBDS3", NULL, 1 },
	{ 1, 0x50, "FBDSBCFGCUR", NULL, 1 },
	{ 1, 0x51, "FBDNBCFGCUR", NULL, 1 },
	{ 1, 0x52, "LINKPARCUR", NULL, 2 },
	{ 1, 0x54, "FBDSBCFGNXT", NULL, 1 },
	{ 1, 0x55, "FBDNBCFGNXT", NULL, 1 },
	{ 1, 0x56, "LINKPARNXT", NULL, 2 },
	{ 1, 0x5a, "SBRXMAR", NULL, 1 },
	{ 1, 0x5b, "NBRXMAR", NULL, 1 },
	{ 1, 0x5c, "MODES", NULL, 1 },
	{ 1, 0x60, "FEATURES", NULL, 2 },
	{ 1, 0x64, "FBDLIS", NULL, 3 },
	{ 1, 0x68, "FBDLOCKTO", NULL, 2 },
	{ 1, 0x6c, "FBDHAC", NULL, 1 },
	{ 1, 0x6e, "FBDLS", NULL, 1 },
	{ 1, 0x70, "RECALDUR", NULL, 1 },
	{ 1, 0x74, "LOSDUR", NULL, 1 },
	{ 1, 0x78, "SYNCTRAININT", NULL, 1 },
	{ 1, 0x7c, "SBCALSTATUS", NULL, 2 },
	{ 1, 0x7e, "NBCALSTATUS", NULL, 2 },
	{ 1, 0x88, "CBC", NULL, 1 },
	{ 1, 0x8c, "EMASK", NULL, 1 },
	{ 1, 0x90, "FERR", NULL, 1 },
	{ 1, 0x94, "NERR", NULL, 1 },
	{ 1, 0x98, "RECCFG", NULL, 2 },
	{ 1, 0x9c, "RECFBD0", NULL, 2 },
	{ 1, 0x9e, "RECFBD1", NULL, 2 },
	{ 1, 0xa0, "RECFBD2", NULL, 2 },
	{ 1, 0xa2, "RECFBD3", NULL, 2 },
	{ 1, 0xa4, "RECFBD4", NULL, 2 },
	{ 1, 0xa6, "RECFBD5", NULL, 2 },
	{ 1, 0xa8, "RECFBD6", NULL, 2 },
	{ 1, 0xaa, "RECFBD7", NULL, 2 },
	{ 1, 0xac, "RECFBD8", NULL, 2 },
	{ 1, 0xae, "RECFBD9", NULL, 2 },
	{ 1, 0xb0, "PERSBYTE0NXT", NULL, 1 },
	{ 1, 0xb1, "PERSBYTE1NXT", NULL, 1 },
	{ 1, 0xb2, "PERSBYTE2NXT", NULL, 1 },
	{ 1, 0xb3, "PERSBYTE3NXT", NULL, 1 },
	{ 1, 0xb4, "PERSBYTE4NXT", NULL, 1 },
	{ 1, 0xb5, "PERSBYTE5NXT", NULL, 1 },
	{ 1, 0xb6, "PERSBYTE6NXT", NULL, 1 },
	{ 1, 0xb7, "PERSBYTE7NXT", NULL, 1 },
	{ 1, 0xb8, "PERSBYTE8NXT", NULL, 1 },
	{ 1, 0xb9, "PERSBYTE9NXT", NULL, 1 },
	{ 1, 0xba, "PERSBYTE10NXT", NULL, 1 },
	{ 1, 0xbb, "PERSBYTE11NXT", NULL, 1 },
	{ 1, 0xbc, "PERSBYTE12NXT", NULL, 1 },
	{ 1, 0xbd, "PERSBYTE13NXT", NULL, 1 },
	{ 1, 0xc0, "PERSBYTE0CUR", NULL, 1 },
	{ 1, 0xc1, "PERSBYTE1CUR", NULL, 1 },
	{ 1, 0xc2, "PERSBYTE2CUR", NULL, 1 },
	{ 1, 0xc3, "PERSBYTE3CUR", NULL, 1 },
	{ 1, 0xc4, "PERSBYTE4CUR", NULL, 1 },
	{ 1, 0xc5, "PERSBYTE5CUR", NULL, 1 },
	{ 1, 0xc6, "PERSBYTE6CUR", NULL, 1 },
	{ 1, 0xc7, "PERSBYTE7CUR", NULL, 1 },
	{ 1, 0xc8, "PERSBYTE8CUR", NULL, 1 },
	{ 1, 0xc9, "PERSBYTE9CUR", NULL, 1 },
	{ 1, 0xca, "PERSBYTE10CUR", NULL, 1 },
	{ 1, 0xcb, "PERSBYTE11CUR", NULL, 1 },
	{ 1, 0xcc, "PERSBYTE12CUR", NULL, 1 },
	{ 1, 0xcd, "PERSBYTE13CUR", NULL, 1 },
	{ 1, 0xe8, "CMD2DATANXT", NULL, 1 },
	{ 1, 0xe9, "CMD2DATACUR", NULL, 1 },
	{ 1, 0xea, "C2DINCRNXT", NULL, 1 },
	{ 1, 0xeb, "C2DINCRCUR", NULL, 1 },
	{ 1, 0xec, "C2DDECRNXT", NULL, 1 },
	{ 1, 0xed, "C2DDECRCUR", NULL, 1 },
	{ 2, 0x40, "SBDRVCFG", NULL, 1 },
	{ 2, 0x41, "NBDRVCFG", NULL, 1 },
	{ 2, 0x42, "FBDPDCFG", NULL, 1 },
	{ 2, 0x44, "RTERMCFG", NULL, 3 },
	{ 2, 0x47, "IDTCTRLCFG", NULL, 1 },
	{ 2, 0x48, "FBDBUFCFG", NULL, 1 },
	{ 2, 0x50, "SBRQOFFS0", NULL, 1 },
	{ 2, 0x51, "SBRQOFFS1", NULL, 1 },
	{ 2, 0x52, "SBRQOFFS2", NULL, 1 },
	{ 2, 0x53, "SBRQOFFS3", NULL, 1 },
	{ 2, 0x54, "SBRQOFFS4", NULL, 1 },
	{ 2, 0x55, "SBRQOFFS5", NULL, 1 },
	{ 2, 0x56, "SBRQOFFS6", NULL, 1 },
	{ 2, 0x57, "SBRQOFFS7", NULL, 1 },
	{ 2, 0x58, "SBRQOFFS8", NULL, 1 },
	{ 2, 0x59, "SBRQOFFS9", NULL, 1 },

	{ 2, 0x60, "NBRQOFFS0", NULL, 1 },
	{ 2, 0x61, "NBRQOFFS1", NULL, 1 },
	{ 2, 0x62, "NBRQOFFS2", NULL, 1 },
	{ 2, 0x63, "NBRQOFFS3", NULL, 1 },
	{ 2, 0x64, "NBRQOFFS4", NULL, 1 },
	{ 2, 0x65, "NBRQOFFS5", NULL, 1 },
	{ 2, 0x66, "NBRQOFFS6", NULL, 1 },
	{ 2, 0x67, "NBRQOFFS7", NULL, 1 },
	{ 2, 0x68, "NBRQOFFS8", NULL, 1 },
	{ 2, 0x69, "NBRQOFFS9", NULL, 1 },
	{ 2, 0x6a, "NBRQOFFS10", NULL, 1 },
	{ 2, 0x6b, "NBRQOFFS11", NULL, 1 },
	{ 2, 0x6c, "NBRQOFFS12", NULL, 1 },
	{ 2, 0x6d, "NBRQOFFS13", NULL, 1 },
	{ 2, 0x90, "TESTCFG", NULL, 1 },
	{ 2, 0x94, "SBTXENCFG", NULL, 2 },
	{ 2, 0x96, "NBTXENCFG", NULL, 2 },
	{ 2, 0x98, "IDLEDETCFG", NULL, 2 },
	{ 2, 0xa4, "SBRXSTATUS", NULL, 4 },
	{ 2, 0xa8, "SBRXSTATUS", NULL, 1 },
	{ 3, 0x3c, "MBIDT", NULL, 1 },
	{ 3, 0x40, "MBCSR", NULL, 4 },
	{ 3, 0x44, "MBADDR", NULL, 4 },
	{ 3, 0x40, "MBDATA0", NULL, 4 },
	{ 3, 0x41, "MBDATA1", NULL, 4 },
	{ 3, 0x42, "MBDATA2", NULL, 4 },
	{ 3, 0x43, "MBDATA3", NULL, 4 },
	{ 3, 0x44, "MBDATA4", NULL, 4 },
	{ 3, 0x45, "MBDATA5", NULL, 4 },
	{ 3, 0x46, "MBDATA6", NULL, 4 },
	{ 3, 0x47, "MBDATA7", NULL, 4 },
	{ 3, 0x48, "MBDATA8", NULL, 4 },
	{ 3, 0x49, "MBDATA9", NULL, 4 },
	{ 3, 0x70, "DAREFTC", NULL, 4 },
	{ 3, 0x74, "DSREFTC", NULL, 3 },
	{ 3, 0x77, "MTR", NULL, 1 },
	{ 3, 0x78, "DRT", NULL, 4 },
	{ 3, 0x7c, "DRC", NULL, 4 },
	{ 3, 0x80, "TEMPLO", NULL, 1 },
	{ 3, 0x81, "TEMPMID", NULL, 1 },
	{ 3, 0x82, "TEMPHI", NULL, 1 },
	{ 3, 0x83, "UPDATED", NULL, 1 },
	{ 3, 0x84, "TEMPSTAT", NULL, 1 },
	{ 3, 0x85, "TEMP", NULL, 1 },
	{ 3, 0x86, "TEMPOFFSET", NULL, 1 },
	{ 3, 0x9c, "MB_START_ADDR", NULL, 4 },
	{ 3, 0xa0, "MB_END_ADDR", NULL, 4 },
	{ 3, 0xa4, "MBLFSRSEED", NULL, 4 },
	{ 3, 0xa8, "MBFADDRPTR", NULL, 4 },
	{ 3, 0xb0, "MB_ERR_DATA00", NULL, 4 },
	{ 3, 0xb4, "MB_ERR_DATA01", NULL, 4 },
	{ 3, 0xb8, "MB_ERR_DATA02", NULL, 4 },
	{ 3, 0xbc, "MB_ERR_DATA03", NULL, 4 },
	{ 3, 0xc0, "MB_ERR_DATA04", NULL, 2 },
	{ 3, 0xc4, "MB_ERR_DATA10", NULL, 4 },
	{ 3, 0xc8, "MB_ERR_DATA11", NULL, 4 },
	{ 3, 0xcc, "MB_ERR_DATA12", NULL, 4 },
	{ 3, 0xd0, "MB_ERR_DATA13", NULL, 4 },
	{ 3, 0xd4, "MB_ERR_DATA14", NULL, 2 },

	{ 4, 0x40, "DCALCSR", NULL, 4 },
	{ 4, 0x44, "DCALADDR", NULL, 4 },
	{ 4, 0x98, "DSRETC", NULL, 4 },
	{ 4, 0xa4, "S3RESTORE0", NULL, 4 },
	{ 4, 0xa8, "S3RESTORE1", NULL, 4 },
	{ 4, 0xac, "S3RESTORE2", NULL, 4 },
	{ 4, 0xb0, "S3RESTORE3", NULL, 4 },
	{ 4, 0xb4, "S3RESTORE4", NULL, 4 },
	{ 4, 0xb8, "S3RESTORE5", NULL, 4 },
	{ 4, 0xbc, "S3RESTORE6", NULL, 4 },
	{ 4, 0xc0, "S3RESTORE7", NULL, 4 },
	{ 4, 0xc4, "S3RESTORE8", NULL, 4 },
	{ 4, 0xc8, "S3RESTORE9", NULL, 4 },
	{ 4, 0xcc, "S3RESTORE10", NULL, 4 },
	{ 4, 0xe8, "FIVESREG", NULL, 4 },
	{ 4, 0xea, "AAAAREG", NULL, 4 },

	{ 5, 0x3c, "TRANSCFG", NULL, 4 },
	{ 5, 0x40, "TRANDERR0", NULL, 2 },
	{ 5, 0x42, "TRANDERR1", NULL, 2 },
	{ 5, 0x44, "TRANDERR2", NULL, 2 },
	{ 5, 0x46, "TRANDERR3", NULL, 2 },
	{ 5, 0x48, "TRANDERR4", NULL, 2 },
	{ 5, 0x4a, "TRANDERR5", NULL, 2 },
	{ 5, 0x4c, "TRANDERR6", NULL, 2 },
	{ 5, 0x4e, "TRANDERR7", NULL, 2 },
	{ 5, 0x50, "TRANDERR8", NULL, 2 },
	{ 5, 0x80, "TRANSCTRL", NULL, 1 },
	{ 5, 0xbc, "SBMATCHU", NULL, 3 },
	{ 5, 0xc0, "SBMATCHL0", NULL, 4 },
	{ 5, 0xcc, "SBMASKU", NULL, 3 },
	{ 5, 0xd0, "SBMASKL0", NULL, 4 },
	{ 5, 0xe0, "EVENTSEL0", NULL, 3 },
	{ 5, 0xfc, "EICNTL", NULL, 1 },
	{ 5, 0xfe, "STUCKL", NULL, 1 },

	{ 6, 0x50, "NBRXSTATUS", NULL, 4 },
	{ 6, 0x7c, "SPAD0", NULL, 4 },
	{ 6, 0x80, "SBFIBPORTCTL", NULL, 4 },
	{ 6, 0x84, "SBFIBPGCTL", NULL, 4 },
	{ 6, 0x88, "SBFIBPATTBUF1", NULL, 3 },
	{ 6, 0x8c, "SBFIBTXMSK", NULL, 2 },
	{ 6, 0x90, "SBFIBRXMSK", NULL, 2 },
	{ 6, 0x94, "SBFIBTXSHFT", NULL, 2 },
	{ 6, 0x98, "SBFIBRXSHFT", NULL, 2 },
	{ 6, 0x9c, "SBFIBRXLNERR", NULL, 2 },
	{ 6, 0xa0, "SBFIBPATTBUF2", NULL, 3 },
	{ 6, 0xa4, "SBFIBPATT2EN", NULL, 2 },
	{ 6, 0xb0, "SBFIBINIT", NULL, 4 },
	{ 6, 0xb4, "SBIBISTMISC", NULL, 3 },
	{ 6, 0xc0, "NBFIBPORTCTL", NULL, 4 },
	{ 6, 0xc4, "NBFIBPGCTL", NULL, 4 },
	{ 6, 0xc8, "NBFIBPATTBUF1", NULL, 3 },
	{ 6, 0xcc, "NBFIBTXMSK", NULL, 2 },
	{ 6, 0xd0, "NBFIBRXMSK", NULL, 2 },
	{ 6, 0xd4, "NBFIBTXSHFT", NULL, 2 },
	{ 6, 0xd8, "NBFIBRXSHFT", NULL, 2 },
	{ 6, 0xdc, "NBFIBRXLNERR", NULL, 2 },
	{ 6, 0xe0, "NBFIBPATTBUF2", NULL, 3 },
	{ 6, 0xe4, "NBFIBPATT2EN", NULL, 2 },
	{ 6, 0xf0, "NBFIBINIT", NULL, 4 },
	{ 6, 0xf4, "NBIBISTMISC", NULL, 3 },

	{ 7, 0x40, "ODRV_ADJ_ADDR_A", NULL, 3 },
	{ 7, 0x43, "ODRV_ADJ_CAS_A", NULL, 3 },
	{ 7, 0x46, "ODRV_ADJ_CKE_A", NULL, 3 },
	{ 7, 0x49, "ODRV_ADJ_ODT_A", NULL, 3 },
	{ 7, 0x4c, "ODRV_ADJ_ADDR_B", NULL, 3 },
	{ 7, 0x4f, "ODRV_ADJ_CAS_B", NULL, 3 },
	{ 7, 0x52, "ODRV_ADJ_CKE_B", NULL, 3 },
	{ 7, 0x55, "ODRV_ADJ_ODT_B", NULL, 3 },
	{ 7, 0x58, "ODRV_ADJ_CLK0", NULL, 3 },
	{ 7, 0x5b, "ODRV_ADJ_CLK1", NULL, 3 },
	{ 7, 0x5e, "ODRV_ADJ_CLK2", NULL, 3 },
	{ 7, 0x61, "ODRV_ADJ_CLK3", NULL, 3 },
	{ 7, 0x64, "ODRV_ADJ_DQ_DQS", NULL, 3 },
	{ 7, 0x67, "DDR_ODTCTL", NULL, 1 },
	{ 7, 0x69, "DDR_ZCAL", NULL, 2 },
	{ 7, 0x6c, "DDR_ZCTL", NULL, 4 },
	{ 7, 0x78, "VHOSTCNTL", NULL, 1 },
	{ 7, 0x79, "DQSDLY_SMBUSCTL", NULL, 1 },
	{ 7, 0x7c, "SPAD1", NULL, 4 },
	{ 7, 0x80, "CTL_ADDR_A", NULL, 1 },
	{ 7, 0x81, "CTL_CAS_A", NULL, 1 },
	{ 7, 0x82, "CTL_CKE_A", NULL, 1 },
	{ 7, 0x83, "CTL_ODT_A", NULL, 1 },
	{ 7, 0x84, "CTL_ADDR_B", NULL, 1 },
	{ 7, 0x85, "CTL_CAS_B", NULL, 1 },
	{ 7, 0x86, "CTL_CKE_B", NULL, 1 },
	{ 7, 0x87, "CTL_ODT_B", NULL, 1 },
	{ 7, 0x88, "CTL_CLK0", NULL, 1 },
	{ 7, 0x89, "CTL_CLK1", NULL, 1 },
	{ 7, 0x8a, "CTL_CLK2", NULL, 1 },
	{ 7, 0x8b, "CTL_CLK3", NULL, 1 },
	{ 7, 0x8c, "CTL_DQ", NULL, 1 },
	{ 7, 0x8d, "CTL_DQS", NULL, 1 },
	{ 0, 0x00, NULL, NULL, 0 },
};

static void dump_amb(volatile void *ambconfig, int branch, int channel, int amb)
{
	struct amb_register *reg;
	static int lastreg, lastfn;
	int bytes;

	volatile void *base = ambconfig + \
		((branch << 16) | (channel << 15) | (amb << 11));

	if ((amb_read_config32(base, 0, 0) == 0xffffffff) |
	    (amb_read_config32(base, 0, 0) == 0x00000000))
		return;

	printf("AMB %d, branch %d, channel %d register dump:\n"
	       "============================================\n",
	       amb, branch, channel);

	for(reg = amb_registers; reg->name; reg++) {
		if (reg->fn == lastfn && reg->offset > 0 && reg->offset - lastreg) {
			bytes = reg->offset - lastreg;

			do {
				if (!(lastreg & 3) && bytes >= 4) {
					amb_printreg32(base, reg->fn, lastreg, "RESERVED", 0);
					bytes -= 4;
					lastreg += 4;
				} else if (!(lastreg & 1) && bytes >= 2) {
					amb_printreg16(base, reg->fn, lastreg, "RESERVED", 0);
					bytes -= 2;
					lastreg += 2;
				} else {
					amb_printreg8(base, reg->fn, lastreg, "RESERVED", 0);
					bytes -= 1;
					lastreg += 1;
				}
			} while(bytes > 0);
		}

		switch(reg->width) {
		case 1:
			amb_printreg8(base, reg->fn, reg->offset, reg->name, 1);
			break;
		case 2:
			amb_printreg16(base, reg->fn, reg->offset, reg->name, 1);
			break;

		case 3:
			amb_printreg24(base, reg->fn, reg->offset, reg->name, 1);
			break;

		case 4:
			amb_printreg32(base, reg->fn, reg->offset, reg->name, 1);
			break;

		default:
			break;
		}

		if (reg->printfunc)
			reg->printfunc(base, reg->fn, reg->offset, reg->name, 1);
		lastreg = reg->offset + reg->width;
		lastfn = reg->fn;
	}
	printf("\n\n");
}

int print_ambs(struct pci_dev *dev, struct pci_access *pacc)
{
	struct pci_dev *dev16;
	int branch, channel, amb;
	int max_branch, max_channel, max_amb;
	volatile void *ambconfig;
	uint64_t ambconfig_phys;

	printf("\n============= AMBs ============\n\n");

	switch (dev->device_id) {
	case PCI_DEVICE_ID_INTEL_I5000P:
	case PCI_DEVICE_ID_INTEL_I5000X:
	case PCI_DEVICE_ID_INTEL_I5000Z:

		max_branch = 2;

		if (!(dev16 = pci_get_dev(pacc, 0, 0, 0x10, 0))) {
			perror("Error: no device 0:16.0\n");
			return 1;
		}

		ambconfig_phys = ((uint64_t)pci_read_long(dev16, 0x4c) << 32) |
			pci_read_long(dev16, 0x48);

		max_channel = pci_read_byte(dev16, 0x56)/max_branch;
		max_amb = pci_read_byte(dev16, 0x57);
		pci_free_dev(dev16);
		break;

	default:
		fprintf(stderr, "Error: Dumping AMBs on this MCH is not (yet) supported.\n");
		return 1;
	}

	if (!(ambconfig = map_physical(ambconfig_phys, AMB_CONFIG_SPACE_SIZE))) {
		fprintf(stderr, "Error mapping AMB config space\n");
		return 1;
	}

	for(branch = 0; branch < max_branch; branch++) {
		for(channel = 0; channel < max_channel; channel++) {
			for(amb = 0; amb < max_amb; amb++) {
				dump_amb(ambconfig, branch, channel, amb);
			}
		}
	}
	unmap_physical((void *)ambconfig, AMB_CONFIG_SPACE_SIZE);
	return 0;
}
