/* this setupcpu function comes from: */
/*==============================================================================*/
/* FILE   :  start16.asm*/
/**/
/* DESC   : A  16 bit mode assembly language startup program, intended for*/
/*          use with on Aspen SC520 platforms.*/
/**/
/* 11/16/2000 Added support for the NetSC520*/
/* 12/28/2000 Modified to boot linux image*/
/**/
/* =============================================================================*/
/*                                                                             */
/*  Copyright 2000 Advanced Micro Devices, Inc.                                */
/*                                                                             */
/* This software is the property of Advanced Micro Devices, Inc  (AMD)  which */
/* specifically grants the user the right to modify, use and distribute this */
/* software provided this COPYRIGHT NOTICE is not removed or altered.  All */
/* other rights are reserved by AMD.                                                       */
/*                                                                            */
/* THE MATERIALS ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY */
/* OF ANY KIND INCLUDING WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT OF */
/* THIRD-PARTY INTELLECTUAL PROPERTY, OR FITNESS FOR ANY PARTICULAR PURPOSE.*/
/* IN NO EVENT SHALL AMD OR ITS SUPPLIERS BE LIABLE FOR ANY DAMAGES WHATSOEVER*/
/* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS*/
/* INTERRUPTION, LOSS OF INFORMATION) ARISING OUT OF THE USE OF OR INABILITY*/
/* TO USE THE MATERIALS, EVEN IF AMD HAS BEEN ADVISED OF THE POSSIBILITY OF*/
/* SUCH DAMAGES.  BECAUSE SOME JURSIDICTIONS PROHIBIT THE EXCLUSION OR*/
/* LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE*/
/* LIMITATION MAY NOT APPLY TO YOU.*/
/**/
/* AMD does not assume any responsibility for any errors that may appear in*/
/* the Materials nor any responsibility to support or update the Materials.*/
/* AMD retains the right to make changes to its test specifications at any*/
/* time, without notice.*/
/**/
/* So that all may benefit from your experience, please report  any  problems */
/* or suggestions about this software back to AMD.  Please include your name, */
/* company,  telephone number,  AMD product requiring support and question or */
/* problem encountered.                                                       */
/*                                                                            */
/* Advanced Micro Devices, Inc.         Worldwide support and contact           */
/* Embedded Processor Division            information available at:               */
/* Systems Engineering                       epd.support@amd.com*/
/* 5204 E. Ben White Blvd.                          -or-*/
/* Austin, TX 78741                http://www.amd.com/html/support/techsup.html*/
/* ============================================================================*/


#define OUTC(addr, val) *(unsigned char *)(addr) = (val)

/* sadly, romcc can't quite handle what we want, so we do this ugly thing */
#define drcctl   (( volatile unsigned char *)0xfffef010)
#define drcmctl   (( volatile unsigned char *)0xfffef012)
#define drccfg   (( volatile unsigned char *)0xfffef014)

#define drcbendadr   (( volatile unsigned long *)0xfffef018)
#define eccctl   (( volatile unsigned char *)0xfffef020)
#define dbctl   (( volatile unsigned char *)0xfffef040)

void setupsc520(void)
{
	volatile unsigned char *cp;
	volatile unsigned short *sp;
	volatile unsigned long *edi;

	/* do this to see if MMCR will start acting right. we suspect
	 * you have to do SOMETHING to get things going. I'm really
	 * starting to hate this processor.
	 */

	/* no, that did not help. I wonder what will?
	 * outl(0x800df0cb, 0xfffc);
	 */

	/* well, this is special! You have to do SHORT writes to the
	 * locations, even though they are CHAR in size and CHAR aligned
	 * and technically, a SHORT write will result in -- yoo ha! --
	 * over writing the next location! Thanks to the u-boot guys
	 * for a reference code I can use. with these short pointers,
	 * it now reliably comes up after power cycle with printk. Ah yi
	 * yi.
   	 */

	/* turn off the write buffer*/
	/* per the note above, make this a short? Let's try it. */
	sp = (unsigned short *)0xfffef040;
	*sp = 0;

	/* as per the book: */
	/* PAR register setup */
	/* set up the PAR registers as they are on the MSM586SEG */
	/*    moved to romstage.c by Stepan, Ron says: */
	/* NOTE: move this to mainboard.c ASAP */
	setup_pars();

	/* CPCSF register */
	sp =  (unsigned short *)0xfffefc24;
	*sp = 0xfe;

	/* ADDDECTL */
	sp =  (unsigned short *)0xfffefc80;
	*sp = 0x10;

	/* byte writes in AMD assembly */
	/* we do short anyway, since u-boot does ... */
	/*set the GP CS offset*/
	sp =  (unsigned short *)0xfffefc08;
	*sp = 0x00001;
	/*set the GP CS width*/
	sp =  (unsigned short *)0xfffefc09;
	*sp = 0x00003;

	/* short writes in AMD assembly */
	/*set the GP CS width*/
	sp =  (unsigned short *)0xfffefc0a;
	*sp = 0x00001;
	/*set the RD pulse width*/
	sp =  (unsigned short *)0xfffefc0b;
	*sp = 0x00003;
	/*set the GP RD offset */
	sp =  (unsigned short *)0xfffefc0c;
	*sp = 0x00001;
	/*set the GP WR pulse width*/
	sp =  (unsigned short *)0xfffefc0d;
	*sp = 0x00003;
	/*set the GP WR offset*/
	sp =  (unsigned short *)0xfffefc0e;
	*sp = 0x00001;


	/* set up the GP IO pins*/
	/*set the GPIO directionreg*/
	sp =  (unsigned short *)0xfffefc2c;
	*sp = 0x00000;
	/*set the GPIO directionreg*/
	sp =  (unsigned short *)0xfffefc2a;
	*sp = 0x00000;
	/*set the GPIO pin function 31-16 reg*/
	sp =  (unsigned short *)0xfffefc22;
	*sp = 0x0FFFF;
	/*set the GPIO pin function 15-0 reg*/
	sp =  (unsigned short *)0xfffefc20;
	*sp = 0x0FFFF;


	/* the 0x80 led should now be working*/
	post_code(0xaa);
#if 0
	/* wtf are 680 leds ... *//* <-- WTF is this comment? */
	par = (unsigned long *) 0xfffef0c4;
	*par = 0x28000680;
	/* well? */
	post_code(0x55);
#endif

	/* set the uart baud rate clocks to the normal 1.8432 MHz.*/
	/* enable interrupts here? Why not? */
	cp = (unsigned char *)0xfffefcc0;
	*cp = 4 | 3;			/* uart 1 clock source */
	cp = (unsigned char *)0xfffefcc4;
	*cp = 4;			/* uart 2 clock source */

#if 0
/*; set the interrupt mapping registers.*/
	cp = (unsigned char *)0x0fffefd20;
	*cp = 0x01;

	cp = (unsigned char *)0x0fffefd28;
	*cp = 0x0c;

	cp = (unsigned char *)0x0fffefd29;
	*cp = 0x0b;

	cp = (unsigned char *)0x0fffefd30;
	*cp = 0x07;

	cp = (unsigned char *)0x0fffefd43;
	*cp = 0x03;

	cp = (unsigned char *)0x0fffefd51;
	*cp = 0x02;
#endif

/* Stepan says: This needs to go to the msm586seg code */
/* "enumerate" the PCI. Mainly set the interrupt bits on the PCnetFast. */
	outl(0x08000683c, 0xcf8);
	outl(0xc, 0xcfc); /* set the interrupt line */


	/* Set the SC520 PCI host bridge to target mode to
	 * allow external bus mastering events
	 */
	/* index the status command register on device 0*/
	outl(0x080000004, 0x0cf8);
	outl(0x2, 0xcfc);		/*set the memory access enable bit*/
	OUTC(0x0fffef072, 1);		/* enable req bits in SYSARBMENB */
}


/*
 *
 *
 */



#define CACHELINESZ   0x00000010  /*  size of our cache line (read buffer)*/

#define COL11_ADR  *(unsigned int *)0x0e001e00 /* 11 col addrs*/
#define COL10_ADR  *(unsigned int *)0x0e000e00 /* 10 col addrs*/
#define COL09_ADR  *(unsigned int *)0x0e000600 /*  9 col addrs*/
#define COL08_ADR  *(unsigned int *)0x0e000200 /*  8 col addrs*/

#define ROW14_ADR  *(unsigned int *)0x0f000000 /* 14 row addrs*/
#define ROW13_ADR  *(unsigned int *)0x07000000 /* 13 row addrs*/
#define ROW12_ADR  *(unsigned int *)0x03000000 /* 12 row addrs*/
#define ROW11_ADR  *(unsigned int *)0x01000000 /* 11 row addrs/also bank switch*/
#define ROW10_ADR  *(unsigned int *)0x00000000 /* 10 row addrs/also bank switch*/

#define COL11_DATA 0x0b0b0b0b	/*  11 col addrs*/
#define COL10_DATA 0x0a0a0a0a	/*  10 col data*/
#define COL09_DATA 0x09090909	/*   9 col data*/
#define COL08_DATA 0x08080808	/*   8 col data*/

#define ROW14_DATA 0x3f3f3f3f	/*  14 row data (MASK)*/
#define ROW13_DATA 0x1f1f1f1f	/*  13 row data (MASK)*/
#define ROW12_DATA 0x0f0f0f0f	/*  12 row data (MASK)*/
#define ROW11_DATA 0x07070707	/*  11 row data/also bank switch (MASK)*/
#define ROW10_DATA 0xaaaaaaaa	/*  10 row data/also bank switch (MASK)*/

void
dummy_write(void){
  volatile unsigned short *ptr = (volatile unsigned short *)CACHELINESZ;
  *ptr = 0;
}

#include "drivers/pc80/udelay_io.c"

static void dumpram(void){
  print_err("ctl "); print_err_hex8(*drcctl); print_err("\n");
  print_err("mctl "); print_err_hex8(*drcmctl); print_err("\n");
  print_err("cfg "); print_err_hex8(*drccfg); print_err("\n");

  print_err("bendadr0 "); print_err_hex8(*drcbendadr); print_err("\n");
  print_err("bendadr1 "); print_err_hex8(*drcbendadr); print_err("\n");
  print_err("bendadr2 "); print_err_hex8(*drcbendadr); print_err("\n");
  print_err("bendadr3"); print_err_hex8(*drcbendadr); print_err("\n");
}

/* there is a lot of silliness in the amd code, and it is
 * causing romcc real headaches, so we're going to be be a little
 * less silly.
 * so, the order of ops is:
 * for i in 3 to 0
 * see if bank is there.
 * if we can write a word, and read it back, to hell with paranoia
 * the bank is there. So write the magic byte, read it back, and
 * use that to get size, etc. Try to keep things very simple,
 * so people can actually follow the damned code.
 */

/* cache is assumed to be disabled */
int sizemem(void)
{

	int rows,banks, cols, i, bank;
	unsigned char al;
	volatile unsigned long *lp = (volatile unsigned long *) CACHELINESZ;
	unsigned long l;
	/* initialize dram controller registers */
	/* disable write buffer/read-ahead buffer */
	*dbctl = 0;
	/* no ecc interrupts of any kind. */
	*eccctl = 0;
	/* Set SDRAM timing for slowest speed. */
	*drcmctl = 0x1e;

	/* setup dram register for all banks
	 * with max cols and max banks
	 * this is the oldest trick in the book. You are going to set up for max rows
	 * and cols, then do a write, then see if the data is wrapped to low memory.
	 * you can actually tell by which data gets to which low memory,
	 * exactly how many rows and cols you have.
	 */
	*drccfg=0xbbbb;

	/* setup loop to do 4 external banks starting with bank 3 */
	*drcbendadr=0x0ff000000;
	/* for now, set it up for one loop of bank 0. Just to get it to go at all. */
	*drcbendadr=0x0ff;

	/* issue a NOP to all DRAMs */
	/* Setup DRAM control register with Disable refresh,
 	 * disable write buffer Test Mode and NOP command select
 	 */
	*drcctl=0x01;

	/* dummy write for NOP to take effect */
	dummy_write();
	print_err("NOP\n");
	/* 100? 200? */
	udelay(100);
	print_err("after udelay\n");

	/* issue all banks precharge */
	*drcctl=0x02;
	print_err("set *drcctl to 2 \n");
	dummy_write();
	print_err("PRE\n");

	/* issue 2 auto refreshes to all banks */
	*drcctl=0x04;
	dummy_write();
	print_err("AUTO1\n");
	dummy_write();
	print_err("AUTO2\n");

	/* issue LOAD MODE REGISTER command */
	*drcctl=0x03;
	dummy_write();
	print_err("LOAD MODE REG\n");

	*drcctl=0x04;
	for (i=0; i<8; i++) /* refresh 8 times */{
		dummy_write();
		print_err("dummy write\n");
	}
	print_err("8 dummy writes\n");

	/* set control register to NORMAL mode */
	*drcctl=0x00;
	print_err("normal\n");

	print_err("HI done normal\n");

	print_err("sizemem\n");
	for(bank = 3; bank >= 0; bank--) {
	  print_err("Try to assign to l\n");
	  *lp = 0xdeadbeef;
	  print_err("assigned l ... \n");
	  if (*lp != 0xdeadbeef) {
	    print_err(" no memory at bank ");
	    // print_err_hex8(bank);
	    //   print_err(" value "); print_err_hex32(*lp);
	    print_err("\n");
	    //	    continue;
	  }
	  *drcctl = 2;
	  dummy_write();
	  *drccfg = *drccfg >> 4;
	  l = *drcbendadr;
	  l >>= 8;
	  *drcbendadr = l;
	  print_err("loop around\n");
	  *drcctl = 0;
	  dummy_write();
	}
#if 0
	/* enable last bank and setup ending address
	 * register for max ram in last bank
	 */
	*drcbendadr=0x0ff000000;


//	dumpram();

	/* issue a NOP to all DRAMs */
	/* Setup DRAM control register with Disable refresh,
 	 * disable write buffer Test Mode and NOP command select
 	 */
	*drcctl=0x01;

	/* dummy write for NOP to take effect */
	dummy_write();
	print_err("NOP\n");
	/* 100? 200? */
	//udelay(100);
	print_err("after udelay\n");

	/* issue all banks precharge */
	*drcctl=0x02;
	print_err("set *drcctl to 2 \n");
	dummy_write();
	print_err("PRE\n");

	/* issue 2 auto refreshes to all banks */
	*drcctl=0x04;
	dummy_write();
	print_err("AUTO1\n");
	dummy_write();
	print_err("AUTO2\n");

	/* issue LOAD MODE REGISTER command */
	*drcctl=0x03;
	dummy_write();
	print_err("LOAD MODE REG\n");

	*drcctl=0x04;
	for (i=0; i<8; i++) /* refresh 8 times */{
		dummy_write();
		print_err("dummy write\n");
	}
	print_err("8 dummy writes\n");

	/* set control register to NORMAL mode */
	*drcctl=0x00;
	print_err("normal\n");

	print_err("HI done normal\n");
	bank = 3;


	/* this is really ugly, it is right from assembly code.
	 * we need to clean it up later
	 */

start:
	/* write col 11 wrap adr */
	COL11_ADR=COL11_DATA;
	if(COL11_ADR!=COL11_DATA)
		goto bad_ram;

print_err("11\n");
	/* write col 10 wrap adr */
	COL10_ADR=COL10_DATA;
	if(COL10_ADR!=COL10_DATA)
		goto bad_ram;
print_err("10\n");

	/* write col 9 wrap adr */
	COL09_ADR=COL09_DATA;
	if(COL09_ADR!=COL09_DATA)
		goto bad_ram;
print_err("9\n");

	/* write col 8 wrap adr */
	COL08_ADR=COL08_DATA;
	if(COL08_ADR!=COL08_DATA)
		goto bad_ram;
print_err("8\n");

	/* write row 14 wrap adr */
	ROW14_ADR=ROW14_DATA;
	if(ROW14_ADR!=ROW14_DATA)
		goto bad_ram;
print_err("14\n");

	/* write row 13 wrap adr */
	ROW13_ADR=ROW13_DATA;
	if(ROW13_ADR!=ROW13_DATA)
		goto bad_ram;
print_err("13\n");

	/* write row 12 wrap adr */
	ROW12_ADR=ROW12_DATA;
	if(ROW12_ADR!=ROW12_DATA)
		goto bad_ram;
print_err("12\n");

	/* write row 11 wrap adr */
	ROW11_ADR=ROW11_DATA;
	if(ROW11_ADR!=ROW11_DATA)
		goto bad_ram;
print_err("11\n");

	/* write row 10 wrap adr */
	ROW10_ADR=ROW10_DATA;
	if(ROW10_ADR!=ROW10_DATA)
		goto bad_ram;
print_err("10\n");

/*
 * read data @ row 12 wrap adr to determine # banks,
 *  and read data @ row 14 wrap adr to determine # rows.
 *  if data @ row 12 wrap adr is not AA, 11 or 12 we have bad RAM.
 * if data @ row 12 wrap == AA, we only have 2 banks, NOT 4
 * if data @ row 12 wrap == 11 or 12, we have 4 banks
 */

	banks=2;
	if (ROW12_ADR != ROW10_DATA) {
		banks=4;
print_err("4b\n");
		if(ROW12_ADR != ROW11_DATA) {
			if(ROW12_ADR != ROW12_DATA)
				goto bad_ram;
		}
	}

	/* validate row mask */
	rows=ROW14_ADR;
	if (rows<ROW11_DATA)
		goto bad_ram;
	if (rows>ROW14_DATA)
		goto bad_ram;
	/* verify all 4 bytes of dword same */
/*
	if(rows&0xffff!=(rows>>16)&0xffff)
		goto bad_ram;
	if(rows&0xff!=(rows>>8)&0xff)
		goto bad_ram;
*/
	/* now just get one of them */
	rows &= 0xff;
	print_err("rows"); print_err_hex32(rows); print_err("\n");
	/* validate column data */
	cols=COL11_ADR;
	if(cols<COL08_DATA)
		goto bad_ram;
	if (cols>COL11_DATA)
		goto bad_ram;
	/* verify all 4 bytes of dword same */
/*
	if(cols&0xffff!=(cols>>16)&0xffff)
		goto bad_ram;
	if(cols&0xff!=(cols>>8)&0xff)
		goto bad_ram;
*/
	print_err("cols"); print_err_hex32(cols); print_err("\n");
	cols -= COL08_DATA;

	/* cols now is in the range of 0 1 2 3 ...
	 */
	i = cols&3;
	//	i = cols + rows;

	/* wacky end addr calculation */
/*
	al = 3;
	al -= (i & 0xff);k
 */

	/* what a fookin' mess this is */
	if(banks==4)
		i+=8; /* <-- i holds merged value */
	/* i now has the col width in bits 0-1 and the bank count (2 or 4)
	 * in bit 3.
	 * this is the format for the drccfg register
	 */

	/* fix ending addr mask*/
	/*FIXME*/
	/* let's just go with this to start ... see if we can get ANYWHERE */
	/* need to get end addr. Need to do it with the bank in mind. */
/*
	al = 3;
	al -= i&3;
	*drcbendaddr = rows >> al;
	print_err("computed ending_adr = "); print_err_hex8(ending_adr);
	print_err("\n");

*/
bad_reinit:
	/* issue all banks recharge */
	*drcctl=0x02;
	dummy_write();

	/* update ending address register */
//	*drcbendadr = ending_adr;

	/* update config register */
	*drccfg &= ~(0xff << bank*4);
	if (ending_adr)
	  *drccfg = ((banks == 4 ? 8 : 0) | cols & 3)<< (bank*4);
//	dumpram();
	/* skip the rest for now */
	//	bank = 0;
	//	*drccfg=*drccfg&YYY|ZZZZ;

	if(bank!=0) {
		bank--;
//		drcbendaddr--;
		*drcbendaddr = 0xff000000;
		//*(&*drcbendadr+XXYYXX)=0xff;
		goto start;
	}

	/* set control register to NORMAL mode */
	*drcctl=0x18;
	dummy_write();
	return bank;

bad_ram:
	print_info("bad ram!\n");
	/* you are here because the read-after-write failed,
	 * in most cases because: no ram in that bank!
	 * set badbank to 1 and go to reinit
	 */
	ending_adr = 0;
	goto bad_reinit;
	while(1)
	print_err("DONE NEXTBANK\n");
#endif
}

/* note: based on AMD code*/
/* This code is known to work on the digital logic board and on the technologic
 * systems ts5300
 */
int staticmem(void)
{
	volatile unsigned long *zero = (unsigned long *) CACHELINESZ;

	/* set up 0x18 .. **/
	*drcbendadr = 0x88;
	*drcmctl = 0x1e;
	*drccfg = 0x9;
	/* nop mode */
	*drcctl = 0x1;
	/* do the dummy write */
	*zero = 0;

	/* precharge */
	*drcctl = 2;
	*zero = 0;

	/* two autorefreshes */
	*drcctl = 4;
	*zero = 0;
	print_debug("one zero out on refresh\n");
	*zero = 0;
	print_debug("two zero out on refresh\n");

	/* load mode register */
	*drcctl = 3;
	*zero = 0;
	print_debug("DONE the load mode reg\n");

	/* normal mode */
	*drcctl = 0x0;
	*zero = 0;
	print_debug("DONE one last write and then turn on refresh etc\n");
	*drcctl = 0x18;
	*zero = 0;
	print_debug("DONE the normal\n");
	*zero = 0xdeadbeef;
	if (*zero != 0xdeadbeef)
	  print_debug("NO LUCK\n");
	else
	  print_debug("did a store and load ...\n");
	//print_err_hex32(*zero);
	//	print_err(" zero is now "); print_err_hex32(*zero); print_err("\n");
}
