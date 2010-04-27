/*
*
* Copyright (C) 2007 Advanced Micro Devices
*
*/

#ifndef CPU_AMD_VR_H
#define CPU_AMD_VR_H

#define VRC_INDEX				0xAC1C	// Index register
#define VRC_DATA				0xAC1E	// Data register
#define VR_UNLOCK				0xFC53	// Virtual register unlock code
#define NO_VR					-1		// No virtual registers

#define	VRC_MISCELLANEOUS		0x00	// Miscellaneous Class
    #define VSA_VERSION_NUM     0x00
	#define HIGH_MEM_ACCESS		0x01
    #define GET_VSM_INFO        0x02	// Used by INFO
       #define GET_BASICS       0x00
       #define GET_EVENT        0x01
       #define GET_STATISTICS   0x02
       #define GET_HISTORY      0x03
       #define GET_HARDWARE     0x04
       #define GET_ERROR        0x05
       #define SET_VSM_TYPE     0x06
	#define SIGNATURE			0x03
       #define VSA2_SIGNATURE	0x56534132	// 'VSA2' returned in EAX

    #define GET_HW_INFO			0x04
    #define VSM_VERSION			0x05
	#define CTRL_ALT_DEL		0x06
    #define MSR_ACCESS          0x07
    #define GET_DESCR_INFO		0x08
    #define PCI_INT_AB			0x09	// GPIO pins for INTA# and INTB#
    #define PCI_INT_CD			0x0A	// GPIO pins for INTC# and INTD#
    #define WATCHDOG			0x0B	// Watchdog timer

    #define MAX_MISC           	WATCHDOG


// NOTE:  Do not change the order of the following registers:
#define	VRC_AUDIO      			0x01	// XpressAudio Class
	#define AUDIO_VERSION      	0x00
	#define PM_STATE        	0x01
	#define SB_16_IO_BASE   	0x02
	#define MIDI_BASE       	0x03
	#define CPU_USAGE       	0x04
	#define CODEC_TYPE      	0x05
	#define STATE_INDEX     	0x06
	#define STATE_DATA      	0x07
	#define AUDIO_IRQ	      	0x08	// For use by native audio drivers
	#define STATUS_PTR			0x09	// For use by native audio drivers
	#define MAX_AUDIO           STATUS_PTR

#define	VRC_VG					0x02	// SoftVG Class
#define	VRC_VGA					0x02	// SoftVGA Class
	#define	VG_MEM_SIZE			0x00	// bits 7:0 - 512K unit size, bit 8 controller priority
	#define	VG_CONFIG			0x00	// Main configuration register
		#define VG_CFG_BYPASS		0x0001	// DOTPLL bypass bit
		#define VG_MEM_MASK			0x00FE	// Memory size mask bits, 2MB increment
		#define VG_CFG_DSMASK		0x0700	// Active display mask bits
			#define VG_CFG_DSCRT		0x0000	// Active display is CRT
			#define VG_CFG_DSPAN		0x0100	// Active display is panel
			#define VG_CFG_DSTV			0x0200	// Active display is TV
			#define VG_CFG_DSSIM		0x0400	// Simultaneous CRT
		#define VG_CFG_PRIORITY		0x0800	// Controller priority bit
		#define VG_CFG_MONO			0x1000	// External monochrome card support bit
		#define VG_CFG_DRIVER		0x2000	// Driver active bit
		#define VG_CRTC_DIAG		0x8000	// Enable CRTC emulation

	// Defined for GX3/GX3VG
	#define	VG_REFRESH			0x01	// Mode refresh, a mode switch without changing modes
		#define VG_FRSH_REF_MASK	0xE000	// Refresh rate mask
		#define VG_FRSH_REF_GO		0x1000	// Refresh rate GO bit
			// Uses CFP_REF_xxx values from below
		#define VG_FRSH_BPP_MASK	0x0E00	// Color depth mask
		#define VG_FRSH_BPP_GO		0x0100	// Color depth GO bit
			#define FRSH_BPP_8RGB		0x0200	// 8 bits per pixel, RGB
			#define FRSH_BPP_16ARGB		0x0400	// 16BPP, ARGB (4:4:4:4)
			#define FRSH_BPP_15RGB		0x0600	// 15BPP, RGB (1:5:5:5)
			#define FRSH_BPP_16RGB		0x0800	// 16BPP, RGB (5:6:5)
			#define FRSH_BPP_24RGB		0x0A00	// 24BPP, RGB (0:8:8:8)
			#define FRSH_BPP_32ARGB		0x0C00	// 32BPP, ARGB (8:8:8:8)
		#define VG_CFG_DPMS			0x00C0	// DPMS mask bits
			#define VG_CFG_DPMS_H		0x0040	// HSYNC mask bit
			#define VG_CFG_DPMS_V		0x0080	// VSYNC mask bit
		#define VG_VESA_SV_RST		0x0020	// VESA Save/Restore state flag
			#define VG_VESA_RST			0x0000	// VESA Restore state
			#define VG_VESA_SV			0x0020	// VESA Save state
		#define VG_FRSH_MODE		0x0002	// Mode refresh flag
		#define VG_FRSH_TIMINGS		0x0001	// Timings only refresh flag

	// Defined for GX2/SoftVG
	#define	VG_PLL_REF			0x01	// PLL reference frequency selection register
		#define	PLL_14MHZ			0x0000	// 14.31818MHz PLL reference frequency (Default)
		#define	PLL_48MHZ			0x0100	// 48MHz PLL reference frequency

	// Defined for GX1/SoftVGA
	#define	VGA_MEM_SIZE		0x01	// bits 7:1 - 128K unit size, bit 0 controller enable

	#define	VG_FP_TYPE			0x02	// Flat panel type data
	// VG_FP_TYPE definitions for GX2/SoftVG
		#define FP_TYPE_SSTN		0x0000	// SSTN panel type value
		#define FP_TYPE_DSTN		0x0001	// DSTN panel type value
		#define FP_TYPE_TFT			0x0002	// TFT panel type value
		#define FP_TYPE_LVDS		0x0003	// LVDS panel type value
		#define FP_RES_6X4			0x0000	// 640x480 resolution value
		#define FP_RES_8X6			0x0008	// 800x600 resolution value
		#define FP_RES_10X7			0x0010	// 1024x768 resolution value
		#define FP_RES_11X8			0x0018	// 1152x864 resolution value
		#define FP_RES_12X10		0x0020	// 1280x1024 resolution value
		#define FP_RES_16X12		0x0028	// 1600x1200 resolution value
		#define FP_WIDTH_8			0x0000	// 8 bit data bus width
		#define FP_WIDTH_9			0x0040	// 9 bit data bus width
		#define FP_WIDTH_12			0x0080	// 12 bit data bus width
		#define FP_WIDTH_18			0x00C0	// 18 bit data bus width
		#define FP_WIDTH_24			0x0100	// 24 bit data bus width
		#define FP_WIDTH_16			0x0140	// 16 bit data bus width - 16 bit Mono DSTN only
		#define FP_COLOR_COLOR		0x0000	// Color panel
		#define FP_COLOR_MONO		0x0200	// Mono Panel
		#define FP_PPC_1PPC			0x0000	// One pixel per clock
		#define FP_PPC_2PPC			0x0400	// Two pixels per clock
		#define	FP_H_POL_LGH		0x0000	// HSync at panel, normally low, active high
		#define FP_H_POL_HGL		0x0800	// HSync at panel, normally high, active low
		#define FP_V_POL_LGH		0x0000	// VSync at panel, normally low, active high
		#define FP_V_POL_HGL		0x1000	// VSync at panel, normally high, active low
		#define FP_REF_60			0x0000	// 60Hz refresh rate
		#define FP_REF_65			0x2000	// 65Hz refresh rate
		#define FP_REF_70			0x4000	// 70Hz refresh rate
		#define FP_REF_72			0x6000	// 72Hz refresh rate
		#define FP_REF_75			0x8000	// 75Hz refresh rate
		#define FP_REF_85			0xA000	// 85Hz refresh rate

	// VG_FP_TYPE definitions for GX3/GX3VG
		#define FP_TYPE_TYPE	0x0003		// Flat panel type bits mask
			#define CFP_TYPE_TFT	0x0000		// TFT panel type value
			#define CFP_TYPE_LVDS	0x0001		// LVDS panel type value
		#define FP_TYPE_RES		0x0038		// Panel resolution bits mask
			#define CFP_RES_3X2		0x0000		// 320x240 resolution value
			#define CFP_RES_6X4		0x0008		// 640x480 resolution value
			#define CFP_RES_8X6		0x0010		// 800x600 resolution value
			#define CFP_RES_10X7	0x0018		// 1024x768 resolution value
			#define CFP_RES_11X8	0x0020		// 1152x864 resolution value
			#define CFP_RES_12X10	0x0028		// 1280x1024 resolution value
			#define CFP_RES_16X12	0x0030		// 1600x1200 resolution value
		#define FP_TYPE_BUS		0x00C0		// Data bus width and pixels/clock mask
			#define CFP_BUS_1PPC	0x0040		// 9, 12, 18 or 24 bit data bus, 1 pixel per clock
			#define CFP_BUS_2PPC	0x0080		// 18 or 24 bit data bus, 2 pixels per clock
		#define FP_TYPE_HPOL	0x0800		// HSYNC polarity into the panel
			#define CFP_HPOL_HGL	0x0000	// HSync at panel, normally high, active low
			#define	CFP_HPOL_LGH	0x0800	// HSync at panel, normally low, active high
		#define FP_TYPE_VPOL	0x1000		// VSYNC polarity into the panel
			#define CFP_VPOL_HGL	0x0000	// VSync at panel, normally high, active low
			#define CFP_VPOL_LGH	0x1000	// VSync at panel, normally low, active high
		#define FP_TYPE_REF		0xE000		// Panel refresh rate
			#define CFP_REF_60		0x0000	// 60Hz refresh rate
			#define CFP_REF_70		0x2000	// 70Hz refresh rate
			#define CFP_REF_75		0x4000	// 75Hz refresh rate
			#define CFP_REF_85		0x6000	// 85Hz refresh rate
			#define CFP_REF_100		0x8000	// 100Hz refresh rate

	#define	VG_FP_OPTION		0x03	// Flat panel option data
		#define FP_OPT_SCLK_NORMAL		0x0000		// SHFTClk not inverted to panel
		#define FP_OPT_SCLK_INVERTED	0x0010		// SHFTClk inverted to panel
		#define FP_OPT_SCLK_ACT_ACTIVE	0x0000		// SHFTClk active during "active" only
		#define FP_OPT_SCLK_ACT_FREE	0x0020		// SHFTClk free-running
		#define FP_OPT_LP_ACT_FREE		0x0000		// LP free-running
		#define FP_OPT_LP_ACT_ACTIVE	0x0040		// LP active during "active" only
		#define FP_OPT_LDE_POL_LGH		0x0000		// LDE/MOD, normally low, active high
		#define FP_OPT_LDE_POL_HGL		0x0080		// LDE/MOD, normally high, active low
		#define FP_OPT_PWR_DLY_32MS		0x0000		// 32MS delay for each step of pwr seq.
		#define FP_OPT_PWR_DLY_128MS	0x0100		// 128MS delay for each step of pwr seq.

	#define	VG_TV_CONFIG		0x04	// TV configuration register
		#define VG_TV_ENC			0x000F	// TV encoder select mask
			#define VG_TV_ADV7171		0x0000	// ADV7171 Encoder
			#define VG_TV_SAA7127		0x0001	// ADV7171 Encoder
			#define VG_TV_ADV7300		0x0002	// ADV7300 Encoder
			#define VG_TV_FS454			0x0003	// FS454 Encoder
		#define VG_TV_FMT			0x0070	// TV encoder output format mask
			#define VG_TV_FMT_SHIFT		0x0004	// Right shift value
			#define VG_TV_NTSC			0x0000	// NTSC output format
			#define VG_TV_PAL			0x0010	// PAL output format
			#define VG_TV_HDTV			0x0020	// HDTV output format

		// The meaning of the VG_TV_RES field is dependent on the selected
		// encoder and output format.  The translations are:
		//		ADV7171 - Not Used
		//		SAA7127 - Not Used
		//		ADV7300 - HDTV resolutions only
		//			LO  -> 720x480p
		//	   		MED -> 1280x720p
		//			HI  -> 1920x1080i
		// 		FS454   - Both SD and HD resolutions
		// 			SD Resolutions - NTSC and PAL
		//				LO  -> 640x480
		//	   			MED -> 800x600
		//				HI  -> 1024x768
		// 			HD Resolutions
		//				LO  -> 720x480p
		//	   			MED -> 1280x720p
		//				HI  -> 1920x1080i
		#define VG_TV_RES			0x0780	// TV resolution select mask
			#define VG_TV_RES_SHIFT		0x0007	// Right shift value
			#define VG_TV_RES_LO		0x0000	// Low resolution
			#define VG_TV_RES_MED		0x0080	// Medium resolution
			#define VG_TV_RES_HI 		0x0100	// High resolution
		#define VG_TV_PASSTHRU		0x0800	// TV passthru mode

	#define	VG_TV_SCALE_ADJ		0x05	// Modifies scaling factors for TV resolutions
		#define VG_TV_HACT_ADJ		0x00FF		// Horizontal active scale adjust value mask
		#define VG_TV_VACT_ADJ		0xFF00		// Vertical active scale adjust value mask

	#define	VG_DEBUG			0x0F	// A debug register

	#define	VG_FT_HTOT			0x10	// Fixed timings, horizontal total
	#define	VG_FT_HACT			0x11	// Fixed timings, horizontal active
	#define	VG_FT_HBST			0x12	// Fixed timings, horizontal blank start
	#define	VG_FT_HBND			0x13	// Fixed timings, horizontal blank end
	#define	VG_FT_HSST			0x14	// Fixed timings, horizontal sync start
	#define	VG_FT_HSND			0x15	// Fixed timings, horizontal sync end

	#define	VG_FT_VTOT			0x16	// Fixed timings, vertical total
	#define	VG_FT_VACT			0x17	// Fixed timings, vertical active
	#define	VG_FT_VBST			0x18	// Fixed timings, vertical blank start
	#define	VG_FT_VBND			0x19	// Fixed timings, vertical blank end
	#define	VG_FT_VSST			0x1A	// Fixed timings, vertical sync start
	#define	VG_FT_VSND			0x1B	// Fixed timings, vertical sync end

	#define	VG_START_OFFS_LO	0x20	// Framebuffer start offset bits 15:0
	#define	VG_START_OFFS_HI	0x21	// Framebuffer start offset bits 27:16

	#define	VG_FT_VEACT			0x28	// Fixed timings, vertical active
	#define	VG_FT_VETOT			0x29	// Fixed timings, vertical total
	#define	VG_FT_VEBST			0x2A	// Fixed timings, vertical blank start
	#define	VG_FT_VEBND			0x2B	// Fixed timings, vertical blank end
	#define	VG_FT_VESST			0x2C	// Fixed timings, vertical sync start
	#define	VG_FT_VESND			0x2D	// Fixed timings, vertical sync end

	#define MAX_VGA           	VGA_MEM_SIZE
//	#define MAX_VG           	VG_FP_OPTION
//	#define MAX_VG           	VG_START_OFFS_HI
	#define MAX_VG           	VG_FT_VESND

#define	VRC_APM					0x03
    #define REPORT_EVENT		0x00
    #define CAPABILITIES    	0x01
	#define APM_PRESENT		0x02
	#define MAX_APM           	APM_PRESENT


#define	VRC_PM					0x04	// Legacy PM Class
	#define	POWER_MODE			0x00
	#define POWER_STATE			0x01
	#define	DOZE_TIMEOUT		0x02
	#define	STANDBY_TIMEOUT	   	0x03
	#define	SUSPEND_TIMEOUT	   	0x04
    #define PS2_TIMEOUT		    0x05
	#define RESUME_ON_RING		0x06
	#define VIDEO_TIMEOUT		0x07
	#define	DISK_TIMEOUT		0x08
	#define	FLOPPY_TIMEOUT	   	0x09
	#define	SERIAL_TIMEOUT	    0x0A
	#define	PARALLEL_TIMEOUT	0x0B
	#define IRQ_WAKEUP_MASK	   	0x0C
//	#define SUSPEND_MODULATION 	0x0D
	#define SLEEP_PIN			0x0E
	#define SLEEP_PIN_ATTR		0x0F
//	#define SMI_WAKEUP_MASK		0x10
	#define INACTIVITY_CONTROL	0x11
	#define PM_S1_CLOCKS		0x12
		#define S1_CLOCKS_ON		0x00
		#define S1_CLOCKS_OFF		0x01
//	#define PM_S2_CLOCKS		0x13
//	#define PM_S3_CLOCKS		0x14
//	#define PM_S4_CLOCKS		0x15
//	#define PM_S5_CLOCKS		0x16
    #define PM_S0_LED           0x17
    #define PM_S1_LED           0x18
    #define PM_S2_LED           0x19
    #define PM_S3_LED           0x1A
    #define PM_S4_LED           0x1B
    #define PM_S5_LED           0x1C
	#define PM_LED_GPIO			0x1D
    #define PM_IMM_LED			0x1E
    #define PM_PWR_LEDS			0x1F
	    #define MB_LED0				0x01
	    #define MB_LED1				0x02
	    #define MB_LED2				0x04
	    #define MB_LED3				0x08
	    #define SIO_LED0			0x10
	    #define SIO_LED1			0x20
	    #define SIO_LED2			0x40
	    #define SIO_LED3			0x80
	#define PM_PME_MASK			0x20
    #define MAX_PM              PM_PME_MASK


#define VRC_INFRARED			0x05
	#define MAX_INFRARED        NO_VR

#define	VRC_TV					0x06	// TV Encoder Class
	#define TV_ENCODER_TYPE		0x00
	#define TV_CALLBACK_MASK	0x01
	#define TV_MODE				0x02
	#define TV_POSITION			0x03
	#define TV_BRIGHTNESS		0x04
	#define TV_CONTRAST			0x05
	#define TV_OUTPUT			0x06
	#define TV_TIMING			0x10	// 0x10...0x1D are all timings
	#define MAX_TV           	TV_TIMING



#define VRC_EXTERNAL_AMP		0x07
    #define EAPD_VERSION        0x00
    #define AMP_POWER          	0x01
	   #define AMP_OFF          0x00
	   #define AMP_ON           0x01
	#define AMP_TYPE            0x02
	#define MAX_EXTERNAL_AMP    	AMP_TYPE


#define	VRC_ACPI				0x08
    #define	ENABLE_ACPI			0x00	// Enable ACPI Mode
	#define SCI_IRQ			   	0x01	// Set the IRQ the SCI is mapped to, sysbios use.
	#define ACPINVS_LO			0x02	// new calls to send 32bit physAddress of
	#define ACPINVS_HI		  	0x03	// ACPI NVS region to VSA
	#define GLOBAL_LOCK			0x04	// read requests semaphore, write clears
	#define ACPI_UNUSED1		0x05
	#define RW_PIRQ				0x06	// read/write PCI IRQ router regs in SB Func0 cfg space
	#define SLPB_CLEAR			0x07	// clear sleep button GPIO status's
	#define PIRQ_ROUTING		0x08	// read the PCI IRQ routing based on BIOS setup
	#define ACPI_UNUSED2		0x09
	#define ACPI_UNUSED3		0x0A
	#define PIC_INTERRUPT		0x0B
	#define ACPI_PRESENT		0x0C
	#define	ACPI_GEN_COMMAND	0x0D
	#define	ACPI_GEN_PARAM1		0x0E
	#define	ACPI_GEN_PARAM2		0x0F
	#define	ACPI_GEN_PARAM3		0x10
	#define	ACPI_GEN_RETVAL		0x11
	#define MAX_ACPI	        ACPI_GEN_RETVAL

#define	VRC_ACPI_OEM			0x09
	#define MAX_ACPI_OEM        NO_VR

#define VRC_POWER               0x0A
    #define BATTERY_UNITS       0x00	// No. battery units
    #define BATTERY_SELECT      0x01
    #define AC_STATUS           0x02
	#define BATTERY_STATUS		0x03
	#define BATTERY_FLAG		0x04
	#define BATTERY_PERCENTAGE	0x05
    #define BATTERY_TIME        0x06
	#define MAX_POWER           BATTERY_TIME



#define VRC_OHCI				0x0B	// OHCI Class
    #define SET_LED             0x00
    #define INIT_OHCI			0x01
    #define MAX_OHCI            INIT_OHCI

#define VRC_KEYBOARD            0x0C	// Kbd Controller Class
    #define KEYBOARD_PRESENT    0x00
    #define SCANCODE            0x01
    #define MOUSE_PRESENT		0x02
    #define MOUSE_BUTTONS       0x03
	#define MOUSE_XY            0x04
	#define MAX_KEYBOARD        MOUSE_XY


#define VRC_DDC     			0x0D	// Video DDC Class
	#define VRC_DDC_ENABLE 		0x00	// Enable/disable register
		#define DDC_DISABLE		0x00
		#define DDC_ENABLE 		0x01
	#define VRC_DDC_IO	  		0x01	// A non-zero value for safety
	#define MAX_DDC           	VRC_DDC_IO

#define VRC_DEBUGGER			0x0E
	#define MAX_DEBUGGER        NO_VR


#define	VRC_STR					0x0F		// Virtual Register class
	#define RESTORE_ADDR		0x00		// Physical address of MSR restore table


#define	VRC_COP8				0x10		// Virtual Register class
	#define	VRC_HIB_ENABLE		0x00		// HIB enable/disable index
	#define		HIB_ENABLE		0x00		// HIB enable command
	#define		HIB_DISABLE		0x01		// HIB disable command
	#define	VRC_HIB_SEND		0x01		// Send packet to COP8
	#define	VRC_HIB_READUART	0x02		// Read byte from COP8 UART
	#define	VRC_HIB_VERSION		0x03		// Read COP8 version
	#define	VRC_HIB_SERIAL		0x04		// Read 8 byte serial number
	#define	VRC_HIB_USRBTN		0x05		// Read POST button pressed status
	#define MAX_COP8	        NO_VR

#define	VRC_OWL					0x11		// Virtual Register class
	#define	VRC_OWL_DAC			0x00		// DAC (Backlight) Control
	#define	VRC_OWL_GPIO 		0x01		// GPIO Control
	#define MAX_OWL				VRC_OWL_GPIO

#define	VRC_SYSINFO				0x12		// Virtual Register class
	#define	VRC_SI_VERSION				0x00		// Sysinfo VSM version
	#define	VRC_SI_CPU_MHZ				0x01	// CPU speed in MHZ
	#define	VRC_SI_CHIPSET_BASE_LOW		0x02
	#define	VRC_SI_CHIPSET_BASE_HI		0x03
	#define	VRC_SI_CHIPSET_ID			0x04
	#define	VRC_SI_CHIPSET_REV			0x05
	#define	VRC_SI_CPU_ID				0x06
	#define	VRC_SI_CPU_REV				0x07
	#define	MAX_SYSINFO			VRC_SI_CPU_REV

#define VRC_SUPERIO				0x13
	#define VRC_SIO_CHIPID				0x00
	#define VRC_SIO_NUMLD				0x01
	#define VRC_SIO_FDCEN				0x02
	#define VRC_SIO_FDCIO				0x03
	#define VRC_SIO_FDCIRQ				0x04
	#define VRC_SIO_FDCDMA				0x05
	#define VRC_SIO_FDCCFG1				0x06
	#define VRC_SIO_FDCCFG2				0x07
	#define VRC_SIO_PP1EN				0x08
	#define VRC_SIO_PP1IO				0x09
	#define VRC_SIO_PP1IRQ				0x0A
	#define VRC_SIO_PP1DMA				0x0B
	#define VRC_SIO_PP1CFG1				0x0C
	#define VRC_SIO_SP1EN				0x0D
	#define VRC_SIO_SP1IO				0x0E
	#define VRC_SIO_SP1IRQ				0x0F
	#define VRC_SIO_SP1CFG1				0x10
	#define VRC_SIO_SP2EN				0x11
	#define VRC_SIO_SP2IO				0x12
	#define VRC_SIO_SP2IRQ				0x13
	#define VRC_SIO_SP2CFG1				0x14
	#define VRC_SIO_KBEN				0x15
	#define VRC_SIO_KBIO1				0x16
	#define VRC_SIO_KBIO2				0x17
	#define VRC_SIO_KBIRQ				0x18
	#define VRC_SIO_KBCFG1				0x19
	#define VRC_SIO_MSEN				0x1A
	#define VRC_SIO_MSIO				0x1B
	#define VRC_SIO_MSIRQ				0x1C
	#define VRC_SIO_RTCEN				0x1D
	#define VRC_SIO_RTCIO1				0x1E
	#define VRC_SIO_RTCIO2				0x1F
	#define VRC_SIO_RTCIRQ				0x20
	#define VRC_SIO_RTCCFG1				0x21
	#define VRC_SIO_RTCCFG2				0x22
	#define VRC_SIO_RTCCFG3				0x23
	#define VRC_SIO_RTCCFG4				0x24
	#define MAX_SUPERIO			VRC_SIO_RTCCFG4

#define VRC_CHIPSET				0x14
	#define VRC_CS_PWRBTN		0x00
	#define VRC_CS_UART1		0x01
	#define VRC_CS_UART2		0x02
	#define MAX_CHIPSET			VRC_CS_UART2

#define VRC_THERMAL     0x15
    #define VRC_THERMAL_CURR_RTEMP      0x00        // read only
    #define VRC_THERMAL_CURR_LTEMP      0x01        // read only
    #define VRC_THERMAL_FAN             0x02
    #define VRC_THERMAL_LOW_THRESHOLD   0x03
    #define VRC_THERMAL_HIGH_THRESHOLD  0x04
    #define VRC_THERMAL_INDEX           0x05
    #define VRC_THERMAL_DATA            0x06
    #define VRC_THERMAL_SMB_ADDRESS     0x07
    #define VRC_THERMAL_SMB_INDEX       0x08
    #define VRC_THERMAL_SMB_DATA        0x09
    #define MAX_THERMAL         VRC_THERMAL_SMB_DATA

#define MAX_VR_CLASS	  		VRC_THERMAL

/*
 * Write to a Virtual Register
 */
static inline void vrWrite(uint16_t wClassIndex, uint16_t wData)
{
	outl(((uint32_t) VR_UNLOCK << 16) | wClassIndex, VRC_INDEX);
	outw(wData, VRC_DATA);
}

 /*
 * Read from a Virtual Register
 * Returns a 16-bit word of data
 */
static inline uint16_t vrRead(uint16_t wClassIndex)
{
	uint16_t wData;
	outl(((uint32_t) VR_UNLOCK << 16) | wClassIndex, VRC_INDEX);
	wData = inw(VRC_DATA);
	return wData;
}
#endif
