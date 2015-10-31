/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Rockchip Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __DWC2_REGISTERS__
#define __DWC2_REGISTERS__

#define MAX_EPS_CHANNELS 16

typedef struct core_reg {
	uint32_t gotgctl;
	uint32_t gotgint;
	uint32_t gahbcfg;
	uint32_t gusbcfg;
	uint32_t grstctl;
	uint32_t gintsts;
	uint32_t gintmsk;
	uint32_t grxstsr;
	uint32_t grxstsp;
	uint32_t grxfsiz;
	uint32_t gnptxfsiz;
	uint32_t gnptxsts;
	uint32_t gi2cctl;
	uint32_t gpvndctl;
	uint32_t ggpio;
	uint32_t guid;
	uint32_t gsnpsid;
	uint32_t ghwcfg1;
	uint32_t ghwcfg2;
	uint32_t ghwcfg3;
	uint32_t ghwcfg4;
	uint32_t reserved1[(0x100 - 0x54) / 4];
	uint32_t hptxfsiz;
	uint32_t dptxfsiz_dieptxf[MAX_EPS_CHANNELS - 1];
	uint32_t reserved2[(0x400 - 0x140) / 4];
} core_reg_t;

typedef struct hc_reg {
	uint32_t hccharn;
	uint32_t hcspltn;
	uint32_t hcintn;
	uint32_t hcintmaskn;
	uint32_t hctsizn;
	uint32_t hcdman;
	uint32_t reserved[2];
} hc_reg_t;

/* Host Mode Register Structures */
typedef struct host_reg {
	uint32_t hcfg;
	uint32_t hfir;
	uint32_t hfnum;
	uint32_t reserved0;
	uint32_t hptxsts;
	uint32_t haint;
	uint32_t haintmsk;
	uint32_t reserved1[(0x440 - 0x41c) / 4];
	uint32_t hprt;
	uint32_t reserved2[(0x500 - 0x444) / 4];
	hc_reg_t hchn[MAX_EPS_CHANNELS];
	uint32_t reserved3[(0x800 - 0x700) / 4];
} host_reg_t;

typedef struct ep_reg {
	uint32_t depctl;
	uint32_t reserved04;
	uint32_t depint;
	uint32_t reserved0c;
	uint32_t deptsiz;
	uint32_t depdma;
	uint32_t reserved18;
	uint32_t depdmab;
} dwc2_ep_reg_t;

/* Device Mode Registers Structures */
typedef struct device_reg {
	uint32_t dcfg;
	uint32_t dctl;
	uint32_t dsts;
	uint32_t unused;
	uint32_t diepmsk;
	uint32_t doepmsk;
	uint32_t daint;
	uint32_t daintmsk;
	uint32_t dtknqr1;
	uint32_t dtknqr2;
	uint32_t dvbusdis;
	uint32_t dvbuspulse;
	uint32_t dtknqr3_dthrctl;
	uint32_t dtknqr4_fifoemptymsk;
	uint32_t reserved1[(0x900 - 0x838) / 4];

	dwc2_ep_reg_t inep[MAX_EPS_CHANNELS];
	dwc2_ep_reg_t outep[MAX_EPS_CHANNELS];
	uint32_t reserved8[(0xe00 - 0xd00) / 4];
} device_reg_t;

typedef struct pwr_clk_ctrl_reg {
	uint32_t pcgcctl;
	uint32_t reserved[(0x1000 - 0xe04) / 4];
} pwr_clk_ctrl_reg_t;

typedef struct data_fifo {
	uint32_t dataport;
	uint32_t reserved[(0x1000 - 0x004) / 4];
} data_fifo_t;

typedef struct dwc2_otg_reg {
	core_reg_t core;
	host_reg_t host;
	device_reg_t device;
	pwr_clk_ctrl_reg_t pcgr;
	data_fifo_t dfifo[MAX_EPS_CHANNELS];
	uint32_t reserved[(0x40000 - 0x11000) / 4];
} dwc2_reg_t;

/**
 * This union represents the bit fields of the Core AHB Configuration
 * Register (GAHBCFG).
 */
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned glblintrmsk:1;
#define GLBINT_ENABLE 1

		unsigned hbstlen:4;
#define DMA_BURST_SINGLE 0
#define DMA_BURST_INCR 1
#define DMA_BURST_INCR4 3
#define DMA_BURST_INCR8 5
#define DMA_BURST_INCR16 7

		unsigned dmaen:1;
		unsigned reserved:1;
		unsigned nptxfemplvl:1;
		unsigned ptxfemplvl:1;
		unsigned reserved9_31:23;
	};
} gahbcfg_t;

/**
 * This union represents the bit fields of the Core USB Configuration
 * Register (GUSBCFG).
 */
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned toutcal:3;
		unsigned phyif:1;
		unsigned ulpiutmisel:1;
		unsigned fsintf:1;
		unsigned physel:1;
		unsigned ddrsel:1;
		unsigned srpcap:1;
		unsigned hnpcap:1;
		unsigned usbtrdtim:4;
		unsigned reserved14:1;
		unsigned phylpwrclksel:1;
		unsigned otgi2csel:1;
		unsigned ulpifsls:1;
		unsigned ulpiautores:1;
		unsigned ulpiclksusm:1;
		unsigned ulpiextvbusdrv:1;
		unsigned ulpiextvbusindicator:1;
		unsigned termseldlpulse:1;
		unsigned reserved23_28:6;
		unsigned forcehstmode:1;
		unsigned forcedevmode:1;
		unsigned cortxpkt:1;
	};
} gusbcfg_t;

/**
 * This union represents the bit fields of the Core Reset Register
 * (GRSTCTL).
 */
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		/** Core Soft Reset (CSftRst) (Device and Host)
		 *
		 * The application can flush the control logic in the
		 * entire core using this bit. This bit resets the
		 * pipelines in the AHB Clock domain as well as the
		 * PHY Clock domain.
		 *
		 * The state machines are reset to an IDLE state, the
		 * control bits in the CSRs are cleared, all the
		 * transmit FIFOs and the receive FIFO are flushed.
		 *
		 * The status mask bits that control the generation of
		 * the interrupt, are cleared, to clear the
		 * interrupt. The interrupt status bits are not
		 * cleared, so the application can get the status of
		 * any events that occurred in the core after it has
		 * set this bit.
		 *
		 * Any transactions on the AHB are terminated as soon
		 * as possible following the protocol. Any
		 * transactions on the USB are terminated immediately.
		 *
		 * The configuration settings in the CSRs are
		 * unchanged, so the software doesn't have to
		 * reprogram these registers (Device
		 * Configuration/Host Configuration/Core System
		 * Configuration/Core PHY Configuration).
		 *
		 * The application can write to this bit, any time it
		 * wants to reset the core. This is a self clearing
		 * bit and the core clears this bit after all the
		 * necessary logic is reset in the core, which may
		 * take several clocks, depending on the current state
		 * of the core.
		 */
		unsigned csftrst:1;
		/** Hclk Soft Reset
		 *
		 * The application uses this bit to reset the control logic in
		 * the AHB clock domain. Only AHB clock domain pipelines are
		 * reset.
		 */
		unsigned hsftrst:1;
		/** Host Frame Counter Reset (Host Only)<br>
		 *
		 * The application can reset the (micro)frame number
		 * counter inside the core, using this bit. When the
		 * (micro)frame counter is reset, the subsequent SOF
		 * sent out by the core, will have a (micro)frame
		 * number of 0.
		 */
		unsigned frmcntrrst:1;
		/** In Token Sequence Learning Queue Flush
		 * (INTknQFlsh) (Device Only)
		 */
		unsigned intknqflsh:1;
		/** RxFIFO Flush (RxFFlsh) (Device and Host)
		 *
		 * The application can flush the entire Receive FIFO
		 * using this bit.	<p>The application must first
		 * ensure that the core is not in the middle of a
		 * transaction.	 <p>The application should write into
		 * this bit, only after making sure that neither the
		 * DMA engine is reading from the RxFIFO nor the MAC
		 * is writing the data in to the FIFO.	<p>The
		 * application should wait until the bit is cleared
		 * before performing any other operations. This bit
		 * will takes 8 clocks (slowest of PHY or AHB clock)
		 * to clear.
		 */
		unsigned rxfflsh:1;
		/** TxFIFO Flush (TxFFlsh) (Device and Host).
		 *
		 * This bit is used to selectively flush a single or
		 * all transmit FIFOs.	The application must first
		 * ensure that the core is not in the middle of a
		 * transaction.	 <p>The application should write into
		 * this bit, only after making sure that neither the
		 * DMA engine is writing into the TxFIFO nor the MAC
		 * is reading the data out of the FIFO.	 <p>The
		 * application should wait until the core clears this
		 * bit, before performing any operations. This bit
		 * will takes 8 clocks (slowest of PHY or AHB clock)
		 * to clear.
		 */
		unsigned txfflsh:1;

		/** TxFIFO Number (TxFNum) (Device and Host).
		 *
		 * This is the FIFO number which needs to be flushed,
		 * using the TxFIFO Flush bit. This field should not
		 * be changed until the TxFIFO Flush bit is cleared by
		 * the core.
		 *	 - 0x0 : Non Periodic TxFIFO Flush
		 *	 - 0x1 : Periodic TxFIFO #1 Flush in device mode
		 *	   or Periodic TxFIFO in host mode
		 *	 - 0x2 : Periodic TxFIFO #2 Flush in device mode.
		 *	 - ...
		 *	 - 0xF : Periodic TxFIFO #15 Flush in device mode
		 *	 - 0x10: Flush all the Transmit NonPeriodic and
		 *	   Transmit Periodic FIFOs in the core
		 */
		unsigned txfnum:5;
		/** Reserved */
		unsigned reserved11_29:19;
		/** DMA Request Signal.	 Indicated DMA request is in
		 * probress.  Used for debug purpose. */
		unsigned dmareq:1;
		/** AHB Master Idle.  Indicates the AHB Master State
		 * Machine is in IDLE condition. */
		unsigned ahbidle:1;
	};
} grstctl_t;

/**
 * This union represents the bit fields of the Core Interrupt Mask
 * Register (GINTMSK).
 */
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned curmod:1;
		unsigned modemis:1;
		unsigned otgint:1;
		unsigned sof:1;
		unsigned rxflvl:1;
		unsigned nptxfemp:1;
		unsigned ginnakeff:1;
		unsigned goutnakeff:1;
		unsigned reserved8:1;
		unsigned i2cint:1;
		unsigned erlysusp:1;
		unsigned usbsusp:1;
		unsigned usbrst:1;
		unsigned enumdone:1;
		unsigned isooutdrop:1;
		unsigned eopf:1;
		unsigned reserved16:1;
		unsigned epmis:1;
		unsigned iepint:1;
		unsigned oepint:1;
		unsigned incompisoin:1;
		unsigned incompip:1;
		unsigned reserved22_23:2;
		unsigned prtint:1;
		unsigned hchint:1;
		unsigned ptxfemp:1;
		unsigned reserved27:1;
		unsigned conidstschng:1;
		unsigned disconnint:1;
		unsigned sessreqint:1;
		unsigned wkupint:1;
	};
} gintmsk_t;

/**
* This union represents the bit fields of the Core Non-Periodic
* Transmit FIFO Size Register(GNPTXFSIZ).
*/
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned txfstaddr:16;
		unsigned txfdep:16;
	};
} gtxfsiz_t;

/**
 * This union represents the bit fields of the Core Receive FIFO Size
 * Register(GRXFSIZ).
 */
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	/*The value in this fieles is in terms of 32-bit words size.
	 */
	struct {
		unsigned rxfdep:16;
		unsigned reserved:16;
	};
} grxfsiz_t;

/**
* This union represents the bit fields of the Core Device
* Transmit FIFO Size Register(GNPTXFSIZ).
*/
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned dtxfstaddr:16;
		unsigned dtxfdep:16;
	};
} dtxfsiz_t;

/**
 * This union represents the bit fields of the Core Interrupt Register
 * (GINTSTS).
 */
typedef union {
	/* raw register data */
	uint32_t d32;
#define SOF_INTR_MASK 0x0008
	/* register bits */
	struct {
		unsigned curmod:1;
#define HOST_MODE 1
#define DEVICE_MODE 0
		unsigned modemis:1;
		unsigned otgint:1;
		unsigned sof:1;
		unsigned rxflvl:1;
		unsigned nptxfemp:1;
		unsigned ginnakeff:1;
		unsigned goutnakeff:1;
		unsigned reserved8:1;
		unsigned i2cint:1;
		unsigned erlysusp:1;
		unsigned usbsusp:1;
		unsigned usbrst:1;
		unsigned enumdone:1;
		unsigned isooutdrop:1;
		unsigned eopf:1;
		unsigned reserved16_17:2;
		unsigned iepint:1;
		unsigned oepint:1;
		unsigned reserved20:1;
		unsigned incompip:1;
		unsigned reserved22_23:2;
		unsigned prtint:1;
		unsigned hchint:1;
		unsigned ptxfemp:1;
		unsigned reserved27:1;
		unsigned conidstschng:1;
		unsigned disconnint:1;
		unsigned sessreqint:1;
		unsigned wkupint:1;
	};
} gintsts_t;

#define GINTSTS_WKUPINT			(1 << 31)
#define GINTSTS_SESSREQINT		(1 << 30)
#define GINTSTS_DISCONNINT		(1 << 29)
#define GINTSTS_CONIDSTSCHNG		(1 << 28)
#define GINTSTS_LPMTRANRCVD		(1 << 27)
#define GINTSTS_PTXFEMP			(1 << 26)
#define GINTSTS_HCHINT			(1 << 25)
#define GINTSTS_PRTINT			(1 << 24)
#define GINTSTS_RESETDET		(1 << 23)
#define GINTSTS_FET_SUSP		(1 << 22)
#define GINTSTS_INCOMPL_IP		(1 << 21)
#define GINTSTS_INCOMPL_SOIN		(1 << 20)
#define GINTSTS_OEPINT			(1 << 19)
#define GINTSTS_IEPINT			(1 << 18)
#define GINTSTS_EPMIS			(1 << 17)
#define GINTSTS_RESTOREDONE		(1 << 16)
#define GINTSTS_EOPF			(1 << 15)
#define GINTSTS_ISOUTDROP		(1 << 14)
#define GINTSTS_ENUMDONE		(1 << 13)
#define GINTSTS_USBRST			(1 << 12)
#define GINTSTS_USBSUSP			(1 << 11)
#define GINTSTS_ERLYSUSP		(1 << 10)
#define GINTSTS_I2CINT			(1 << 9)
#define GINTSTS_ULPI_CK_INT		(1 << 8)
#define GINTSTS_GOUTNAKEFF		(1 << 7)
#define GINTSTS_GINNAKEFF		(1 << 6)
#define GINTSTS_NPTXFEMP		(1 << 5)
#define GINTSTS_RXFLVL			(1 << 4)
#define GINTSTS_SOF			(1 << 3)
#define GINTSTS_OTGINT			(1 << 2)
#define GINTSTS_MODEMIS			(1 << 1)
#define GINTSTS_CURMODE_HOST		(1 << 0)

/**
 * This union represents the bit fields of the User HW Config3 Register
 * (GHWCFG3).
 */
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned reserved:16;
		unsigned dfifodepth:16;
	};
} ghwcfg3_t;

/**
 * This union represents the bit fields in the Host Configuration Register.
 */
typedef union {
	/* raw register data */
	uint32_t d32;

	/* register bits */
	struct {
		/** FS/LS Phy Clock Select */
		unsigned fslspclksel:2;
#define PHYCLK_30_60_MHZ 0
#define PHYCLK_48_MHZ 1
#define PHYCLK_6_MHZ 2

		/** FS/LS Only Support */
		unsigned fslssupp:1;
	};
} hcfg_t;

/**
 * This union represents the bit fields in the Host Frame Number/Frame Time
 * Remaining Register
 */
typedef union {
	/* raw register data */
	uint32_t d32;

	/* register bits */
	struct {
		/** Frame Number */
		unsigned frnum:16;
		/** Frame Time Remaining */
		unsigned frrem:16;
	};
} hfnum_t;

/**
 * This union represents the bit fields in the Host Port Control and status
 * Register.
 */
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned prtconnsts:1;
		unsigned prtconndet:1;
		unsigned prtena:1;
		unsigned prtenchng:1;
		unsigned prtovrcurract:1;
		unsigned prtovrcurrchng:1;
		unsigned prtres:1;
		unsigned prtsusp:1;
		unsigned prtrst:1;
		unsigned reserved9:1;
		unsigned prtlnsts:2;
		unsigned prtpwr:1;
		unsigned prttstctl:4;
		unsigned prtspd:2;
#define PRTSPD_HIGH 0
#define PRTSPD_FULL 1
#define PRTSPD_LOW	2
		unsigned reserved19_31:13;
	};
} hprt_t;
/* Mask W1C bits */
#define HPRT_W1C_MASK (~((1 << 1) | (1 << 2) | (1 << 3) | (1 << 5)))

/**
 * This union represents the bit fields in the Host Channel Characteristics
 * Register.
 */
typedef union {
	/* raw register data */
	uint32_t d32;

	/* register bits */
	struct {
		/** Maximum packet size in bytes */
		unsigned mps:11;
		/** Endpoint number */
		unsigned epnum:4;
		/** 0: OUT, 1: IN */
		unsigned epdir:1;
		unsigned reserved:1;
		/** 0: Full/high speed device, 1: Low speed device */
		unsigned lspddev:1;
		/** 0: Control, 1: Isoc, 2: Bulk, 3: Intr */
		unsigned eptype:2;
		/** Packets per frame for periodic transfers. 0 is reserved. */
		unsigned multicnt:2;
		/** Device address */
		unsigned devaddr:7;
		/**
		 * Frame to transmit periodic transaction.
		 * 0: even, 1: odd
		 */
		unsigned oddfrm:1;
		/** Channel disable */
		unsigned chdis:1;
		/** Channel enable */
		unsigned chen:1;
	};
} hcchar_t;

/**
 * This union represents the bit fields in the Host Channel-n Split Control
 * Register.
 */
typedef union {
	/* raw register data */
	uint32_t d32;

	/* register bits */
	struct {
		/** Port Address */
		unsigned prtaddr:7;
		/** Hub Address */
		unsigned hubaddr:7;
		/** Transaction Position */
		unsigned xactpos:2;
		/** Do Complete Split */
		unsigned compsplt:1;
		unsigned reserved:14;
		/** Split Enable */
		unsigned spltena:1;
	};
} hcsplit_t;

typedef enum {
	EPDIR_OUT = 0,
	EPDIR_IN,
} ep_dir_t;

/**
 * This union represents the bit fields in the Host All Interrupt
 * Register.
 */
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		/** Transfer Complete */
		unsigned xfercomp:1;
		/** Channel Halted */
		unsigned chhltd:1;
		/** AHB Error */
		unsigned ahberr:1;
		/** STALL Response Received */
		unsigned stall:1;
		/** NAK Response Received */
		unsigned nak:1;
		/** ACK Response Received */
		unsigned ack:1;
		/** NYET Response Received */
		unsigned nyet:1;
		/** Transaction Err */
		unsigned xacterr:1;
		/** Babble Error */
		unsigned bblerr:1;
		/** Frame Overrun */
		unsigned frmovrun:1;
		/** Data Toggle Error */
		unsigned datatglerr:1;
		/** Reserved */
		unsigned reserved:21;
	};
} hcint_t;

/**
 * This union represents the bit fields in the Host Channel Transfer Size
 * Register.
 */
typedef union {
	/* raw register data */
	uint32_t d32;

	/* register bits */
	struct {
		/* Total transfer size in bytes */
		unsigned xfersize:19;
		/** Data packets to transfer */
		unsigned pktcnt:10;
		/**
		 * Packet ID for next data packet
		 * 0: DATA0
		 * 1: DATA2
		 * 2: DATA1
		 * 3: MDATA (non-Control), SETUP (Control)
		 */
		unsigned pid:2;
#define PID_DATA0 0
#define PID_DATA1 2
#define PID_DATA2 1
#define PID_MDATA 3
#define PID_SETUP 3
		/* Do PING protocol when 1 */
		unsigned dopng:1;
	};
} hctsiz_t;

/**
 * This union represents the bit fields in the Host Channel Interrupt Mask
 * Register.
 */
typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned xfercomp:1;
		unsigned chhltd:1;
		unsigned ahberr:1;
		unsigned stall:1;
		unsigned nak:1;
		unsigned ack:1;
		unsigned nyet:1;
		unsigned xacterr:1;
		unsigned bblerr:1;
		unsigned frmovrun:1;
		unsigned datatglerr:1;
		unsigned reserved:21;
	};
} hcintmsk_t;

typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned devspd:2;
		unsigned nzstsouthshk:1;
		unsigned ena32ksusp:1;
		unsigned devaddr:7;
		unsigned perfrint:2;
		unsigned endevoutnak:1;
		unsigned reservedi14_17:4;
		unsigned epmiscnt:5;
		unsigned reserved26_31:9;
	};
} dcfg_t;

typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned suspsts:1;
		unsigned enumspd:2;
		unsigned errticerr:1;
		unsigned reserved4_31:28;
	};
} dsts_t;

typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned rmtwkupsig:1;
		unsigned sftdiscon:1;
		unsigned gnpinnaksts:1;
		unsigned goutnaksts:1;
		unsigned tstctl:3;
		unsigned sgnpinnak:1;
		unsigned cgnpinnak:1;
		unsigned sgoutnak:1;
		unsigned cgoutnak:1;
		unsigned pwronprgdone:1;
		unsigned reserved12:1;
		unsigned gmc:2;
		unsigned ignrfrmnum:1;
		unsigned nakonbble:1;
		unsigned encontbna:1;
		unsigned reserved19_31:14;
	};
} dctl_t;

typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
#define D0EPCTL_MPS_SHIFT		0
#define D0EPCTL_MPS_64			0
#define D0EPCTL_MPS_32			1
#define D0EPCTL_MPS_16			2
#define D0EPCTL_MPS_8			3
		unsigned mps:11;
		unsigned nextep:4;
		unsigned usbactep:1;
		unsigned dpid:1;
		unsigned naksts:1;
		unsigned eptype:2;
		unsigned reserved20:1;
		unsigned stall:1;
		unsigned txfnum:4;
		unsigned cnak:1;
		unsigned snak:1;
		unsigned setd0pid:1;
		unsigned setd1pid:1;
		unsigned epdis:1;
		unsigned epena:1;
	};
} depctl_t;

typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned xfercompl:1;
		unsigned epdisbld:1;
		unsigned ahberr:1;
		unsigned setup:1;
		unsigned reserved4_14:12;
		unsigned stuppktrcvd:1;
		unsigned reserved16_31:15;
	};
} depint_t;

#define DXEPINT_SETUP_RCVD		(1 << 15)
#define DXEPINT_INEPNAKEFF		(1 << 6)
#define DXEPINT_BACK2BACKSETUP		(1 << 6)
#define DXEPINT_INTKNEPMIS		(1 << 5)
#define DXEPINT_INTKNTXFEMP		(1 << 4)
#define DXEPINT_OUTTKNEPDIS		(1 << 4)
#define DXEPINT_TIMEOUT			(1 << 3)
#define DXEPINT_SETUP			(1 << 3)
#define DXEPINT_AHBERR			(1 << 2)
#define DXEPINT_EPDISBLD		(1 << 1)
#define DXEPINT_XFERCOMPL		(1 << 0)

typedef union {
	/* raw register data */
	uint32_t d32;
	/* register bits */
	struct {
		unsigned xfersize:19;
		unsigned pktcnt:10;
		unsigned mc:2;
		unsigned reserved31:1;
	};
} depsiz_t;

#define DAINT_OUTEP_SHIFT	MAX_EPS_CHANNELS

#endif
