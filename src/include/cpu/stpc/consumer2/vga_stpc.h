/*
 * by
 * Steve M. Gehlbach
 * for stpc consumer II
 */

//
// STPC extended registers, non-standard
// must write 57h to seq reg 6 to access these
// must be done individually, not in par array

#define	CRTC_REPAINT_C0			0x19
#define	CRTC_REPAINT_C1			0x1A
#define	CRTC_REPAINT_C2			0x1B
#define	CRTC_REPAINT_C3			0x1C
#define	CRTC_PAGE_R0			0x1D
#define	CRTC_PAGE_R1			0x1E
#define	CRTC_GE_ENABLE			0x1F
#define	CRTC_GE_GBASE			0x20
#define	CRTC_GE_APER			0x21
#define	CRTC_REPAINT_C4			0x25
#define	CRTC_REPAINT_C5			0x27
#define	CRTC_PALLETTE_C			0x28
#define	CRTC_URGENT_START		0x33
#define	CRTC_DISP_YOFFS_L		0x34
#define	CRTC_DISP_YOFFS_H		0x35
#define CRTC_GV_R			0x3B

// these are read only for STPC, no settings
#define CRTC_GCD_R			0x22
#define CRTC_AA_FF_R			0x24
#define CRTC_AI_R			0x26

// not in automatic list; has to be done manual
#define SEQ_XREG_UNLOCK			0x06
