/*
 * msys_doc.h: header file of msys_doc.c
 *
 *
 * Copyright 2003   Niki W. Waibel <niki.waibel@gmx.net>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */



#ifndef __MSYS_DOC_H__
#define __MSYS_DOC_H__ 1



/* idea from include/linux/mtd/doc2000.h */
/* registers with __ should not be read/written directly */
#define MSYSTEMS_DOC_R__ChipID                    0x1000
#define MSYSTEMS_DOC_R__DOCStatus                 0x1001
#define MSYSTEMS_DOC_W_DOCControl                 0x1002
#define MSYSTEMS_DOC_R_FloorSelect                0x1003
#define MSYSTEMS_DOC_W_FloorSelect                0x1003
#define MSYSTEMS_DOC_R__CDSNControl               0x1004
#define MSYSTEMS_DOC_W__CDSNControl               0x1004
#define MSYSTEMS_DOC_R_CDSNDeviceSelect           0x1005
#define MSYSTEMS_DOC_W_CDSNDeviceSelect           0x1005
#define MSYSTEMS_DOC_R_ECCConfiguration           0x1006
#define MSYSTEMS_DOC_W_ECCConfiguration           0x1006
#define MSYSTEMS_DOC_R_CDSNSlowIO                 0x100d
#define MSYSTEMS_DOC_W_CDSNSlowIO                 0x100d
#define MSYSTEMS_DOC_R_ECCSyndrome0               0x1010
#define MSYSTEMS_DOC_R_ECCSyndrome1               0x1011
#define MSYSTEMS_DOC_R_ECCSyndrome2               0x1012
#define MSYSTEMS_DOC_R_ECCSyndrome3               0x1013
#define MSYSTEMS_DOC_R_ECCSyndrome4               0x1014
#define MSYSTEMS_DOC_R_ECCSyndrome5               0x1015
#define MSYSTEMS_DOC_R_AliasResolution            0x101b
#define MSYSTEMS_DOC_W_AliasResolution            0x101b
#define MSYSTEMS_DOC_R_ConfigurationInput         0x101c
#define MSYSTEMS_DOC_W_ConfigurationInput         0x101c
#define MSYSTEMS_DOC_R_ReadPipelineInitialization 0x101d
#define MSYSTEMS_DOC_W_WritePipelineTermination   0x101e
#define MSYSTEMS_DOC_R_LastDataRead               0x101f
#define MSYSTEMS_DOC_W_LastDataRead               0x101f
#define MSYSTEMS_DOC_R_NOP                        0x1020
#define MSYSTEMS_DOC_W_NOP                        0x1020

#define MSYSTEMS_DOC_R_IPL_0x0000                 0x0000
#define MSYSTEMS_DOC_R_IPL_0x0001                 0x0001
#define MSYSTEMS_DOC_R_IPL_0x0002                 0x0002
#define MSYSTEMS_DOC_R_IPL_0x0003                 0x0003

#define MSYSTEMS_DOC_R_CDSNIO_BASE                0x0800
#define MSYSTEMS_DOC_W_CDSNIO_BASE                0x0800



#define doc_read(base,reg) \
	(*(volatile uint8_t *)(base + MSYSTEMS_DOC_R_##reg))

#define doc_read_nop(base) \
	doc_read(base, NOP)

#define doc_read_2nop(base) \
	{ doc_read_nop(base); doc_read_nop(base); }

#define doc_read_4nop(base) \
	{ doc_read_2nop(base); doc_read_2nop(base); }

#define doc_write(data,base,reg) \
	(*(volatile uint8_t *)(base + MSYSTEMS_DOC_W_##reg)) = data

#define doc_write_nop(base) \
	doc_write(0, base, NOP)

#define doc_write_2nop(base) \
	{ doc_write_nop(base); doc_write_nop(base); }

#define doc_write_4nop(base) \
	{ doc_write_2nop(base); doc_write_2nop(base); }

#define _doc_busy(base)   /* 0: ready; -1: busy */ \
	( ((doc_read(base, _CDSNControl) & 0x80) >> 7) - 1)

#define doc_toggle(base)   /* 0, 1, 0, 1, 0, 1, ... if a doc is present */ \
	( (doc_read(base, ECCConfiguration) & 0x04) >> 2 )



extern int probe_md2802(struct flashchip *flash);
extern int read_md2802(struct flashchip *flash, uint8_t *buf);
extern int erase_md2802(struct flashchip *flash);
extern int write_md2802(struct flashchip *flash, uint8_t *buf);



#endif				/* !__MSYS_DOC_H__ */
