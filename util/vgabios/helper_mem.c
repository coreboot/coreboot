/* $XFree86: xc/programs/Xserver/hw/xfree86/int10/helper_mem.c,v 1.21 2001/05/22 16:24:37 tsi Exp $ */
/*
 *                   XFree86 int10 module
 *   execute BIOS int 10h calls in x86 real mode environment
 *                 Copyright 1999 Egbert Eich
 */
#include <stdio.h>

#define _INT10_PRIVATE

#define REG pInt
#if 0
typedef enum {
	OPT_NOINT10,
	OPT_INIT_PRIMARY,
	OPT_BIOS_LOCATION
} INT10Opts;

static const OptionInfoRec INT10Options[] = {
	{OPT_NOINT10, "NoINT10", OPTV_BOOLEAN, {0}, FALSE},
	{OPT_INIT_PRIMARY, "InitPrimary", OPTV_BOOLEAN, {0}, FALSE},
	{OPT_BIOS_LOCATION, "BiosLocation", OPTV_STRING, {0}, FALSE},
	{-1, NULL, OPTV_NONE, {0}, FALSE},
};
#endif

#ifdef DEBUG
void dprint(unsigned long start, unsigned long size)
{
	int i, j;
	char *c = (char *) start;

	for (j = 0; j < (size >> 4); j++) {
		char *d = c;
		printf("\n0x%lx:  ", (unsigned long) c);
		for (i = 0; i < 16; i++)
			printf("%2.2x ", (unsigned char) (*(c++)));
		c = d;
		for (i = 0; i < 16; i++) {
			printf("%c", ((((unsigned char) (*c)) > 32) && (((unsigned char) (*c)) < 128)) ?
			       (unsigned char) (*(c)) : '.');
			c++;
		}
	}
	printf("\n");
}
#endif /* DEBUG */

#if 0
#ifndef _PC
/*
 * here we are really paranoid about faking a "real"
 * BIOS. Most of this information was pulled from
 * dosemu.
 */
void setup_int_vect(void)
{
	int i;

	/* let the int vects point to the SYS_BIOS seg */
	for (i = 0; i < 0x80; i++) {
		MEM_WW(i << 2, 0);
		MEM_WW((i << 2) + 2, SYS_BIOS >> 4);
	}

	reset_int_vect(current);
	/* font tables default location (int 1F) */
	MEM_WW(0x1f << 2, 0xfa6e);

	/* int 11 default location (Get Equipment Configuration) */
	MEM_WW(0x11 << 2, 0xf84d);
	/* int 12 default location (Get Conventional Memory Size) */
	MEM_WW(0x12 << 2, 0xf841);
	/* int 15 default location (I/O System Extensions) */
	MEM_WW(0x15 << 2, 0xf859);
	/* int 1A default location (RTC, PCI and others) */
	MEM_WW(0x1a << 2, 0xff6e);
	/* int 05 default location (Bound Exceeded) */
	MEM_WW(0x05 << 2, 0xff54);
	/* int 08 default location (Double Fault) */
	MEM_WW(0x08 << 2, 0xfea5);
	/* int 13 default location (Disk) */
	MEM_WW(0x13 << 2, 0xec59);
	/* int 0E default location (Page Fault) */
	MEM_WW(0x0e << 2, 0xef57);
	/* int 17 default location (Parallel Port) */
	MEM_WW(0x17 << 2, 0xefd2);
	/* fdd table default location (int 1e) */
	MEM_WW(0x1e << 2, 0xefc7);

	/* Set Equipment flag to VGA */
	i = MEM_RB(0x0410) & 0xCF;
	MEM_WB(0x0410, i);
	/* XXX Perhaps setup more of the BDA here.  See also int42(0x00). */
}
#endif

int setup_system_bios(void *base_addr)
{
	char *base = (char *) base_addr;

	/*
	 * we trap the "industry standard entry points" to the BIOS
	 * and all other locations by filling them with "hlt"
	 * TODO: implement hlt-handler for these
	 */
	memset(base, 0xf4, 0x10000);

	/* set bios date */
	strcpy(base + 0x0FFF5, "06/11/99");
	/* set up eisa ident string */
	strcpy(base + 0x0FFD9, "PCI_ISA");
	/* write system model id for IBM-AT */
	*((unsigned char *) (base + 0x0FFFE)) = 0xfc;

	return 1;
}

void reset_int_vect(void)
{
	/*
	 * This table is normally located at 0xF000:0xF0A4.  However, int 0x42,
	 * function 0 (Mode Set) expects it (or a copy) somewhere in the bottom
	 * 64kB.  Note that because this data doesn't survive POST, int 0x42 should
	 * only be used during EGA/VGA BIOS initialisation.
	 */
	static const unsigned char VideoParms[] = {
		/* Timing for modes 0x00 & 0x01 */
		0x38, 0x28, 0x2d, 0x0a, 0x1f, 0x06, 0x19, 0x1c,
		0x02, 0x07, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
		/* Timing for modes 0x02 & 0x03 */
		0x71, 0x50, 0x5a, 0x0a, 0x1f, 0x06, 0x19, 0x1c,
		0x02, 0x07, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
		/* Timing for modes 0x04, 0x05 & 0x06 */
		0x38, 0x28, 0x2d, 0x0a, 0x7f, 0x06, 0x64, 0x70,
		0x02, 0x01, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
		/* Timing for mode 0x07 */
		0x61, 0x50, 0x52, 0x0f, 0x19, 0x06, 0x19, 0x19,
		0x02, 0x0d, 0x0b, 0x0c, 0x00, 0x00, 0x00, 0x00,
		/* Display page lengths in little endian order */
		0x00, 0x08,	/* Modes 0x00 and 0x01 */
		0x00, 0x10,	/* Modes 0x02 and 0x03 */
		0x00, 0x40,	/* Modes 0x04 and 0x05 */
		0x00, 0x40,	/* Modes 0x06 and 0x07 */
		/* Number of columns for each mode */
		40, 40, 80, 80, 40, 40, 80, 80,
		/* CGA Mode register value for each mode */
		0x2c, 0x28, 0x2d, 0x29, 0x2a, 0x2e, 0x1e, 0x29,
		/* Padding */
		0x00, 0x00, 0x00, 0x00
	};
	int i;

	for (i = 0; i < sizeof(VideoParms); i++)
		MEM_WB(i + (0x1000 - sizeof(VideoParms)), VideoParms[i]);
	MEM_WW(0x1d << 2, 0x1000 - sizeof(VideoParms));
	MEM_WW((0x1d << 2) + 2, 0);

	printf("SETUP INT\n");
	MEM_WW(0x10 << 2, 0xf065);
	MEM_WW((0x10 << 2) + 2, SYS_BIOS >> 4);
	MEM_WW(0x42 << 2, 0xf065);
	MEM_WW((0x42 << 2) + 2, SYS_BIOS >> 4);
	MEM_WW(0x6D << 2, 0xf065);
	MEM_WW((0x6D << 2) + 2, SYS_BIOS >> 4);
}

void set_return_trap(void)
{
	/*
	 * Here we set the exit condition:  We return when we encounter
	 * 'hlt' (=0xf4), which we locate at address 0x600 in x86 memory.
	 */
	MEM_WB(0x0600, 0xf4);

	/*
	 * Allocate a segment for the stack
	 */
	xf86Int10AllocPages(1, current->stackseg);
}

void *xf86HandleInt10Options(ScrnInfoPtr pScrn, int entityIndex)
{
	EntityInfoPtr pEnt = xf86GetEntityInfo(entityIndex);
	OptionInfoPtr options = NULL;

	if (pEnt->device) {
		pointer configOptions = NULL;

		/* Check if xf86CollectOptions() has already been called */
		if (((pEnt->index < 0) ||
		     !xf86Screens[pEnt->index] ||
		     !(configOptions = xf86Screens[pEnt->index]->options)) &&
		    pEnt->device)
			configOptions = pEnt->device->options;

		if (configOptions) {
			if (!(options = (OptionInfoPtr) xalloc(sizeof(INT10Options))))
				return NULL;

			(void) memcpy(options, INT10Options, sizeof(INT10Options));
			xf86ProcessOptions(pScrn->scrnIndex, configOptions, options);
		}
	}
	xfree(pEnt);

	return options;
}

Bool int10skip(void *options)
{
	Bool noint10 = FALSE;

	if (!options)
		return FALSE;

	xf86GetOptValBool(options, OPT_NOINT10, &noint10);
	return noint10;
}

Bool int10_check_bios(int scrnIndex, int codeSeg, unsigned char *vbiosMem)
{
	int size;

	if ((codeSeg & 0x1f) ||	/* Not 512-byte aligned otherwise */
	    ((codeSeg << 4) < V_BIOS) || ((codeSeg << 4) >= SYS_SIZE))
		return FALSE;

	if (xf86IsPc98())
		return FALSE;

	if ((*vbiosMem != 0x55) || (*(vbiosMem + 1) != 0xAA) || !*(vbiosMem + 2))
		return FALSE;

	size = *(vbiosMem + 2) * 512;

	if ((size + (codeSeg << 4)) > SYS_SIZE)
		return FALSE;

	if (bios_checksum(vbiosMem, size))
		xf86DrvMsg(scrnIndex, X_WARNING, "Bad V_BIOS checksum\n");

	return TRUE;
}

Bool initPrimary(void *options)
{
	Bool initPrimary = FALSE;

	if (!options)
		return FALSE;

	xf86GetOptValBool(options, OPT_INIT_PRIMARY, &initPrimary);
	return initPrimary;
}

void xf86int10ParseBiosLocation(void *options, xf86int10BiosLocationPtr bios)
{
	char *s;
	char *p;
	char *str = NULL;

	if (options)
		str = xf86GetOptValString(options, OPT_BIOS_LOCATION);

	bios->bus = BUS_NONE;
	if (!str)
		return;

	s = xstrdup(str);
	p = strtok(s, ":");
	if (xf86NameCmp(p, "pci"))
		bios->bus = BUS_PCI;
	else if (xf86NameCmp(p, "primary"))
		bios->bus = BUS_ISA;

	xfree(s);

	if (bios->bus == BUS_NONE)
		return;

	s = xstrdup(str);
	p = strchr(s, ':');

	switch (bios->bus) {
	case BUS_ISA:
		if (p)
			bios->location.legacy = atoi(++p);
		else
			bios->location.legacy = 0;
		break;
	case BUS_PCI:
		if (p) {
			bios->location.pci.bus = atoi(++p);
			if ((p = strchr(p, ':'))) {
				bios->location.pci.dev = atoi(++p);
				if ((p = strchr(p, ':'))) {
					bios->location.pci.func = atoi(++p);
					break;
				}
			}
		}
		/* fall through */
		bios->bus = BUS_NONE;
		break;
	default:
		break;
	}
	xfree(s);
}


#endif
