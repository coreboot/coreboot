#ifndef ROM_STPC_H
#define	ROM_STPC_H

/*
 * Bootstrap code for the STPC Elite
 *
 * Modified by Peter Fox:
 * Removed graphics stuff
 * Added GPL header
 *
 * This software comes with ABSOLUTELY NO WARRANTY; This is free software, and
 * you are welcome to redistribute it under certain conditions; you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2 of the
 * License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA. "
 *
 * from Consumer II
 * by Steve M. Gehlbach
 *
 * orig by: johryd
 *
 *
 */

/* 
 *	SIP configration registers:
 */
#define	STPC_CONF_REG_INDEX	0x22
#define	STPC_CONF_REG_DATA	0x23

/* 
 *	DRAM types:
 */
#define	STPC_DRAM_FPM_60	0
#define	STPC_DRAM_FPM_70	1
#define	STPC_DRAM_EDO_60	2
#define	STPC_DRAM_EDO_70	3
#define	STPC_DRAM_SDRAM		4

/*
 *	Macro:		stpc_conf_write_macro
 *	Prototype:	value = stpc_conf_write_macro(reg, data)
 *			 %al                          %ah   %al
 */
#define	stpc_conf_write_macro				\
	xchgb	%ah, %al				;\
	outb	%al, $STPC_CONF_REG_INDEX		;\
	xchgb	%ah, %al				;\
	outb	%al, $STPC_CONF_REG_DATA

/* 
 *	Macro:		stpc_conf_read_macro
 *	Prototype:	value = stpc_conf_write_read(reg)
 *			 %al                         %ah
 */
#define	stpc_conf_read_macro				\
	xchgb	%ah, %al				;\
	outb	%al, $STPC_CONF_REG_INDEX		;\
	xchgb	%ah, %al				;\
	inb	$STPC_CONF_REG_DATA, %al

/*
 *	Macro:		stpc_chip_getbusspeed_macro
 *	Prototype:	value = stpc_chip_getbusspeed()
 *			 %al
 *	Trashed:	%ah, %al
 */
#define	stpc_chip_getbusspeed_macro			\
	movb	$0x5f, %ah				;\
	stpc_conf_read_macro				;\
	andb	$0b00111000, %al			;\
	shrb	$3, %al

/*
 *	Macro:	stpc_outb_macro
 *	Prototype:	stpc_outb_macro(value, port)
 *	Trashed:	%al
 */
#define	stpc_outb_macro(value,port)			\
	movb	$(value), %al				;\
	outb	%al, $(port)

/*
 *	Macro:	stpc_chip_post_macro
 *	Prototype:	stpc_chip_post(value)
 *	Trashed:	%al
 */
#define	stpc_chip_post_macro(value)			\
	movb	$(value), %al				;\
	outb	%al, $0x80

/*
 *	Macro:	stpc_delay_macro
 *	Prototype:	stpc_delay_macro(value)
 *	Trashed:	%ecx, %al
 */
#define	stpc_delay_macro(value)			\
	movl    $value, %ecx               ;\
0:  inb     $0x80, %al                 ;\
	loop    0b

/*
 *  Sets all of the mem regs from %bh to 0x33 to
 *  the value in %al
 *	Macro:	stpc_set_memreg_macro
 *	Prototype:	stpc_set_memreg_macro
 *  Input:  	%bh: first mem reg to set (0x30-0x33)
 *          	%al: setting for all the registers
 *	Trashed:	%ah
 */
#define	stpc_set_memreg_macro	\
    movb    %bh, %ah            ;\
0:								;\
    stpc_conf_write_macro		;\
    inc     %ah					;\
    cmpb    $0x33, %ah			;\
    jbe     0b

#endif /* ROM_STPC_H */
