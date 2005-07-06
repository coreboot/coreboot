
/*
 *
 *
 */

#define DRCCTL        *(char*)0x0fffef010  // DRAM control register
#define DRCTMCTL      *(char*)0x0fffef012  // DRAM timing control register
#define DRCCFG        *(char*)0x0fffef014  // DRAM bank configuration register
#define DRCBENDADR    *(char*)0x0fffef018  // DRAM bank ending address register
#define ECCCTL        *(char*)0x0fffef020  // DRAM ECC control register
#define DBCTL         *(char*)0x0fffef040  // DRAM buffer control register

#define CACHELINESZ   0x00000010  //  size of our cache line (read buffer)

#define COL11_ADR  *(unsigned int *)0x0e001e00 // 11 col addrs
#define COL10_ADR  *(unsigned int *)0x0e000e00 // 10 col addrs
#define COL09_ADR  *(unsigned int *)0x0e000600 //  9 col addrs
#define COL08_ADR  *(unsigned int *)0x0e000200 //  8 col addrs

#define ROW14_ADR  *(unsigned int *)0x0f000000 // 14 row addrs
#define ROW13_ADR  *(unsigned int *)0x07000000 // 13 row addrs
#define ROW12_ADR  *(unsigned int *)0x03000000 // 12 row addrs
#define ROW11_ADR  *(unsigned int *)0x01000000 // 11 row addrs/also bank switch
#define ROW10_ADR  *(unsigned int *)0x00000000 // 10 row addrs/also bank switch

#define COL11_DATA 0x0b0b0b0b	//  11 col addrs
#define COL10_DATA 0x0a0a0a0a	//  10 col data
#define COL09_DATA 0x09090909	//   9 col data
#define COL08_DATA 0x08080808	//   8 col data
#define ROW14_DATA 0x3f3f3f3f	//  14 row data (MASK)
#define ROW13_DATA 0x1f1f1f1f	//  13 row data (MASK)
#define ROW12_DATA 0x0f0f0f0f	//  12 row data (MASK)
#define ROW11_DATA 0x07070707	//  11 row data/also bank switch (MASK)
#define ROW10_DATA 0xaaaaaaaa	//  10 row data/also bank switch (MASK)

#define dummy_write()   *(short *)CACHELINESZ=0x1010

int nextbank(int bank)
{
	int rows,banks;
	
start:
	/* write col 11 wrap adr */
	COL11_ADR=COL11_DATA;
	if(COL11_ADR!=COL11_DATA)
		goto bad_ram;

	/* write col 10 wrap adr */
	COL10_ADR=COL10_DATA;
	if(COL10_ADR!=COL10_DATA)
		goto bad_ram;

	/* write col 9 wrap adr */
	COL9_ADR=COL9_DATA;
	if(COL9_ADR!=COL9_DATA)
		goto bad_ram;

	/* write col 8 wrap adr */
	COL8_ADR=COL8_DATA;
	if(COL8_ADR!=COL8_DATA)
		goto bad_ram;

	/* write row 14 wrap adr */
	ROW14_ADR=ROW14_DATA;
	if(ROW14_ADR!=ROW14_DATA)
		goto bad_ram;

	/* write row 13 wrap adr */
	ROW13_ADR=ROW13_DATA;
	if(ROW13_ADR!=ROW13_DATA)
		goto bad_ram;

	/* write row 12 wrap adr */
	ROW12_ADR=ROW12_DATA;
	if(ROW12_ADR!=ROW12_DATA)
		goto bad_ram;

	/* write row 11 wrap adr */
	ROW11_ADR=ROW11_DATA;
	if(ROW11_ADR!=ROW11_DATA)
		goto bad_ram;

	/* write row 10 wrap adr */
	ROW10_ADR=ROW10_DATA;
	if(ROW10_ADR!=ROW10_DATA)
		goto bad_ram;

/*
 * read data @ row 12 wrap adr to determine # banks,
 *  and read data @ row 14 wrap adr to determine # rows.
 *  if data @ row 12 wrap adr is not AA, 11 or 12 we have bad RAM.
 * if data @ row 12 wrap == AA, we only have 2 banks, NOT 4
 * if data @ row 12 wrap == 11 or 12, we have 4 banks
 */

	banks=2;
	if (ROW12_ADDR != ROW10_DATA) {
		banks=4;
		if(ROW12_ADDR != ROW11_DATA) {
			if(ROW12_ADDR != ROW12_DATA)
				goto bad_ram;
		}
	}

	/* validate row mask */
	i=ROW14_ADDR;
	if (i<ROW11_DATA)
		goto bad_ram;
	if (i>ROW14_DATA)
		goto bad_ram;
	/* verify all 4 bytes of dword same */
	if(i&0xffff!=(i>>16)&0xffff)
		goto bad_ram;
	if(i&0xff!=(i>>8)&0xff)
		goto bad_ram;
	
	
	/* validate column data */
	i=COL11_ADDR;
	if(i<COL8_DATA)
		goto bad_ram;
	if (i>COL11_DATA)
		goto bad_ram;
	/* verify all 4 bytes of dword same */
	if(i&0xffff!=(i>>16)&0xffff)
		goto bad_ram;
	if(i&0xff!=(i>>8)&0xff)
		goto bad_ram;
	
	if(banks==4)
		i+=8; /* <-- i holds merged value */
	
	/* fix ending addr mask*/
	/*FIXME*/
	ending_adr=0xff;

bad_reint:
	/* issue all banks recharge */
	DRCCTL=0x02;
	dummy_write();

	/* update ending address register */
	*(DRCBENDADR+xxxx)=ending_adr;
	
	/* update config register */
	DRCCFG=DRCCFG&YYY|ZZZZ;

	if(bank!=0) {
		bank--;
		*(&DRCBENDADR+XXYYXX)=0xff;
		goto start;
	}

	/* set control register to NORMAL mode */
	DRCCTL=0x00;
	dummy_write();
	return bank;
	
bad_ram:
	printk_error("bad ram!\r\n");
}

/* cache is assumed to be disabled */
int sizemem(void)
{
	int i;
	/* initialize dram controller registers */

	DBCTL=0; /* disable write buffer/read-ahead buffer */
	ECCCTL=0; /* disable ECC */
	DRCTMCTL=0x1e; /* Set SDRAM timing for slowest speed. */

	/* setup loop to do 4 external banks starting with bank 3 */

	/* enable last bank and setup ending address 
	 * register for max ram in last bank
	 */
	DRCBENDADR=0x0ff000000;
	/* setup dram register for all banks
	 * with max cols and max banks
	 */
	DRCCFG=0xbbbb;

	/* issue a NOP to all DRAMs */

	/* Asetup DRAM control register with Disable refresh,
 	 * disable write buffer Test Mode and NOP command select
 	 */
	DRCCTL=0x01;

	/* dummy write for NOP to take effect */
	dummy_write();

	/* 100? 200? */
	udelay(100);

	/* issue all banks precharge */
	DRCCTL=0x02;
	dummy_write();

	/* issue 2 auto refreshes to all banks */
	DRCCTL=0x04;
	dummy_write();
	dummy_write();

	/* issue LOAD MODE REGISTER command */
	DRCCTL=0x03;
	dummy_write();

	DRCCTL=0x04;
	for (i=0; i<8; i++) /* refresh 8 times */
		dummy_write();

	/* set control register to NORMAL mode */
	DRCCTL=0x00;

	nextbank(3);

}	
