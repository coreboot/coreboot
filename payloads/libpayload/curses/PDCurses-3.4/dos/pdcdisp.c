/* Public Domain Curses */

#include "pdcdos.h"

RCSID("$Id: pdcdisp.c,v 1.65 2008/07/13 16:08:17 wmcbrine Exp $")

/* ACS definitions originally by jshumate@wrdis01.robins.af.mil -- these
   match code page 437 and compatible pages (CP850, CP852, etc.) */

#ifdef CHTYPE_LONG

# define A(x) ((chtype)x | A_ALTCHARSET)

chtype acs_map[128] =
{
    A(0), A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8), A(9), A(10), 
    A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18), A(19), 
    A(20), A(21), A(22), A(23), A(24), A(25), A(26), A(27), A(28), 
    A(29), A(30), A(31), ' ', '!', '"', '#', '$', '%', '&', '\'', '(', 
    ')', '*',

    A(0x1a), A(0x1b), A(0x18), A(0x19),

    '/',

    0xdb,

    '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', 
    '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',

    A(0x04), 0xb1,

    'b', 'c', 'd', 'e',

    0xf8, 0xf1, 0xb0, A(0x0f), 0xd9, 0xbf, 0xda, 0xc0, 0xc5, 0x2d, 0x2d, 
    0xc4, 0x2d, 0x5f, 0xc3, 0xb4, 0xc1, 0xc2, 0xb3, 0xf3, 0xf2, 0xe3, 
    0xd8, 0x9c, 0xf9,

    A(127)
};

# undef A

#endif

#ifdef __PACIFIC__
void movedata(unsigned sseg, unsigned soff, unsigned dseg,
              unsigned doff, unsigned n)
{
    far char *src = MK_FP(sseg, soff);
    far char *dst = MK_FP(dseg, doff);

    while (n--)
        *dst++ = *src++;
}
#endif

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    PDCREGS regs;

    PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

    regs.h.ah = 0x02;
    regs.h.bh = 0;
    regs.h.dh = (unsigned char) row;
    regs.h.dl = (unsigned char) col;
    PDCINT(0x10, regs);
}

/* update the given physical line to look like the corresponding line in 
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    int j;

    PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

    if (pdc_direct_video)
    {
#if SMALL || MEDIUM
# ifndef __PACIFIC__
        struct SREGS segregs;
# endif
        int ds;
#endif
        /* this should be enough for the maximum width of a screen */

        struct {unsigned char text, attr;} temp_line[256];

        /* replace the attribute part of the chtype with the actual 
           color value for each chtype in the line */

        for (j = 0; j < len; j++)
        {
            chtype ch = srcp[j];

            temp_line[j].attr = pdc_atrtab[ch >> PDC_ATTR_SHIFT];
#ifdef CHTYPE_LONG
            if (ch & A_ALTCHARSET && !(ch & 0xff80))
                ch = acs_map[ch & 0x7f];
#endif
            temp_line[j].text = ch & 0xff;
        }

#ifdef __DJGPP__
        dosmemput(temp_line, len * 2,
                  (unsigned long)_FAR_POINTER(pdc_video_seg,
                  pdc_video_ofs + (lineno * curscr->_maxx + x) * 2));
#else
# if SMALL || MEDIUM
#  ifdef __PACIFIC__
        ds = FP_SEG((void far *) temp_line);
#  else
        segread(&segregs);
        ds = segregs.ds;
#  endif
        movedata(ds, (int)temp_line, pdc_video_seg,
                 pdc_video_ofs + (lineno * curscr->_maxx + x) * 2, len * 2);
# else
        memcpy((void *)_FAR_POINTER(pdc_video_seg,
               pdc_video_ofs + (lineno * curscr->_maxx + x) * 2),
               temp_line, len * 2);
# endif
#endif

    }
    else
        for (j = 0; j < len;)
        {
            PDCREGS regs;
            unsigned short count = 1;
            chtype ch = srcp[j];

            while ((j + count < len) && (ch == srcp[j + count]))
                count++;

            PDC_gotoyx(lineno, j + x);

            regs.h.ah = 0x09;
            regs.W.bx = pdc_atrtab[ch >> PDC_ATTR_SHIFT];
            regs.W.cx = count;
#ifdef CHTYPE_LONG
            if (ch & A_ALTCHARSET && !(ch & 0xff80))
                ch = acs_map[ch & 0x7f];
#endif
            regs.h.al = (unsigned char) (ch & 0xff);

            PDCINT(0x10, regs);

            j += count;
        }
}
