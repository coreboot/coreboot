/* SPDX-License-Identifier: GPL-2.0-only */

int intel_vga_int15_handler(void);

enum {
	GMA_INT15_BOOT_DISPLAY_DEFAULT	 = 0,
	GMA_INT15_BOOT_DISPLAY_CRT	 = (1 << 0),
	GMA_INT15_BOOT_DISPLAY_TV	 = (1 << 1),
	GMA_INT15_BOOT_DISPLAY_EFP	 = (1 << 2),
	GMA_INT15_BOOT_DISPLAY_LFP	 = (1 << 3),
	GMA_INT15_BOOT_DISPLAY_CRT2	 = (1 << 4),
	GMA_INT15_BOOT_DISPLAY_TV2	 = (1 << 5),
	GMA_INT15_BOOT_DISPLAY_EFP2	 = (1 << 6),
	GMA_INT15_BOOT_DISPLAY_LFP2	 = (1 << 7),
};
enum {
	GMA_INT15_PANEL_FIT_DEFAULT	= 0,
	GMA_INT15_PANEL_FIT_CENTERING	= (1 << 0),
	GMA_INT15_PANEL_FIT_TXT_STRETCH	= (1 << 1),
	GMA_INT15_PANEL_FIT_GFX_STRETCH	= (1 << 2),
};

enum {
	GMA_INT15_ACTIVE_LFP_NONE	= 0x00,
	GMA_INT15_ACTIVE_LFP_INT_LVDS	= 0x01,
	GMA_INT15_ACTIVE_LFP_SVDO_LVDS	= 0x02,
	GMA_INT15_ACTIVE_LFP_EDP	= 0x03,
};

#if CONFIG(VGA_ROM_RUN)
/* Install custom int15 handler for VGA OPROM */
void install_intel_vga_int15_handler(int active_lfp, int pfit, int display, int panel_type);
#else
static inline void install_intel_vga_int15_handler(int active_lfp, int pfit, int display, int panel_type) {}
#endif
