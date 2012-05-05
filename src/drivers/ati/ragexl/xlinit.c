/*
 *  ATI Rage XL Initialization. Support for Xpert98 and Victoria
 *  PCI cards.
 *
 *  Copyright (C) 2002 MontaVista Software Inc.
 *  Author: MontaVista Software, Inc.
 *         	stevel@mvista.com or source@mvista.com
 *  Copyright (C) 2004 Tyan Computer.
 *  Auther: Yinghai Lu   yhlu@tyan.com
 *	   move to coreboot
 * This code is distributed without warranty under the GPL v2 (see COPYING) *
 */
#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

// FIXME BTEXT console within coreboot has been obsoleted
// and will go away. The BTEXT code in this file should be
// fixed to export a framebuffer console through the coreboot
// table (and possibly make it available for bootsplash use)
// Hence do only remove this if you fix the code.
#define CONFIG_CONSOLE_BTEXT 0

#if CONFIG_CONSOLE_BTEXT

#define PLL_CRTC_DECODE 0
#define SUPPORT_8_BPP_ABOVE 0

#include "fb.h"
#include "fbcon.h"

struct aty_cmap_regs {
	u8 windex;
	u8 lut;
	u8 mask;
	u8 rindex;
	u8 cntl;
};

#include <console/btext.h>

#endif /*CONFIG_CONSOLE_BTEXT*/

#include "mach64.h"

#include "atyfb.h"

#include "mach64_ct.c"

#define MPLL_GAIN       0xad
#define VPLL_GAIN       0xd5

#define HAS_VICTORIA 0

enum {
#if HAS_VICTORIA==1
	VICTORIA = 0,
	XPERT98,
#else
	XPERT98=0,
#endif
	NUM_XL_CARDS
};

//static struct aty_pll_ops aty_pll_ct;

#define DEFAULT_CARD XPERT98
static int xl_card = DEFAULT_CARD;

static const struct xl_card_cfg_t {
	int ref_crystal; // 10^4 Hz
	int mem_type;
	int mem_size;
	u32 mem_cntl;
	u32 ext_mem_cntl;
	u32 mem_addr_config;
	u32 bus_cntl;
	u32 dac_cntl;
	u32 hw_debug;
	u32 custom_macro_cntl;
	u8  dll2_cntl;
	u8  pll_yclk_cntl;
} card_cfg[NUM_XL_CARDS] = {
#if HAS_VICTORIA==1
	// VICTORIA
	{	2700, SDRAM, 0x800000,
		0x10757A3B, 0x64000C81, 0x00110202, 0x7b33A040,
		0x82010102, 0x48803800, 0x005E0179,
		0x50, 0x25
	},
#endif
	// XPERT98
	{	1432,  WRAM, 0x800000,
		0x00165A2B, 0xE0000CF1, 0x00200213, 0x7333A001,
		0x8000000A, 0x48833800, 0x007F0779,
		0x10, 0x19
	}
};

typedef struct {
	u8 lcd_reg;
	u32 val;
} lcd_tbl_t;

static const lcd_tbl_t lcd_tbl[] = {
	{ 0x01,	0x000520C0 },
	{ 0x08,	0x02000408 },
	{ 0x03,	0x00000F00 },
	{ 0x00,	0x00000000 },
	{ 0x02,	0x00000000 },
	{ 0x04,	0x00000000 },
	{ 0x05,	0x00000000 },
	{ 0x06,	0x00000000 },
	{ 0x33,	0x00000000 },
	{ 0x34,	0x00000000 },
	{ 0x35,	0x00000000 },
	{ 0x36,	0x00000000 },
	{ 0x37,	0x00000000 }
};

static inline u32 aty_ld_lcd(u8 lcd_reg, struct fb_info_aty *info)
{
	aty_st_8(LCD_INDEX, lcd_reg, info);
	return aty_ld_le32(LCD_DATA, info);
}

static inline void aty_st_lcd(u8 lcd_reg, u32 val,
			      struct fb_info_aty *info)
{
	aty_st_8(LCD_INDEX, lcd_reg, info);
	aty_st_le32(LCD_DATA, val, info);
}

static void reset_gui(struct fb_info_aty *info)
{
	aty_st_8(GEN_TEST_CNTL+1, 0x01, info);
	aty_st_8(GEN_TEST_CNTL+1, 0x00, info);
	aty_st_8(GEN_TEST_CNTL+1, 0x02, info);
	mdelay(5);
}


static void reset_sdram(struct fb_info_aty *info)
{
	u8 temp;

	temp = aty_ld_8(EXT_MEM_CNTL, info);
	temp |= 0x02;
	aty_st_8(EXT_MEM_CNTL, temp, info); // MEM_SDRAM_RESET = 1b
	temp |= 0x08;
	aty_st_8(EXT_MEM_CNTL, temp, info); // MEM_CYC_TEST    = 10b
	temp |= 0x0c;
	aty_st_8(EXT_MEM_CNTL, temp, info); // MEM_CYC_TEST    = 11b
	mdelay(5);
	temp &= 0xf3;
	aty_st_8(EXT_MEM_CNTL, temp, info); // MEM_CYC_TEST    = 00b
	temp &= 0xfd;
	aty_st_8(EXT_MEM_CNTL, temp, info); // MEM_SDRAM_REST  = 0b
	mdelay(5);
}

static void init_dll(struct fb_info_aty *info)
{
	// enable DLL
	aty_st_pll(PLL_GEN_CNTL,
		   aty_ld_pll(PLL_GEN_CNTL, info) & 0x7f,
		   info);

	// reset DLL
	aty_st_pll(DLL_CNTL, 0x82, info);
	aty_st_pll(DLL_CNTL, 0xE2, info);
	mdelay(5);
	aty_st_pll(DLL_CNTL, 0x82, info);
	mdelay(6);
}

static void reset_clocks(struct fb_info_aty *info, struct pll_ct *pll,
			 int hsync_enb)
{
	reset_gui(info);
	aty_st_pll(MCLK_FB_DIV, pll->mclk_fb_div, info);
	aty_st_pll(SCLK_FB_DIV, pll->sclk_fb_div, info);

	mdelay(15);
	init_dll(info);
	aty_st_8(GEN_TEST_CNTL+1, 0x00, info);
	mdelay(5);
	aty_st_8(CRTC_GEN_CNTL+3, 0x04, info);
	mdelay(6);
	reset_sdram(info);
	aty_st_8(CRTC_GEN_CNTL+3,
		 hsync_enb ? 0x00 : 0x04, info);

	aty_st_pll(SPLL_CNTL2, pll->spll_cntl2, info);
	aty_st_pll(PLL_GEN_CNTL, pll->pll_gen_cntl, info);
	aty_st_pll(PLL_VCLK_CNTL, pll->pll_vclk_cntl, info);
}

static int atyfb_xl_init(struct fb_info_aty *info)
{
	int i, err;
	u32 temp;
	union aty_pll pll;
	const struct xl_card_cfg_t * card = &card_cfg[xl_card];

	aty_st_8(CONFIG_STAT0, 0x85, info);
	mdelay(10);

	/*
	 * The following needs to be set before the call
	 * to var_to_pll() below. They'll be re-set again
	 * to the same values in aty_init().
	 */
	info->ref_clk_per = 100000000UL/card->ref_crystal;
	info->ram_type = card->mem_type;
	info->total_vram = card->mem_size;
#if HAS_VICTORIA == 1
	if (xl_card == VICTORIA) {
		// the MCLK, XCLK are 120MHz on victoria card
		info->mclk_per = 1000000/120;
		info->xclk_per = 1000000/120;
		info->features &= ~M64F_MFB_TIMES_4;
	}
#endif

	/*
	 * Calculate mclk and xclk dividers, etc. The passed
	 * pixclock and bpp values don't matter yet, the vclk
	 * isn't programmed until later.
	 */
	if ((err = aty_var_to_pll_ct(info, 39726, 8, &pll))) return err;
//        if ((err = aty_pll_ct.var_to_pll(info, 39726, 8, &pll))) return err;


	aty_st_pll(LVDS_CNTL0, 0x00, info);
	aty_st_pll(DLL2_CNTL, card->dll2_cntl, info);
	aty_st_pll(V2PLL_CNTL, 0x10, info);
	aty_st_pll(MPLL_CNTL, MPLL_GAIN, info);
	aty_st_pll(VPLL_CNTL, VPLL_GAIN, info);
	aty_st_pll(PLL_VCLK_CNTL, 0x00, info);
	aty_st_pll(VFC_CNTL, 0x1B, info);
	aty_st_pll(PLL_REF_DIV, pll.ct.pll_ref_div, info);
	aty_st_pll(PLL_EXT_CNTL, pll.ct.pll_ext_cntl, info);
	aty_st_pll(SPLL_CNTL2, 0x03, info);
	aty_st_pll(PLL_GEN_CNTL, 0x44, info);

	reset_clocks(info, &pll.ct, 0);
	mdelay(10);

	aty_st_pll(VCLK_POST_DIV, 0x03, info);
	aty_st_pll(VCLK0_FB_DIV, 0xDA, info);
	aty_st_pll(VCLK_POST_DIV, 0x0F, info);
	aty_st_pll(VCLK1_FB_DIV, 0xF5, info);
	aty_st_pll(VCLK_POST_DIV, 0x3F, info);
	aty_st_pll(PLL_EXT_CNTL, 0x40 | pll.ct.pll_ext_cntl, info);
	aty_st_pll(VCLK2_FB_DIV, 0x00, info);
	aty_st_pll(VCLK_POST_DIV, 0xFF, info);
	aty_st_pll(PLL_EXT_CNTL, 0xC0 | pll.ct.pll_ext_cntl, info);
	aty_st_pll(VCLK3_FB_DIV, 0x00, info);

	aty_st_8(BUS_CNTL, 0x01, info);
	aty_st_le32(BUS_CNTL, card->bus_cntl | 0x08000000, info);

	aty_st_le32(CRTC_GEN_CNTL, 0x04000200, info);
	aty_st_le16(CONFIG_STAT0, 0x0020, info);
	aty_st_le32(MEM_CNTL, 0x10151A33, info);
	aty_st_le32(EXT_MEM_CNTL, 0xE0000C01, info);
	aty_st_le16(CRTC_GEN_CNTL+2, 0x0000, info);
	aty_st_le32(DAC_CNTL, card->dac_cntl, info);
	aty_st_le16(GEN_TEST_CNTL, 0x0100, info);
	aty_st_le32(CUSTOM_MACRO_CNTL, 0x003C0171, info);
	aty_st_le32(MEM_BUF_CNTL, 0x00382848, info);

	aty_st_le32(HW_DEBUG, card->hw_debug, info);
	aty_st_le16(MEM_ADDR_CONFIG, 0x0000, info);
	aty_st_le16(GP_IO+2, 0x0000, info);
	aty_st_le16(GEN_TEST_CNTL, 0x0000, info);
	aty_st_le16(EXT_DAC_REGS+2, 0x0000, info);
	aty_st_le32(CRTC_INT_CNTL, 0x00000000, info);
	aty_st_le32(TIMER_CONFIG, 0x00000000, info);
	aty_st_le32(0xEC, 0x00000000, info);
	aty_st_le32(0xFC, 0x00000000, info);

	for (i=0; i<ARRAY_SIZE(lcd_tbl); i++) {
		aty_st_lcd(lcd_tbl[i].lcd_reg, lcd_tbl[i].val, info);
	}

	aty_st_le16(CONFIG_STAT0, 0x00A4, info);
	mdelay(10);

	aty_st_8(BUS_CNTL+1, 0xA0, info);
	mdelay(10);

	reset_clocks(info, &pll.ct, 1);
	mdelay(10);

	// something about power management
	aty_st_8(LCD_INDEX, 0x08, info);
	aty_st_8(LCD_DATA, 0x0A, info);
	aty_st_8(LCD_INDEX, 0x08, info);
	aty_st_8(LCD_DATA+3, 0x02, info);
	aty_st_8(LCD_INDEX, 0x08, info);
	aty_st_8(LCD_DATA, 0x0B, info);
	mdelay(2);

	// enable display requests, enable CRTC
	aty_st_8(CRTC_GEN_CNTL+3, 0x02, info);
	// disable display
	aty_st_8(CRTC_GEN_CNTL, 0x40, info);
	// disable display requests, disable CRTC
	aty_st_8(CRTC_GEN_CNTL+3, 0x04, info);
	mdelay(10);

	aty_st_pll(PLL_YCLK_CNTL, 0x25, info);

	aty_st_le16(CUSTOM_MACRO_CNTL, 0x0179, info);
	aty_st_le16(CUSTOM_MACRO_CNTL+2, 0x005E, info);
	aty_st_le16(CUSTOM_MACRO_CNTL+2, card->custom_macro_cntl>>16, info);
	aty_st_8(CUSTOM_MACRO_CNTL+1,
		 (card->custom_macro_cntl>>8) & 0xff, info);

	aty_st_le32(MEM_ADDR_CONFIG, card->mem_addr_config, info);
	aty_st_le32(MEM_CNTL, card->mem_cntl, info);
	aty_st_le32(EXT_MEM_CNTL, card->ext_mem_cntl, info);

	aty_st_8(CONFIG_STAT0, 0xA0 | card->mem_type, info);

	aty_st_pll(PLL_YCLK_CNTL, 0x01, info);
	mdelay(15);
	aty_st_pll(PLL_YCLK_CNTL, card->pll_yclk_cntl, info);
	mdelay(1);

	reset_clocks(info, &pll.ct, 0);
	mdelay(50);
	reset_clocks(info, &pll.ct, 0);
	mdelay(50);

	// enable extended register block
	aty_st_8(BUS_CNTL+3, 0x7B, info);
	mdelay(1);
	// disable extended register block
	aty_st_8(BUS_CNTL+3, 0x73, info);

	aty_st_8(CONFIG_STAT0, 0x80 | card->mem_type, info);

	// disable display requests, disable CRTC
	aty_st_8(CRTC_GEN_CNTL+3, 0x04, info);
	// disable mapping registers in VGA aperture
	aty_st_8(CONFIG_CNTL, aty_ld_8(CONFIG_CNTL, info) & ~0x04, info);
	mdelay(50);
	// enable display requests, enable CRTC
	aty_st_8(CRTC_GEN_CNTL+3, 0x02, info);

	// make GPIO's 14,15,16 all inputs
	aty_st_8(LCD_INDEX, 0x07, info);
	aty_st_8(LCD_DATA+3, 0x00, info);

	// enable the display
	aty_st_8(CRTC_GEN_CNTL, 0x00, info);
	mdelay(17);
	// reset the memory controller
	aty_st_8(GEN_TEST_CNTL+1, 0x02, info);
	mdelay(15);
	aty_st_8(GEN_TEST_CNTL+1, 0x00, info);
	mdelay(30);

	// enable extended register block
	aty_st_8(BUS_CNTL+3,
		 (u8)(aty_ld_8(BUS_CNTL+3, info) | 0x08),
		 info);
	// set FIFO size to 512 (PIO)
	aty_st_le32(GUI_CNTL,
		    aty_ld_le32(GUI_CNTL, info) & ~0x3,
		    info);

	// enable CRT and disable lcd
	aty_st_8(LCD_INDEX, 0x01, info);
	temp = aty_ld_le32(LCD_DATA, info);
	temp = (temp | 0x01) & ~0x02;
	aty_st_le32(LCD_DATA, temp, info);

	return 0;
}

static char m64n_xl_33[] = "3D RAGE (XL PCI-33MHz)";
static char m64n_xl_66[] = "3D RAGE (XL PCI-66MHz)";


#if CONFIG_CONSOLE_BTEXT
static void aty_set_crtc(const struct fb_info_aty *info,
                         const struct crtc *crtc);
static int aty_var_to_crtc(const struct fb_info_aty *info,
                           const struct fb_var_screeninfo *var,
                           struct crtc *crtc);
#if PLL_CRTC_DECODE==1
static int aty_crtc_to_var(const struct crtc *crtc,
                           struct fb_var_screeninfo *var);
#endif

static void atyfb_set_par(const struct atyfb_par *par,
                          struct fb_info_aty *info);
static int atyfb_decode_var(const struct fb_var_screeninfo *var,
                            struct atyfb_par *par,
                            const struct fb_info_aty *info);
#if PLL_CRTC_DECODE==1
static int atyfb_encode_var(struct fb_var_screeninfo *var,
                            const struct atyfb_par *par,
                            const struct fb_info_aty *info);
#endif

static void do_install_cmap(int con, struct fb_info_aty *info);

#if 0
static u32 default_vram  = 0;
#endif

unsigned char color_table[] = { 0, 4, 2, 6, 1, 5, 3, 7,
                                       8,12,10,14, 9,13,11,15 };
#if 0
/* the default colour table, for VGA+ colour systems */
int default_red[] = {0x00,0xaa,0x00,0xaa,0x00,0xaa,0x00,0xaa,
    0x55,0xff,0x55,0xff,0x55,0xff,0x55,0xff};
int default_grn[] = {0x00,0x00,0xaa,0x55,0x00,0x00,0xaa,0xaa,
    0x55,0x55,0xff,0xff,0x55,0x55,0xff,0xff};
int default_blu[] = {0x00,0x00,0x00,0x00,0xaa,0xaa,0xaa,0xaa,
    0x55,0x55,0x55,0x55,0xff,0xff,0xff,0xff};
#endif

struct fb_var_screeninfo default_var = {
	/* 640x480, 60 Hz, Non-Interlaced (25.175 MHz dotclock) */
	640, 480, 640, 480, 0, 0, 8, 0,
	{0, 8, 0}, {0, 8, 0}, {0, 8, 0}, {0, 0, 0},
	0, 0, -1, -1, 0, 39722, 48, 16, 33, 10, 96, 2,
	0, FB_VMODE_NONINTERLACED
};

#endif /*CONFIG_CONSOLE_BTEXT*/

static struct {
	u16 pci_id, chip_type;
	u8 rev_mask, rev_val;
	const char *name;
	int pll, mclk, xclk;
	u32 features;
} aty_chips[] = {
	/* 3D RAGE XL PCI-66/BGA */
	{ 0x474f, 0x474f, 0x00, 0x00, m64n_xl_66, 230, 83, 63, M64F_GT | M64F_INTEGRATED | M64F_RESET_3D | M64F_GTB_DSP | M64F_SDRAM_MAGIC_PLL | M64F_EXTRA_BRIGHT | M64F_XL_DLL | M64F_MFB_TIMES_4 },
	/* 3D RAGE XL PCI-33/BGA */
	{ 0x4752, 0x4752, 0x00, 0x00, m64n_xl_33, 230, 83, 63, M64F_GT | M64F_INTEGRATED | M64F_RESET_3D | M64F_GTB_DSP | M64F_SDRAM_MAGIC_PLL | M64F_EXTRA_BRIGHT | M64F_XL_DLL | M64F_MFB_TIMES_4 },
};
#if CONFIG_CONSOLE_BTEXT
static void aty_calc_mem_refresh(struct fb_info_aty *info, u16 id, int xclk)
{
        int i, size;
#if 0
        const int ragepro_tbl[] = {
                44, 50, 55, 66, 75, 80, 100
        };
#endif
        const int ragexl_tbl[] = {
                50, 66, 75, 83, 90, 95, 100, 105,
                110, 115, 120, 125, 133, 143, 166
        };
        const int *refresh_tbl;
#if 0
        if (IS_XL(id)) {
#endif
                refresh_tbl = ragexl_tbl;
                size = sizeof(ragexl_tbl)/sizeof(int);
#if 0
        } else {
                refresh_tbl = ragepro_tbl;
                size = sizeof(ragepro_tbl)/sizeof(int);
        }
#endif

        for (i=0; i < size; i++) {
                if (xclk < refresh_tbl[i])
                        break;
        }

        info->mem_refresh_rate = i;
}
#endif /*CONFIG_CONSOLE_BTEXT */
static void ati_ragexl_init(device_t dev)
{
        u32 chip_id;
    	int j;
    	u16 type;
        u8 rev;
    	const char *chipname = NULL;
#if CONFIG_CONSOLE_BTEXT
	u32 i;
	const char *xtal;
#endif
    	int pll, mclk, xclk;

#if CONFIG_CONSOLE_BTEXT

#if 0
	int gtb_memsize, k;
#endif

        struct fb_var_screeninfo var;
#if 0
        struct display *disp;
#endif

#if 0
	u8 pll_ref_div;
#endif

#endif /*CONFIG_CONSOLE_BTEXT==1 */

        struct fb_info_aty *info;
        struct fb_info_aty info_t;
        struct resource *res;
        info = &info_t;

#define USE_AUX_REG 1


	res = dev->resource_list;
	if(res->flags & IORESOURCE_IO) {
		res = res->next;
	}

#if CONFIG_CONSOLE_BTEXT
	info->frame_buffer = res->base;
#endif /* CONFIG_CONSOLE_BTEXT */

#if USE_AUX_REG==0
        info->ati_regbase = res->base+0x7ff000+0xc00;
#else
	/* Fix this to look for the correct index. */
	//if (dev->resource_list && dev->resource_list->next)
        res = dev->resource_list->next->next;
        if(res->flags & IORESOURCE_MEM) {
                info->ati_regbase = res->base+0x400; //using auxiliary register
        }

#endif

#if CONFIG_CONSOLE_BTEXT
        info->aty_cmap_regs = (struct aty_cmap_regs *)(info->ati_regbase+0xc0);
#endif

#if 0
	printk(BIOS_DEBUG, "ati_regbase = 0x%08x, frame_buffer = 0x%08x\n", info->ati_regbase, info->frame_buffer);
#endif

    	chip_id = aty_ld_le32(CONFIG_CHIP_ID, info);
    	type = chip_id & CFG_CHIP_TYPE;
    	rev = (chip_id & CFG_CHIP_REV)>>24;
    	for (j = 0; j < ARRAY_SIZE(aty_chips); j++)
        	if (type == aty_chips[j].chip_type &&
            		(rev & aty_chips[j].rev_mask) == aty_chips[j].rev_val) {
            		chipname = aty_chips[j].name;
            		pll = aty_chips[j].pll;
            		mclk = aty_chips[j].mclk;
            		xclk = aty_chips[j].xclk;
            		info->features = aty_chips[j].features;
            		goto found;
        }
    	printk(BIOS_SPEW, "ati_ragexl_init: Unknown mach64 0x%04x rev 0x%04x\n", type, rev);
    	return ;

found:
    	printk(BIOS_INFO, "ati_ragexl_init: %s [0x%04x rev 0x%02x]\n", chipname, type, rev);
#if 0
    	if (M64_HAS(INTEGRATED)) {
        	/* for many chips, the mclk is 67 MHz for SDRAM, 63 MHz otherwise */
        	if (mclk == 67 && info->ram_type < SDRAM)
            		mclk = 63;
    	}
#endif
#if CONFIG_CONSOLE_BTEXT
        aty_calc_mem_refresh(info, type, xclk);
#endif /* CONFIG_CONSOLE_BTEXT */

    	info->pll_per = 1000000/pll;
    	info->mclk_per = 1000000/mclk;
    	info->xclk_per = 1000000/xclk;

//        info->dac_ops = &aty_dac_ct;
//        info->pll_ops = &aty_pll_ct;
        info->bus_type = PCI;


	atyfb_xl_init(info);

#if CONFIG_CONSOLE_BTEXT

	info->ram_type = (aty_ld_le32(CONFIG_STAT0, info) & 0x07);

        info->ref_clk_per = 1000000000000ULL/14318180;
    	xtal = "14.31818";
#if 0
    	if (M64_HAS(GTB_DSP) && (pll_ref_div = aty_ld_pll(PLL_REF_DIV, info))) {
        	int diff1, diff2;
        	diff1 = 510*14/pll_ref_div-pll;
        	diff2 = 510*29/pll_ref_div-pll;
        	if (diff1 < 0)
            		diff1 = -diff1;
        	if (diff2 < 0)
            		diff2 = -diff2;
        	if (diff2 < diff1) {
            		info->ref_clk_per = 1000000000000ULL/29498928;
            		xtal = "29.498928";
        	}
    	}
#endif

    i = aty_ld_le32(MEM_CNTL, info);
#if 0
    gtb_memsize = M64_HAS(GTB_DSP);
    if (gtb_memsize)  // We have
#endif
        switch (i & 0xF) {      /* 0xF used instead of MEM_SIZE_ALIAS */
            case MEM_SIZE_512K:
                info->total_vram = 0x80000;
                break;
            case MEM_SIZE_1M:
                info->total_vram = 0x100000;
                break;
            case MEM_SIZE_2M_GTB:
                info->total_vram = 0x200000;
                break;
            case MEM_SIZE_4M_GTB:
                info->total_vram = 0x400000;
                break;
            case MEM_SIZE_6M_GTB:
                info->total_vram = 0x600000;
                break;
            case MEM_SIZE_8M_GTB:
                info->total_vram = 0x800000;
                break;
            default:
                info->total_vram = 0x80000;
        }
#if 0
    else
        switch (i & MEM_SIZE_ALIAS) {
            case MEM_SIZE_512K:
                info->total_vram = 0x80000;
                break;
            case MEM_SIZE_1M:
                info->total_vram = 0x100000;
                break;
            case MEM_SIZE_2M:
                info->total_vram = 0x200000;
                break;
            case MEM_SIZE_4M:
                info->total_vram = 0x400000;
                break;
            case MEM_SIZE_6M:
                info->total_vram = 0x600000;
                break;
            case MEM_SIZE_8M:
                info->total_vram = 0x800000;
                break;
            default:
                info->total_vram = 0x80000;
       }
#endif

    if (M64_HAS(MAGIC_VRAM_SIZE)) {
        if (aty_ld_le32(CONFIG_STAT1, info) & 0x40000000)
          info->total_vram += 0x400000;
    }
#if 0
    if (default_vram) {
        info->total_vram = default_vram*1024;
        i = i & ~(gtb_memsize ? 0xF : MEM_SIZE_ALIAS);
        if (info->total_vram <= 0x80000)
            i |= MEM_SIZE_512K;
        else if (info->total_vram <= 0x100000)
            i |= MEM_SIZE_1M;
        else if (info->total_vram <= 0x200000)
            i |= gtb_memsize ? MEM_SIZE_2M_GTB : MEM_SIZE_2M;
        else if (info->total_vram <= 0x400000)
            i |= gtb_memsize ? MEM_SIZE_4M_GTB : MEM_SIZE_4M;
        else if (info->total_vram <= 0x600000)
            i |= gtb_memsize ? MEM_SIZE_6M_GTB : MEM_SIZE_6M;
        else
            i |= gtb_memsize ? MEM_SIZE_8M_GTB : MEM_SIZE_8M;
        aty_st_le32(MEM_CNTL, i, info);
    }
#endif

    /* Clear the video memory */
//    fb_memset((void *)info->frame_buffer, 0, info->total_vram);
#if 0
	disp = &info->disp;

//    strcpy(info->fb_info.modename, atyfb_name);
    info->fb_info.node = -1;
//    info->fb_info.fbops = &atyfb_ops;
    info->fb_info.disp = disp;
//    strcpy(info->fb_info.fontname, fontname);
    info->fb_info.changevar = NULL;
//    info->fb_info.switch_con = &atyfbcon_switch;
//    info->fb_info.updatevar = &atyfbcon_updatevar;
//    info->fb_info.blank = &atyfbcon_blank;
    info->fb_info.flags = FBINFO_FLAG_DEFAULT;
#endif
	var = default_var;

#if 0
    if (noaccel)  // We has noaccel in default
        var.accel_flags &= ~FB_ACCELF_TEXT;
    else
        var.accel_flags |= FB_ACCELF_TEXT;
#endif

    if (var.yres == var.yres_virtual) {
        u32 vram = info->total_vram ;
        var.yres_virtual = ((vram * 8) / var.bits_per_pixel) / var.xres_virtual;
        if (var.yres_virtual < var.yres)
                var.yres_virtual = var.yres;
    }

    if (atyfb_decode_var(&var, &info->default_par, info)) {
#if 0
        printk(BIOS_DEBUG, "atyfb: can't set default video mode\n");
#endif
        return ;
    }
#if 0
    for (j = 0; j < 16; j++) {
        k = color_table[j];
        info->palette[j].red = default_red[k];
        info->palette[j].green = default_grn[k];
        info->palette[j].blue = default_blu[k];
    }
#endif

#if 0
    if (curblink && M64_HAS(INTEGRATED)) {
        info->cursor = aty_init_cursor(info);
        if (info->cursor) {
            info->dispsw.cursor = atyfb_cursor;
            info->dispsw.set_font = atyfb_set_font;
        }
    }
#endif

#if PLL_CRTC_DECODE==1
	atyfb_set_var(&var, -1, &info->fb_info);
#else
            atyfb_set_par(&info->default_par, info);
//            do_install_cmap(-1, &info->fb_info);
	    do_install_cmap(-1, info);
#endif

#if PLL_CRTC_DECODE==1

    printk(BIOS_SPEW, "framebuffer=0x%08x, width=%d, height=%d, bpp=%d, pitch=%d\n",info->frame_buffer,
                         (((info->current_par.crtc.h_tot_disp>>16) & 0xff)+1)*8,
                         ((info->current_par.crtc.v_tot_disp>>16) & 0x7ff)+1,
                         info->current_par.crtc.bpp,
                         info->current_par.crtc.vxres*info->default_par.crtc.bpp/8
                        );
 btext_setup_display(
                         (((info->current_par.crtc.h_tot_disp>>16) & 0xff)+1)*8,
                         ((info->current_par.crtc.v_tot_disp>>16) & 0x7ff)+1,
                         info->current_par.crtc.bpp,
                         info->current_par.crtc.vxres*info->current_par.crtc.bpp/8,info->frame_buffer);
#else
    printk(BIOS_SPEW, "framebuffer=0x%08x, width=%d, height=%d, bpp=%d, pitch=%d\n",info->frame_buffer,
                         (((info->default_par.crtc.h_tot_disp>>16) & 0xff)+1)*8,
                         ((info->default_par.crtc.v_tot_disp>>16) & 0x7ff)+1,
                         info->default_par.crtc.bpp,
                         info->default_par.crtc.vxres*info->default_par.crtc.bpp/8
                        );
 btext_setup_display(
                         (((info->default_par.crtc.h_tot_disp>>16) & 0xff)+1)*8,
                         ((info->default_par.crtc.v_tot_disp>>16) & 0x7ff)+1,
                         info->default_par.crtc.bpp,
                         info->default_par.crtc.vxres*info->default_par.crtc.bpp/8,info->frame_buffer);
#endif

 btext_clearscreen();

 map_boot_text();

#if 0

 btext_drawstring("test framebuffer\n");

 mdelay(10000);
// test end
#endif

#endif /* CONFIG_CONSOLE_BTEXT */

}

#if CONFIG_CONSOLE_BTEXT

static int atyfb_decode_var(const struct fb_var_screeninfo *var,
                            struct atyfb_par *par,
                            const struct fb_info_aty *info)
{
    int err;

    if ((err = aty_var_to_crtc(info, var, &par->crtc)) ||
        (err = aty_var_to_pll_ct(info, var->pixclock, par->crtc.bpp,
                                         &par->pll)))
        return err;

#if 0
    if (var->accel_flags & FB_ACCELF_TEXT)
        par->accel_flags = FB_ACCELF_TEXT;
    else
#endif
        par->accel_flags = 0;

#if 0 /* fbmon is not done. uncomment for 2.5.x -brad */
    if (!fbmon_valid_timings(var->pixclock, htotal, vtotal, info))
        return -EINVAL;
#endif

    return 0;
}
#if PLL_CRTC_DECODE==1
static int atyfb_encode_var(struct fb_var_screeninfo *var,
                            const struct atyfb_par *par,
                            const struct fb_info_aty *info)
{
    int err;

    memset(var, 0, sizeof(struct fb_var_screeninfo));

    if ((err = aty_crtc_to_var(&par->crtc, var)))
        return err;
    var->pixclock = aty_pll_ct_to_var(info, &par->pll);

    var->height = -1;
    var->width = -1;
    var->accel_flags = par->accel_flags;

    return 0;
}
#endif
static void aty_set_crtc(const struct fb_info_aty *info,
                         const struct crtc *crtc)
{
    aty_st_le32(CRTC_H_TOTAL_DISP, crtc->h_tot_disp, info);
    aty_st_le32(CRTC_H_SYNC_STRT_WID, crtc->h_sync_strt_wid, info);
    aty_st_le32(CRTC_V_TOTAL_DISP, crtc->v_tot_disp, info);
    aty_st_le32(CRTC_V_SYNC_STRT_WID, crtc->v_sync_strt_wid, info);
    aty_st_le32(CRTC_VLINE_CRNT_VLINE, 0, info);
    aty_st_le32(CRTC_OFF_PITCH, crtc->off_pitch, info);
    aty_st_le32(CRTC_GEN_CNTL, crtc->gen_cntl, info);
}

static int aty_var_to_crtc(const struct fb_info_aty *info,
                           const struct fb_var_screeninfo *var,
                           struct crtc *crtc)
{
    u32 xres, yres, vxres, vyres, xoffset, yoffset, bpp;
    u32 left, right, upper, lower, hslen, vslen, sync, vmode;
    u32 h_total, h_disp, h_sync_strt, h_sync_dly, h_sync_wid, h_sync_pol;
    u32 v_total, v_disp, v_sync_strt, v_sync_wid, v_sync_pol, c_sync;
    u32 pix_width, dp_pix_width, dp_chain_mask;

    /* input */
    xres = var->xres;
    yres = var->yres;
    vxres = var->xres_virtual;
    vyres = var->yres_virtual;
    xoffset = var->xoffset;
    yoffset = var->yoffset;
    bpp = var->bits_per_pixel;
    left = var->left_margin;
    right = var->right_margin;
    upper = var->upper_margin;
    lower = var->lower_margin;
    hslen = var->hsync_len;
    vslen = var->vsync_len;
    sync = var->sync;
    vmode = var->vmode;

    /* convert (and round up) and validate */
    xres = (xres+7) & ~7;
    xoffset = (xoffset+7) & ~7;
    vxres = (vxres+7) & ~7;
    if (vxres < xres+xoffset)
        vxres = xres+xoffset;
    h_disp = xres/8-1;
    if (h_disp > 0xff)
        FAIL("h_disp too large");
    h_sync_strt = h_disp+(right/8);
    if (h_sync_strt > 0x1ff)
        FAIL("h_sync_start too large");
    h_sync_dly = right & 7;
    h_sync_wid = (hslen+7)/8;
    if (h_sync_wid > 0x1f)
        FAIL("h_sync_wid too large");
    h_total = h_sync_strt+h_sync_wid+(h_sync_dly+left+7)/8;
    if (h_total > 0x1ff)
     FAIL("h_total too large");
    h_sync_pol = sync & FB_SYNC_HOR_HIGH_ACT ? 0 : 1;

    if (vyres < yres+yoffset)
        vyres = yres+yoffset;
    v_disp = yres-1;
    if (v_disp > 0x7ff)
        FAIL("v_disp too large");
    v_sync_strt = v_disp+lower;
    if (v_sync_strt > 0x7ff)
        FAIL("v_sync_strt too large");
    v_sync_wid = vslen;
    if (v_sync_wid > 0x1f)
        FAIL("v_sync_wid too large");
    v_total = v_sync_strt+v_sync_wid+upper;
    if (v_total > 0x7ff)
        FAIL("v_total too large");
    v_sync_pol = sync & FB_SYNC_VERT_HIGH_ACT ? 0 : 1;

    c_sync = sync & FB_SYNC_COMP_HIGH_ACT ? CRTC_CSYNC_EN : 0;

    if (bpp <= 8) {
        bpp = 8;
        pix_width = CRTC_PIX_WIDTH_8BPP;
        dp_pix_width = HOST_8BPP | SRC_8BPP | DST_8BPP | BYTE_ORDER_LSB_TO_MSB;
        dp_chain_mask = 0x8080;
    }
#if SUPPORT_8_BPP_ABOVE==1
   else if (bpp <= 16) {
        bpp = 16;
        pix_width = CRTC_PIX_WIDTH_15BPP;
        dp_pix_width = HOST_15BPP | SRC_15BPP | DST_15BPP |
                       BYTE_ORDER_LSB_TO_MSB;
        dp_chain_mask = 0x4210;
    } else if (bpp <= 24 && M64_HAS(INTEGRATED)) {
        bpp = 24;
        pix_width = CRTC_PIX_WIDTH_24BPP;
        dp_pix_width = HOST_8BPP | SRC_8BPP | DST_8BPP | BYTE_ORDER_LSB_TO_MSB;
        dp_chain_mask = 0x8080;
    } else if (bpp <= 32) {
        bpp = 32;
        pix_width = CRTC_PIX_WIDTH_32BPP;
        dp_pix_width = HOST_32BPP | SRC_32BPP | DST_32BPP |
                       BYTE_ORDER_LSB_TO_MSB;
        dp_chain_mask = 0x8080;
    }
#endif
else
        FAIL("invalid bpp");

    if (vxres*vyres*bpp/8 > info->total_vram)
        FAIL("not enough video RAM");
  if ((vmode & FB_VMODE_MASK) != FB_VMODE_NONINTERLACED)
        FAIL("invalid vmode");

    /* output */
    crtc->vxres = vxres;
    crtc->vyres = vyres;
    crtc->xoffset = xoffset;
    crtc->yoffset = yoffset;
    crtc->bpp = bpp;
    crtc->h_tot_disp = h_total | (h_disp<<16);
    crtc->h_sync_strt_wid = (h_sync_strt & 0xff) | (h_sync_dly<<8) |
                            ((h_sync_strt & 0x100)<<4) | (h_sync_wid<<16) |
                            (h_sync_pol<<21);
    crtc->v_tot_disp = v_total | (v_disp<<16);
    crtc->v_sync_strt_wid = v_sync_strt | (v_sync_wid<<16) | (v_sync_pol<<21);
    crtc->off_pitch = ((yoffset*vxres+xoffset)*bpp/64) | (vxres<<19);
    crtc->gen_cntl = pix_width | c_sync | CRTC_EXT_DISP_EN | CRTC_ENABLE;
    if (M64_HAS(MAGIC_FIFO)) {
        /* Not VTB/GTB */
        /* FIXME: magic FIFO values */
        crtc->gen_cntl |= aty_ld_le32(CRTC_GEN_CNTL, info) & 0x000e0000;
    }
    crtc->dp_pix_width = dp_pix_width;
    crtc->dp_chain_mask = dp_chain_mask;

    return 0;
}
#if PLL_CRTC_DECODE==1
static int aty_crtc_to_var(const struct crtc *crtc,
                           struct fb_var_screeninfo *var)
{
    u32 xres, yres, bpp, left, right, upper, lower, hslen, vslen, sync;
    u32 h_total, h_disp, h_sync_strt, h_sync_dly, h_sync_wid, h_sync_pol;
    u32 v_total, v_disp, v_sync_strt, v_sync_wid, v_sync_pol, c_sync;
    u32 pix_width;

    /* input */
    h_total = crtc->h_tot_disp & 0x1ff;
    h_disp = (crtc->h_tot_disp>>16) & 0xff;
    h_sync_strt = (crtc->h_sync_strt_wid & 0xff) |
                  ((crtc->h_sync_strt_wid>>4) & 0x100);
    h_sync_dly = (crtc->h_sync_strt_wid>>8) & 0x7;
    h_sync_wid = (crtc->h_sync_strt_wid>>16) & 0x1f;
    h_sync_pol = (crtc->h_sync_strt_wid>>21) & 0x1;
    v_total = crtc->v_tot_disp & 0x7ff;
    v_disp = (crtc->v_tot_disp>>16) & 0x7ff;
    v_sync_strt = crtc->v_sync_strt_wid & 0x7ff;
    v_sync_wid = (crtc->v_sync_strt_wid>>16) & 0x1f;
    v_sync_pol = (crtc->v_sync_strt_wid>>21) & 0x1;
    c_sync = crtc->gen_cntl & CRTC_CSYNC_EN ? 1 : 0;
    pix_width = crtc->gen_cntl & CRTC_PIX_WIDTH_MASK;

    /* convert */
    xres = (h_disp+1)*8;
    yres = v_disp+1;
    left = (h_total-h_sync_strt-h_sync_wid)*8-h_sync_dly;
    right = (h_sync_strt-h_disp)*8+h_sync_dly;
    hslen = h_sync_wid*8;
    upper = v_total-v_sync_strt-v_sync_wid;
    lower = v_sync_strt-v_disp;
    vslen = v_sync_wid;
    sync = (h_sync_pol ? 0 : FB_SYNC_HOR_HIGH_ACT) |
           (v_sync_pol ? 0 : FB_SYNC_VERT_HIGH_ACT) |
           (c_sync ? FB_SYNC_COMP_HIGH_ACT : 0);

    switch (pix_width) {
#if 0
        case CRTC_PIX_WIDTH_4BPP:
            bpp = 4;
            var->red.offset = 0;
            var->red.length = 8;
            var->green.offset = 0;
            var->green.length = 8;
            var->blue.offset = 0;
            var->blue.length = 8;
            var->transp.offset = 0;
            var->transp.length = 0;
            break;
#endif
        case CRTC_PIX_WIDTH_8BPP:
            bpp = 8;
            var->red.offset = 0;
            var->red.length = 8;
            var->green.offset = 0;
            var->green.length = 8;
            var->blue.offset = 0;
            var->blue.length = 8;
            var->transp.offset = 0;
            var->transp.length = 0;
            break;
#if SUPPORT_8_BPP_ABOVE==1
        case CRTC_PIX_WIDTH_15BPP:      /* RGB 555 */
            bpp = 16;
            var->red.offset = 10;
            var->red.length = 5;
            var->green.offset = 5;
            var->green.length = 5;
            var->blue.offset = 0;
            var->blue.length = 5;
            var->transp.offset = 0;
            var->transp.length = 0;
            break;
        case CRTC_PIX_WIDTH_16BPP:      /* RGB 565 */
            bpp = 16;
            var->red.offset = 11;
            var->red.length = 5;
            var->green.offset = 5;
            var->green.length = 6;
            var->blue.offset = 0;
            var->blue.length = 5;
            var->transp.offset = 0;
            var->transp.length = 0;
            break;
        case CRTC_PIX_WIDTH_24BPP:      /* RGB 888 */
            bpp = 24;
            var->red.offset = 16;
            var->red.length = 8;
            var->green.offset = 8;
            var->green.length = 8;
            var->blue.offset = 0;
            var->blue.length = 8;
           var->transp.offset = 0;
            var->transp.length = 0;
            break;
        case CRTC_PIX_WIDTH_32BPP:      /* ARGB 8888 */
            bpp = 32;
            var->red.offset = 16;
            var->red.length = 8;
            var->green.offset = 8;
            var->green.length = 8;
            var->blue.offset = 0;
            var->blue.length = 8;
            var->transp.offset = 24;
            var->transp.length = 8;
            break;
#endif
        default:
            FAIL("Invalid pixel width");
    }

    /* output */
    var->xres = xres;
    var->yres = yres;
    var->xres_virtual = crtc->vxres;
    var->yres_virtual = crtc->vyres;
    var->bits_per_pixel = bpp;
    var->xoffset = crtc->xoffset;
    var->yoffset = crtc->yoffset;
    var->left_margin = left;
    var->right_margin = right;
    var->upper_margin = upper;
    var->lower_margin = lower;
    var->hsync_len = hslen;
    var->vsync_len = vslen;
    var->sync = sync;
    var->vmode = FB_VMODE_NONINTERLACED;

    return 0;
}
#endif

#if 0
static int encode_fix(struct fb_fix_screeninfo *fix,
                      const struct atyfb_par *par,
                      const struct fb_info_aty *info)
{
    memset(fix, 0, sizeof(struct fb_fix_screeninfo));

   // strcpy(fix->id, atyfb_name);
	 memcpy(fix->id, "atyfb", 5);
    fix->smem_start = info->frame_buffer;
    fix->smem_len = (u32)info->total_vram;

    /*
     *  Reg Block 0 (CT-compatible block) is at ati_regbase_phys
     *  Reg Block 1 (multimedia extensions) is at ati_regbase_phys-0x400
     */
    if (M64_HAS(GX)) {
        fix->mmio_start = info->ati_regbase;
        fix->mmio_len = 0x400;
        fix->accel = FB_ACCEL_ATI_MACH64GX;
    } else if (M64_HAS(CT)) {
        fix->mmio_start = info->ati_regbase;
        fix->mmio_len = 0x400;
        fix->accel = FB_ACCEL_ATI_MACH64CT;
    } else if (M64_HAS(VT)) {
        fix->mmio_start = info->ati_regbase-0x400;
        fix->mmio_len = 0x800;
        fix->accel = FB_ACCEL_ATI_MACH64VT;
    } else /* if (M64_HAS(GT)) */ {
        fix->mmio_start = info->ati_regbase-0x400;
        fix->mmio_len = 0x800;
        fix->accel = FB_ACCEL_ATI_MACH64GT;
    }
    fix->type = FB_TYPE_PACKED_PIXELS;
    fix->type_aux = 0;
    fix->line_length = par->crtc.vxres*par->crtc.bpp/8;
    fix->visual = par->crtc.bpp <= 8 ? FB_VISUAL_PSEUDOCOLOR
                                     : FB_VISUAL_DIRECTCOLOR;
    fix->ywrapstep = 0;
    fix->xpanstep = 8;
    fix->ypanstep = 1;

    return 0;
}
#endif
   /*
     *  Set the User Defined Part of the Display
     */
#if PLL_CRTC_DECODE==1
static int atyfb_set_var(struct fb_var_screeninfo *var, int con,
                         struct fb_info *fb)
{
    struct fb_info_aty *info = (struct fb_info_aty *)fb;
    struct atyfb_par par;
#if 0
    struct display *display;
    int oldxres, oldyres, oldvxres, oldvyres, oldbpp, oldaccel, accel;
#endif
    int err;
    int activate = var->activate;

#if 0
    if (con >= 0)
        display = &fb_display[con];
    else
#endif
#if 0
        display = fb->disp;     /* used during initialization */
#endif

    if ((err = atyfb_decode_var(var, &par, info)))
        return err;

    atyfb_encode_var(var, &par, (struct fb_info_aty *)info);

#if 0
    printk(BIOS_INFO, "atyfb_set_var: activate=%d\n", activate & FB_ACTIVATE_MASK);
#endif

    if ((activate & FB_ACTIVATE_MASK) == FB_ACTIVATE_NOW) {
#if 0
        oldxres = display->var.xres;
        oldyres = display->var.yres;
        oldvxres = display->var.xres_virtual;
        oldvyres = display->var.yres_virtual;
        oldbpp = display->var.bits_per_pixel;
        oldaccel = display->var.accel_flags;
        display->var = *var;
        accel = var->accel_flags & FB_ACCELF_TEXT;
        if (oldxres != var->xres || oldyres != var->yres ||
            oldvxres != var->xres_virtual || oldvyres != var->yres_virtual ||
            oldbpp != var->bits_per_pixel || oldaccel != var->accel_flags) {
            struct fb_fix_screeninfo fix;

            encode_fix(&fix, &par, info);
            display->screen_base = (char *)info->frame_buffer;
            display->visual = fix.visual;
            display->type = fix.type;
            display->type_aux = fix.type_aux;
            display->ypanstep = fix.ypanstep;
            display->ywrapstep = fix.ywrapstep;
            display->line_length = fix.line_length;
            display->can_soft_blank = 1;
            display->inverse = 0;
#if 0
           if (accel)
                display->scrollmode = (info->bus_type == PCI) ? SCROLL_YNOMOVE : 0;
            else
#endif
                display->scrollmode = SCROLL_YREDRAW;
#if 0
            if (info->fb_info.changevar)
                (*info->fb_info.changevar)(con);
#endif
        }
#endif
//        if (!info->fb_info.display_fg ||
//            info->fb_info.display_fg->vc_num == con) {
            atyfb_set_par(&par, info);
#if 0
            atyfb_set_dispsw(display, info, par.crtc.bpp, accel);
#endif
//        }
#if 0
        if (oldbpp != var->bits_per_pixel) {
            if ((err = fb_alloc_cmap(&display->cmap, 0, 0)))
                return err;
#endif
            do_install_cmap(con, info);
#if 0
        }
#endif
    }

    return 0;
}

#endif
/* ------------------------------------------------------------------------- */

static void atyfb_set_par(const struct atyfb_par *par,
                          struct fb_info_aty *info)
{
    u32 i;
    int accelmode;
    u8 tmp;

    accelmode = par->accel_flags;  /* hack */

#if PLL_CRTC_DECODE==1
    info->current_par = *par;
#endif

    if (info->blitter_may_be_busy)
        wait_for_idle(info);
    tmp = aty_ld_8(CRTC_GEN_CNTL + 3, info);
    aty_set_crtc(info, &par->crtc);
    aty_st_8(CLOCK_CNTL + info->clk_wr_offset, 0, info);
                                        /* better call aty_StrobeClock ?? */
    aty_st_8(CLOCK_CNTL + info->clk_wr_offset, CLOCK_STROBE, info);

    //info->dac_ops->set_dac(info, &par->pll, par->crtc.bpp, accelmode);
    //info->pll_ops->set_pll(info, &par->pll);
   aty_set_pll_ct(info, &par->pll);


    if (!M64_HAS(INTEGRATED)) {
        /* Don't forget MEM_CNTL */
        i = aty_ld_le32(MEM_CNTL, info) & 0xf0ffffff;
        switch (par->crtc.bpp) {
            case 8:
                i |= 0x02000000;
                break;
#if SUPPORT_8_BPP_ABOVE==1
            case 16:
                i |= 0x03000000;
                break;
            case 32:
                i |= 0x06000000;
                break;
#endif
        }
        aty_st_le32(MEM_CNTL, i, info);
    } else {
        i = aty_ld_le32(MEM_CNTL, info) & 0xf00fffff;
        if (!M64_HAS(MAGIC_POSTDIV))
                i |= info->mem_refresh_rate << 20;
        switch (par->crtc.bpp) {
            case 8:
#if SUPPORT_8_BPP_ABOVE==1
            case 24:
#endif
                i |= 0x00000000;
               break;
#if SUPPORT_8_BPP_ABOVE==1
            case 16:
                i |= 0x04000000;
                break;
            case 32:
                i |= 0x08000000;
                break;
#endif
        }
        if (M64_HAS(CT_BUS)) {
            aty_st_le32(DAC_CNTL, 0x87010184, info);
            aty_st_le32(BUS_CNTL, 0x680000f9, info);
        } else if (M64_HAS(VT_BUS)) {
            aty_st_le32(DAC_CNTL, 0x87010184, info);
            aty_st_le32(BUS_CNTL, 0x680000f9, info);
        }  else if (M64_HAS(MOBIL_BUS)) {
            aty_st_le32(DAC_CNTL, 0x80010102, info);
            aty_st_le32(BUS_CNTL, 0x7b33a040, info);
        }  else {
            /* GT */
            aty_st_le32(DAC_CNTL, 0x86010102, info);
            aty_st_le32(BUS_CNTL, 0x7b23a040, info);
            aty_st_le32(EXT_MEM_CNTL,
                        aty_ld_le32(EXT_MEM_CNTL, info) | 0x5000001, info);
        }

        aty_st_le32(MEM_CNTL, i, info);
    }
    aty_st_8(DAC_MASK, 0xff, info);

    /* Initialize the graphics engine */
#if 0
    if (par->accel_flags & FB_ACCELF_TEXT)
        aty_init_engine(par, info);
#endif


}
#if 0
static u16 red2[] = {
    0x0000, 0xaaaa
};
static u16 green2[] = {
    0x0000, 0xaaaa
};
static u16 blue2[] = {
    0x0000, 0xaaaa
};

static u16 red4[] = {
    0x0000, 0xaaaa, 0x5555, 0xffff
};
static u16 green4[] = {
    0x0000, 0xaaaa, 0x5555, 0xffff
};
static u16 blue4[] = {
    0x0000, 0xaaaa, 0x5555, 0xffff
};

static u16 red8[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0xaaaa, 0xaaaa, 0xaaaa, 0xaaaa
};
static u16 green8[] = {
    0x0000, 0x0000, 0xaaaa, 0xaaaa, 0x0000, 0x0000, 0x5555, 0xaaaa
};
static u16 blue8[] = {
    0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa
};
#endif
static u16 red16[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0xaaaa, 0xaaaa, 0xaaaa, 0xaaaa,
    0x5555, 0x5555, 0x5555, 0x5555, 0xffff, 0xffff, 0xffff, 0xffff
};
static u16 green16[] = {
    0x0000, 0x0000, 0xaaaa, 0xaaaa, 0x0000, 0x0000, 0x5555, 0xaaaa,
    0x5555, 0x5555, 0xffff, 0xffff, 0x5555, 0x5555, 0xffff, 0xffff
};
static u16 blue16[] = {
    0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa,
    0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff
};
#if 0
static struct fb_cmap default_2_colors = {
    0, 2, red2, green2, blue2, NULL
};
static struct fb_cmap default_8_colors = {
    0, 8, red8, green8, blue8, NULL
};
static struct fb_cmap default_4_colors = {
    0, 4, red4, green4, blue4, NULL
};
#endif
static struct fb_cmap default_16_colors = {
    0, 16, red16, green16, blue16, NULL
};



static int atyfb_setcolreg(u_int regno, u_int red, u_int green, u_int blue,
                           u_int transp, struct fb_info_aty *info)
{
    int i, scale;

    if (regno > 255)
        return 1;
    red >>= 8;
    green >>= 8;
    blue >>= 8;
#if 0
//We don't need to store it
    info->palette[regno].red = red;
    info->palette[regno].green = green;
    info->palette[regno].blue = blue;
#endif
    i = aty_ld_8(DAC_CNTL, info) & 0xfc;
    if (M64_HAS(EXTRA_BRIGHT))
        i |= 0x2;       /*DAC_CNTL|0x2 turns off the extra brightness for gt*/
    aty_st_8(DAC_CNTL, i, info);
    aty_st_8(DAC_MASK, 0xff, info);
#if PLL_CRTC_DECODE==1
    scale = (M64_HAS(INTEGRATED) && info->current_par.crtc.bpp == 16) ? 3 : 0;
#else
    scale = (M64_HAS(INTEGRATED) && info->default_par.crtc.bpp == 16) ? 3 : 0;
#endif
    write8(&info->aty_cmap_regs->windex, regno << scale)
    write8(&info->aty_cmap_regs->lut, red);
    write8(&info->aty_cmap_regs->lut, green);
    write8(&info->aty_cmap_regs->lut, blue);
    return 0;
}

int fb_set_cmap(struct fb_cmap *cmap, int kspc,
                int (*setcolreg)(u_int, u_int, u_int, u_int, u_int,
                                 struct fb_info_aty *),
                struct fb_info_aty *info)
{
    int i, start;
    u16 *red, *green, *blue, *transp;
    u_int hred, hgreen, hblue, htransp;

    red = cmap->red;
    green = cmap->green;
    blue = cmap->blue;
    transp = cmap->transp;
    start = cmap->start;

    if (start < 0)
        return -EINVAL;
    for (i = 0; i < cmap->len; i++) {
            hred = *red;
            hgreen = *green;
            hblue = *blue;
            htransp = transp ? *transp : 0;
        red++;
        green++;
        blue++;
        if (transp)
            transp++;
        if (setcolreg(start++, hred, hgreen, hblue, htransp, info))
            return 0;
    }
    return 0;
}

struct fb_cmap *fb_default_cmap(int len)
{
#if 0
    if (len <= 2)
        return &default_2_colors;
    if (len <= 4)
        return &default_4_colors;
    if (len <= 8)
        return &default_8_colors;
#endif
    return &default_16_colors;
}

static void do_install_cmap(int con, struct fb_info_aty *info)
{
#if PLL_CRTC_DECODE==1
        int size = info->current_par.crtc.bpp == 16 ? 32 : 256;
#else
	int size = 256;
#endif
        fb_set_cmap(fb_default_cmap(size), 1, atyfb_setcolreg, info);
}

#endif /*CONFIG_CONSOLE_BTEXT */

static struct device_operations ati_ragexl_graph_ops  = {
        .read_resources   = pci_dev_read_resources,
        .set_resources    = pci_dev_set_resources,
        .enable_resources = pci_dev_enable_resources,
        .init             = ati_ragexl_init,
        .scan_bus         = 0,
};

static const struct pci_driver ati_ragexl_graph_driver __pci_driver = {
        .ops    = &ati_ragexl_graph_ops,
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_215XL,
};


