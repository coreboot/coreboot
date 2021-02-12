/* SPDX-License-Identifier: GPL-2.0-only */

/* DefinitionBlock Statement */
#include <acpi/acpi.h>
DefinitionBlock (
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	#include <acpi/dsdt_top.asl>

	/* Data to be patched by the BIOS during POST */
	/* FIXME the patching is not done yet! */
	/* Memory related values */
	Name(LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */
	Name(PBAD, 0x0)	/* Address of BIOS area (If TOM2 != 0, Addr >> 16) */
	Name(PBLN, 0x0)	/* Length of BIOS area */

	Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)	/* Base address of PCIe config space */
	Name(HPBA, 0xFED00000)	/* Base address of HPET table */

	/* USB overcurrent mapping pins.   */
	Name(UOM0, 0)
	Name(UOM1, 2)
	Name(UOM2, 0)
	Name(UOM3, 7)
	Name(UOM4, 2)
	Name(UOM5, 2)
	Name(UOM6, 6)
	Name(UOM7, 2)
	Name(UOM8, 6)
	Name(UOM9, 6)

	/* Some global data */
	Name(OSVR, 3)		/* Assume nothing. WinXp = 1, Vista = 2, Linux = 3, WinCE = 4 */
	Name(OSV, Ones)	/* Assume nothing */

	/*
	 * Processor Object
	 *
	 */
	Scope (\_SB) {		/* define processor scope */
		Device (C000) {
		Name (_HID, "ACPI0007")
		Name (_UID, 0)
		}
		Device (C001) {
		Name (_HID, "ACPI0007")
		Name (_UID, 1)
		}
		Device (C002) {
		Name (_HID, "ACPI0007")
		Name (_UID, 2)
		}
		Device (C003) {
		Name (_HID, "ACPI0007")
		Name (_UID, 3)
		}
	} /* End _SB scope */

	/* Client Management index/data registers */
	OperationRegion(CMT, SystemIO, 0x00000C50, 0x00000002)
		Field(CMT, ByteAcc, NoLock, Preserve) {
		CMTI,      8,
		/* Client Management Data register */
		G64E,   1,
		G64O,      1,
		G32O,      2,
		,       2,
		GPSL,     2,
	}

	/* GPM Port register */
	OperationRegion(GPT, SystemIO, 0x00000C52, 0x00000001)
		Field(GPT, ByteAcc, NoLock, Preserve) {
		GPB0,1,
		GPB1,1,
		GPB2,1,
		GPB3,1,
		GPB4,1,
		GPB5,1,
		GPB6,1,
		GPB7,1,
	}

	/* Flash ROM program enable register */
	OperationRegion(FRE, SystemIO, 0x00000C6F, 0x00000001)
		Field(FRE, ByteAcc, NoLock, Preserve) {
		,     0x00000006,
		FLRE, 0x00000001,
	}

	/* PM2 index/data registers */
	OperationRegion(PM2R, SystemIO, 0x00000CD0, 0x00000002)
		Field(PM2R, ByteAcc, NoLock, Preserve) {
		PM2I, 0x00000008,
		PM2D, 0x00000008,
	}

	/* Power Management I/O registers, TODO:PMIO is quite different in SB800. */
	OperationRegion(PIOR, SystemIO, 0x00000CD6, 0x00000002)
		Field(PIOR, ByteAcc, NoLock, Preserve) {
		PIOI, 0x00000008,
		PIOD, 0x00000008,
	}
	IndexField (PIOI, PIOD, ByteAcc, NoLock, Preserve) {
		    , 1,	/* MiscControl */
		T1EE, 1,
		T2EE, 1,
		Offset(0x01),	/* MiscStatus */
		, 1,
		T1E, 1,
		T2E, 1,
		Offset(0x04),	/* SmiWakeUpEventEnable3 */
		, 7,
		SSEN, 1,
		Offset(0x07),	/* SmiWakeUpEventStatus3 */
		, 7,
		CSSM, 1,
		Offset(0x10),	/* AcpiEnable */
		, 6,
		PWDE, 1,
		Offset(0x1C),	/* ProgramIoEnable */
		, 3,
		MKME, 1,
		IO3E, 1,
		IO2E, 1,
		IO1E, 1,
		IO0E, 1,
		Offset(0x1D),	/* IOMonitorStatus */
		, 3,
		MKMS, 1,
		IO3S, 1,
		IO2S, 1,
		IO1S, 1,
		IO0S,1,
		Offset(0x20),	/* AcpiPmEvtBlk. TODO: should be 0x60 */
		APEB, 16,
		Offset(0x36),	/* GEvtLevelConfig */
		, 6,
		ELC6, 1,
		ELC7, 1,
		Offset(0x37),	/* GPMLevelConfig0 */
		, 3,
		PLC0, 1,
		PLC1, 1,
		PLC2, 1,
		PLC3, 1,
		PLC8, 1,
		Offset(0x38),	/* GPMLevelConfig1 */
		, 1,
		 PLC4, 1,
		 PLC5, 1,
		, 1,
		 PLC6, 1,
		 PLC7, 1,
		Offset(0x3B),	/* PMEStatus1 */
		GP0S, 1,
		GM4S, 1,
		GM5S, 1,
		APS, 1,
		GM6S, 1,
		GM7S, 1,
		GP2S, 1,
		STSS, 1,
		Offset(0x55),	/* SoftPciRst */
		SPRE, 1,
		, 1,
		, 1,
		PNAT, 1,
		PWMK, 1,
		PWNS, 1,

		Offset(0x65),	/* UsbPMControl */
		, 4,
		URRE, 1,
		Offset(0x68),	/* MiscEnable68 */
		, 3,
		TMTE, 1,
		, 1,
		Offset(0x92),	/* GEVENTIN */
		, 7,
		E7IS, 1,
		Offset(0x96),	/* GPM98IN */
		G8IS, 1,
		G9IS, 1,
		Offset(0x9A),	/* EnhanceControl */
		,7,
		HPDE, 1,
		Offset(0xA8),	/* PIO7654Enable */
		IO4E, 1,
		IO5E, 1,
		IO6E, 1,
		IO7E, 1,
		Offset(0xA9),	/* PIO7654Status */
		IO4S, 1,
		IO5S, 1,
		IO6S, 1,
		IO7S, 1,
	}

	/* PM1 Event Block
	* First word is PM1_Status, Second word is PM1_Enable
	*/
	OperationRegion(P1EB, SystemIO, APEB, 0x04)
		Field(P1EB, ByteAcc, NoLock, Preserve) {
		TMST, 1,
		,    3,
		BMST,    1,
		GBST,   1,
		Offset(0x01),
		PBST, 1,
		, 1,
		RTST, 1,
		, 3,
		PWST, 1,
		SPWS, 1,
		Offset(0x02),
		TMEN, 1,
		, 4,
		GBEN, 1,
		Offset(0x03),
		PBEN, 1,
		, 1,
		RTEN, 1,
		, 3,
		PWDA, 1,
	}

	#include "acpi/routing.asl"

	#include <southbridge/amd/cimx/sb800/acpi/pcie.asl>

	Scope(\_SB) {

		Method(OSFL, 0){

			if (OSVR != Ones) {Return(OSVR)}	/* OS version was already detected */

			if(CondRefOf(\_OSI))
			{
				OSVR = 1                /* Assume some form of XP */
				if (\_OSI("Windows 2006"))      /* Vista */
				{
					OSVR = 2
				}
			} else {
				If(WCMP(\_OS,"Linux")) {
					OSVR = 3            /* Linux */
				} Else {
					OSVR = 4            /* Gotta be WinCE */
				}
			}
			Return(OSVR)
		}

	}   /* End Scope(_SB)  */

	/* Contains the supported sleep states for this chipset */
	#include <southbridge/amd/common/acpi/sleepstates.asl>

	/* Wake status package */
	Name(WKST,Package(){Zero, Zero})

	/*
	* \_PTS - Prepare to Sleep method
	*
	*	Entry:
	*		Arg0=The value of the sleeping state S1=1, S2=2, etc
	*
	* Exit:
	*		-none-
	*
	* The _PTS control method is executed at the beginning of the sleep process
	* for S1-S5. The sleeping value is passed to the _PTS control method.  This
	* control method may be executed a relatively long time before entering the
	* sleep state and the OS may abort the operation without notification to
	* the ACPI driver.  This method cannot modify the configuration or power
	* state of any device in the system.
	*/
	Method(\_PTS, 1) {

		/* Don't allow PCIRST# to reset USB */
		if (Arg0 == 3){
			URRE = 0
		}

		/* Clear wake status structure. */
		WKST [0] = 0
		WKST [1] = 0
	} /* End Method(\_PTS) */

	/*
	*  \_WAK System Wake method
	*
	*	Entry:
	*		Arg0=The value of the sleeping state S1=1, S2=2
	*
	*	Exit:
	*		Return package of 2 DWords
	*		Dword 1 - Status
	*			0x00000000	wake succeeded
	*			0x00000001	Wake was signaled but failed due to lack of power
	*			0x00000002	Wake was signaled but failed due to thermal condition
	*		Dword 2 - Power Supply state
	*			if non-zero the effective S-state the power supply entered
	*/
	Method(\_WAK, 1) {

		/* Re-enable HPET */
		HPDE = 1

		/* Restore PCIRST# so it resets USB */
		if (Arg0 == 3){
			URRE = 1
		}

		/* Arbitrarily clear PciExpWakeStatus */
		Local1 = PWST
		PWST = Local1

		Return(WKST)
	} /* End Method(\_WAK) */

	Scope(\_GPE) {	/* Start Scope GPE */

		/*  General event 3  */
		Method(_L03) {
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  Legacy PM event  */
		Method(_L08) {
		}

		/*  Temp warning (TWarn) event  */
		Method(_L09) {
		}

		/*  USB controller PME#  */
		Method(_L0B) {
			Notify(\_SB.PCI0.UOH1, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH2, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH3, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH4, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH5, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH6, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UEH1, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  ExtEvent0 SCI event  */
		Method(_L10) {
		}

		/*  ExtEvent1 SCI event  */
		Method(_L11) {
		}

		/*  GPIO0 or GEvent8 event  */
		Method(_L18) {
			Notify(\_SB.PCI0.PBR2, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR4, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR5, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR6, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR7, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  Azalia SCI event  */
		Method(_L1B) {
			Notify(\_SB.PCI0.AZHD, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}
	}	/* End Scope GPE */

	#include "acpi/usb.asl"

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		#include <arch/x86/acpi/globutil.asl> /* global utility methods expected within the \_SB scope */

		/*  _SB.PCI0 */
		/* Note: Only need HID on Primary Bus */
		Device(PCI0) {
			External (TOM1)
			External (TOM2)
			Name(_HID, EISAID("PNP0A08"))	/* PCI Express Root Bridge */
			Name(_CID, EISAID("PNP0A03"))	/* PCI Root Bridge */

			/* Operating System Capabilities Method */
			Method (_OSC, 4)
			{
				/* Check for PCI/PCI-X/PCIe GUID */
				If (Arg0 == ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
				{
					/* Let OS control everything */
					Return (Arg3)
				}
				Else
				{
					/* Unrecognized UUID, so set bit 2 of Arg3 to 1 */
					CreateDWordField (Arg3, 0, CDW1)
					CDW1 |= 4
					Return (Arg3)
				}
			} /* End _OSC */

			Method(_BBN, 0) { /* Bus number = 0 */
				Return(0)
			}
			Method(_STA, 0) {
				Return(0x0B)     /* Status is visible */
			}

			Method(_PRT,0) {
				If(PICM){ Return(APR0) }   /* APIC mode */
				Return (PR0)                  /* PIC Mode */
			} /* end _PRT */

			/* Describe the Northbridge devices */
			Device(AMRT) {
				Name(_ADR, 0x00000000)
			} /* end AMRT */

			/* The internal GFX bridge */
			Device(AGPB) {
				Name(_ADR, 0x00010000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					Return (APR1)
				}
			}  /* end AGPB */

			/* The external GFX bridge */
			Device(PBR2) {
				Name(_ADR, 0x00020000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APS2) }   /* APIC mode */
					Return (PS2)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR2 */

			/* Dev3 is also an external GFX bridge, not used in Herring */

			Device(PBR4) {
				Name(_ADR, 0x00040000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APS4) }   /* APIC mode */
					Return (PS4)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR4 */

			Device(PBR5) {
				Name(_ADR, 0x00050000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APS5) }   /* APIC mode */
					Return (PS5)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR5 */

			Device(PBR6) {
				Name(_ADR, 0x00060000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APS6) }   /* APIC mode */
					Return (PS6)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR6 */

			/* The onboard EtherNet chip */
			Device(PBR7) {
				Name(_ADR, 0x00070000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APS7) }   /* APIC mode */
					Return (PS7)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR7 */

			/* GPP */
			Device(PBR9) {
				Name(_ADR, 0x00090000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APS9) }   /* APIC mode */
					Return (PS9)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBR9 */

			Device(PBRa) {
				Name(_ADR, 0x000A0000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APSa) }   /* APIC mode */
					Return (PSa)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PBRa */

			Device(PE20) {
				Name(_ADR, 0x00150000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APE0) }   /* APIC mode */
					Return (PE0)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PE20 */
			Device(PE21) {
				Name(_ADR, 0x00150001)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APE1) }   /* APIC mode */
					Return (PE1)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PE21 */
			Device(PE22) {
				Name(_ADR, 0x00150002)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APE2) }   /* APIC mode */
					Return (APE2)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PE22 */
			Device(PE23) {
				Name(_ADR, 0x00150003)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PICM){ Return(APE3) }   /* APIC mode */
					Return (PE3)                  /* PIC Mode */
				} /* end _PRT */
			} /* end PE23 */

			/* PCI slot 1, 2, 3 */
			Device(PIBR) {
				Name(_ADR, 0x00140004)
				Name(_PRW, Package() {0x18, 4})

				Method(_PRT, 0) {
					Return (PCIB)
				}
			}

			/* Describe the Southbridge devices */
			Device(STCR) {
				Name(_ADR, 0x00110000)
				#include "acpi/sata.asl"
			} /* end STCR */

			Device(UOH1) {
				Name(_ADR, 0x00120000)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH1 */

			Device(UOH2) {
				Name(_ADR, 0x00120002)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH2 */

			Device(UOH3) {
				Name(_ADR, 0x00130000)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH3 */

			Device(UOH4) {
				Name(_ADR, 0x00130002)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH4 */

			Device(UOH5) {
				Name(_ADR, 0x00160000)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH5 */

			Device(UOH6) {
				Name(_ADR, 0x00160002)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UOH5 */

			Device(UEH1) {
				Name(_ADR, 0x00140005)
				Name(_PRW, Package() {0x0B, 3})
			} /* end UEH1 */

			Device(SBUS) {
				Name(_ADR, 0x00140000)
			} /* end SBUS */

			Device(AZHD) {
				Name(_ADR, 0x00140002)
				OperationRegion(AZPD, PCI_Config, 0x00, 0x100)
					Field(AZPD, AnyAcc, NoLock, Preserve) {
					offset (0x42),
					NSDI, 1,
					NSDO, 1,
					NSEN, 1,
					offset (0x44),
					IPCR, 4,
					offset (0x54),
					PWST, 2,
					, 6,
					PMEB, 1,
					, 6,
					PMST, 1,
					offset (0x62),
					MMCR, 1,
					offset (0x64),
					MMLA, 32,
					offset (0x68),
					MMHA, 32,
					offset (0x6C),
					MMDT, 16,
				}
			} /* end AZHD */

			Device(LIBR) {
				Name(_ADR, 0x00140003)

				/* Real Time Clock Device */
				Device(RTC0) {
					Name(_HID, EISAID("PNP0B00"))   /* AT Real Time Clock (not PIIX4 compatible) */
					Name(_CRS, ResourceTemplate() {
						IRQNoFlags(){8}
						IO(Decode16,0x0070, 0x0070, 0, 2)
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.RTC0) */

				Device(TMR) {	/* Timer */
					Name(_HID,EISAID("PNP0100"))	/* System Timer */
					Name(_CRS, ResourceTemplate() {
						IRQNoFlags(){0}
						IO(Decode16, 0x0040, 0x0040, 0, 4)
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.TMR) */

				Device(SPKR) {	/* Speaker */
					Name(_HID,EISAID("PNP0800"))	/* AT style speaker */
					Name(_CRS, ResourceTemplate() {
						IO(Decode16, 0x0061, 0x0061, 0, 1)
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.SPKR) */

				Device(PIC) {
					Name(_HID,EISAID("PNP0000"))	/* AT Interrupt Controller */
					Name(_CRS, ResourceTemplate() {
						IRQNoFlags(){2}
						IO(Decode16,0x0020, 0x0020, 0, 2)
						IO(Decode16,0x00A0, 0x00A0, 0, 2)
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.PIC) */

				Device(MAD) { /* 8257 DMA */
					Name(_HID,EISAID("PNP0200"))	/* Hardware Device ID */
					Name(_CRS, ResourceTemplate() {
						DMA(Compatibility,BusMaster,Transfer8){4}
						IO(Decode16, 0x0000, 0x0000, 0x10, 0x10)
						IO(Decode16, 0x0081, 0x0081, 0x01, 0x03)
						IO(Decode16, 0x0087, 0x0087, 0x01, 0x01)
						IO(Decode16, 0x0089, 0x0089, 0x01, 0x03)
						IO(Decode16, 0x008F, 0x008F, 0x01, 0x01)
						IO(Decode16, 0x00C0, 0x00C0, 0x10, 0x20)
					}) /* End Name(_SB.PCI0.LpcIsaBr.MAD._CRS) */
				} /* End Device(_SB.PCI0.LpcIsaBr.MAD) */

				Device(COPR) {
					Name(_HID,EISAID("PNP0C04"))	/* Math Coprocessor */
					Name(_CRS, ResourceTemplate() {
						IO(Decode16, 0x00F0, 0x00F0, 0, 0x10)
						IRQNoFlags(){13}
					})
				} /* End Device(_SB.PCI0.LpcIsaBr.COPR) */
				#include "acpi/superio.asl"
			} /* end LIBR */

			Device(HPBR) {
				Name(_ADR, 0x00140004)
			} /* end HostPciBr */

			Device(ACAD) {
				Name(_ADR, 0x00140005)
			} /* end Ac97audio */

			Device(ACMD) {
				Name(_ADR, 0x00140006)
			} /* end Ac97modem */

			Name(CRES, ResourceTemplate() {
				/* Set the Bus number and Secondary Bus number for the PCI0 device
				 * The Secondary bus range for PCI0 lets the system
				 * know what bus values are allowed on the downstream
				 * side of this PCI bus if there is a PCI-PCI bridge.
				 * PCI busses can have 256 secondary busses which
				 * range from [0-0xFF] but they do not need to be
				 * sequential.
				 */
				WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
					0x0000,		/* address granularity */
					0x0000,		/* range minimum */
					0x00FF,		/* range maximum */
					0x0000,		/* translation */
					0x0100,		/* length */
					,, PSB0)	/* ResourceSourceIndex, ResourceSource, DescriptorName */

				IO(Decode16, 0x004E, 0x004E, 1, 2)	/* SIO config regs */
#if CONFIG(BOARD_LIPPERT_FRONTRUNNER_AF)
				IO(Decode16, 0x0E00, 0x0E00, 1, 0x80)	/* SIO runtime regs */
#endif
				IO(Decode16, 0x0CF8, 0x0CF8, 1,	8)

				WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x0000,			/* address granularity */
					0x0000,			/* range minimum */
					0x0CF7,			/* range maximum */
					0x0000,			/* translation */
					0x0CF8			/* length */
				)

				WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x0000,			/* address granularity */
					0x0D00,			/* range minimum */
					0xFFFF,			/* range maximum */
					0x0000,			/* translation */
					0xF300			/* length */
				)

				Memory32Fixed(READONLY, 0x000A0000, 0x00020000, VGAM)	/* VGA memory space */
				/* memory space for PCI BARs below 4GB */
				Memory32Fixed(ReadOnly, 0x00000000, 0x00000000, MMIO)
			}) /* End Name(_SB.PCI0.CRES) */

			Method(_CRS, 0) {
				CreateDWordField(CRES, ^MMIO._BAS, MM1B)
				CreateDWordField(CRES, ^MMIO._LEN, MM1L)
				/*
				 * Declare memory between TOM1 and 4GB as available
				 * for PCI MMIO.
				 * Use ShiftLeft to avoid 64bit constant (for XP).
				 * This will work even if the OS does 32bit arithmetic, as
				 * 32bit (0x00000000 - TOM1) will wrap and give the same
				 * result as 64bit (0x100000000 - TOM1).
				 */
				MM1B = TOM1
				Local0 = 0x10000000 << 4
				Local0 -= TOM1
				MM1L = Local0

				Return(CRES) /* note to change the Name buffer */
			} /* end of Method(_SB.PCI0._CRS) */

			/*
			*
			*               FIRST METHOD CALLED UPON BOOT
			*
			*  1. If debugging, print current OS and ACPI interpreter.
			*  2. Get PCI Interrupt routing from ACPI VSM, this
			*     value is based on user choice in BIOS setup.
			*/
			Method(_INI, 0) {
				/* Determine the OS we're running on */
				OSFL()
			} /* End Method(_SB._INI) */
		} /* End Device(PCI0)  */

		Device(PWRB) {	/* Start Power button device */
			Name(_HID, EISAID("PNP0C0C"))
			Name(_UID, 0xAA)
			Name(_PRW, Package () {3, 0x04})	/* wake from S1-S4 */
			Name(_STA, 0x0B) /* sata is invisible */
		}
	} /* End \_SB scope */

	Scope(\_SI) {
		Method(_SST, 1) {
		}
	} /* End Scope SI */
}
/* End of ASL file */
