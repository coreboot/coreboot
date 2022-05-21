/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/tcss.h>
#include <soc/iomap.h>

/*
 * Type C Subsystem(TCSS) topology provides Runtime D3 support for USB host controller(xHCI),
 * USB device controller(xDCI), Thunderbolt DMA devices and Thunderbolt PCIe controllers.
 * PCIe RP0/RP1 is grouped with DMA0 and PCIe RP2/RP3 is grouped with DMA1.
 */
#define TCSS_TBT_PCIE0_RP0			0
#define TCSS_TBT_PCIE0_RP1			1
#define TCSS_TBT_PCIE0_RP2			2
#define TCSS_TBT_PCIE0_RP3			3
#define TCSS_XHCI				4
#define TCSS_XDCI				5
#define TCSS_DMA0				6
#define TCSS_DMA1				7

/*
 * MAILBOX_BIOS_CMD_TCSS_DEVEN_INTERFACE
 * Command code 0x15
 * Description: Gateway command for handling TCSS DEVEN clear/restore.
 * Field PARAM1[15:8] of the _INTERFACE register is used in this command to select from
 * a pre-defined set of subcommands.
 */
#define MAILBOX_BIOS_CMD_TCSS_DEVEN_INTERFACE		0x00000015
#define TCSS_DEVEN_MAILBOX_SUBCMD_GET_STATUS		0  /* Sub-command 0 */
#define TCSS_DEVEN_MAILBOX_SUBCMD_TCSS_CHANGE_REQ	1  /* Sub-command 1 */
#define TCSS_IOM_ACK_TIMEOUT_IN_MS			100

#define MCHBAR_TCSS_DEVEN_OFFSET			0x7090

#define REVISION_ID					1
#define UNRECOGNIZED_UUID				0x4
#define UNRECOGNIZED_REVISION				0x8

#define USB_TUNNELING					0x1
#define DISPLAY_PORT_TUNNELING				0x2
#define PCIE_TUNNELING					0x4
#define INTER_DOMAIN_USB4_INTERNET_PROTOCOL		0x8

Scope (\_SB)
{
#if !CONFIG(SOC_INTEL_ALDERLAKE_S3)
	Name (S0IX, 0)
#else
	Name (S0IX, 1)
#endif

	/* Device base address */
	Method (BASE, 1)
	{
		Local0 = Arg0 & 0x7             /* Function number */
		Local1 = (Arg0 >> 16) & 0x1F    /* Device number */
		Local2 = (Local0 << 12) + (Local1 << 15)
		Local3 = \_SB.PCI0.GPCB() + Local2
		Return (Local3)
	}

	/*
	 * Define PCH ACPIBASE IO as an ACPI operating region. The base address can be
	 * found in Device 31, Function 2, Offset 40h.
	 */
	OperationRegion (PMIO, SystemIO, ACPI_BASE_ADDRESS, 0x80)
	Field (PMIO, ByteAcc, NoLock, Preserve) {
		Offset(0x6C),   /* 0x6C, General Purpose Event 0 Status [127:96] */
		    ,  19,
		CPWS,  1,       /* CPU WAKE STATUS */
		Offset(0x7C),   /* 0x7C, General Purpose Event 0 Enable [127:96] */
		    ,  19,
		CPWE,  1        /* CPU WAKE EN */
	}

	Name (C2PW, 0)  /* Set default value to 0. */

	/*
	 * C2PM (CPU to PCH Method)
	 *
	 * This object is Enable/Disable GPE_CPU_WAKE_EN.
	 * Arguments: (4)
	 * Arg0 - An Integer containing the device wake capability
	 * Arg1 - An Integer containing the target system state
	 * Arg2 - An Integer containing the target device state
	 * Arg3 - An Integer containing the request device type
	 * Return Value:
	 * return 0
	 */
	Method (C2PM, 4, NotSerialized)
	{
		Local0 = 1 << Arg3
		/* This method is used to enable/disable wake from Tcss Device (WKEN). */
		If (Arg0 && Arg1)
		{  /* If entering Sx and enabling wake, need to enable WAKE capability. */
			If (CPWE == 0) {  /* If CPU WAKE EN is not set, Set it. */
				If (CPWS) {  /* If CPU WAKE STATUS is set, Clear it. */
					/* Clear CPU WAKE STATUS by writing 1. */
					CPWS = 1
				}
				CPWE = 1  /* Set CPU WAKE EN by writing 1. */
			}
			If ((C2PW & Local0) == 0) {
				/* Set Corresponding Device En BIT in C2PW. */
				C2PW |= Local0
			}
		} Else {  /* If Staying in S0 or Disabling Wake. */
			If (Arg0 || Arg2) {  /* Check if Exiting D0 and arming for wake. */
				/* If CPU WAKE EN is not set, Set it. */
				If (CPWE == 0) {
					/* If CPU WAKE STATUS is set, Clear it. */
					If (CPWS) {
						/* Clear CPU WAKE STATUS by writing 1. */
						CPWS = 1
					}
					CPWE = 1  /* Set CPU WAKE EN by writing 1. */
				}
				If ((C2PW & Local0) == 0) {
					/* Set Corresponding Device En BIT in C2PW. */
					C2PW |= Local0
				}
			} Else {
				/*
				 * Disable runtime PME, either because staying in D0 or
				 * disabling wake.
				 */
				If ((C2PW & Local0) != 0) {
					/*
					 * Clear Corresponding Device En BIT in C2PW.
					 */
					C2PW &= ~Local0
				}
				If ((CPWE != 0) && (C2PW == 0)) {
					/*
					 * If CPU WAKE EN is set, Clear it. Clear CPU WAKE EN
					 * by writing 0.
					 */
					CPWE = 0
				}
			}
		}
		Return (0)
	}

	Method (_OSC, 4, Serialized)
	{
		CreateDWordField (Arg3, 0, CDW1)
		If (Arg0 == ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48")) {
			/* Platform-Wide _OSC Capabilities
			 * Arg0: UUID = {0811B06E-4A27-44F9-8D60-3CBBC22E7B48}
			 * Arg1: Revision ID = 1
			 * Arg2: Count of entries (DWORD) in Arge3 (Integer): 3
			 * Arg3: DWORD capabilities buffer:
			 * First DWORD: The standard definition bits are used to return errors.
			 * Second DWORD: See ACPI specification Platform-Wide _OSC Capabilities
			 * DWORD2 table for Bits 0-17. Bit 18 is newly defined as native USB4
			 * support. The OS sets this bit to indicate support for an OSPM-native
			 * USB4 Connection Manager which handles USB4 connection events and
			 * link management.
			 */
			If (Arg1 != REVISION_ID) {
				CDW1 |= UNRECOGNIZED_REVISION
			}
			Return (Arg3)
#if CONFIG(SOFTWARE_CONNECTION_MANAGER)
		/*
		 * Software Connection Manager doesn't work with Linux 5.13 or later and
		 * results in TBT ports timing out. Not advertising this results in
		 * Firmware Connection Manager being used and TBT works correctly.
		 */
		} ElseIf (Arg0 == ToUUID("23A0D13A-26AB-486C-9C5F-0FFA525A575A")) {
			/*
			 * Operating System Capabilities for USB4
			 * Arg0: UUID = {23A0D13A-26AB-486C-9C5F-0FFA525A575A}
			 * Arg1: Revision ID = 1
			 * Arg2: Count of entries (DWORD) in Arg3 (Integer): 3
			 * Arg3: DWORD capabilities buffer:
			 * First DWORD: The standard definition bits are used to return errors.
			 * Second DWORD: OSPM support field for USB4, bits [31:0] reserved.
			 * Third DWORD: OSPM control field for USB4.
			 *       bit 0: USB tunneling
			 *       bit 1: DisplayPort tunneling
			 *       bit 2: PCIe tunneling
			 *       bit 3: Inter-domain USB4 internet protocol
			 *       bit 31:4: reserved
			 * Return: The platform acknowledges the capabilities buffer by
			 * returning a buffer of DWORD of the same length. Masked/Cleared bits
			 * in the control field indicate that the platform does not permit OSPM
			 * control of the respectively capabilities or features.
			 */
			CreateDWordField (Arg3, 8, CDW3)
			Local0 = CDW3

			If (Arg1 != REVISION_ID) {
				CDW1 |= UNRECOGNIZED_REVISION
				Return (Arg3)
			}
			Local0 |= USB_TUNNELING | DISPLAY_PORT_TUNNELING | PCIE_TUNNELING |
				INTER_DOMAIN_USB4_INTERNET_PROTOCOL
			CDW3 = Local0
			Return (Arg3)
#endif
		} Else {
			CDW1 |= UNRECOGNIZED_UUID
			Return (Arg3)
		}
	}
}

Scope (_GPE)
{
	/* PCI Express Hot-Plug wake event */
	Method (_L61, 0, NotSerialized)
	{
		/*
		 * Delay for 100ms to meet the timing requirements of the PCI Express Base
		 * Specification, Revision 1.0A, Section 6.6 ("...software must wait at least
		 * 100ms from the end of reset of one or more device before it is permitted
		 * to issue Configuration Requests to those devices").
		 */
		Sleep (100)

		If (CondRefOf (\_SB.PCI0.TXHC)) {
			/* Invoke PCIe root ports wake event handler */
			\_SB.PCI0.TRP0.HPEV()
			\_SB.PCI0.TRP1.HPEV()
			\_SB.PCI0.TRP2.HPEV()
			\_SB.PCI0.TRP3.HPEV()
		}

		/* Check Root Port 0 for a Hot Plug Event if the port is enabled */
		If (((\_SB.PCI0.TRP0.VDID != 0xFFFFFFFF) && \_SB.PCI0.TRP0.HPSX)) {
			If (\_SB.PCI0.TRP0.PDCX) {
				/* Clear all status bits */
				\_SB.PCI0.TRP0.PDCX = 1
				\_SB.PCI0.TRP0.HPSX = 1
				/*
				 * Intercept Presence Detect Changed interrupt and make sure
				 * the L0s is disabled on empty slots.
				 */
				If (!\_SB.PCI0.TRP0.PDSX) {
					/*
					 * The PCIe slot is empty, so disable L0s on hot unplug.
					 */
					\_SB.PCI0.TRP0.L0SE = 0
				}
				/* Performs proper notification to the OS. */
				Notify (\_SB.PCI0.TRP0, 0)
			} Else {
				/* False event. Clear Hot-Plug status, then exit. */
				\_SB.PCI0.TRP0.HPSX = 1
			}
		}

		/* Check Root Port 1 for a Hot Plug Event if the port is enabled */
		If (((\_SB.PCI0.TRP1.VDID != 0xFFFFFFFF) && \_SB.PCI0.TRP1.HPSX)) {
			If (\_SB.PCI0.TRP1.PDCX) {
				\_SB.PCI0.TRP1.PDCX = 1
				\_SB.PCI0.TRP1.HPSX = 1
				If (!\_SB.PCI0.TRP1.PDSX) {
					\_SB.PCI0.TRP1.L0SE = 0
				}
				Notify (\_SB.PCI0.TRP1, 0)
			} Else {
				\_SB.PCI0.TRP1.HPSX = 1
			}
		}

		/* Check Root Port 2 for a Hot Plug Event if the port is enabled */
		If (((\_SB.PCI0.TRP2.VDID != 0xFFFFFFFF) && \_SB.PCI0.TRP2.HPSX)) {
			If (\_SB.PCI0.TRP2.PDCX) {
				\_SB.PCI0.TRP2.PDCX = 1
				\_SB.PCI0.TRP2.HPSX = 1
				If (!\_SB.PCI0.TRP2.PDSX) {
					\_SB.PCI0.TRP2.L0SE = 0
				}
				Notify (\_SB.PCI0.TRP2, 0)
			} Else {
				\_SB.PCI0.TRP2.HPSX = 1
			}
		}

		/* Check Root Port 3 for a Hot Plug Event if the port is enabled */
		If (((\_SB.PCI0.TRP3.VDID != 0xFFFFFFFF) && \_SB.PCI0.TRP3.HPSX)) {
			If (\_SB.PCI0.TRP3.PDCX) {
				\_SB.PCI0.TRP3.PDCX = 1
				\_SB.PCI0.TRP3.HPSX = 1
				If (!\_SB.PCI0.TRP3.PDSX) {
					\_SB.PCI0.TRP3.L0SE = 0
				}
				Notify (\_SB.PCI0.TRP3, 0)
			} Else {
				\_SB.PCI0.TRP3.HPSX = 1
			}
		}
	}

	/* PCI Express power management event */
	Method (_L69, 0, Serialized)
	{
		If (CondRefOf (\_SB.PCI0.TXHC)) {
			If (\_SB.PCI0.TRP0.HPME() == 1) {
				Notify (\_SB.PCI0.TDM0, 0x2)
				Notify (\_SB.PCI0.TRP0, 0x2)
			}

			If (\_SB.PCI0.TRP1.HPME() == 1) {
				Notify (\_SB.PCI0.TDM0, 0x2)
				Notify (\_SB.PCI0.TRP1, 0x2)
			}

			If (\_SB.PCI0.TRP2.HPME() == 1) {
				Notify (\_SB.PCI0.TDM1, 0x2)
				Notify (\_SB.PCI0.TRP2, 0x2)
			}

			If (\_SB.PCI0.TRP3.HPME() == 1) {
				Notify (\_SB.PCI0.TDM1, 0x2)
				Notify (\_SB.PCI0.TRP3, 0x2)
			}
		}

		/* Invoke PCIe root ports power management status handler */
		\_SB.PCI0.TRP0.HPME()
		\_SB.PCI0.TRP1.HPME()
		\_SB.PCI0.TRP2.HPME()
		\_SB.PCI0.TRP3.HPME()
	}
}

Scope (\_SB.PCI0)
{
	Device (IOM)
	{
		Name (_HID, "INTC1079")
		Name (_DDN, "Intel(R) Alder Lake Input Output Manager(IOM) driver")
		/* IOM preserved MMIO range from 0xFBC10000 to 0xFBC11600. */
		Name (_CRS, ResourceTemplate () {
			Memory32Fixed (ReadWrite, IOM_BASE_ADDRESS, IOM_BASE_SIZE)
		})
		/* Hide the device so that Windows does not complain on missing driver */
		Name (_STA, 0xB)
	}

	/*
	 * Operation region defined to access the TCSS_DEVEN. Get the MCHBAR in offset
	 * 0x48 in B0:D0:F0. TCSS device enable base address is in offset 0x7090 of MCHBAR.
	 */
	OperationRegion (TDEN, SystemMemory, (GMHB() + MCHBAR_TCSS_DEVEN_OFFSET), 0x4)
	Field (TDEN, ByteAcc, NoLock, Preserve)
	{
		TRE0, 1,  /* PCIE0_EN */
		TRE1, 1,  /* PCIE1_EN */
		TRE2, 1,  /* PCIE2_EN */
		TRE3, 1,  /* PCIE3_EN */
		,     4,
		THCE, 1,  /* XHCI_EN */
		TDCE, 1,  /* XDCI_EN */
		DME0, 1,  /* TBT_DMA0_EN */
		DME1, 1,  /* TBT_DMA1_EN */
		,     20
	}

	/*
	 * Operation region defined to access the IOM REGBAR. Get the MCHBAR in offset
	 * 0x48 in B0:D0:F0. REGBAR Base address is in offset 0x7110 of MCHBAR.
	 */
	OperationRegion (MBAR, SystemMemory, (GMHB() + 0x7100), 0x1000)
	Field (MBAR, ByteAcc, NoLock, Preserve)
	{
		Offset(0x10),
		RBAR, 64        /* RegBar, offset 0x7110 in MCHBAR */
	}

	/*
	 * Operation region defined to access the pCode mailbox interface. Get the MCHBAR
	 * in offset 0x48 in B0:D0:F0. MMIO address is in offset 0x5DA0 of MCHBAR.
	 */
	OperationRegion (PBAR, SystemMemory, (GMHB() + 0x5DA0), 0x08)
	Field (PBAR, DWordAcc, NoLock, Preserve)
	{
		PMBD, 32,  /* pCode MailBox Data, offset 0x5DA0 in MCHBAR */
		PMBC, 8,   /* pCode MailBox Command, [7:0] of offset 0x5DA4 in MCHBAR */
		PSCM, 8,   /* pCode MailBox Sub-Command, [15:8] of offset 0x5DA4 in MCHBAR */
		,     15,  /* Reserved */
		PMBR, 1    /* pCode MailBox RunBit, [31:31] of offset 0x5DA4 in MCHBAR */
	}

	/*
	 * Poll pCode MailBox Ready
	 *
	 * Return 0xFF - Timeout
	 *	  0x00 - Ready
	 */
	Method (PMBY, 0)
	{
		Local0 = 0
		While (PMBR && (Local0 < 1000)) {
			Local0++
			Stall (1)
		}
		If (Local0 == 1000) {
			Printf("Timeout occurred.")
			Return (0xFF)
		}
		Return (0)
	}

	/*
	 * IOM REG BAR Base address is in offset 0x7110 in MCHBAR.
	 */
	Method (IOMA, 0)
	{
		Return (^RBAR & ~0x1)
	}

	/*
	 * From RegBar Base, IOM_TypeC_SW_configuration_1 is in offset 0xC10040, where
	 * 0x40 is the register offset.
	 */
	OperationRegion (IOMR, SystemMemory, (IOMA() + 0xC10000), 0x100)
	Field (IOMR, DWordAcc, NoLock, Preserve)
	{
		Offset(0x40),
		,     15,
		TD3C, 1,          /* [15:15] Type C D3 cold bit */
		TACK, 1,          /* [16:16] IOM Acknowledge bit */
		DPOF, 1,          /* [17:17] Set 1 to indicate IOM, all the */
				  /* display is OFF, clear otherwise */
		Offset(0x70),     /* Physical addr is offset 0x70. */
		IMCD, 32,         /* R_SA_IOM_BIOS_MAIL_BOX_CMD */
		IMDA, 32          /* R_SA_IOM_BIOS_MAIL_BOX_DATA */
	}

	/*
	 * TBT Group0 ON method
	 */
	Method (TG0N, 0)
	{
		If (\_SB.PCI0.TDM0.VDID == 0xFFFFFFFF) {
			Printf("TDM0 does not exist.")
		} Else {
			If (\_SB.PCI0.TDM0.STAT == 0) {
				/* DMA0 is in D3Cold early. */
				\_SB.PCI0.TDM0.D3CX()  /* RTD3 Exit */

				Printf("Bring TBT RPs out of D3Code.")
				If (\_SB.PCI0.TRP0.VDID != 0xFFFFFFFF) {
					/* RP0 D3 cold exit. */
					\_SB.PCI0.TRP0.D3CX()
				}
				If (\_SB.PCI0.TRP1.VDID != 0xFFFFFFFF) {
					/* RP1 D3 cold exit. */
					\_SB.PCI0.TRP1.D3CX()
				}
			} Else {
				Printf("Drop TG0N due to it is already exit D3 cold.")
			}

			/* TBT RTD3 exit 10ms delay. */
			Sleep (10)
		}
	}

	/*
	 * TBT Group0 OFF method
	 */
	Method (TG0F, 0)
	{
		If (\_SB.PCI0.TDM0.VDID == 0xFFFFFFFF) {
			Printf("TDM0 does not exist.")
		} Else {
			If (\_SB.PCI0.TDM0.STAT == 1) {
				/* DMA0 is not in D3Cold now. */
				\_SB.PCI0.TDM0.D3CE()  /* Enable DMA RTD3 */

				If (\_SB.PCI0.TDM0.IF30 != 1) {
					Return
				}

				Printf("Push TBT RPs to D3Cold together")
				If (\_SB.PCI0.TRP0.VDID != 0xFFFFFFFF) {
					/* Put RP0 to D3 cold. */
					\_SB.PCI0.TRP0.D3CE()
				}
				If (\_SB.PCI0.TRP1.VDID != 0xFFFFFFFF) {
					/* Put RP1 to D3 cold. */
					\_SB.PCI0.TRP1.D3CE()
				}
			}
		}
	}

	/*
	 * TBT Group1 ON method
	 */
	Method (TG1N, 0)
	{
		If (\_SB.PCI0.TDM1.VDID == 0xFFFFFFFF) {
			Printf("TDM1 does not exist.")
		} Else {
			If (\_SB.PCI0.TDM1.STAT == 0) {
				/* DMA1 is in D3Cold early. */
				\_SB.PCI0.TDM1.D3CX()  /* RTD3 Exit */

				Printf("Bring TBT RPs out of D3Code.")
				If (\_SB.PCI0.TRP2.VDID != 0xFFFFFFFF) {
					/* RP2 D3 cold exit. */
					\_SB.PCI0.TRP2.D3CX()
				}
				If (\_SB.PCI0.TRP3.VDID != 0xFFFFFFFF) {
					/* RP3 D3 cold exit. */
					\_SB.PCI0.TRP3.D3CX()
				}
			} Else {
				Printf("Drop TG1N due to it is already exit D3 cold.")
			}

			/* TBT RTD3 exit 10ms delay. */
			Sleep (10)
		}
	}

	/*
	 * TBT Group1 OFF method
	 */
	Method (TG1F, 0)
	{
		If (\_SB.PCI0.TDM1.VDID == 0xFFFFFFFF) {
			 Printf("TDM1 does not exist.")
		} Else {
			If (\_SB.PCI0.TDM1.STAT == 1) {
				/* DMA1 is not in D3Cold now */
				\_SB.PCI0.TDM1.D3CE()  /* Enable DMA RTD3. */

				If (\_SB.PCI0.TDM1.IF30 != 1) {
					Return
				}

				Printf("Push TBT RPs to D3Cold together")
				If (\_SB.PCI0.TRP2.VDID != 0xFFFFFFFF) {
					/* Put RP2 to D3 cold. */
					\_SB.PCI0.TRP2.D3CE()
				}
				If (\_SB.PCI0.TRP3.VDID != 0xFFFFFFFF) {
					/* Put RP3 to D3 cold */
					\_SB.PCI0.TRP3.D3CE()
				}
			}
		}
	}

	PowerResource (TBT0, 5, 1)
	{
		Method (_STA, 0)
		{
			Return (\_SB.PCI0.TDM0.STAT)
		}

		Method (_ON, 0)
		{
			TG0N()
		}

		Method (_OFF, 0)
		{
			If (\_SB.PCI0.TDM0.SD3C == 0) {
				TG0F()
			}
		}
	}

	PowerResource (TBT1, 5, 1)
	{
		Method (_STA, 0)
		{
			Return (\_SB.PCI0.TDM1.STAT)
		}

		Method (_ON, 0)
		{
			TG1N()
		}

		Method (_OFF, 0)
		{
			If (\_SB.PCI0.TDM1.SD3C == 0) {
				TG1F()
			}
		}
	}

#if !CONFIG(SOC_INTEL_ALDERLAKE_S3)
	Method (TCON, 0)
	{
		/* Reset IOM D3 cold bit if it is in D3 cold now. */
		If (TD3C == 1)  /* It was in D3 cold before. */
		{
			/* Reset IOM D3 cold bit. */
			TD3C = 0    /* Request IOM for D3 cold exit sequence. */
			Local0 = 0  /* Time check counter variable */
			/* Wait for ack, the maximum wait time for the ack is 100 msec. */
			While ((TACK != 0) && (Local0 < TCSS_IOM_ACK_TIMEOUT_IN_MS)) {
				/*
				 * Wait in this loop until TACK becomes 0 with timeout
				 * TCSS_IOM_ACK_TIMEOUT_IN_MS by default.
				 */
				Sleep (1)  /* Delay of 1ms. */
				Local0++
			}

			If (Local0 == TCSS_IOM_ACK_TIMEOUT_IN_MS) {
				Printf("Error: Timeout occurred.")
			}
			Else
			{
				Printf("TCSS D3 exit.");
			}
		}
		Else {
			Printf("Drop TCON due to it is already exit D3 cold.")
		}
	}

	Method (TCOF, 0)
	{
		If ((\_SB.PCI0.TXHC.SD3C != 0) || (\_SB.PCI0.TDM0.SD3C != 0)
					       || (\_SB.PCI0.TDM1.SD3C != 0))
		{
			Printf("Skip D3C entry.")
			Return
		}

		/* Request IOM for D3 cold entry sequence. */
		TD3C = 1
	}

	PowerResource (D3C, 5, 0)
	{
		/*
		 * Variable to save power state
		 * 1 - TC Cold request cleared.
		 * 0 - TC Cold request sent.
		 */
		Name (STAT, 0x1)

		Method (_STA, 0)
		{
			Return (STAT)
		}

		Method (_ON, 0)
		{
			\_SB.PCI0.TCON()
			STAT = 1
		}

		Method (_OFF, 0)
		{
			\_SB.PCI0.TCOF()
			STAT = 0
		}
	}
#endif

	/*
	 * TCSS xHCI device
	 */
	Device (TXHC)
	{
		Name (_ADR, 0x000D0000)
		Name (_DDN, "North XHCI controller")
		Name (_STR, Unicode ("North XHCI controller"))
		Name (DCPM, TCSS_XHCI)

		Method (_STA, 0x0, NotSerialized)
		{
			If (THCE == 1) {
				Return (0x0F)
			} Else {
				Return (0x0)
			}
		}
		#include "tcss_xhci.asl"
	}

	/*
	 * TCSS DMA0 device
	 */
	Device (TDM0)
	{
		Name (_ADR, 0x000D0002)
		Name (_DDN, "TBT DMA0 controller")
		Name (_STR, Unicode ("TBT DMA0 controller"))
		Name (DUID, 0)  /* TBT DMA number */
		Name (DCPM, TCSS_DMA0)

		Method (_STA, 0x0, NotSerialized)
		{
			If (DME0 == 1) {
				Return (0x0F)
			} Else {
				Return (0x0)
			}
		}
		#include "tcss_dma.asl"
	}

	/*
	 * TCSS DMA1 device
	 */
	Device (TDM1)
	{
		Name (_ADR, 0x000D0003)
		Name (_DDN, "TBT DMA1 controller")
		Name (_STR, Unicode ("TBT DMA1 controller"))
		Name (DUID, 1)  /* TBT DMA number */
		Name (DCPM, TCSS_DMA1)

		Method (_STA, 0x0, NotSerialized)
		{
			If (DME1 == 1) {
				Return (0x0F)
			} Else {
				Return (0x0)
			}
		}
		#include "tcss_dma.asl"
	}

	/*
	 * TCSS PCIE Root Port #00
	 */
	Device (TRP0)
	{
		Name (_ADR, 0x00070000)
		Name (TUID, 0)  /* TBT PCIE RP Number 0 for RP00 */
		Name (LTEN, 0)  /* Latency Tolerance Reporting Mechanism, 0:Disable, 1:Enable */
		Name (LMSL, 0)  /* PCIE LTR max snoop Latency */
		Name (LNSL, 0)  /* PCIE LTR max no snoop Latency */
		Name (DCPM, TCSS_TBT_PCIE0_RP0)

		Method (_STA, 0x0, NotSerialized)
		{
			If (TRE0 == 1) {
				Return (0x0F)
			} Else {
				Return (0x0)
			}
		}

		Method (_INI)
		{
			LTEN = 0
			LMSL = 0x88C8
			LNSL = 0x88C8
		}
		#include "tcss_pcierp.asl"
	}

	/*
	 * TCSS PCIE Root Port #01
	 */
	Device (TRP1)
	{
		Name (_ADR, 0x00070001)
		Name (TUID, 1)  /* TBT PCIE RP Number 1 for RP01 */
		Name (LTEN, 0)  /* Latency Tolerance Reporting Mechanism, 0:Disable, 1:Enable */
		Name (LMSL, 0)  /* PCIE LTR max snoop Latency */
		Name (LNSL, 0)  /* PCIE LTR max no snoop Latency */
		Name (DCPM, TCSS_TBT_PCIE0_RP1)

		Method (_STA, 0x0, NotSerialized)
		{
			If (TRE1 == 1) {
				Return (0x0F)
			} Else {
				Return (0x0)
			}
		}

		Method (_INI)
		{
			LTEN = 0
			LMSL = 0x88C8
			LNSL = 0x88C8
		}
		#include "tcss_pcierp.asl"
	}

	/*
	 * TCSS PCIE Root Port #02
	 */
	Device (TRP2)
	{
		Name (_ADR, 0x00070002)
		Name (TUID, 2)  /* TBT PCIE RP Number 2 for RP02 */
		Name (LTEN, 0)  /* Latency Tolerance Reporting Mechanism, 0:Disable, 1:Enable */
		Name (LMSL, 0)  /* PCIE LTR max snoop Latency */
		Name (LNSL, 0)  /* PCIE LTR max no snoop Latency */
		Name (DCPM, TCSS_TBT_PCIE0_RP2)

		Method (_STA, 0x0, NotSerialized)
		{
			If (TRE2 == 1) {
				Return (0x0F)
			} Else {
				Return (0x0)
			}
		}

		Method (_INI)
		{
			LTEN = 0
			LMSL = 0x88C8
			LNSL = 0x88C8
		}
		#include "tcss_pcierp.asl"
	}

	/*
	 * TCSS PCIE Root Port #03
	 */
	Device (TRP3)
	{
		Name (_ADR, 0x00070003)
		Name (TUID, 3)  /* TBT PCIE RP Number 3 for RP03 */
		Name (LTEN, 0)  /* Latency Tolerance Reporting Mechanism, 0:Disable, 1:Enable */
		Name (LMSL, 0)  /* PCIE LTR max snoop Latency */
		Name (LNSL, 0)  /* PCIE LTR max no snoop Latency */
		Name (DCPM, TCSS_TBT_PCIE0_RP3)

		Method (_STA, 0x0, NotSerialized)
		{
			If (TRE3 == 1) {
				Return (0x0F)
			} Else {
				Return (0x0)
			}
		}

		Method (_INI)
		{
			LTEN = 0
			LMSL = 0x88C8
			LNSL = 0x88C8
		}
		#include "tcss_pcierp.asl"
	}
}
