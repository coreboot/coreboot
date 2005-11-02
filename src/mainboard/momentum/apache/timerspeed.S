#include <ppc970.h>

/*----------------------------------------------------------------------------+
| Cycle counts ((1/ 9600)* 10) / (1/speed)) - 2%.
+----------------------------------------------------------------------------*/
#define	SPEED_6_25    		(0x0000196E- (65* 2))
#define SPEED_7_159		(0x00001D21- (74* 2))
#define	SPEED_8_33    		(0x000021E5- (86* 2))
#define	SPEED_10_4    		(0x00002A51- (108* 2))
#define SPEED_14_318		(0x00003A42- (149* 2))
#define	SPEED_16_66    		(0x000043D0- (173* 2))
#define	SPEED_25    		(0x000065B9- (260* 2))
#define	SPEED_33    		(0x000087A2- (347* 2))
#define	SPEED_40    		(0x0000A2C2- (416* 2))
#define	SPEED_50    		(0x0000CB73- (520* 2))
#define	SPEED_66    		(0x00010C8E- (687* 2))
#define	SPEED_80    		(0x00014585- (833* 2))
#define	SPEED_100    		(0x000196E6- (1041* 2))
#define	SPEED_125    		(0x0001FCA0- (1302* 2))
#define	SPEED_133    		(0x00021D2D- (1385* 2))
#define	SPEED_150    		(0x0002625A- (1562* 2))
#define	SPEED_166    		(0x0002A374- (1729* 2))
#define	SPEED_175    		(0x0002C813- (1822* 2))
#define	SPEED_200    		(0x00032DCD- (2093* 2))
#define	SPEED_225    		(0x00039387- (2343* 2))
#define	SPEED_250    		(0x0003F940- (2604* 2))
#define	SPEED_275    		(0x00045EFA- (2864* 2))
#define	SPEED_300    		(0x0004C4B4- (3125* 2))
#define	SPEED_3375    		(0x00055D4A- (3515* 2))
#define	SPEED_375    		(0x0005F5E1- (3906* 2))
#define	SPEED_400    		(0x00065B9A- (4166* 2))
#define	SPEED_433    		(0x0006E1E1- (4510* 2))
#define	SPEED_466	    	(0x00076828- (4854* 2))
#define	SPEED_500    		(0x0007F281- (5208* 2))

/*----------------------------------------------------------------------------+
| Timebase_speed_calc
+----------------------------------------------------------------------------*/
	function_prolog(timebase_speed_calc)
	mfmsr	r10
	rlwinm	r11,r10,0,17,15
	mtmsrd	r11,1
	isync
        /*--------------------------------------------------------------------+
        | Make sure that all the characters in the transmit buffer are sent.
        +--------------------------------------------------------------------*/
..sent: lbz	r6,asyncLSR(r3)
	andi.	r6,r6,0x0060
	cmpi	cr0,1,r6,0x0060
	bne	..sent
        /*--------------------------------------------------------------------+
        | Store current serial port settings in r11, r12.
        | r11 BH (baud high), BL (baud low), LCR
        | r12 MCR, IER, FCR
        +--------------------------------------------------------------------*/
	lbz	r4,asyncLCR(r3)
	ori	r11,r4,0x0000
	lbz	r4,asyncFCR(r3)
	ori	r12,r4,0x0000
	lbz	r4,asyncIER(r3)
	rlwimi	r12,r4,8,16,23
	lbz	r4,asyncMCR(r3)
	rlwimi	r12,r4,16,8,15
        /*--------------------------------------------------------------------+
        | Store BH and BL and program new baud rate.
        +--------------------------------------------------------------------*/
	addi    r4,r0,0x80   	
	stb     r4,asyncLCR(r3)
	lbz	r4,asyncDLABMsb(r3)
	rlwimi  r11,r4,16,8,15
	addi    r4,r0,DIV_HIGH_9600
	stb	r4,asyncDLABMsb(r3)
	lbz	r4,asyncDLABLsb(r3)
	rlwimi  r11,r4,8,16,23
	addi    r4,r0,DIV_LOW_9600
	stb     r4,asyncDLABLsb(r3)
	addi    r4,r0,0x03   	
	stb     r4,asyncLCR(r3)
        /*--------------------------------------------------------------------+
        | Put the serial port in loop-back mode.
        +--------------------------------------------------------------------*/
	addi    r4,r0,0x00   	
	stb     r4,asyncFCR(r3)
	stb     r4,asyncIER(r3)
	addi    r4,r0,0x10
	stb     r4,asyncMCR(r3)
        lbz	r4,asyncRxBuffer(r3)
	addi    r4,r0,0x0041	
        /*--------------------------------------------------------------------+
        | Again make sure there are no characters in transmit buffer.
        +--------------------------------------------------------------------*/
        addi	r5,r0,0
..again:lbz	r6,asyncLSR(r3)
	andi.	r6,r6,0x0060
	cmpi	cr0,1,r6,0x0060
	bne	..again
        /*--------------------------------------------------------------------+
        | Take a snapshot of the timebase.
        +--------------------------------------------------------------------*/
        mfspr   r7,tblr
        /*--------------------------------------------------------------------+
        | Send a character while in loopback mode.  This will be done twice.
        | Once to get the instuctions into I-cache, the second time for the
        | real measurement.
        +--------------------------------------------------------------------*/
	stb     r4,asyncTxBuffer(r3)
..spnlp:lbz     r6,asyncLSR(r3)
        andi.   r6,r6,0x01
        beq     ..spnlp
	mfspr	r9,tblr
        /*--------------------------------------------------------------------+
        | Perform subtraction to determine how many timebase ticks it took
        | to transmit the character.
        +--------------------------------------------------------------------*/
        subfc   r9,r7,r9
        /*--------------------------------------------------------------------+
        | Consume the character sent in loopback mode.
        +--------------------------------------------------------------------*/
        lbz     r4,asyncRxBuffer(r3)
	eieio
        /*--------------------------------------------------------------------+
        | If the first character was just sent, go back and send a second.
        +--------------------------------------------------------------------*/
        cmpi	cr0,1,r5,0x0000
        addi	r5,r5,1
        beq	..again
        /*--------------------------------------------------------------------+
        | Restore serial port settings.
        +--------------------------------------------------------------------*/
	addi    r4,r0,0x80   	
	stb     r4,asyncLCR(r3)
	rlwinm	r4,r11,16,24,31
	stb     r4,asyncDLABMsb(r3)
	rlwinm	r4,r11,24,24,31
	stb     r4,asyncDLABLsb(r3)
	rlwinm	r4,r11,0,24,31
	stb     r4,asyncLCR(r3)
	rlwinm	r4,r12,16,24,31
	stb     r4,asyncMCR(r3)
	rlwinm	r4,r12,24,24,31
	stb     r4,asyncIER(r3)
	rlwinm	r4,r12,0,24,31
	stb     r4,asyncFCR(r3)
        /*--------------------------------------------------------------------+
        | Calculate timebase speed (r9 is the time we are referencing).
        +--------------------------------------------------------------------*/
	addis	r4,r0,SPEED_7_159@h
	ori	r4,r4,SPEED_7_159@l
	cmp	cr0,r9,r4
	blt	..freq6		
	addis	r4,r0,SPEED_8_33@h
	ori	r4,r4,SPEED_8_33@l
	cmp	cr0,r9,r4
	blt	..freq7		
        addis   r4,r0,SPEED_10_4@h
        ori     r4,r4,SPEED_10_4@l
	cmp	cr0,r9,r4
	blt	..freq8		
        addis   r4,r0,SPEED_14_318@h
        ori     r4,r4,SPEED_14_318@l
	cmp	cr0,r9,r4
	blt	..freq10
        addis   r4,r0,SPEED_16_66@h
        ori     r4,r4,SPEED_16_66@l
	cmp	cr0,r9,r4
	blt	..freq14
        addis   r4,r0,SPEED_25@h
        ori     r4,r4,SPEED_25@l
	cmp	cr0,r9,r4
	blt	..freq16
        addis   r4,r0,SPEED_33@h
        ori     r4,r4,SPEED_33@l
	cmp	cr0,r9,r4
	blt	..freq25
        addis   r4,r0,SPEED_40@h
        ori     r4,r4,SPEED_40@l
        cmp     cr0,r9,r4
	blt     ..freq33
        addis   r4,r0,SPEED_50@h
        ori     r4,r4,SPEED_50@l
        cmp     cr0,r9,r4
        blt     ..freq40
        addis   r4,r0,SPEED_66@h
        ori     r4,r4,SPEED_66@l
        cmp     cr0,r9,r4
        blt     ..freq50
        addis   r4,r0,SPEED_80@h
        ori     r4,r4,SPEED_80@l
        cmp     cr0,r9,r4
        blt     ..freq66
        addis   r4,r0,SPEED_100@h
        ori     r4,r4,SPEED_100@l
        cmp     cr0,r9,r4
        blt     ..freq80
        addis   r4,r0,SPEED_125@h
        ori     r4,r4,SPEED_125@l
        cmp     cr0,r9,r4
        blt     ..freq100
        addis   r4,r0,SPEED_133@h
        ori     r4,r4,SPEED_133@l
        cmp     cr0,r9,r4
        blt     ..freq125
        addis   r4,r0,SPEED_150@h
        ori     r4,r4,SPEED_150@l
        cmp     cr0,r9,r4
        blt     ..freq133
        addis   r4,r0,SPEED_166@h
        ori     r4,r4,SPEED_166@l
        cmp     cr0,r9,r4
        blt     ..freq150
        addis   r4,r0,SPEED_175@h
        ori     r4,r4,SPEED_175@l
        cmp     cr0,r9,r4
        blt     ..freq166
        addis   r4,r0,SPEED_200@h
        ori     r4,r4,SPEED_200@l
        cmp     cr0,r9,r4
        blt     ..freq175
        addis   r4,r0,SPEED_225@h
        ori     r4,r4,SPEED_225@l
        cmp     cr0,r9,r4
        blt     ..freq200
        addis   r4,r0,SPEED_250@h
	ori	r4,r4,SPEED_250@l
        cmp     cr0,r9,r4
        blt     ..freq225
        addis   r4,r0,SPEED_275@h
	ori	r4,r4,SPEED_275@l
        cmp     cr0,r9,r4
        blt     ..freq250
        addis   r4,r0,SPEED_300@h
	ori	r4,r4,SPEED_300@l
        cmp     cr0,r9,r4
        blt     ..freq275
        addis   r4,r0,SPEED_3375@h
	ori	r4,r4,SPEED_3375@l
        cmp     cr0,r9,r4
        blt     ..freq300
        addis   r4,r0,SPEED_375@h
	ori	r4,r4,SPEED_375@l
        cmp     cr0,r9,r4
        blt     ..freq3375
        addis   r4,r0,SPEED_400@h
	ori	r4,r4,SPEED_400@l
        cmp     cr0,r9,r4
        blt     ..freq375
        addis   r4,r0,SPEED_433@h
	ori	r4,r4,SPEED_433@l
        cmp     cr0,r9,r4
        blt     ..freq400
        addis   r4,r0,SPEED_466@h
	ori	r4,r4,SPEED_466@l
        cmp     cr0,r9,r4
        blt     ..freq433
        addis   r4,r0,SPEED_500@h
	ori	r4,r4,SPEED_500@l
        cmp     cr0,r9,r4
        blt     ..freq466
        b       ..freq500
..freq6:
	addis   r3,r0,0x005F
        ori     r3,r3,0x5e10
	b	..end
..freq7:
	addis   r3,r0,0x006D
        ori     r3,r3,0x3CD8
	b	..end
..freq8:
	addis   r3,r0,0x007F
        ori     r3,r3,0x2815
	b	..end
..freq10:
	addis   r3,r0,0x009e
        ori     r3,r3,0xb100
	b	..end
..freq14:
	addis   r3,r0,0x00da
        ori     r3,r3,0x79b0
	b	..end
..freq16:
	addis   r3,r0,0x00fe
        ori     r3,r3,0x502A
	b	..end
..freq25:
	addis   r3,r0,0x017D
        ori     r3,r3,0x7840
	b	..end
..freq33:
	addis   r3,r0,0x01FC
        ori     r3,r3,0xA055
        b       ..end
..freq40:
	addis   r3,r0,0x0262
        ori     r3,r3,0x5A00
        b       ..end
..freq50:
	addis   r3,r0,0x02FA
        ori     r3,r3,0xF080
        b       ..end
..freq66:
	addis   r3,r0,0x03F9
        ori     r3,r3,0x40AA
        b       ..end
..freq80:
	addis   r3,r0,0x04C4
        ori     r3,r3,0xB400
        b       ..end
..freq100:
	addis   r3,r0,0x05F5
        ori     r3,r3,0xE100
        b       ..end
..freq125:
	addis   r3,r0,0x0773
        ori     r3,r3,0x5940
        b       ..end
..freq133:
	addis   r3,r0,0x07F2
        ori     r3,r3,0x8155
        b       ..end
..freq150:
	addis   r3,r0,0x08F0
        ori     r3,r3,0xD180
        b       ..end
..freq166:
	addis   r3,r0,0x09EF
        ori     r3,r3,0x21AA
        b       ..end
..freq175:
	addis   r3,r0,0x0A6E
        ori     r3,r3,0x49C0
        b       ..end
..freq200:
	addis   r3,r0,0x0BEB
        ori     r3,r3,0xC200
        b       ..end
..freq225:
	addis   r3,r0,0x0D69
        ori     r3,r3,0x3A40
        b       ..end
..freq250:
	addis   r3,r0,0x0EE6
        ori     r3,r3,0xB280
        b       ..end
..freq275:
	addis   r3,r0,0x1064
        ori     r3,r3,0x2AC0
        b       ..end
..freq300:
	addis   r3,r0,0x11E1
        ori     r3,r3,0xA300
        b       ..end
..freq3375:
	addis   r3,r0,0x141D
        ori     r3,r3,0xD760
        b       ..end
..freq375:
	addis   r3,r0,0x165A
        ori     r3,r3,0x0BC0
        b       ..end
..freq400:
	addis   r3,r0,0x17D7
        ori     r3,r3,0x8400
        b       ..end
..freq433:
	addis   r3,r0,0x19CF
        ori     r3,r3,0x0E40
        b       ..end
..freq466:
	addis   r3,r0,0x1BC6
        ori     r3,r3,0x9880
        b       ..end
..freq500:
	addis   r3,r0,0x1DCD
        ori     r3,r3,0x6500
..end:  mtmsrd	r10,1
	isync
	blr
	function_epilog(timebase_speed_calc)
