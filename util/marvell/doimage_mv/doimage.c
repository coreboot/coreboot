/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.

*******************************************************************************/
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <limits.h>

#define _HOST_COMPILER
#include "bootstrap_def.h"

#include "doimage.h"

#undef DEBUG

#ifdef DEBUG
#define DB(x...) fprintf(stdout, x)
#else
#define DB(x...)
#endif

/* Global variables */

int f_in = -1;
int f_out = -1;
int f_header = -1;
struct stat fs_stat;

/*******************************************************************************
*    pre_load_image
*          pre-load the binary image into memory buffer taking into account
*paddings
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int pre_load_image(USER_OPTIONS *opt, char *buf_in)
{
	int offset = 0;

	opt->image_buf = malloc(opt->image_sz);
	if (opt->image_buf == NULL)
		return -1;

	memset(opt->image_buf, 0, opt->image_sz);

	if ((opt->pre_padding) && (opt->prepadding_size)) {
		memset(opt->image_buf, 0x5, opt->prepadding_size);
		offset = opt->prepadding_size;
	}

	if ((opt->post_padding) && (opt->postpadding_size))
		memset(opt->image_buf + opt->image_sz - 4 -
			   opt->postpadding_size,
		       0xA, opt->postpadding_size);

	memcpy(opt->image_buf + offset, buf_in, fs_stat.st_size);

	return 0;
} /* end of pre_load_image() */

/*******************************************************************************
*    build_twsi_header
*          create TWSI header and write it into output stream
*    INPUT:
*          opt        user options
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_twsi_header(USER_OPTIONS *opt)
{
	FILE *f_twsi;
	MV_U8 *tmpTwsi = NULL;
	MV_U32 *twsi_reg;
	int i;
	MV_U32 twsi_size = 0;

	tmpTwsi = malloc(MAX_TWSI_HDR_SIZE);
	if (tmpTwsi == NULL) {
		fprintf(stderr, "TWSI space allocation error!\n");
		return -1;
	}
	memset(tmpTwsi, 0xFF, MAX_TWSI_HDR_SIZE);
	twsi_reg = (MV_U32 *)tmpTwsi;

	f_twsi = fopen(opt->fname_twsi, "r");
	if (f_twsi == NULL) {
		fprintf(stderr, "Failed to open file '%s'\n", opt->fname_twsi);
		perror("Error:");
		return -1;
	}

	for (i = 0; i < (MAX_TWSI_HDR_SIZE / 4); i++) {
		if (EOF == fscanf(f_twsi, "%x\n", twsi_reg))
			break;

		/* Swap Enianess */
		*twsi_reg =
		    (((*twsi_reg >> 24) & 0xFF) | ((*twsi_reg >> 8) & 0xFF00) |
		     ((*twsi_reg << 8) & 0xFF0000) |
		     ((*twsi_reg << 24) & 0xFF000000));
		twsi_reg++;
	}

	fclose(f_twsi);

	/* Align to size = 512,1024,.. with at least 8 0xFF bytes @ the end */
	twsi_size = ((((i + 2) * 4) & ~0x1FF) + 0x200);

	if ((write(f_out, tmpTwsi, twsi_size)) != twsi_size) {
		fprintf(stderr, "Error writing %s file\n", opt->fname.out);
		return -1;
	}

	return 0;
} /* end of build_twsi_header() */

/*******************************************************************************
*    build_reg_header
*        create BIN header and write it into output stream
*    INPUT:
*       fname        - source file name
*       buffer       - Start address of boot image buffer
*       current_size - number of bytes already placed into the boot image buffer
*    OUTPUT:
*       none
*    RETURN:
*	size of a reg header or 0 on fail
*******************************************************************************/
int build_reg_header(char *fname, MV_U8 *buffer, MV_U32 current_size)
{
	FILE *f_dram;
	BHR_t *mainHdr = (BHR_t *)buffer;
	headExtBHR_t *headExtHdr = headExtHdr =
	    (headExtBHR_t *)(buffer + current_size);
	tailExtBHR_t *prevExtHdrTail =
	    NULL; /* tail of the previous extension header */
	MV_U32 max_bytes_to_write;
	MV_U32 *dram_reg =
	    (MV_U32 *)(buffer + current_size + sizeof(headExtBHR_t));
	MV_U32 tmp_len;
	int i;

	if (mainHdr->ext == 255) {
		fprintf(stderr, "Maximum number of extensions reached!\n");
		return 0;
	}

	/* Indicate next header in previous extension if any */
	if (mainHdr->ext != 0) {
		prevExtHdrTail = (tailExtBHR_t *)(buffer + current_size -
						  sizeof(tailExtBHR_t));
		prevExtHdrTail->nextHdr = 1;
	}

	/* Count extension headers in the main header */
	mainHdr->ext++;

	headExtHdr->type = EXT_HDR_TYP_REGISTER;
	max_bytes_to_write = MAX_HEADER_SIZE - current_size - EXT_HDR_BASE_SIZE;
	f_dram = fopen(fname, "r");
	if (f_dram  == NULL) {
		fprintf(stderr, "Failed to open file '%s'\n", fname);
		perror("Error:");
		return 0;
	}

	for (i = 0; i < (max_bytes_to_write / 8); i += 2) {
		if (fscanf(f_dram, "%x %x\n", &dram_reg[i], &dram_reg[i + 1]) ==
		    EOF)
			break;
		else if ((dram_reg[i] == 0x0) && (dram_reg[i + 1] == 0x0))
			break;
	}

	fclose(f_dram);

	if (i >= (max_bytes_to_write / 8)) {
		fprintf(stderr, "Registers configure exceeds maximum size\n");
		return 0;
	}

	/* Include extended header head and tail sizes */
	tmp_len = EXT_HDR_BASE_SIZE + i * 4;
	/* Write total length into the current header fields */
	EXT_HDR_SET_LEN(headExtHdr, tmp_len);

	return tmp_len;
} /* end of build_reg_header() */

/*******************************************************************************
*    build_bin_header
*        create BIN header and write it into putput stream
*    INPUT:
*       fname        - source file name
*       buffer       - Start address of boot image buffer
*       current_size - number of bytes already placed into the boot image buffer
*    OUTPUT:
*       none
*    RETURN:
*	size of reg header
*******************************************************************************/
int build_bin_header(char *fname, MV_U8 *buffer, MV_U32 current_size)
{
	FILE *f_bin;
	BHR_t *mainHdr = (BHR_t *)buffer;
	headExtBHR_t *headExtHdr = (headExtBHR_t *)(buffer + current_size);
	tailExtBHR_t *prevExtHdrTail =
	    NULL; /* tail of the previous extension header */
	MV_U32 max_bytes_to_write;
	MV_U32 *bin_reg =
	    (MV_U32 *)(buffer + current_size + sizeof(headExtBHR_t));
	MV_U32 tmp_len;
	int i;

	if (mainHdr->ext == 255) {
		fprintf(stderr, "Maximum number of extensions reached!\n");
		return 0;
	}

	/* Indicate next header in previous extension if any */
	if (mainHdr->ext != 0) {
		prevExtHdrTail = (tailExtBHR_t *)(buffer + current_size -
						  sizeof(tailExtBHR_t));
		prevExtHdrTail->nextHdr = 1;
	}

	/* Count extension headers in main header */
	mainHdr->ext++;

	headExtHdr->type = EXT_HDR_TYP_BINARY;
	max_bytes_to_write = MAX_HEADER_SIZE - current_size - EXT_HDR_BASE_SIZE;

	f_bin = fopen(fname, "r");
	if (f_bin == NULL) {
		fprintf(stderr, "Failed to open file '%s'\n", fname);
		perror("Error:");
		return 0;
	}

	for (i = 0; i < (max_bytes_to_write / 4); i++) {
		if (fread(bin_reg, 1, 4, f_bin) != 4)
			break;

		bin_reg++;
	}

	fclose(f_bin);

	if (i >= (max_bytes_to_write / 4)) {
		fprintf(stderr, "Binary extension exeeds the maximum size\n");
		return 0;
	}

	/* Include extended header head and tail sizes */
	tmp_len = EXT_HDR_BASE_SIZE + i * 4;
	/* Write total length into the current header fields */
	EXT_HDR_SET_LEN(headExtHdr, tmp_len);

	return tmp_len;
} /* end of build_exec_header() */

/*******************************************************************************
*    build_headers
*          build headers block based on user options and write it into output
*stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_headers(USER_OPTIONS *opt, char *buf_in)
{
	BHR_t *hdr = NULL;
	secExtBHR_t *secExtHdr = NULL;
	headExtBHR_t *headExtHdr = NULL;
	tailExtBHR_t *tailExtHdr = NULL;
	MV_U8 *tmpHeader = NULL;
	int i;
	MV_U32 header_size = 0;
	int size_written = 0;
	MV_U32 max_bytes_to_write;
	int error = 1;

	tmpHeader = malloc(MAX_HEADER_SIZE);
	if (tmpHeader == NULL) {
		fprintf(stderr, "Header space allocation error!\n");
		goto header_error;
	}

	memset(tmpHeader, 0, MAX_HEADER_SIZE);
	hdr = (BHR_t *)tmpHeader;

	switch (opt->image_type) {
	case IMG_SATA:
		hdr->blockID = IBR_HDR_SATA_ID;
		break;

	case IMG_UART:
		hdr->blockID = IBR_HDR_UART_ID;
		break;

	case IMG_FLASH:
		hdr->blockID = IBR_HDR_SPI_ID;
		break;

	case IMG_MMC:
		hdr->blockID = IBR_HDR_MMC_ID;
		break;

	case IMG_NAND:
		hdr->blockID = IBR_HDR_NAND_ID;
		/*hdr->nandEccMode = (MV_U8)opt->nandEccMode; <<== reserved */
		/*hdr->nandPageSize = (MV_U16)opt->nandPageSize; <<== reserved
		 */
		hdr->nandBlockSize = (MV_U8)opt->nandBlkSize;
		if ((opt->nandCellTech == 'S') || (opt->nandCellTech == 's'))
			hdr->nandTechnology = MAIN_HDR_NAND_SLC;
		else
			hdr->nandTechnology = MAIN_HDR_NAND_MLC;
		break;

	case IMG_PEX:
		hdr->blockID = IBR_HDR_PEX_ID;
		break;

	case IMG_I2C:
		hdr->blockID = IBR_HDR_I2C_ID;
		break;

	default:
		fprintf(stderr,
			"Illegal image type %d for header construction!\n",
			opt->image_type);
		return 1;
	}

	/* Debug print options */
	if (opt->flags & p_OPTION_MASK)
		hdr->flags &= ~BHR_FLAG_PRINT_EN;
	else
		hdr->flags |=
		    BHR_FLAG_PRINT_EN; /* Enable printing by default */

	if (opt->flags & b_OPTION_MASK) {
		switch (opt->baudRate) {
		case 2400:
			hdr->options = BHR_OPT_BAUD_2400;
			break;

		case 4800:
			hdr->options = BHR_OPT_BAUD_4800;
			break;

		case 9600:
			hdr->options = BHR_OPT_BAUD_9600;
			break;

		case 19200:
			hdr->options = BHR_OPT_BAUD_19200;
			break;

		case 38400:
			hdr->options = BHR_OPT_BAUD_38400;
			break;

		case 57600:
			hdr->options = BHR_OPT_BAUD_57600;
			break;

		case 115200:
			hdr->options = BHR_OPT_BAUD_115200;
			break;

		default:
			fprintf(stderr, "Unsupported baud rate - %d!\n",
				opt->baudRate);
			return 1;
		}
	} else
		hdr->options = BHR_OPT_BAUD_DEFAULT;

	/* debug port number */
	if (opt->flags & u_OPTION_MASK)
		hdr->options |= (opt->debugPortNum << BHR_OPT_UART_PORT_OFFS) &
				BHR_OPT_UART_PORT_MASK;

	/* debug port MPP setup index */
	if (opt->flags & m_OPTION_MASK)
		hdr->options |= (opt->debugPortMpp << BHR_OPT_UART_MPPS_OFFS) &
				BHR_OPT_UART_MPPS_MASK;

	hdr->destinationAddr = opt->image_dest;
	hdr->executionAddr = (MV_U32)opt->image_exec;
	hdr->version = MAIN_HDR_VERSION;
	hdr->blockSize = fs_stat.st_size;

	header_size = sizeof(BHR_t);

	/* Update Block size address */
	if ((opt->flags & X_OPTION_MASK) || (opt->flags & Y_OPTION_MASK)) {
		/* Align padding to 32 bit */
		if (opt->prepadding_size & 0x3)
			opt->prepadding_size +=
			    (4 - (opt->prepadding_size & 0x3));

		if (opt->postpadding_size & 0x3)
			opt->postpadding_size +=
			    (4 - (opt->postpadding_size & 0x3));

		hdr->blockSize += opt->prepadding_size + opt->postpadding_size;
	}

	/* Align the image size to 4 byte boundary */
	if (hdr->blockSize & 0x3) {
		opt->bytesToAlign = (4 - (hdr->blockSize & 0x3));
		hdr->blockSize += opt->bytesToAlign;
	}

	/***************************** TWSI Header
	 * ********************************/

	/* TWSI header has a special purpose and placed ahead of the main header
	 */
	if (opt->flags & M_OPTION_MASK) {
		if (opt->fname_twsi) {
			if (build_twsi_header(opt) != 0)
				goto header_error;
		} /* opt->fname_twsi */
	}	 /* (opt->flags & M_OPTION_MASK) */

	/************************** End of TWSI Header
	 * ****************************/

	/********************** Single Register Header
	 * ***************************/

	if (opt->flags & R_OPTION_MASK) {
		if (opt->fname_dram) {
			MV_U32 rhdr_len = build_reg_header(
			    opt->fname_dram, tmpHeader, header_size);
			if (rhdr_len <= 0)
				goto header_error;

			header_size += rhdr_len;
			tailExtHdr = (tailExtBHR_t *)(tmpHeader + header_size -
						      sizeof(tailExtBHR_t));
		} /* if (fname_dram) */
	}	 /* if (opts & R_OPTION_MASK) */

	/******************** End of Single Register Header
	 * ************************/

	/************************* Single Binary Header
	 * ****************************/

	if (opt->flags & G_OPTION_MASK) {
		if (opt->fname_bin) {
			MV_U32 bhdr_len = build_bin_header(
			    opt->fname_bin, tmpHeader, header_size);
			if (bhdr_len <= 0)
				goto header_error;

			header_size += bhdr_len;
			tailExtHdr = (tailExtBHR_t *)(tmpHeader + header_size -
						      sizeof(tailExtBHR_t));
		} /* if (fname_bin) */
	}	 /* (opt->flags & G_OPTION_MASK) */

	/******************* End of Single Binary Header
	 * ***************************/

	/************************* BIN/REG Headers list
	 * ****************************/

	if (opt->flags & C_OPTION_MASK) {
		if (opt->fname_list) {
			FILE *f_list;
			char buffer[PATH_MAX + 5];
			char *fname;
			MV_U32 hdr_len = 0, last;
			int (*build_hdr_func)(char *, MV_U8 *, MV_U32);

			f_list = fopen(opt->fname_list, "r");
			if (f_list == NULL) {
				fprintf(stderr, "File not found\n");
				goto header_error;
			}
			/* read the headers list row by row */
			while (fgets(buffer, PATH_MAX + 4, f_list) != NULL) {
				/* Ignore strings that are not starting with
				 * BIN/REG */
				if (strncmp(buffer, "BIN", 3) == 0)
					build_hdr_func = build_bin_header;
				else if (strncmp(buffer, "REG", 3) == 0)
					build_hdr_func = build_reg_header;
				else
					continue;

				fname = buffer + 3;
				/* strip leading spaces/tabs if any */
				while ((strncmp(fname, " ", 1) == 0) ||
				       (strncmp(fname, "\t", 1) == 0))
					fname++;

				/* strip trailing LF/CR symbols */
				last = strlen(fname) - 1;
				while ((strncmp(fname + last, "\n", 1) == 0) ||
				       (strncmp(fname + last, "\r", 1) == 0)) {
					fname[last] = 0;
					last--;
				}
				/* Insert required header into the output buffer
				 */
				hdr_len = build_hdr_func(fname, tmpHeader,
							 header_size);
				if (hdr_len <= 0)
					goto header_error;

				header_size += hdr_len;
				tailExtHdr =
				    (tailExtBHR_t *)(tmpHeader + header_size -
						     sizeof(tailExtBHR_t));
			}

			fclose(f_list);
		} /* if (fname_list) */
	}	 /* (opt->flags & C_OPTION_MASK) */

	/********************** End of BIN/REG Headers list
	 * ************************/

	/* Align the headers block to... */
	if (opt->image_type == IMG_NAND) {
		/* ... NAND page size */
		header_size +=
		    opt->nandPageSize - (header_size & (opt->nandPageSize - 1));
	} else if ((opt->image_type == IMG_SATA) ||
		   (opt->image_type == IMG_MMC)) {
		/* ... disk logical block size */
		header_size += 512 - (header_size & 0x1FF);
	} else if (opt->image_type == IMG_UART) {
		/* ... Xmodem packet size */
		header_size += 128 - (header_size & 0x7F);
	}
	/* Setup the image source address */
	if (opt->image_type == IMG_SATA) {
		if ((opt->image_source) && (opt->image_source > header_size))
			hdr->sourceAddr = opt->image_source;
		else
			hdr->sourceAddr =
			    header_size >> 9; /* Already aligned to 512 */
	} else {
		if ((opt->image_source) && (opt->image_source > header_size)) {
			hdr->sourceAddr = opt->image_source;
			opt->img_gap = opt->image_source - header_size;
		} else {
			/* The source imgage address should be aligned
			   to 32 byte boundary (cache line size).
			   For NAND it should be aligned to 512 bytes boundary
			   (for ECC)
			   The image immediately follows the header block,
			   so if the source address is undefined, it should be
			   derived from the header size.
			   The headers size is always  alighed to 4 byte
			   boundary */
			int boundary = 32;

			if ((opt->image_type == IMG_NAND) ||
			    (opt->image_type == IMG_MMC))
				boundary = 512;

			if (header_size & (boundary - 1))
				opt->img_gap =
				    boundary - (header_size & (boundary - 1));

			hdr->sourceAddr = header_size + opt->img_gap;
		}
	}

	/* source address and extension headers number can be written now */
	fprintf(stdout,
		"Ext. headers = %d, Header size = %d bytes Hdr-to-Img gap = %d bytes\n",
		hdr->ext, header_size, opt->img_gap);

	/* If not UART/TWSI image, an extra word for boot image checksum is
	 * needed */
	if ((opt->image_type == IMG_FLASH) || (opt->image_type == IMG_NAND) ||
	    (opt->image_type == IMG_MMC) || (opt->image_type == IMG_SATA) ||
	    (opt->image_type == IMG_PEX) || (opt->image_type == IMG_I2C))
		hdr->blockSize += 4;

	fprintf(stdout,
		"New image size = %#x[%d] Source image size = %#x[%d]\n",
		hdr->blockSize, hdr->blockSize, (unsigned int)fs_stat.st_size,
		(int)fs_stat.st_size);

	hdr->hdrSizeMsb = (header_size & 0x00FF0000) >> 16;
	hdr->hdrSizeLsb = header_size & 0x0000FFFF;

	opt->image_sz = hdr->blockSize;

	/* Load image into memory buffer */
	if (REGULAR_IMAGE(opt)) {
		if (0 != pre_load_image(opt, buf_in)) {
			fprintf(stderr, "Failed image pre-load!\n");
			goto header_error;
		}
	}

	/* Now the headers block checksum should be calculated and wrote in the
	 * header */
	/* This checksum value should be valid for both secure and unsecure boot
	 * modes */
	/* This value will be checked first before RSA key and signature
	 * verification */
	hdr->checkSum = checksum8((void *)hdr, MAIN_HDR_GET_LEN(hdr), 0);

	/* Write to file(s) */
	if (opt->header_mode == HDR_IMG_TWO_FILES) {
		/* copy header to output image */
		size_written = write(f_header, tmpHeader, header_size);
		if (size_written != header_size) {
			fprintf(stderr, "Error writing %s file\n"
						, opt->fname.hdr_out);
			goto header_error;
		}

		fprintf(stdout, "====>>>> %s was created\n",
			opt->fname_arr[HDR_FILE_INDX]);
		/* if (header_mode == HDR_IMG_TWO_FILES) */
	} else {
		/* copy header to output image */
		size_written = write(f_out, tmpHeader, header_size);
		if (size_written != header_size) {
			fprintf(stderr, "Error writing %s file\n"
						, opt->fname.out);
			goto header_error;
		}

	} /* if (header_mode != HDR_IMG_TWO_FILES) */

	error = 0;

header_error:

	if (tmpHeader)
		free(tmpHeader);

	return error;

} /* end of build_headers() */

/*******************************************************************************
*    build_bootrom_img
*          create image in bootrom format and write it into output stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_bootrom_img(USER_OPTIONS *opt, char *buf_in)
{
	unsigned int CRC_32 = 0;
	int tmpSize = BOOTROM_SIZE - sizeof(CRC_32);
	char *tmpImg = NULL;
	int size_written = 0;
	int error = 1;

	tmpImg = malloc(tmpSize);
	if (tmpImg == NULL)
		return 1;

	/* PAD image with Zeros until BOOTROM_SIZE*/
	memcpy(tmpImg, buf_in, fs_stat.st_size);
	memset(tmpImg + fs_stat.st_size, 0, tmpSize - fs_stat.st_size);

	/* copy input image to output image */
	size_written = write(f_out, tmpImg, tmpSize);

	/* calculate checsum */
	CRC_32 = crc32(0, (u32 *)tmpImg, tmpSize / 4);
	tmpSize += sizeof(CRC_32);
	printf("Image CRC32 (size = %d) = 0x%08x\n", tmpSize, CRC_32);

	size_written += write(f_out, &CRC_32, sizeof(CRC_32));

	if (size_written == tmpSize)
		error = 0;

bootrom_img_error:

	if (tmpImg)
		free(tmpImg);

	return error;
} /* end of build_bootrom_img() */

/*******************************************************************************
*    build_hex_img
*          create image in hex format and write it into output stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_hex_img(USER_OPTIONS *opt, char *buf_in)
{
	FILE *f_desc[2] = {NULL};
	char *tmpImg = NULL;
	int hex_len;
	int hex_unaligned_len = 0;
	unsigned char *hex8 = NULL;
	unsigned char tmp8;
	unsigned short *hex16 = NULL;
	unsigned short tmp16;
	unsigned int *hex32 = NULL;
	unsigned int tmp32;
	unsigned int tmp32_low;
	int size_written = 0;
	int alignment = 0;
	int files_num;
	int i;
	int error = 1;

	/* Calculate aligned image size */
	hex_len = fs_stat.st_size;

	alignment = opt->hex_width >> 3;
	hex_unaligned_len = fs_stat.st_size & (alignment - 1);

	if (hex_unaligned_len) {
		hex_len -= hex_unaligned_len;
		hex_len += alignment;
	}

	/* Copy the input image to memory buffer */
	tmpImg = malloc(hex_len);
	if (tmpImg == NULL)
		goto hex_image_end;

	memset(tmpImg, 0, hex_len);
	memcpy(tmpImg, buf_in, fs_stat.st_size);

	if (opt->fname.hdr_out)
		files_num = 2;
	else
		files_num = 1;

	for (i = 0; i < files_num; i++) {
		f_desc[i] = fopen(opt->fname_arr[i + 1], "w");
		if (f_desc[i] == NULL)
			goto hex_image_end;
	}

	switch (opt->hex_width) {
	case 8:
		hex8 = (unsigned char *)tmpImg;

		for (i = 0; hex_len > 0; hex_len--) {
			fprintf(f_desc[i], "%02X\n", *hex8++);
			size_written += 1;
			i ^= files_num - 1;
		}
		break;

	case 16:
		hex16 = (unsigned short *)tmpImg;

		for (; hex_len > 0; hex_len -= 2) {
			fprintf(f_desc[0], "%04X\n", *hex16++);
			size_written += 2;
		}
		break;

	case 32:
		hex32 = (unsigned int *)tmpImg;

		for (; hex_len > 0; hex_len -= 4) {
			fprintf(f_desc[0], "%08X\n", *hex32++);
			size_written += 4;
		}
		break;

	case 64:
		hex32 = (unsigned int *)tmpImg;

		for (; hex_len > 0; hex_len -= 8) {
			fprintf(f_desc[0], "%08X%08X\n", *hex32++, *hex32++);
			size_written += 8;
		}
		break;

	} /* switch (opt->hex_width)*/

	error = 0;

hex_image_end:

	if (tmpImg)
		free(tmpImg);

	for (i = 0; i < files_num; i++) {
		if (f_desc[i] != NULL)
			fclose(f_desc[i]);
	}

	return error;
} /* end of build_hex_img() */

/*******************************************************************************
*    build_bin_img
*          create image in binary format and write it into output stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_bin_img(USER_OPTIONS *opt, char *buf_in)
{
	FILE *f_ds = NULL;
	FILE *f_desc[4] = {NULL};
	char *tmpImg = NULL;
	int hex_len = 0;
	int one_file_len = 0;
	int size_written = 0;
	int alignment = 0;
	int hex_unaligned_len = 0;
	unsigned char *hex8 = NULL;
	unsigned char tmp8;
	unsigned short *hex16 = NULL;
	unsigned short tmp16;
	unsigned long *hex32 = NULL;
	unsigned long tmp32;
	unsigned long tmp32low;
	int i;
	int fidx;
	int files_num = 1;
	int error = 1;

	/* Calculate aligned image size */
	hex_len = fs_stat.st_size;

	alignment = opt->hex_width >> 3;
	hex_unaligned_len = fs_stat.st_size & (alignment - 1);

	if (hex_unaligned_len) {
		hex_len -= hex_unaligned_len;
		hex_len += alignment;
	}

	/* prepare output files */
	if (opt->fname.romd) /*16KB*/
		files_num = 4;
	else if (opt->fname.romc) /*12KB*/
		files_num = 3;
	else if (opt->fname.hdr_out)
		files_num = 2;

	one_file_len = hex_len / files_num;

	for (i = 0; i < files_num; i++) {
		f_desc[i] = fopen(opt->fname_arr[i + 1], "w");
		if (f_desc[i] == NULL)
			goto bin_image_end;
	}

	/* Copy the input image to memory buffer */
	tmpImg = malloc(hex_len);
	if (tmpImg == NULL)
		goto bin_image_end;

	memset(tmpImg, 0, (hex_len));
	memcpy(tmpImg, buf_in, fs_stat.st_size);

	/* Split output image buffer according to width and number of files */
	switch (opt->hex_width) {
	case 8:
		hex8 = (unsigned char *)tmpImg;
		if (files_num != 2) {
			fprintf(stderr,
				"Must supply two output file names for this width!\n");
			goto bin_image_end;
		}

		for (fidx = 1; (fidx >= 0) && (hex_len > 0); fidx--) {
			f_ds = f_desc[1 - fidx];

			for (; hex_len > (fidx * one_file_len); hex_len--) {
				tmp8 = *hex8;

				for (i = 0; i < opt->hex_width; i++) {
					fprintf(f_ds, "%d",
						((tmp8 & 0x80) >> 7));
					tmp8 <<= 1;
				}
				fprintf(f_ds, "\n");

				hex8++;
				size_written += 1;
			}
		}
		break;

	case 16:
		hex16 = (unsigned short *)tmpImg;

		for (; hex_len > 0; hex_len -= 2) {
			tmp16 = *hex16;

			for (i = 0; i < opt->hex_width; i++) {
				fprintf(f_desc[0], "%d",
					((tmp16 & 0x8000) >> 15));
				tmp16 <<= 1;
			}
			fprintf(f_desc[0], "\n");

			hex16++;
			size_written += 2;
		}
		break;

	case 32:
		hex32 = (long *)tmpImg;

		for (fidx = files_num - 1; (fidx >= 0) && (hex_len > 0);
		     fidx--) {
			f_ds = f_desc[files_num - 1 - fidx];

			for (; hex_len > (fidx * one_file_len); hex_len -= 4) {
				tmp32 = *hex32;

				for (i = 0; i < opt->hex_width; i++) {
					fprintf(f_ds, "%ld",
						((tmp32 & 0x80000000) >> 31));
					tmp32 <<= 1;
				}
				fprintf(f_ds, "\n");
				hex32++;
				size_written += 4;
			}
		}
		break;

	case 64:
		hex32 = (long *)tmpImg;

		for (; hex_len > 0; hex_len -= 8) {
			tmp32low = *hex32++;
			tmp32 = *hex32++;

			for (i = 0; i < 32; i++) {
				fprintf(f_desc[0], "%ld",
					((tmp32 & 0x80000000) >> 31));
				tmp32 <<= 1;
			}
			for (i = 0; i < 32; i++) {
				fprintf(f_desc[0], "%ld",
					((tmp32low & 0x80000000) >> 31));
				tmp32low <<= 1;
			}
			fprintf(f_desc[0], "\n");
			size_written += 8;
		}
		break;
	} /* switch (opt->hex_width) */

	error = 0;

bin_image_end:

	if (tmpImg)
		free(tmpImg);

	for (i = 0; i < files_num; i++) {
		if (f_desc[i] != NULL)
			fclose(f_desc[i]);
	}

	return error;

} /*  end of build_bin_img() */

/*******************************************************************************
*    build_regular_img
*          create regular boot image and write it into output stream
*    INPUT:
*          opt        user options
*          buf_in     mmapped input file
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
int build_regular_img(USER_OPTIONS *opt, char *buf_in)
{
	int size_written = 0;
	int new_file_size = 0;
	MV_U32 chsum32 = 0;

	new_file_size = opt->image_sz;

	if (0 != opt->img_gap) { /* cache line/NAND page/requested offset image
				    alignment */
		MV_U8 *gap_buf;

		gap_buf = calloc(opt->img_gap, sizeof(MV_U8));
		if (gap_buf == NULL) {
			fprintf(stderr,
				"Failed to allocate memory for header to image gap!\n");
			return 1;
		}
		size_written += write(f_out, gap_buf, opt->img_gap);
		new_file_size += opt->img_gap;
		free(gap_buf);
	}

	/* Calculate checksum and copy it to the image tail */
	chsum32 = checksum32((void *)opt->image_buf, opt->image_sz - 4, 0);
	memcpy(opt->image_buf + opt->image_sz - 4, &chsum32, 4);

	/* copy input image to output image */
	size_written += write(f_out, opt->image_buf, opt->image_sz);
	free(opt->image_buf);

	if (new_file_size != size_written) {
		fprintf(stderr, "Size mismatch between calculated/written\n");
		return 1;
	}

	return 0;
} /* end of build_other_img() */

/*******************************************************************************
*    process_image
*          handle input and output file options, read and verify RSA and AES
*keys.
*    INPUT:
*          opt        user options
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
******************************************************************************/
int process_image(USER_OPTIONS *opt)
{
	int i;
	int override[2];
	char *buf_in = NULL;
	int err = 1;

	/* check if the output image exist */
	printf(" ");
	for (i = IMG_FILE_INDX; i <= HDR_FILE_INDX; i++) {
		if (opt->fname_arr[i]) {
			override[i] = 0;

			if (0 == stat(opt->fname_arr[i], &fs_stat)) {
				char c;
				/* ask for overwrite permissions */
				fprintf(stderr,
					"File '%s' already exist! Overwrite it (Y/n)?",
					opt->fname_arr[i]);
				c = getc(stdin);
				if ((c == 'N') || (c == 'n')) {
					printf("exit.. nothing done.\n");
					exit(0);
				} else if ((c == 'Y') || (c == 'y')) {
					/* additional read is needed for Enter
					 * key */
					c = getc(stdin);
				}
				override[i] = 1;
			}
		}
	}

	/* open input image file and check if it's size is OK */
	if (opt->header_mode != HDR_ONLY) {
		f_in = open(opt->fname.in, O_RDONLY | O_BINARY);
		if (f_in == -1) {
			fprintf(stderr, "File '%s' not found\n", opt->fname.in);
			goto end;
		}
		/* get the size of the input image */
		if (0 != fstat(f_in, &fs_stat)) {
			fprintf(stderr, "fstat failed for file: '%s' err=%d\n",
				opt->fname.in, err);
			goto end;
		}
		/*Check the source image size for limited output storage
		 * (bootrom) */
		if (opt->image_type == IMG_BOOTROM) {
			int max_img_size = BOOTROM_SIZE - sizeof(u32);

			if (fs_stat.st_size > max_img_size) {
				fprintf(stderr,
					"ERROR : source image is bigger than %d bytes\n",
					max_img_size);
				goto end;
			}
		}
		/* map the input image */
		buf_in =
		    mmap(0, fs_stat.st_size, PROT_READ, MAP_SHARED, f_in, 0);
		if (!buf_in) {
			fprintf(stderr, "Error mapping %s file\n",
				opt->fname.in);
			goto end;
		}
	}

	/* open the output image file */
	if (override[IMG_FILE_INDX] == 0)
		f_out = open(opt->fname.out,
			     O_RDWR | O_TRUNC | O_CREAT | O_BINARY, 0666);
	else
		f_out = open(opt->fname.out, O_RDWR | O_BINARY);

	if (f_out == -1) {
		fprintf(stderr, "Error opening %s file\n", opt->fname.out);
		goto end;
	}

	/* open the output header file */
	if (opt->header_mode == HDR_IMG_TWO_FILES) {
		if (override[HDR_FILE_INDX] == 0)
			f_header =
			    open(opt->fname.hdr_out,
				 O_RDWR | O_TRUNC | O_CREAT | O_BINARY, 0666);
		else
			f_header = open(opt->fname.hdr_out, O_RDWR | O_BINARY);

		if (f_header == -1) {
			fprintf(stderr, "Error opening %s file\n",
				opt->fname.hdr_out);
			goto end;
		}
	}

	/* Image Header(s)  */
	if (opt->header_mode != IMG_ONLY) {
		if (0 != build_headers(opt, buf_in))
			goto end;
	}

	/* Output Image  */
	if (opt->header_mode != HDR_ONLY) {
		if (opt->image_type == IMG_BOOTROM)
			err = build_bootrom_img(opt, buf_in);
		else if (opt->image_type == IMG_HEX)
			err = build_hex_img(opt, buf_in);
		else if (opt->image_type == IMG_BIN)
			err = build_bin_img(opt, buf_in);
		else
			err = build_regular_img(opt, buf_in);

		if (err != 0) {
			fprintf(stderr, "Error writing %s file\n"
						, opt->fname.out);
			goto end;
		}

		fprintf(stdout, "====>>>> %s was created\n",
			opt->fname_arr[IMG_FILE_INDX]);

	} /* if (opt->header_mode != HDR_ONLY) */

end:
	/* close handles */
	if (f_out != -1)
		close(f_out);

	if (f_header != -1)
		close(f_header);

	if (buf_in)
		munmap((void *)buf_in, fs_stat.st_size);

	if (f_in != -1)
		close(f_in);

	return err;

} /* end of process_image() */

/*******************************************************************************
*    print_usage
*          print command switches and their description
*    INPUT:
*          none
*    OUTPUT:
*          none
*    RETURN:
*          none
*******************************************************************************/
void print_usage(void)
{
	printf(
	    "==============================================================================================\n\n");
	printf("Marvell doimage Tool version %s\n", VERSION_NUMBER);
	printf("Supported SoC devices:\n\t%s\n", PRODUCT_SUPPORT);
	printf(
	    "==============================================================================================\n\n");
	printf("Usage:\n");
	printf(
	    "doimage <mandatory_opt> [other_options] [bootrom_output] <image_in> <image_out> [header_out]\n\n");

	printf("<mandatory_opt> - can be one or more of the following:\n");
	printf(
	    "==============================================================================================\n\n");

	printf(
	    "-T image_type:   sata\\uart\\flash\\bootrom\\nand\\hex\\bin\\pex\\mmc\n");
	printf("-D image_dest:   image destination in dram (in hex)\n");
	printf("-E image_exec:   execution address in dram (in hex)\n");
	printf(
	    "                 if image_type is 'flash' and image_dest is 0xffffffff\n");
	printf("                 then execution address on the flash\n");
	printf(
	    "-S image_source: if image_type is sata then the starting sector of\n");
	printf("                 the source image on the disk\n");
	printf(
	    "                 if image_type is flash\\nand then the starting offset of\n");
	printf(
	    "                 the source image at the flash - optional for flash\\nand\n");
	printf("-W hex_width :   HEX file width, can be 8,16,32,64\n");
	printf(
	    "-M twsi_file:    ascii file name that contains the I2C init regs set by h/w.\n");
	printf("                 this is used in i2c boot only\n");

	printf("\nThe following options are mandatory for NAND image type:\n");
	printf(
	    "-----------------------------------------------------------------------------------------------\n\n");

	printf(
	    "-L nand_blk_size:NAND block size in KBytes (decimal int in range 64-16320)\n");
	printf(
	    "                 This parameter is ignored for flashes with  512B pages\n");
	printf(
	    "                 Such small page flashes always use 16K block sizes\n");
	printf(
	    "-N nand_cell_typ:NAND cell technology type (char: M for MLC, S for SLC)\n");
	printf(
	    "-P nand_pg_size: NAND page size: (decimal 512, 2048, 4096 or 8192)\n");

	printf(
	    "-G exec_file:    ascii file name that contains binary routine (ARM 5TE THUMB)\n");
	printf(
	    "                 to run before the bootloader image execution.\n");
	printf(
	    "                 The routine must contain an appropriate code for saving\n");
	printf(
	    "                 all registers at the routine start and restore them\n");
	printf("                 before return from the routine\n");
	printf(
	    "-R dram_file:    ascii file name that contains the list of dram regs\n");
	printf(
	    "-C hdrs_file:    ascii file name that defines BIN/REG headers order and their sources\n");
	printf("-X pre_padding_size (hex)\n");
	printf("-Y post_padding_size (hex)\n");
	printf("-H header_mode: Header mode, can be:\n");
	printf(
	    "                -H 1 : will create one file (image_out) for header and image\n");
	printf(
	    "                -H 2 : will create two files, (image_out) for image , (header_out) for header\n");
	printf(
	    "                -H 3 : will create one file (image_out) for header only\n");
	printf(
	    "                -H 4 : will create one file (image_out) for image only\n");

	printf(
	    "\n[bootrom_output] - optional and can be one or more of the following:\n");
	printf(
	    "==============================================================================================\n\n");

	printf(
	    "-p               Disable BootROM messages output to UART port (enabled by default)\n");
	printf("-b baudrate      Set BootROM debug port UART baudrate\n");
	printf(
	    "                 value = 2400,4800,9600,19200,38400,57600,115200 (use default baudrate is omitted)\n");
	printf(
	    "-u port_num      Set BootROM debug port UART number value = 0-3 (use default port if omitted)\n");
	printf(
	    "-m mpp_config    Select BootROM debug port MPPs configuration value = 0-7 (BootROM-specific)\n");

	printf("\nCommand examples:\n\n");

	printf("doimage -T hex -W width image_in image_out\n");
	printf("doimage -T bootrom image_in image_out\n");
	printf("doimage -T resume image_in image_out\n");
	printf("doimage -T sata -S sector -D image_dest -E image_exec\n");
	printf("         [other_options] image_in image_out header_out\n\n");
	printf("doimage -T flash -D image_dest -E image_exec [-S address]\n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T pex -D image_dest -E image_exec\n");
	printf("         [other_options] image_in image_out\n\n");
	printf(
	    "doimage -T nand -D image_dest -E image_exec [-S address] -P page_size\n");
	printf("         -L 2 -N S [other_options] image_in image_out\n\n");
	printf("doimage -T uart -D image_dest -E image_exec\n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T pex -D image_dest -E image_exec\n");
	printf("         [other_options] image_in image_out\n\n");
	printf("\n\n");

} /* end of print_usage() */

/*******************************************************************************
*    checksum8
*          calculate 8-bit checksum of memory buffer
*    INPUT:
*          start        buffer start
*          len          buffer length
*          csum         initial checksum value
*    OUTPUT:
*          none
*    RETURN:
*          8-bit buffer checksum
*******************************************************************************/
MV_U8 checksum8(void *start, MV_U32 len, MV_U8 csum)
{
	register MV_U8 sum = csum;

	volatile MV_U8 *startp = (volatile MV_U8 *)start;

	do {
		sum += *(MV_U8 *)startp;
		startp++;

	} while (--len);

	return sum;

} /* end of checksum8 */

/*******************************************************************************
*    checksum32
*          calculate 32-bit checksum of memory buffer
*    INPUT:
*          start        buffer start
*          len          buffer length
*          csum         initial checksum value
*    OUTPUT:
*          none
*    RETURN:
*          32-bit buffer checksum
*******************************************************************************/
MV_U32 checksum32(void *start, MV_U32 len, MV_U32 csum)
{
	register MV_U32 sum = csum;
	volatile MV_U32 *startp = (volatile MV_U32 *)start;

	do {
		sum += *(MV_U32 *)startp;
		startp++;
		len -= 4;

	} while (len);

	return sum;

} /* *end of checksum32() */

/*******************************************************************************
*    make_crc_table
*          init CRC table
*    INPUT:
*          crc_table   CRC table location
*    OUTPUT:
*          crc_table   CRC table location
*    RETURN:
*          none
*******************************************************************************/
void make_crc_table(MV_U32 *crc_table)
{
	MV_U32 c;
	MV_32 n, k;
	MV_U32 poly;

	/* terms of polynomial defining this crc (except x^32): */
	static const MV_U8 p[] = {0,  1,  2,  4,  5,  7,  8,
				  10, 11, 12, 16, 22, 23, 26};

	/* make exclusive-or pattern from polynomial (0xedb88320L) */
	poly = 0L;
	for (n = 0; n < sizeof(p) / sizeof(MV_U8); n++)
		poly |= 1L << (31 - p[n]);

	for (n = 0; n < 256; n++) {
		c = (MV_U32)n;
		for (k = 0; k < 8; k++)
			c = c & 1 ? poly ^ (c >> 1) : c >> 1;
		crc_table[n] = c;
	}

} /* end of make_crc_table */

#define DO1(buf) (crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8))
#define DO2(buf) do {                                                 \
	DO1(buf);                                                     \
	DO1(buf);							\
	} while (0)
#define DO4(buf) do {                                                   \
	DO2(buf);                                                       \
	DO2(buf);							\
	} while (0)
#define DO8(buf) do {                                                   \
	DO4(buf);                                                       \
	DO4(buf);							\
	} while (0)

/*******************************************************************************
*    crc32
*          calculate CRC32 on memory buffer
*    INPUT:
*          crc       initial CRC value
*          buf       memory buffer
*          len       buffer length
*    OUTPUT:
*          none
*    RETURN:
*          CRC32 of the memory buffer
*******************************************************************************/
MV_U32 crc32(MV_U32 crc, volatile MV_U32 *buf, MV_U32 len)
{
	MV_U32 crc_table[256];

	/* Create the CRC table */
	make_crc_table(crc_table);

	crc = crc ^ 0xffffffffL;
	while (len >= 8) {
		DO8(buf);
		len -= 8;
	}

	if (len) {
		do {
			DO1(buf);
		} while (--len);
	}

	return crc ^ 0xffffffffL;

} /* end of crc32() */

/*******************************************************************************
*    select_image
*          select image options by the image name
*    INPUT:
*          img_name       image name
*    OUTPUT:
*          opt            image options
*    RETURN:
*          0 on success, 1 if image name is invalid
*******************************************************************************/
int select_image(char *img_name, USER_OPTIONS *opt)
{
	int i;

	static IMG_MAP img_map[] = {
	    {IMG_SATA, "sata", D_OPTION_MASK | T_OPTION_MASK | E_OPTION_MASK},
	    {IMG_UART, "uart", D_OPTION_MASK | T_OPTION_MASK | E_OPTION_MASK},
	    {IMG_FLASH, "flash", D_OPTION_MASK | T_OPTION_MASK | E_OPTION_MASK},
	    {IMG_MMC, "mmc", D_OPTION_MASK | T_OPTION_MASK | E_OPTION_MASK},
	    {IMG_BOOTROM, "bootrom", T_OPTION_MASK},
	    {IMG_NAND, "nand", D_OPTION_MASK | T_OPTION_MASK | E_OPTION_MASK |
				   L_OPTION_MASK | N_OPTION_MASK |
				   P_OPTION_MASK},
	    {IMG_HEX, "hex", T_OPTION_MASK | W_OPTION_MASK},
	    {IMG_BIN, "bin", T_OPTION_MASK | W_OPTION_MASK},
	    {IMG_PEX, "pex", D_OPTION_MASK | T_OPTION_MASK | E_OPTION_MASK},
	    {IMG_I2C, "i2c",
	     D_OPTION_MASK | T_OPTION_MASK | E_OPTION_MASK | M_OPTION_MASK},
	};

	for (i = 0; i < ARRAY_SIZE(img_map); i++) {
		if (strcmp(img_name, img_map[i].img_name) == 0) {
			opt->image_type = img_map[i].img_type;
			opt->req_flags = img_map[i].img_opt;
			return 0;
		}
	}

	return 1;

} /* *end of select_image() */

/*******************************************************************************
*    main
*******************************************************************************/
int main(int argc, char **argv)
{
	USER_OPTIONS options;
	int optch; /* command-line option char */
	static char optstring[] =
	    "T:D:E:X:Y:S:P:W:H:R:M:G:L:N:C:b:u:m:p";
	int i, k;

	if (argc < 2)
		goto parse_error;

	memset(&options, 0, sizeof(USER_OPTIONS));
	options.header_mode = HDR_IMG_ONE_FILE;

	fprintf(stdout, "\n");

	while ((optch = getopt(argc, argv, optstring)) != -1) {
		char *endptr = NULL;

		switch (optch) {
		case 'T': /* image type */
			if ((select_image(optarg, &options) != 0) ||
			    (options.flags & T_OPTION_MASK))
				goto parse_error;
			options.flags |= T_OPTION_MASK;
			break;

		case 'D': /* image destination  */
			options.image_dest = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & D_OPTION_MASK))
				goto parse_error;
			options.flags |= D_OPTION_MASK;
			DB("Image destination address %#x\n",
			   options.image_dest);
			break;

		case 'E': /* image execution  */
			options.image_exec = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & E_OPTION_MASK))
				goto parse_error;
			options.flags |= E_OPTION_MASK;
			DB("Image execution address %#x\n", options.image_exec);
			break;

		case 'X': /* Pre - Padding */
			options.prepadding_size = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & X_OPTION_MASK))
				goto parse_error;
			options.pre_padding = 1;
			options.flags |= X_OPTION_MASK;
			DB("Pre-pad image by %#x bytes\n",
			   options.prepadding_size);
			break;

		case 'Y': /* Post - Padding */
			options.postpadding_size = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & Y_OPTION_MASK))
				goto parse_error;
			options.post_padding = 1;
			options.flags |= Y_OPTION_MASK;
			DB("Post-pad image by %#x bytes\n",
			   options.postpadding_size);
			break;

		case 'S': /* starting sector */
			options.image_source = strtoul(optarg, &endptr, 16);
			if (*endptr || (options.flags & S_OPTION_MASK))
				goto parse_error;
			options.flags |= S_OPTION_MASK;
			DB("Image start sector (image source) %#x\n",
			   options.image_source);
			break;

		case 'P': /* NAND Page Size */
			options.nandPageSize = strtoul(optarg, &endptr, 10);
			if (*endptr || (options.flags & P_OPTION_MASK))
				goto parse_error;
			options.flags |= P_OPTION_MASK;
			DB("NAND page size %d bytes\n", options.nandPageSize);
			break;

		case 'C': /* headers definition filename */
			options.fname_list = optarg;
			if (options.flags & C_OPTION_MASK)
				goto parse_error;
			options.flags |= C_OPTION_MASK;
			DB("Headers definition file name %s\n",
			   options.fname_list);
			break;

		case 'W': /* HEX file width */
			options.hex_width = strtoul(optarg, &endptr, 10);
			if (*endptr || (options.flags & W_OPTION_MASK))
				goto parse_error;
			options.flags |= W_OPTION_MASK;
			DB("HEX file width %d bytes\n", options.hex_width);
			break;

		case 'H': /* Header file mode */
			options.header_mode = strtoul(optarg, &endptr, 10);
			if (*endptr || (options.flags & H_OPTION_MASK))
				goto parse_error;
			options.flags |= H_OPTION_MASK;
			DB("Header file mode is %d\n", options.header_mode);
			break;

		case 'R': /* dram file */
			options.fname_dram = optarg;
			if (options.flags & R_OPTION_MASK)
				goto parse_error;
			options.flags |= R_OPTION_MASK;
			DB("Registers header file name %s\n",
			   options.fname_dram);
			break;

		case 'M': /* TWSI file */
			options.fname_twsi = optarg;
			if (options.flags & M_OPTION_MASK)
				goto parse_error;
			options.flags |= M_OPTION_MASK;
			DB("TWSI header file name %s\n", options.fname_twsi);
			break;

		case 'G': /* binary file */
			options.fname_bin = optarg;
			if (options.flags & G_OPTION_MASK)
				goto parse_error;
			options.flags |= G_OPTION_MASK;
			DB("Binary header file name %s\n", options.fname_bin);
			break;

		case 'L': /* NAND block size */
			options.nandBlkSize = strtoul(optarg, &endptr, 10) / 64;
			if (*endptr || (options.flags & L_OPTION_MASK))
				goto parse_error;
			options.flags |= L_OPTION_MASK;
			DB("NAND block size %d\n", options.nandBlkSize);
			break;

		case 'N': /* NAND cell technology */
			options.nandCellTech = optarg[0];
			if (options.flags & N_OPTION_MASK)
				goto parse_error;
			options.flags |= N_OPTION_MASK;
			DB("NAND cell technology %c\n", options.nandCellTech);
			break;

		case 'p': /* BootROM debug output */
			if (options.flags & p_OPTION_MASK)
				goto parse_error;
			options.flags |= p_OPTION_MASK;
			DB("BootROM debug output disabled\n");
			break;

		case 'b': /* BootROM debug port baudrate */
			options.baudRate = strtoul(optarg, &endptr, 10);
			if (*endptr || (options.flags & b_OPTION_MASK))
				goto parse_error;
			options.flags |= b_OPTION_MASK;
			DB("BootROM debug port baudrate %d\n",
			   options.baudRate);
			break;

		case 'u': /* BootROM debug port number */
			options.debugPortNum = strtoul(optarg, &endptr, 10);
			if (*endptr || (options.flags & u_OPTION_MASK))
				goto parse_error;
			options.flags |= u_OPTION_MASK;
			DB("BootROM debug port number %d\n",
			   options.debugPortNum);
			break;

		case 'm': /* BootROM debug port MPP settings */
			options.debugPortMpp = strtoul(optarg, &endptr, 10);
			if (*endptr || (options.flags & m_OPTION_MASK))
				goto parse_error;
			options.flags |= m_OPTION_MASK;
			DB("BootROM debug port MPP setup # %d\n",
			   options.debugPortMpp);
			break;

		default:
			goto parse_error;
		}
	} /* parse command-line options */

	/* assign file names */
	for (i = 0; (optind < argc) && (i < ARRAY_SIZE(options.fname_arr));
	     ++optind, i++) {
		options.fname_arr[i] = argv[optind];
		DB("File @ array index %d is %s (option index is %d)\n", i,
		   argv[optind], optind);
		/* verify that all file names are different */
		for (k = 0; k < i; k++) {
			if (0 == strcmp(options.fname_arr[i],
					options.fname_arr[k])) {
				fprintf(stderr,
				    "\nError: Input and output images can't be the same\n");
				exit(1);
			}
		}
	}

	if (!(options.flags & T_OPTION_MASK))
		goto parse_error;

	/* verify HEX/BIN file width selection to be valid */
	if ((options.flags & W_OPTION_MASK) && (options.hex_width != 8) &&
	    (options.hex_width != 16) && (options.hex_width != 32) &&
	    (options.hex_width != 64))
		goto parse_error;
	/* BootROM test images, no header is needed */
	if ((options.image_type == IMG_BOOTROM) ||
	    (options.image_type == IMG_HEX) || (options.image_type == IMG_BIN))
		options.header_mode = IMG_ONLY;

	if (options.header_mode == IMG_ONLY) {
		/* remove unneeded options */
		options.req_flags &=
		    ~(D_OPTION_MASK | E_OPTION_MASK | S_OPTION_MASK |
		      R_OPTION_MASK | P_OPTION_MASK | L_OPTION_MASK |
		      N_OPTION_MASK);
	}

	if (options.req_flags != (options.flags & options.req_flags))
		goto parse_error;

	if ((options.flags & L_OPTION_MASK) &&
	    ((options.nandBlkSize > 255) ||
	     ((options.nandBlkSize == 0) && (options.nandPageSize != 512)))) {
		fprintf(stderr, "Error: wrong NAND block size %d!\n\n\n\n\n",
			64 * options.nandBlkSize);
		goto parse_error;
	}

	if ((options.flags & N_OPTION_MASK) && (options.nandCellTech != 'S') &&
	    (options.nandCellTech != 'M') && (options.nandCellTech != 's') &&
	    (options.nandCellTech != 'm')) {
		fprintf(stderr,
			"Error: Wrong NAND cell technology type!\n\n\n\n\n");
		goto parse_error;
	}

	return process_image(&options);

parse_error:

	print_usage();
	exit(1);

} /* end of main() */
