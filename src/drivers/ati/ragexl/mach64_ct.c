#define DEBUG_PLL 0
#define PLL_CRTC_DECODE 0

/* FIXME: remove the FAIL definition */
#if 0
#define FAIL(x) do { printk(BIOS_DEBUG, x); return -EINVAL; } while (0)
#else
#define FAIL(x)
#endif

static int aty_valid_pll_ct(const struct fb_info_aty *info, u32 vclk_per,
			    struct pll_ct *pll);
static int aty_dsp_gt(const struct fb_info_aty *info, u32 bpp,
		      struct pll_ct *pll);
static int aty_var_to_pll_ct(const struct fb_info_aty *info, u32 vclk_per,
			     u8 bpp, union aty_pll *pll);
#if PLL_CRTC_DECODE==1
static u32 aty_pll_ct_to_var(const struct fb_info_aty *info,
			     const union aty_pll *pll);
#endif

/* ------------------------------------------------------------------------- */

    /*
     *  PLL programming (Mach64 CT family)
     */
static int aty_dsp_gt(const struct fb_info_aty *info, u32 bpp,
		      struct pll_ct *pll)
{
    u32 dsp_xclks_per_row, dsp_loop_latency, dsp_precision, dsp_off, dsp_on;
    u32 xclks_per_row, fifo_off, fifo_on, y, fifo_size;
    u32 memcntl, n, t_pfc, t_rp, t_ras, t_rcd, t_crd, t_rcc, t_lat;

#if DEBUG_PLL==1
    printk(BIOS_DEBUG, "aty_dsp_gt : mclk_fb_mult=%d\n", pll->mclk_fb_mult);
#endif

    /* (64*xclk/vclk/bpp)<<11 = xclocks_per_row<<11 */
    xclks_per_row = ((u32)pll->mclk_fb_mult * (u32)pll->mclk_fb_div *
		     (u32)pll->vclk_post_div_real * 64) << 11;
    xclks_per_row /=
	    (2 * (u32)pll->vclk_fb_div * (u32)pll->xclk_post_div_real * bpp);

    if (xclks_per_row < (1<<11))
	FAIL("Dotclock too high");
    if (M64_HAS(FIFO_24)) {
	fifo_size = 24;
	dsp_loop_latency = 0;
    } else {
	fifo_size = 32;
	dsp_loop_latency = 2;
    }
    dsp_precision = 0;
    y = (xclks_per_row*fifo_size)>>11;
    while (y) {
	y >>= 1;
	dsp_precision++;
    }
    dsp_precision -= 5;

    /* fifo_off<<6 */
    fifo_off = ((xclks_per_row*(fifo_size-1))>>5); // + (3<<6);

    if (info->total_vram > 1*1024*1024) {
	switch (info->ram_type) {
	case WRAM:
	    /* >1 MB WRAM */
	    dsp_loop_latency += 9;
	    n = 4;
	    break;
	case SDRAM:
	case SGRAM:
	    /* >1 MB SDRAM */
	    dsp_loop_latency += 8;
	    n = 2;
	    break;
	default:
	    /* >1 MB DRAM */
	    dsp_loop_latency += 6;
	    n = 3;
	    break;
	}
    } else {
	if (info->ram_type >= SDRAM) {
	    /* <2 MB SDRAM */
	    dsp_loop_latency += 9;
	    n = 2;
	} else {
	    /* <2 MB DRAM */
	    dsp_loop_latency += 8;
	    n = 3;
	}
    }

    memcntl = aty_ld_le32(MEM_CNTL, info);
    t_rcd = ((memcntl >> 10) & 0x03) + 1;
    t_crd = ((memcntl >> 12) & 0x01);
    t_rp  = ((memcntl >>  8) & 0x03) + 1;
    t_ras = ((memcntl >> 16) & 0x07) + 1;
    t_lat =  (memcntl >>  4) & 0x03;

    t_pfc = t_rp + t_rcd + t_crd;

    t_rcc = max(t_rp + t_ras, t_pfc + n);

    /* fifo_on<<6 */
    fifo_on = (2 * t_rcc + t_pfc + n - 1) << 6;

    dsp_xclks_per_row = xclks_per_row>>dsp_precision;
    dsp_on = fifo_on>>dsp_precision;
    dsp_off = fifo_off>>dsp_precision;

    pll->dsp_config = (dsp_xclks_per_row & 0x3fff) |
		      ((dsp_loop_latency & 0xf)<<16) |
		      ((dsp_precision & 7)<<20);
    pll->dsp_on_off = (dsp_off & 0x7ff) | ((dsp_on & 0x7ff)<<16);
    return 0;
}


static int aty_valid_pll_ct(const struct fb_info_aty *info, u32 vclk_per,
			    struct pll_ct *pll)
{
#if DEBUG_PLL==1
    int pllmclk, pllsclk;
#endif
    u32 q;

    pll->pll_ref_div = info->pll_per*2*255/info->ref_clk_per;

    /* FIXME: use the VTB/GTB /3 post divider if it's better suited */

    /* actually 8*q */
    q = info->ref_clk_per*pll->pll_ref_div*4/info->mclk_per;

    if (q < 16*8 || q > 255*8)
	FAIL("mclk out of range\n");
    else if (q < 32*8)
	pll->mclk_post_div_real = 8;
    else if (q < 64*8)
	pll->mclk_post_div_real = 4;
    else if (q < 128*8)
	pll->mclk_post_div_real = 2;
    else
	pll->mclk_post_div_real = 1;
    pll->sclk_fb_div = q*pll->mclk_post_div_real/8;

#if DEBUG_PLL==1
    pllsclk = (1000000 * 2 * pll->sclk_fb_div) /
	    (info->ref_clk_per * pll->pll_ref_div);

    printk(BIOS_DEBUG, "aty_valid_pll_ct: pllsclk=%d MHz, mclk=%d MHz\n",
	   pllsclk, pllsclk / pll->mclk_post_div_real);
#endif

    pll->mclk_fb_mult = M64_HAS(MFB_TIMES_4) ? 4 : 2;

    /* actually 8*q */
    q = info->ref_clk_per * pll->pll_ref_div * 8 /
	    (pll->mclk_fb_mult * info->xclk_per);

    if (q < 16*8 || q > 255*8)
	FAIL("mclk out of range\n");
    else if (q < 32*8)
	pll->xclk_post_div_real = 8;
    else if (q < 64*8)
	pll->xclk_post_div_real = 4;
    else if (q < 128*8)
	pll->xclk_post_div_real = 2;
    else
	pll->xclk_post_div_real = 1;
    pll->mclk_fb_div = q*pll->xclk_post_div_real/8;

#if DEBUG_PLL==1
    pllmclk = (1000000 * pll->mclk_fb_mult * pll->mclk_fb_div) /
	    (info->ref_clk_per * pll->pll_ref_div);
    printk(BIOS_DEBUG, "aty_valid_pll_ct: pllmclk=%d MHz, xclk=%d MHz\n",
	   pllmclk, pllmclk / pll->xclk_post_div_real);
#endif

    /* FIXME: use the VTB/GTB /{3,6,12} post dividers if they're better suited */
    q = info->ref_clk_per*pll->pll_ref_div*4/vclk_per;	/* actually 8*q */
    if (q < 16*8 || q > 255*8)
	FAIL("vclk out of range\n");
    else if (q < 32*8)
	pll->vclk_post_div_real = 8;
    else if (q < 64*8)
	pll->vclk_post_div_real = 4;
    else if (q < 128*8)
	pll->vclk_post_div_real = 2;
    else
	pll->vclk_post_div_real = 1;
    pll->vclk_fb_div = q*pll->vclk_post_div_real/8;
    return 0;
}

static void aty_calc_pll_ct(const struct fb_info_aty *info, struct pll_ct *pll)
{
    u8 xpostdiv = 0;
    u8 mpostdiv = 0;
    u8 vpostdiv = 0;

    if (M64_HAS(SDRAM_MAGIC_PLL) && (info->ram_type >= SDRAM))
	    pll->pll_gen_cntl = 0x64; /* mclk = sclk */
    else
	pll->pll_gen_cntl = 0xe4; /* mclk = sclk */

    switch (pll->mclk_post_div_real) {
	case 1:
	    mpostdiv = 0;
	    break;
	case 2:
	    mpostdiv = 1;
	    break;
	case 4:
	    mpostdiv = 2;
	    break;
	case 8:
	    mpostdiv = 3;
	    break;
    }

    pll->spll_cntl2 = mpostdiv << 4; /* sclk == pllsclk / mpostdiv */

    switch (pll->xclk_post_div_real) {
	case 1:
	    xpostdiv = 0;
	    break;
	case 2:
	    xpostdiv = 1;
	    break;
	case 3:
	    xpostdiv = 4;
	    break;
	case 4:
	    xpostdiv = 2;
	    break;
	case 8:
	    xpostdiv = 3;
	    break;
    }

    if (M64_HAS(MAGIC_POSTDIV))
	pll->pll_ext_cntl = 0;
    else
	pll->pll_ext_cntl = xpostdiv;	/* xclk == pllmclk / xpostdiv */

    if (pll->mclk_fb_mult == 4)
	    pll->pll_ext_cntl |= 0x08;

    switch (pll->vclk_post_div_real) {
	case 2:
	    vpostdiv = 1;
	    break;
	case 3:
	    pll->pll_ext_cntl |= 0x10;
	case 1:
	    vpostdiv = 0;
	    break;
	case 6:
	    pll->pll_ext_cntl |= 0x10;
	case 4:
	    vpostdiv = 2;
	    break;
	case 12:
	    pll->pll_ext_cntl |= 0x10;
	case 8:
	    vpostdiv = 3;
	    break;
    }

    pll->pll_vclk_cntl = 0x03;	/* VCLK = PLL_VCLK/VCLKx_POST */
    pll->vclk_post_div = vpostdiv;
}

int aty_var_to_pll_ct(const struct fb_info_aty *info, u32 vclk_per,
			     u8 bpp, union aty_pll *pll)
{
    int err;
    if ((err = aty_valid_pll_ct(info, vclk_per, &pll->ct)))
	return err;
    if (M64_HAS(GTB_DSP) && (err = aty_dsp_gt(info, bpp, &pll->ct)))
	return err;
    aty_calc_pll_ct(info, &pll->ct);
    return 0;
}
#if CONFIG_CONSOLE_BTEXT
#if PLL_CRTC_DECODE==1
u32 aty_pll_ct_to_var(const struct fb_info_aty *info,
			     const union aty_pll *pll)
{
    u32 ref_clk_per = info->ref_clk_per;
    u8 pll_ref_div = pll->ct.pll_ref_div;
    u8 vclk_fb_div = pll->ct.vclk_fb_div;
    u8 vclk_post_div = pll->ct.vclk_post_div_real;

    return ref_clk_per*pll_ref_div*vclk_post_div/vclk_fb_div/2;
}
#endif
void aty_set_pll_ct(const struct fb_info_aty *info, const union aty_pll *pll)
{
#if DEBUG_PLL==1
    printk(BIOS_DEBUG, "aty_set_pll_ct: about to program:\n"
	   "refdiv=%d, extcntl=0x%02x, mfbdiv=%d\n"
	   "spllcntl2=0x%02x, sfbdiv=%d, gencntl=0x%02x\n"
	   "vclkcntl=0x%02x, vpostdiv=0x%02x, vfbdiv=%d\n"
	   "clocksel=%d\n",
	   pll->ct.pll_ref_div, pll->ct.pll_ext_cntl,
	   pll->ct.mclk_fb_div, pll->ct.spll_cntl2,
	   pll->ct.sclk_fb_div, pll->ct.pll_gen_cntl,
	   pll->ct.pll_vclk_cntl, pll->ct.vclk_post_div,
	   pll->ct.vclk_fb_div, aty_ld_le32(CLOCK_CNTL, info) & 0x03);
#endif

    aty_st_pll(PLL_REF_DIV, pll->ct.pll_ref_div, info);

    aty_st_pll(PLL_EXT_CNTL, pll->ct.pll_ext_cntl, info);
    aty_st_pll(MCLK_FB_DIV, pll->ct.mclk_fb_div, info); // for XCLK

    aty_st_pll(SPLL_CNTL2, pll->ct.spll_cntl2, info);
    aty_st_pll(SCLK_FB_DIV, pll->ct.sclk_fb_div, info); // for MCLK

    aty_st_pll(PLL_GEN_CNTL, pll->ct.pll_gen_cntl, info);

    aty_st_pll(EXT_VPLL_CNTL, 0, info);
    aty_st_pll(PLL_VCLK_CNTL, pll->ct.pll_vclk_cntl, info);
    aty_st_pll(VCLK_POST_DIV, pll->ct.vclk_post_div, info);
    aty_st_pll(VCLK0_FB_DIV, pll->ct.vclk_fb_div, info);

    if (M64_HAS(GTB_DSP)) {
	u8 dll_cntl;

	if (M64_HAS(XL_DLL))
	    dll_cntl = 0x80;
	else if (info->ram_type >= SDRAM)
	    dll_cntl = 0xa6;
	else
	    dll_cntl = 0xa0;
	aty_st_pll(DLL_CNTL, dll_cntl, info);
	aty_st_pll(VFC_CNTL, 0x1b, info);
	aty_st_le32(DSP_CONFIG, pll->ct.dsp_config, info);
	aty_st_le32(DSP_ON_OFF, pll->ct.dsp_on_off, info);

	mdelay(10);
	aty_st_pll(DLL_CNTL, dll_cntl, info);
	mdelay(10);
	aty_st_pll(DLL_CNTL, dll_cntl | 0x40, info);
	mdelay(10);
	aty_st_pll(DLL_CNTL, dll_cntl & ~0x40, info);
    }
}
#if 0
static int dummy(void)
{
    return 0;
}
static  struct aty_dac_ops aty_dac_ct = {
    set_dac:	(void *)dummy,
};

static struct aty_pll_ops aty_pll_ct = {
    var_to_pll:	aty_var_to_pll_ct,
#if 0
    pll_to_var:	aty_pll_ct_to_var,
    set_pll:	aty_set_pll_ct,
#endif
};
#endif

#endif /* CONFIG_CONSOLE_BTEXT */
