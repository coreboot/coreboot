#include <stdio.h>
#include "test.h"
#include "pci.h"

void x86emu_dump_xregs(void);
extern ptr current;
extern int verbose;


#ifndef _PC
/*
 * This is derived from a number of PC system BIOS'es.  The intent here is to
 * provide very primitive video support, before an EGA/VGA BIOS installs its
 * own interrupt vector.  Here, "Ignored" calls should remain so.  "Not
 * Implemented" denotes functionality that can be implemented should the need
 * arise.  What are "Not Implemented" throughout are video memory accesses.
 * Also, very little input validity checking is done here.
 */
int int42_handler()
{
#if 0
	if (verbose && X86_AH != 0x0e) {
		printf("int%x\n", current->num);
		x86emu_dump_xregs();
	}

	switch (X86_AH) {
	case 0x00:
		/* Set Video Mode                                     */
		/* Enter:  AL = video mode number                     */
		/* Leave:  Nothing                                    */
		/* Implemented (except for clearing the screen)       */
		{		/* Localise */
			int i;
			u16 ioport, int1d, regvals, tmp;
			u8 mode, cgamode, cgacolour;

			/*
			 * Ignore all mode numbers but 0x00-0x13.  Some systems also ignore
			 * 0x0B and 0x0C, but don't do that here.
			 */
			if (X86_AL > 0x13)
				break;

			/*
			 * You didn't think that was really the mode set, did you?  There
			 * are only so many slots in the video parameter table...
			 */
			mode = X86_AL;
			ioport = 0x03D4;
			switch (MEM_RB(0x0410) & 0x30) {
			case 0x30:	/* MDA */
				mode = 0x07;	/* Force mode to 0x07 */
				ioport = 0x03B4;
				break;
			case 0x10:	/* CGA 40x25 */
				if (mode >= 0x07)
					mode = 0x01;
				break;
			case 0x20:	/* CGA 80x25 (MCGA?) */
				if (mode >= 0x07)
					mode = 0x03;
				break;
			case 0x00:	/* EGA/VGA */
				if (mode >= 0x07)	/* Don't try MDA timings */
					mode = 0x01;	/* !?!?! */
				break;
			}

			/* Locate data in video parameter table */
			int1d = MEM_RW(0x1d << 2);
			regvals = ((mode >> 1) << 4) + int1d;
			cgacolour = 0x30;
			if (mode == 0x06) {
				regvals -= 0x10;
				cgacolour = 0x3F;
			}

	    /** Update BIOS Data Area **/

			/* Video mode */
			MEM_WB(0x0449, mode);

			/* Columns */
			tmp = MEM_RB(mode + int1d + 0x48);
			MEM_WW(0x044A, tmp);

			/* Page length */
			tmp = MEM_RW((mode & 0x06) + int1d + 0x40);
			MEM_WW(0x044C, tmp);

			/* Start Address */
			MEM_WW(0x044E, 0);

			/* Cursor positions, one for each display page */
			for (i = 0x0450; i < 0x0460; i += 2)
				MEM_WW(i, 0);

			/* Cursor start & end scanlines */
			tmp = MEM_RB(regvals + 0x0B);
			MEM_WB(0x0460, tmp);
			tmp = MEM_RB(regvals + 0x0A);
			MEM_WB(0x0461, tmp);

			/* Current display page number */
			MEM_WB(0x0462, 0);

			/* CRTC I/O address */
			MEM_WW(0x0463, ioport);

			/* CGA Mode register value */
			cgamode = MEM_RB(mode + int1d + 0x50);
			MEM_WB(0x0465, cgamode);

			/* CGA Colour register value */
			MEM_WB(0x0466, cgacolour);

			/* Rows */
			MEM_WB(0x0484, (25 - 1));

			/* Programme the mode */
			outb(ioport + 4, cgamode & 0x37);	/* Turn off screen */
			for (i = 0; i < 0x10; i++) {
				tmp = MEM_RB(regvals + i);
				outb(ioport, i);
				outb(ioport + 1, tmp);
			}
			outb(ioport + 5, cgacolour);	/* Select colour mode */
			outb(ioport + 4, cgamode);	/* Turn on screen */
		}
		break;

	case 0x01:
		/* Set Cursor Type                                    */
		/* Enter:  CH = starting line for cursor              */
		/*         CL = ending line for cursor                */
		/* Leave:  Nothing                                    */
		/* Implemented                                        */
		{		/* Localise */
			u16 ioport = MEM_RW(0x0463);

			MEM_WB(0x0460, X86_CL);
			MEM_WB(0x0461, X86_CH);

			outb(ioport, 0x0A);
			outb(ioport + 1, X86_CH);
			outb(ioport, 0x0B);
			outb(ioport + 1, X86_CL);
		}
		break;

	case 0x02:
		/* Set Cursor Position                                */
		/* Enter:  BH = display page number                   */
		/*         DH = row                                   */
		/*         DL = column                                */
		/* Leave:  Nothing                                    */
		/* Implemented                                        */
		{		/* Localise */
			u16 offset, ioport;

			MEM_WB((X86_BH << 1) + 0x0450, X86_DL);
			MEM_WB((X86_BH << 1) + 0x0451, X86_DH);

			if (X86_BH != MEM_RB(0x0462))
				break;

			offset = (X86_DH * MEM_RW(0x044A)) + X86_DL;
			offset += MEM_RW(0x044E) << 1;

			ioport = MEM_RW(0x0463);
			outb(ioport, 0x0E);
			outb(ioport + 1, offset >> 8);
			outb(ioport, 0x0F);
			outb(ioport + 1, offset & 0xFF);
		}
		break;

	case 0x03:
		/* Get Cursor Position                                */
		/* Enter:  BH = display page number                   */
		/* Leave:  CH = starting line for cursor              */
		/*         CL = ending line for cursor                */
		/*         DH = row                                   */
		/*         DL = column                                */
		/* Implemented                                        */
		{		/* Localise */
			X86_CL = MEM_RB(0x0460);
			X86_CH = MEM_RB(0x0461);
			X86_DL = MEM_RB((X86_BH << 1) + 0x0450);
			X86_DH = MEM_RB((X86_BH << 1) + 0x0451);
		}
		break;

	case 0x04:
		/* Get Light Pen Position                             */
		/* Enter:  Nothing                                    */
		/* Leave:  AH = 0x01 (down/triggered) or 0x00 (not)   */
		/*         BX = pixel column                          */
		/*         CX = pixel row                             */
		/*         DH = character row                         */
		/*         DL = character column                      */
		/* Not Implemented                                    */
		{		/* Localise */
			printf("int%x - Get Light Pen Position. "
			       "Function not implemented.\n", current->num);
			x86emu_dump_xregs();
			X86_AH = X86_BX = X86_CX = X86_DX = 0;
		}
		break;

	case 0x05:
		/* Set Display Page                                   */
		/* Enter:  AL = display page number                   */
		/* Leave:  Nothing                                    */
		/* Implemented                                        */
		{		/* Localise */
			u16 start, ioport = MEM_RW(0x0463);
			u8 x, y;

			/* Calculate new start address */
			MEM_WB(0x0462, X86_AL);
			start = X86_AL * MEM_RW(0x044C);
			MEM_WW(0x044E, start);
			start <<= 1;

			/* Update start address */
			outb(ioport, 0x0C);
			outb(ioport + 1, start >> 8);
			outb(ioport, 0x0D);
			outb(ioport + 1, start & 0xFF);

			/* Switch cursor position */
			y = MEM_RB((X86_AL << 1) + 0x0450);
			x = MEM_RB((X86_AL << 1) + 0x0451);
			start += (y * MEM_RW(0x044A)) + x;

			/* Update cursor position */
			outb(ioport, 0x0E);
			outb(ioport + 1, start >> 8);
			outb(ioport, 0x0F);
			outb(ioport + 1, start & 0xFF);
		}
		break;

	case 0x06:
		/* Initialise or Scroll Window Up                     */
		/* Enter:  AL = lines to scroll up                    */
		/*         BH = attribute for blank                   */
		/*         CH = upper y of window                     */
		/*         CL = left x of window                      */
		/*         DH = lower y of window                     */
		/*         DL = right x of window                     */
		/* Leave:  Nothing                                    */
		/* Not Implemented                                    */
		{		/* Localise */
			printf("int%x: Initialise or Scroll Window Up - "
			       "Function not implemented.\n", current->num);
			x86emu_dump_xregs();
		}
		break;

	case 0x07:
		/* Initialise or Scroll Window Down                   */
		/* Enter:  AL = lines to scroll down                  */
		/*         BH = attribute for blank                   */
		/*         CH = upper y of window                     */
		/*         CL = left x of window                      */
		/*         DH = lower y of window                     */
		/*         DL = right x of window                     */
		/* Leave:  Nothing                                    */
		/* Not Implemented                                    */
		{		/* Localise */
			printf("int%x: Initialise or Scroll Window Down - "
			       "Function not implemented.\n", current->num);
			x86emu_dump_xregs();

		}
		break;

	case 0x08:
		/* Read Character and Attribute at Cursor             */
		/* Enter:  BH = display page number                   */
		/* Leave:  AH = attribute                             */
		/*         AL = character                             */
		/* Not Implemented                                    */
		{		/* Localise */
			printf
			    ("int%x: Read Character and Attribute at Cursor - "
			     "Function not implemented.\n", current->num);
			x86emu_dump_xregs();

			X86_AX = 0;
		}
		break;

	case 0x09:
		/* Write Character and Attribute at Cursor            */
		/* Enter:  AL = character                             */
		/*         BH = display page number                   */
		/*         BL = attribute (text) or colour (graphics) */
		/*         CX = replication count                     */
		/* Leave:  Nothing                                    */
		/* Not Implemented                                    */
		{		/* Localise */
			printf
			    ("int%x: Write Character and Attribute at Cursor - "
			     "Function not implemented.\n", current->num);
			x86emu_dump_xregs();

		}
		break;

	case 0x0a:
		/* Write Character at Cursor                          */
		/* Enter:  AL = character                             */
		/*         BH = display page number                   */
		/*         BL = colour                                */
		/*         CX = replication count                     */
		/* Leave:  Nothing                                    */
		/* Not Implemented                                    */
		{		/* Localise */
			printf("int%x: Write Character at Cursor - "
			       "Function not implemented.\n", current->num);
			x86emu_dump_xregs();

		}
		break;

	case 0x0b:
		/* Set Palette, Background or Border                  */
		/* Enter:  BH = 0x00 or 0x01                          */
		/*         BL = colour or palette (respectively)      */
		/* Leave:  Nothing                                    */
		/* Implemented                                        */
		{		/* Localise */
			u16 ioport = MEM_RW(0x0463) + 5;
			u8 cgacolour = MEM_RB(0x0466);

			if (X86_BH) {
				cgacolour &= 0xDF;
				cgacolour |= (X86_BL & 0x01) << 5;
			} else {
				cgacolour &= 0xE0;
				cgacolour |= X86_BL & 0x1F;
			}

			MEM_WB(0x0466, cgacolour);
			outb(ioport, cgacolour);
		}
		break;

	case 0x0c:
		/* Write Graphics Pixel                               */
		/* Enter:  AL = pixel value                           */
		/*         BH = display page number                   */
		/*         CX = column                                */
		/*         DX = row                                   */
		/* Leave:  Nothing                                    */
		/* Not Implemented                                    */
		{		/* Localise */
			printf("int%x: Write Graphics Pixel - "
			       "Function not implemented.\n", current->num);
			x86emu_dump_xregs();

		}
		break;

	case 0x0d:
		/* Read Graphics Pixel                                */
		/* Enter:  BH = display page number                   */
		/*         CX = column                                */
		/*         DX = row                                   */
		/* Leave:  AL = pixel value                           */
		/* Not Implemented                                    */
		{		/* Localise */
			printf("int%x: Write Graphics Pixel - "
			       "Function not implemented.\n", current->num);
			x86emu_dump_xregs();

			X86_AL = 0;

		}
		break;

	case 0x0e:
		/* Write Character in Teletype Mode                   */
		/* Enter:  AL = character                             */
		/*         BH = display page number                   */
		/*         BL = foreground colour                     */
		/* Leave:  Nothing                                    */
		/* Not Implemented                                    */
		/* WARNING:  Emulation of BEL characters will require */
		/*           emulation of RTC and PC speaker I/O.     */
		/*           Also, this recurses through int 0x10     */
		/*           which might or might not have been       */
		/*           installed yet.                           */
		{		/* Localise */
#ifdef PARANOID
			printf("int%x: Write Character in Teletype Mode - "
			       "Function not implemented.\n", current->num);
			x86emu_dump_xregs();
#endif
			printf("%c", X86_AL);
		}
		break;

	case 0x0f:
		/* Get Video Mode                                     */
		/* Enter:  Nothing                                    */
		/* Leave:  AH = number of columns                     */
		/*         AL = video mode number                     */
		/*         BH = display page number                   */
		/* Implemented                                        */
		{		/* Localise */
			X86_AH = MEM_RW(0x044A);
			X86_AL = MEM_RB(0x0449);
			X86_BH = MEM_RB(0x0462);
		}
		break;

	case 0x10:
		/* Colour Control (subfunction in AL)                 */
		/* Enter:  Various                                    */
		/* Leave:  Various                                    */
		/* Ignored                                            */
		break;

	case 0x11:
		/* Font Control (subfunction in AL)                   */
		/* Enter:  Various                                    */
		/* Leave:  Various                                    */
		/* Ignored                                            */
		break;

	case 0x12:
		/* Miscellaneous (subfunction in BL)                  */
		/* Enter:  Various                                    */
		/* Leave:  Various                                    */
		/* Ignored.  Previous code here optionally allowed    */
		/* the enabling and disabling of VGA, but no system   */
		/* BIOS I've come across actually implements it.      */
		break;

	case 0x13:
		/* Write String in Teletype Mode                      */
		/* Enter:  AL = write mode                            */
		/*         BL = attribute (if (AL & 0x02) == 0)       */
		/*         CX = string length                         */
		/*         DH = row                                   */
		/*         DL = column                                */
		/*         ES:BP = string segment:offset              */
		/* Leave:  Nothing                                    */
		/* Not Implemented                                    */
		/* WARNING:  Emulation of BEL characters will require */
		/*           emulation of RTC and PC speaker I/O.     */
		/*           Also, this recurses through int 0x10     */
		/*           which might or might not have been       */
		/*           installed yet.                           */
		{		/* Localise */
			printf("int%x: Write String in Teletype Mode - "
			       "Function not implemented.\n", current->num);
			x86emu_dump_xregs();

		}
		break;

	default:
		/* Various extensions                                 */
		/* Enter:  Various                                    */
		/* Leave:  Various                                    */
		/* Ignored                                            */
		break;
	}
#endif
	return 1;
}
#endif
