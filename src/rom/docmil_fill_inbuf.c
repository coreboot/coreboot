#if defined(USE_DOC_MIL) || defined(USE_DOC_2000_TSOP)

#include <cpu/p5/io.h>
#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/fill_inbuf.h>

#ifdef USE_NEW_DOC_CODE
#include <mtd/doc2000.h>
#include <mtd/nand.h>
#include <mtd/nand_ids.h>
#endif
#ifndef DOC_KERNEL_START
#define DOC_KERNEL_START 65536
#endif

#ifndef DOC_MIL_BASE
#define DOC_MIL_BASE  0xffffe000
#endif

static unsigned char *inbuf;	/* input buffer */
static unsigned int insize;	/* valid bytes in inbuf */
static unsigned int inptr;	/* index of next byte to be processed in inbuf */

static unsigned char *nvram;
static int block_count;
static int firstfill = 1;

static void memcpy_from_doc_mil(void *dest, const void *src, size_t n);
static volatile unsigned char *doc_mil = (unsigned char *) DOC_MIL_BASE;
#ifdef CHECK_DOC_MIL
static unsigned char *checkbuf;
#endif /* CHECK_DOC_MIL */

static unsigned char *ram;
#define K64 (64 * 1024)

#ifdef RESET_DOC
void
reset_doc()
{
#ifdef USE_NEW_DOC_CODE
	doc_mil[DoC_DOCControl] = DOC_MODE_CLR_ERR | DOC_MODE_MDWREN | DOC_MODE_RESET;
	doc_mil[DoC_DOCControl] = DOC_MODE_CLR_ERR | DOC_MODE_MDWREN | DOC_MODE_RESET;

	doc_mil[DoC_DOCControl] = DOC_MODE_CLR_ERR | DOC_MODE_MDWREN | DOC_MODE_NORMAL;
	doc_mil[DoC_DOCControl] = DOC_MODE_CLR_ERR | DOC_MODE_MDWREN | DOC_MODE_NORMAL;

#else
	*(volatile unsigned char *) (doc_mil + 0x1002) = 0x84;
	*(volatile unsigned char *) (doc_mil + 0x1002) = 0x84;
	*(volatile unsigned char *) (doc_mil + 0x1002) = 0x85;
	*(volatile unsigned char *) (doc_mil + 0x1002) = 0x85;
#endif
}
#endif

static int 
fill_inbuf(void)
{
#ifdef CHECK_DOC_MIL
redo:
#endif
	if (firstfill) {
		// it is possible that we can get in here and the 
		// doc has never been reset. So go ahead and reset it again.
#ifdef RESET_DOC
		reset_doc();
#endif		
		if ((ram = malloc(K64)) == NULL) {
			printk_emerg("%6d:%s() - ram malloc failed\n",
			      __LINE__, __FUNCTION__);
			return (0);
		}

#ifdef CHECK_DOC_MIL
		if ((checkbuf = malloc(K64)) == NULL) {
			printk_emerg("%6d:%s() - checkbuf malloc failed\n",
			      __LINE__, __FUNCTION__);
			printk_emerg("Checking disabled\n");
		}
#endif

		printk_debug("%6d:%s() - ram buffer:0x%p\n",
		       __LINE__, __FUNCTION__, ram);

		block_count = 0;
		firstfill = 0;
		nvram = (unsigned char *) DOC_KERNEL_START;
	}

#ifdef CHECK_DOC_MIL
	printk_info("DOC MIL address 0x%x\n", nvram);
#endif
	memcpy_from_doc_mil(ram, nvram, K64);

#ifdef CHECK_DOC_MIL
	if (checkbuf) {
		memcpy_from_doc_mil(checkbuf, nvram, K64);
		if (memcmp(checkbuf, ram, K64)) {
			int i;
			for(i = 0; i < K64; i++)
				if (checkbuf[i] != ram[i])
				printk_info("at %d First read 0x%x check 0x%x\n", 
					i, ram[i], checkbuf[i]);
			printk_emerg("CHECKBUF FAILS for doc mil!\n");
			printk_emerg( "address 0x%x\n", nvram);
			goto redo;
		}
	}
#if 0
	{ 
		int i, j;
		for(i = 0; i < K64; i += 16) {
			printk_info("0x%x: ", ram + i);
			for(j = 0; j < 16; j++)
				printk_info("%x ",ram[i+j]);
			printk_info( "\n");
		}
	}
#endif /* 0 */
#endif

	printk_debug("%6d:%s() - nvram:0x%p  block_count:%d\n",
	       __LINE__, __FUNCTION__, nvram, block_count);

	nvram += K64;
	inbuf = ram;
	insize = K64;
	inptr = 1;

	post_code(0xd0 + block_count);
	block_count++;
	return inbuf[0];
}


#ifdef USE_NEW_DOC_CODE
/**************************************************************************
 *
 * New DoC code for LinxuBIOS. This code is from:
 * doctest.c - a quick and dirty test for DoC2001 in MB BIOS socket
 *
 * compile: gcc -O2 -g -o doctest doctest.c
 *
 * Copyright Steven James <pyro@linuxlabs.com>, Linux Labs (www.linuxlabs.com)
 *
 * License GPL v2 or later (for what it's worth)
 *
 * DoC access code derived from Linux kernel drivers
 * (c) 1999 Machine Vision Holdings, Inc.
 * Author: David Woodhouse <dwmw2@mvhi.com>
 * 
 *************************************************************************/



/* Access routines for DoC Mil */
#define _WriteDoC(data, adr, reg) adr[reg] = data
#define WriteDOC(data, adr, reg) _WriteDoC(data, adr, DoC_##reg)

#define _ReadDoC(adr, reg) adr[reg]
#define ReadDOC(adr, reg) _ReadDoC(adr, DoC_##reg)


/* Perform the required delsy cycles by reading from the NOP register */
static void DoC_Delay(volatile char *docptr, unsigned short cycles)
{
	volatile char dummy;
	int i;

	for (i = 0; i < cycles; i++)
		dummy = ReadDOC(docptr, NOP);
}

/* DOC_WaitReady: Wait for RDY line to be asserted by the flash chip */
static int _DoC_WaitReady(volatile char *docptr)
{
	unsigned short c = 0xffff;

	/* Out-of-line routine to wait for chip response */
	while (!(ReadDOC(docptr, CDSNControl) & CDSN_CTRL_FR_B) && --c)
		;

	return (c == 0);
}

static __inline__ int DoC_WaitReady(volatile char *docptr) 
{
	/* This is inline, to optimise the common case, where it's ready instantly */
	int ret = 0;

	/* 4 read form NOP register should be issued in prior to the read from CDSNControl
	   see Software Requirement 11.4 item 2. */
	DoC_Delay(docptr, 4);

	if (!(ReadDOC(docptr, CDSNControl) & CDSN_CTRL_FR_B))
		/* Call the out-of-line routine to wait */
		ret = _DoC_WaitReady(docptr);

	/* issue 2 read from NOP register after reading from CDSNControl register
	   see Software Requirement 11.4 item 2. */
	DoC_Delay(docptr, 2);

	return ret;
}

/* DoC_Command: Send a flash command to the flash chip through the CDSN Slow IO register to
   bypass the internal pipeline. Each of 4 delay cycles (read from the NOP register) is
   required after writing to CDSN Control register, see Software Requirement 11.4 item 3. */
/* SURELY this is wrong what about bit 8? But I could be wrong... SMJ */

static __inline__ void DoC_Command(volatile char *docptr, unsigned char command,
				   unsigned char xtraflags)
{
	/* Assert the CLE (Command Latch Enable) line to the flash chip */
	WriteDOC(xtraflags | CDSN_CTRL_CLE | CDSN_CTRL_CE, docptr, CDSNControl);
	DoC_Delay(docptr, 4);

	/* Send the command */
	WriteDOC(command, docptr, CDSNSlowIO);
	WriteDOC(command, docptr, Mil_CDSN_IO);

	/* Lower the CLE line */
	WriteDOC(xtraflags | CDSN_CTRL_CE, docptr, CDSNControl);
	DoC_Delay(docptr, 4);
}

/* DoC_Address: Set the current address for the flash chip through the CDSN Slow IO register to
   bypass the internal pipeline. Each of 4 delay cycles (read from the NOP register) is
   required after writing to CDSN Control register, see Software Requirement 11.4 item 3. */
static __inline__ void DoC_Address (volatile char *docptr, int numbytes, unsigned long ofs,
			       unsigned char xtraflags1, unsigned char xtraflags2)
{
	/* Assert the ALE (Address Latch Enable line to the flash chip */
	WriteDOC(xtraflags1 | CDSN_CTRL_ALE | CDSN_CTRL_CE, docptr, CDSNControl);
	DoC_Delay(docptr, 4);

	/* Send the address */
	switch (numbytes)
	    {
	    case 1:
		/* Send single byte, bits 0-7. */
		WriteDOC(ofs & 0xff, docptr, CDSNSlowIO);
		WriteDOC(ofs & 0xff, docptr, Mil_CDSN_IO);
		break;
	    case 2:
		/* Send bits 9-16 followed by 17-23 */
		WriteDOC((ofs >> 9)  & 0xff, docptr, CDSNSlowIO);
		WriteDOC((ofs >> 9)  & 0xff, docptr, Mil_CDSN_IO);
		WriteDOC((ofs >> 17) & 0xff, docptr, CDSNSlowIO);
		WriteDOC((ofs >> 17) & 0xff, docptr, Mil_CDSN_IO);
		break;
	    case 3:
		/* Send 0-7, 9-16, then 17-23 */
		WriteDOC(ofs & 0xff, docptr, CDSNSlowIO);
		WriteDOC(ofs & 0xff, docptr, Mil_CDSN_IO);
		WriteDOC((ofs >> 9)  & 0xff, docptr, CDSNSlowIO);
		WriteDOC((ofs >> 9)  & 0xff, docptr, Mil_CDSN_IO);
		WriteDOC((ofs >> 17) & 0xff, docptr, CDSNSlowIO);
		WriteDOC((ofs >> 17) & 0xff, docptr, Mil_CDSN_IO);
		break;
	    default:
		return;
	    }

	/* Lower the ALE line */
	WriteDOC(xtraflags1 | xtraflags2 | CDSN_CTRL_CE, docptr, CDSNControl);
	DoC_Delay(docptr, 4);
}

/* DoC_SelectChip: Select a given flash chip within the current floor */
static int DoC_SelectChip(volatile char *docptr, int chip)
{
	/* Select the individual flash chip requested */
	WriteDOC(chip, docptr, CDSNDeviceSelect);
	DoC_Delay(docptr, 4);

	/* Wait for it to be ready */
	return DoC_WaitReady(docptr);
}

/* DoC_SelectFloor: Select a given floor (bank of flash chips) */
static int DoC_SelectFloor(volatile char *docptr, int floor)
{
	/* Select the floor (bank) of chips required */
	WriteDOC(floor, docptr, FloorSelect);

	/* Wait for the chip to be ready */
	return DoC_WaitReady(docptr);
}

/* DoC_IdentChip: Identify a given NAND chip given {floor,chip} */
static int DoC_IdentChip(volatile char *docptr, int floor, int chip, int *mfr, int *id)
{
	int i;
	volatile char dummy;

	/* Page in the required floor/chip
		FIXME: is this supported by Millennium ?? */

	DoC_SelectFloor(docptr, floor);
	DoC_SelectChip(docptr, chip);

	/* Reset the chip, see Software Requirement 11.4 item 1. */
	DoC_Command(docptr, NAND_CMD_RESET, CDSN_CTRL_WP);
	DoC_WaitReady(docptr);


	/* Read the NAND chip ID: 1. Send ReadID command */ 
	DoC_Command(docptr, NAND_CMD_READID, CDSN_CTRL_WP);

	/* Read the NAND chip ID: 2. Send address byte zero */ 
	DoC_Address(docptr, 1, 0x00, CDSN_CTRL_WP, 0x00);

	/* Read the manufacturer and device id codes of the flash device through
	   CDSN Slow IO register see Software Requirement 11.4 item 5.*/
	dummy = ReadDOC(docptr, CDSNSlowIO);
	DoC_Delay(docptr, 2);
	*mfr = ReadDOC(docptr, Mil_CDSN_IO);

	dummy = ReadDOC(docptr, CDSNSlowIO);
	DoC_Delay(docptr, 2);
	*id  = ReadDOC(docptr, Mil_CDSN_IO);

	/* No response - return failure */
	if (*mfr == 0xff || *mfr == 0)
		return 0;

	/* FIXME: to deal with mulit-flash on multi-Millennium case more carefully */
	for (i = 0; nand_flash_ids[i].name != NULL; i++) {
		if (*mfr == nand_flash_ids[i].manufacture_id &&
		    *id == nand_flash_ids[i].model_id) {
			printk_info("Flash chip found: Manufacture ID: %2.2X, Chip ID: %2.2X (%s)\n",
			       *mfr, *id, nand_flash_ids[i].name);
			break;
		}
	}

	if (nand_flash_ids[i].name == NULL)
		return 0;
	else
		return 1;
}

static int DoCMil_is_alias(volatile char *docptr1, volatile char *docptr2)
{
	int tmp1, tmp2, retval;

	if(docptr1 == docptr2)
		return(1);

	/* Use the alias resolution register which was set aside for this
	 * purpose. If it's value is the same on both chips, they might
	 * be the same chip, and we write to one and check for a change in
	 * the other. It's unclear if this register is usuable in the
	 * DoC 2000 (it's in the Millenium docs), but it seems to work. */
	tmp1 = ReadDOC(docptr1, AliasResolution);
	tmp2 = ReadDOC(docptr2, AliasResolution);
	if (tmp1 != tmp2)
		return 0;
	
	WriteDOC((tmp1+1) % 0xff, docptr1, AliasResolution);
	tmp2 = ReadDOC(docptr2, AliasResolution);
	if (tmp2 == (tmp1+1) % 0xff)
		retval = 1;
	else
		retval = 0;

	/* Restore register contents.  May not be necessary, but do it just to
	 * be safe. */
	WriteDOC(tmp1, docptr1, AliasResolution);

	return retval;
}

#if 0
static int WriteBlockECC(volatile char *docptr, unsigned int block, const char *buf)
{
	unsigned char eccbuf[6];
	volatile char dummy;
	int i;


	DoC_SelectFloor(docptr, 0);
	DoC_SelectChip(docptr, 0);

	/* Reset the chip, see Software Requirement 11.4 item 1. */
	DoC_Command(docptr, NAND_CMD_RESET, 0x00);
	DoC_WaitReady(docptr);
	/* Set device to main plane of flash */
	DoC_Command(docptr, NAND_CMD_READ0, 0x00);

	/* issue the Serial Data In command to initial the Page Program process
*/
	DoC_Command(docptr, NAND_CMD_SEQIN, 0x00);
	DoC_Address(docptr, 3, block <<9, 0x00, 0x00);
	DoC_WaitReady(docptr);

	/* init the ECC engine, see Reed-Solomon EDC/ECC 11.1 .*/
	WriteDOC (DOC_ECC_RESET, docptr, ECCConf);
	WriteDOC (DOC_ECC_EN | DOC_ECC_RW, docptr, ECCConf);

	/* Write the data via the internal pipeline through CDSN IO register,
		see Pipelined Write Operations 11.2 */

	for (i = 0; i < 512; i++) {
		/* N.B. you have to increase the source address in this way or the
                   ECC logic will not work properly */
		WriteDOC(buf[i], docptr, Mil_CDSN_IO + i);
	}

	WriteDOC(0x00, docptr, WritePipeTerm);

	/* Write ECC data to flash, the ECC info is generated by the DiskOnChip ECC logic
                   see Reed-Solomon EDC/ECC 11.1 */

	WriteDOC(0, docptr, NOP);
	WriteDOC(0, docptr, NOP);
	WriteDOC(0, docptr, NOP);

	/* Read the ECC data through the DiskOnChip ECC logic */
	for (i = 0; i < 6; i++) {
		eccbuf[i] = ReadDOC(docptr, ECCSyndrome0 + i);
	}

	/* ignore the ECC engine */
	WriteDOC(DOC_ECC_DIS, docptr , ECCConf);
	
	/* Write the ECC data to flash */
	for (i = 0; i < 6; i++) {
		WriteDOC(eccbuf[i], docptr, Mil_CDSN_IO + i);
	}

	/* write the block status BLOCK_USED (0x5555) at the end of ECC data FIXME: this is only a hack for programming the IPL area for LinuxBIOS and should be replace with proper codes in user space utilities */

	WriteDOC(0x55, docptr, Mil_CDSN_IO);
	WriteDOC(0x55, docptr, Mil_CDSN_IO + 1);
	
	WriteDOC(0x00, docptr, WritePipeTerm);

	/* Commit the Page Program command and wait for ready
		see Software Requirement 11.4 item 1.*/

	DoC_Command(docptr, NAND_CMD_PAGEPROG, 0x00);
	DoC_WaitReady(docptr);

	/* Read the status of the flash device through CDSN Slow IO register
	see Software Requirement 11.4 item 5.*/
	DoC_Command(docptr, NAND_CMD_STATUS, CDSN_CTRL_WP);
	dummy = ReadDOC(docptr, CDSNSlowIO);
	DoC_Delay(docptr, 2);

	if (ReadDOC(docptr, Mil_CDSN_IO) & 1) {
		printk_info("Error programming flash\n");
		return -EIO;
	}

	return(0);
}

#endif
static int ReadBlockECC( volatile unsigned char *docptr, unsigned int block, char *buf)
{
	int i, ret;
	volatile char dummy;
	unsigned char syndrome[6];
	unsigned char eccbuf[6];

	/* issue the Read0 or Read1 command depend on which half of the page
		we are accessing. Polling the Flash Ready bit after issue 3 bytes
		address in Sequence Read Mode, see Software Requirement 11.4 item 1.*/

	/* This differs from SiS ipl.S which always issues 0! */

	DoC_Command(docptr, 0, CDSN_CTRL_WP);
//	DoC_Command(docptr, block & 1, CDSN_CTRL_WP);
	DoC_Address(docptr, 3, block << 9, CDSN_CTRL_WP, 0x00);
	DoC_WaitReady(docptr);

	WriteDOC (DOC_ECC_RESET, docptr, ECCConf);
	WriteDOC (DOC_ECC_EN, docptr, ECCConf);

	dummy = ReadDOC(docptr, ReadPipeInit);
	for (i = 0; i < 511; i++) {
		buf[i] = ReadDOC(docptr, Mil_CDSN_IO + i);
	}

	buf[511] = ReadDOC(docptr, LastDataRead);

	/* Read the ECC data from Spare Data Area,
		see Reed-Solomon EDC/ECC 11.1 */

	dummy = ReadDOC(docptr, ReadPipeInit);
	for (i = 0; i < 5; i++) {
		eccbuf[i] = ReadDOC(docptr, Mil_CDSN_IO + i);
	}

	eccbuf[i] = ReadDOC(docptr, LastDataRead);

	/* Flush the pipeline */
	dummy = ReadDOC(docptr, ECCConf);
	dummy = ReadDOC(docptr, ECCConf);

	/* Check the ECC Status */
	if (ReadDOC(docptr, ECCConf) & 0x80) {
		int nb_errors;
	
		/* There was an ECC error */
		printk_info("ECC error in block %u\n",block);

		/* Read the ECC syndrom through the DiskOnChip ECC logic.
			syndrome will be all ZERO when there is no error */

		for (i = 0; i < 6; i++) {
			syndrome[i] = ReadDOC(docptr, ECCSyndrome0 + i);
		}
#ifdef CHECK_ECC
		nb_errors = doc_decode_ecc(buf, syndrome);

		if(nb_errors <0) {
			printk_info("ECC errors uncorrectable!\n");
			return(-EIO);
		}

		printk_info("Corrected %u errors!\n",nb_errors);
#else
		printk_info("ECC Syndrom bytes:\n");
		for(i=0; i<6 ; i++)
			printk_info("%02x,",syndrome[i]);
		printk_info("\n");
#endif
	}

	/* disable the ECC engine */
	WriteDOC(DOC_ECC_DIS, docptr , ECCConf);

	return(0);
}


#endif
static void
memcpy_from_doc_mil(void *dest, const void *src, size_t n)
{
	int i;
	unsigned long address = (unsigned long) src;

	for (i = n; i >= 0; i -= 0x200) {
#ifdef USE_NEW_DOC_CODE
		ReadBlockECC(doc_mil,(address >> 9), dest);

#else
		unsigned short c = 0x1000;
		volatile unsigned char dummy;
		/* issue Read00 flash command */
		*(volatile unsigned char *) (doc_mil + 0x1004) = 0x03;
		*(volatile unsigned char *) (doc_mil + 0x800)  = 0x00;
		*(volatile unsigned char *) (doc_mil + 0x101e) = 0x00;
		*(volatile unsigned char *) (doc_mil + 0x1004) = 0x01;

		/* issue Address to flash */
		*(volatile unsigned char *) (doc_mil + 0x1004) = 0x05;
		*(volatile unsigned char *) (doc_mil + 0x800)  = address & 0xff;
		*(volatile unsigned char *) (doc_mil + 0x800)  = (address >> 9) & 0xff;
		*(volatile unsigned char *) (doc_mil + 0x800)  = (address >> 17) & 0xff;
		*(volatile unsigned char *) (doc_mil + 0x101e) = 0x00;
		*(volatile unsigned char *) (doc_mil + 0x1004) = 0x01;

		/* We are using the "side effect" of the assignment to force GCC reload 
		 * *(doc_mil + 0x1004) on each iteration */
		while (!((dummy = *(volatile unsigned char *) (doc_mil + 0x1004)) & 0x80) && --c)
			/* wait for chip response */;

		/* copy 512 bytes of data from CDSN_IO registers */
		dummy = *(volatile unsigned char *) (doc_mil + 0x101d);
#ifdef CHECK_DOC_MIL
		{ unsigned char val; unsigned char *cp = dest;
			for(i = 0; i < 0x200; i++)  {
			val = *(volatile unsigned char *) (doc_mil + 0x800 + i);
			printk_info("0x%x: 0x%x\n", 
					i + address, val);
			cp[i] = val;
			}
		}
#else
		memcpy(dest, doc_mil + 0x800, 0x200);
#endif /* CHECK_DOC_MIL */

#endif /* USE_NEW_DOC_CODE */
		dest += 0x200;
		address += 0x200;
	}

}

/* FIXME this is a very lazy ugly port of the new interface to the doc millenium 
 * find a good way to implement this...
 */

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf())

static int init_bytes(void)
{
	return;
}
static void fini_bytes(void)
{
	return;
}
static byte_offset_t read_bytes(void *vdest, byte_offset_t count)
{
	byte_offset_t bytes = 0;
	unsigned char *dest = vdest;
	while(bytes < count) {
		*(dest++) = get_byte();
	}
	return count;
}

static byte_offset_t skip_bytes(byte_offset_t count)
{
	byte_offset_t bytes = 0;
	while(bytes < count) {
		unsigned char byte;
		byte = get_byte();
	}
	return count;
}

static struct stream doc_mil_stream __stream = {
	.init = init_bytes,
	.read = read_bytes,	       
	.skip = skip_bytes,
	.fini = fini_bytes,
}

#endif /* USE_DOC_MIL */
