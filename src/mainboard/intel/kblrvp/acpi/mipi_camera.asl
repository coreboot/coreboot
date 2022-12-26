/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C2)
{
	Device (PMIC)
	{
		Name (_HID, "INT3472") /* _HID: Hardware ID */
		Name (_UID, 0)  // _UID: Unique ID */
		Name (_DDN, "TPS68470 PMIC")  /* _DDN: DOS Device Name */
		Name (CAMD, 0x64)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		/* Marks the availability of all the operation regions */
		Name (AVP1, 0)
		Name (AVGP, 0)
		Name (AVB0, 0)
		Name (AVB1, 0)
		Name (AVB2, 0)
		Name (AVB3, 0)
		Method (_REG, 2, NotSerialized)
		{
			If (Arg0 == 0x08)
			{
				/* Marks the availability of GeneralPurposeIO
				 * 0x08: opregion space for GeneralPurposeIO
				 */
				AVGP = Arg1
			}
			If (Arg0 == 0xB0)
			{
				/* Marks the availability of
				 * TI_PMIC_POWER_OPREGION_ID */
				AVB0 = Arg1
			}
			If (Arg0 == 0xB1)
			{
				/* Marks the availability of
				 * TI_PMIC_VR_VAL_OPREGION_ID */
				AVB1 = Arg1
			}
			If (Arg0 == 0xB2)
			{
				/* Marks the availability of
				 * TI_PMIC_CLK_OPREGION_ID */
				AVB2 = Arg1
			}
			If (Arg0 == 0xB3)
			{
				/* Marks the availability of
				 * TI_PMIC_CLK_FREQ_OPREGION_ID */
				AVB3 = Arg1
			}
			If (AVGP && AVB0 && AVB1 && AVB2 && AVB3)
			{
				/* Marks the availability of all opregions */
				AVP1 = 1
			}
			Else
			{
				AVP1 = 0
			}
		}

		OperationRegion (GPOP, GeneralPurposeIo, 0, 0x2)
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x004D, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,
			)
			/* GPIO.9 is XSHUTDOWN pin for world facing camera */
			GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
				IoRestrictionOutputOnly, "\\_SB.PCI0.I2C2.PMIC",
				0x00, ResourceConsumer,,)
			{
				9
			}
		})

		/* PMIC operation regions */
		/* 0xB0: TI_PMIC_POWER_OPREGION_ID
		 * VSIO: Sensor IO LDO output
		 * VCMC: VCM LDO output
		 * VAX1: Auxiliary LDO1 output
		 * VAX2: Auxiliary LDO2 output
		 * VACT: Analog LDO output
		 * VDCT: Core buck output
		 */
		OperationRegion (PWR1, 0xB0, 0, 0x0100)
		Field (PWR1, DWordAcc, NoLock, Preserve)
		{
			VSIO, 32,
			VCMC, 32,
			VAX1, 32,
			VAX2, 32,
			VACT, 32,
			VDCT, 32,
		}

		/* 0xB1: TI_PMIC_VR_VAL_OPREGION_ID
		 * SIOV: VSIO VR voltage value
		 * IOVA: VIO VR voltage value
		 * VCMV: VCM VR voltage value
		 * AX1V: Auxiliary LDO1 VR voltage value
		 * AX2V: Auxiliary LDO2 VR voltage value
		 * ACVA: Analog LDO VR voltage
		 * DCVA: Core buck VR voltage
		 */
		OperationRegion (PWR2, 0xB1, 0, 0x0100)
		Field (PWR2, DWordAcc, NoLock, Preserve)
		{
			SIOV, 32,
			IOVA, 32,
			VCMV, 32,
			AX1V, 32,
			AX2V, 32,
			ACVA, 32,
			DCVA, 32,
		}

		/* 0xB2: TI_PMIC_CLK_OPREGION_ID
		 * PCTL: PLL control register
		 * PCT2: PLL control 2 register
		 * CFG1: Clock configuration 1 register
		 * CFG2: Clock configuration 2 register
		 */
		OperationRegion (CLKC, 0xB2, 0, 0x0100)
		Field (CLKC, DWordAcc, NoLock, Preserve)
		{
			PCTL, 32,
			PCT2, 32,
			CFG1, 32,
			CFG2, 32,
		}

		/* 0xB3: TI_PMIC_CLK_FREQ_OPREGION_ID
		 * PDV2: PLL output divider for HCLK_B
		 * BODI: PLL output divider for boost clock
		 * BUDI: PLL output divider for buck clock
		 * PSWR: PLL reference clock setting
		 * XTDV: Reference crystal divider
		 * PLDV: PLL feedback divider
		 * PODV: PLL output divider for HCLK_A
		 */
		OperationRegion (CLKF, 0xB3, 0, 0x0100)
		Field (CLKF, DWordAcc, NoLock, Preserve)
		{
			PDV2, 32,
			BODI, 32,
			BUDI, 32,
			PSWR, 32,
			XTDV, 32,
			PLDV, 32,
			PODV, 32,
		}

		Mutex (MUTC, 0)
		Method (CLKE, 0, Serialized) {
			/* save Acquire result so we can check for
			Mutex acquired */
			Local0 = Acquire (MUTC, 1000)
			/* check for Mutex acquired */
			If (Local0 == 0) {
				/* Set boost clock divider */
				BODI = 3
				/* Set buck clock divider */
				BUDI = 2
				/* Set the PLL_REF_CLK cyles */
				PSWR = 19
				/* Set the reference crystal divider */
				XTDV = 170
				/* Set PLL feedback divider */
				PLDV = 32
				/* Set PLL output divider for HCLK_A */
				PODV = 1
				/* Enable HCLK_A clock.
				 * CFG1: output selection for HCLK_A.
				 * CFG2: set drive strength for HCLK_A.
				 */
				CFG2 = 1
				CFG1 = 2
				/* Enable PLL output, crystal oscillator
				 * input capacitance control and set
				 * Xtal oscillator as clock source.
				 */
				PCTL = 209
				Sleep(1)
				Release (MUTC)
			}
		}

		Method (CLKD, 0, Serialized) {
			/* save Acquire result so we can check for
			Mutex acquired */
			Local0 = Acquire (MUTC, 1000)
			/* check for Mutex acquired */
			If (Local0 == 0) {
				BODI = 0
				BUDI = 0
				PSWR = 0
				XTDV = 0
				PLDV = 0
				PODV = 0
				/* Disable HCLK_A clock */
				CFG2 = 0
				CFG1 = 0
				PCTL = 0
				Release (MUTC)
			}
		}

		/* Reference count for VSIO */
		Mutex (MUTV, 0)
		Name (VSIC, 0)
		Method (DOVD, 1, Serialized) {
			/* Save Acquire result so we can check for
			Mutex acquired */
			Local0 = Acquire (MUTV, 1000)
			/* Check for Mutex acquired */
			If (Local0 == 0) {
				/* Turn off VSIO */
				If (Arg0 == 0) {
					/* Decrement only if VSIC > 0 */
					if (VSIC > 0) {
						VSIC--
						If (VSIC == 0) {
							VSIO = 0
						}
					}
				} ElseIf (Arg0 == 1) {
					/* Increment only if VSIC < 2 */
					If (VSIC < 2) {
						/* Turn on VSIO */
						If (VSIC == 0) {
							VSIO = 3
						}
						VSIC++
					}
				}

				Release (MUTV)
			}
		}

		/* Power resource methods for CAM0 */
		PowerResource (OVTH, 0, 0) {
			Name (STA, 0)
			Method (_ON, 0, Serialized) {
				If (AVP1 == 1) {
					If (STA == 0) {
						/* Enable VSIO regulator +
						daisy chain */
						DOVD(1)

						if (IOVA != 52) {
							/* Set VSIO value as
							1.8006 V */
							IOVA = 52
						}
						if (SIOV != 52) {
							/* Set VSIO value as
							1.8006 V */
							SIOV = 52
						}
						Sleep(3)

						VACT = 1
						if (ACVA != 109) {
							/* Set ANA at 2.8152V */
							ACVA = 109
						}
						Sleep(3)

						\_SB.PCI0.I2C2.PMIC.CLKE()

						VDCT = 1
						if (DCVA != 12) {
							/* Set CORE at 1.2V */
							DCVA = 12
						}
						Sleep(3)
						\_SB.PCI0.I2C2.CAM0.CRST(1)
						Sleep(5)

						STA = 1
					}
				}
			}

			Method (_OFF, 0, Serialized) {
				If (AVP1 == 1) {
					If (STA == 1) {
						Sleep(2)
						\_SB.PCI0.I2C2.PMIC.CLKD()
						Sleep(2)
						\_SB.PCI0.I2C2.CAM0.CRST(0)
						Sleep(3)
						VDCT = 0
						Sleep(3)
						VACT = 0
						Sleep(1)
						DOVD(0)
						Sleep(1)
					}
				}
				STA = 0
			}
			Method (_STA, 0, NotSerialized) {
				Return (STA)
			}
		}

		/* Power resource methods for VCM */
		PowerResource (VCMP, 0, 0) {
			Name (STA, 0)
			Method (_ON, 0, Serialized) {
				If (AVP1 == 1) {
					If (STA == 0) {
						/* Enable VSIO regulator +
						daisy chain */
						DOVD(1)
						if (IOVA != 52) {
							/* Set VSIO value as
							1.8006 V */
							IOVA = 52
						}
						if (SIOV != 52) {
							/* Set VSIO value as
							1.8006 V */
							SIOV = 52
						}
						Sleep(3)

						/* Enable VCM regulator */
						VCMC = 1
						if (VCMV != 109) {
							/* Set VCM value at
							2.8152 V */
							VCMV = 109
						}
						Sleep(3)

						STA = 1
					}
				}
			}

			Method (_OFF, 0, Serialized) {
				If (AVP1 == 1) {
					If (STA == 1) {
						VCMC = 0 /* Disable regulator */
						Sleep(1)
						DOVD(0) /* Disable regulator */
						Sleep(1)
						STA = 0
					}
				}
			}

			Method (_STA, 0, NotSerialized) {
				Return (STA)
			}
		}
	}

	Device (CAM0)
	{
		Name (_HID, "OVTID858")  /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "OV 13858 Camera") /* _DDN: DOS Device Name */
		Name (CAMD, 0x02)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() {\_SB.PCI0.I2C2.PMIC})
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0010, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,
				)
		})

		Field (\_SB.PCI0.I2C2.PMIC.GPOP, ByteAcc, NoLock, Preserve)
		{
			Connection
			(
				GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
					IoRestrictionOutputOnly,
					"\\_SB.PCI0.I2C2.PMIC", 0x00,
					ResourceConsumer,,)
				{
					9
				}
			),
			GRST, 1,
		}

		/* Set or clear GRST GPIO */
		Method (CRST, 1, Serialized)
		{
			GRST = Arg0
		}

		Name (_PR0, Package () { ^^I2C2.PMIC.OVTH })
		Name (_PR3, Package () { ^^I2C2.PMIC.OVTH })

		/* Port0 of CAM0 is connected to port0 of CIO2 device */
		Name (_DSD, Package () {
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () { "port0", "PRT0" },
			},
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "clock-frequency", 19200000 },
			}
		})

		Name (PRT0, Package() {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "port", 0 },
			},
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () { "endpoint0", "EP00" },
			}
		})

		Name (EP00, Package() {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "endpoint", 0 },
				Package () { "clock-lanes", 0 },
				Package () { "data-lanes",
					Package () { 1, 2, 3, 4 }
				},
				Package () { "link-frequencies",
					Package() { 1190400000, 640000000 }
				},
				Package () { "remote-endpoint",
					Package() { \_SB.PCI0.CIO2, 0, 0 }
				},
			}
		})

		Method (SSDB, 0, Serialized)
		{
			Return (Buffer (0x5E)
			{
				/* 0000 */   0x00, 0x50, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
				/* 0008 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0010 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0018 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
				/* 0020 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0028 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0030 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0038 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0040 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0048 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x03,
				/* 0050 */   0x09, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 0x36,
				/* 0058 */   0x6E, 0x01, 0x00, 0x00, 0x00, 0x00
			})
		}
	}

	Device (VCM0)
	{
		Name (_HID, "DWDWD000")  /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "Dongwoon AF DAC") /* _DDN: DOS Device Name */
		Name (CAMD, 0x03)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() {\_SB.PCI0.I2C2.PMIC})
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x000C, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,
				)
		})

		Name (_PR0, Package () { ^PMIC.VCMP })
		Name (_PR3, Package () { ^PMIC.VCMP })
	}
}

Scope (\_SB.PCI0.I2C3)
{
	Device (PMIC)
	{
		Name (_HID, "INT3473") /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "TPS68470 PMIC 2") /* _DDN: DOS Device Name */
		Name (CAMD, 0x64)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		/* Marks the availability of all the operation regions */
		Name (AVP2, 0)
		Name (AVGP, 0)
		Name (AVB0, 0)
		Name (AVB1, 0)
		Name (AVB2, 0)
		Name (AVB3, 0)
		Method (_REG, 2, NotSerialized)
		{
			If (Arg0 == 0x08)
			{
				/* Marks the availability of GeneralPurposeIO
				 * 0x08: opregion space for GeneralPurposeIO
				 */
				AVGP = Arg1
			}
			If (Arg0 == 0xB0)
			{
				/* Marks the availability of
				 * TI_PMIC_POWER_OPREGION_ID */
				AVB0 = Arg1
			}
			If (Arg0 == 0xB1)
			{
				/* Marks the availability of
				 * TI_PMIC_VR_VAL_OPREGION_ID */
				AVB1 = Arg1
			}
			If (Arg0 == 0xB2)
			{
				/* Marks the availability of
				 * TI_PMIC_CLK_OPREGION_ID */
				AVB2 = Arg1
			}
			If (Arg0 == 0xB3)
			{
				/* Marks the availability of
				 * TI_PMIC_CLK_FREQ_OPREGION_ID */
				AVB3 = Arg1
			}
			If (AVGP && AVB0 && AVB1 && AVB2 && AVB3)
			{
				/* Marks the availability of all opregions */
				AVP2 = 1
			}
			Else
			{
				AVP2 = 0
			}
		}

		OperationRegion (GPOP, GeneralPurposeIo, 0, 0x2)
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0049, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C3",
				0x00, ResourceConsumer, ,
			)
			/* GPIO.4 is AVDD pin for user facing camera */
			GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
				IoRestrictionOutputOnly, "\\_SB.PCI0.I2C3.PMIC",
				0x00, ResourceConsumer,,)
			{
				4
			}
			/* GPIO.5 is XSHUTDOWN pin for user facing camera */
			GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
				IoRestrictionOutputOnly, "\\_SB.PCI0.I2C3.PMIC",
				0x00, ResourceConsumer,,)
			{
				5
			}
		})

		/* PMIC operation regions */
		/* 0xB0: TI_PMIC_POWER_OPREGION_ID
		 * VSIO: Sensor IO LDO output
		 * VCMC: VCM LDO output
		 * VAX1: Auxiliary LDO1 output
		 * VAX2: Auxiliary LDO2 output
		 * VACT: Analog LDO output
		 * VDCT: Core buck output
		 */
		OperationRegion (PWR1, 0xB0, 0, 0x0100)
		Field (PWR1, DWordAcc, NoLock, Preserve)
		{
			VSIO, 32,
			VCMC, 32,
			VAX1, 32,
			VAX2, 32,
			VACT, 32,
			VDCT, 32,
		}

		/* 0xB1: TI_PMIC_VR_VAL_OPREGION_ID
		 * SIOV: VSIO VR voltage value
		 * IOVA: VIO VR voltage value
		 * VCMV: VCM VR voltage value
		 * AX1V: Auxiliary LDO1 VR voltage value
		 * AX2V: Auxiliary LDO2 VR voltage value
		 * ACVA: Analog LDO VR voltage
		 * DCVA: Core buck VR voltage
		 */
		OperationRegion (PWR2, 0xB1, 0, 0x0100)
		Field (PWR2, DWordAcc, NoLock, Preserve)
		{
			SIOV, 32,
			IOVA, 32,
			VCMV, 32,
			AX1V, 32,
			AX2V, 32,
			ACVA, 32,
			DCVA, 32,
		}

		/* 0xB2: TI_PMIC_CLK_OPREGION_ID
		 * PCTL: PLL control register
		 * PCT2: PLL control 2 register
		 * CFG1: Clock configuration 1 register
		 * CFG2: Clock configuration 2 register
		 */
		OperationRegion (CLKC, 0xB2, 0, 0x0100)
		Field (CLKC, DWordAcc, NoLock, Preserve)
		{
			PCTL, 32,
			PCT2, 32,
			CFG1, 32,
			CFG2, 32,
		}

		/* 0xB3: TI_PMIC_CLK_FREQ_OPREGION_ID
		 * PDV2: PLL output divider for HCLK_B
		 * BODI: PLL output divider for boost clock
		 * BUDI: PLL output divider for buck clock
		 * PSWR: PLL reference clock setting
		 * XTDV: Reference crystal divider
		 * PLDV: PLL feedback divider
		 * PODV: PLL output divider for HCLK_A
		 */
		OperationRegion (CLKF, 0xB3, 0, 0x0100)
		Field (CLKF, DWordAcc, NoLock, Preserve)
		{
			PDV2, 32,
			BODI, 32,
			BUDI, 32,
			PSWR, 32,
			XTDV, 32,
			PLDV, 32,
			PODV, 32,
		}

		Mutex (MUTC, 0)
		Method (CLKE, 0, Serialized) {
			/* save Acquire result so we can check for
			Mutex acquired */
			Local0 = Acquire (MUTC, 1000)
			/* check for Mutex acquired */
			If (Local0 == 0) {
				/* Set boost clock divider */
				BODI = 3
				/* Set buck clock divider */
				BUDI = 2
				/* Set the PLL_REF_CLK cyles */
				PSWR = 19
				/* Set the reference crystal divider */
				XTDV = 170
				/* Set PLL feedback divider */
				PLDV = 32
				/* Set PLL output divider for HCLK_A */
				PODV = 1
				/* Enable HCLK_A clock.
				 * CFG1: output selection for HCLK_A.
				 * CFG2: set drive strength for HCLK_A.
				 */
				CFG2 = 1
				CFG1 = 2
				/* Enable PLL output, crystal oscillator
				 * input capacitance control and set
				 * Xtal oscillator as clock source.
				 */
				PCTL = 209
				Sleep(1)
				Release (MUTC)
			}
		}

		Method (CLKD, 0, Serialized) {
			/* save Acquire result so we can check for
			Mutex acquired */
			Local0 = Acquire (MUTC, 1000)
			/* check for Mutex acquired */
			If (Local0 == 0) {
				BODI = 0
				BUDI = 0
				PSWR = 0
				XTDV = 0
				PLDV = 0
				PODV = 0
				/* Disable HCLK_A clock */
				CFG2 = 0
				CFG1 = 0
				PCTL = 0
				Release (MUTC)
			}
		}

		/* Reference count for VSIO */
		Mutex (MUTV, 0)
		Name (VSIC, 0)
		Method (DOVD, 1, Serialized) {
			/* Save Acquire result so we can check for
			Mutex acquired */
			Local0 = Acquire (MUTV, 1000)
			/* Check for Mutex acquired */
			If (Local0 == 0) {
				/* Turn off VSIO */
				If (Arg0 == 0) {
					VSIO = 0
				} ElseIf (Arg0 == 1) {
					VSIO = 3
				}
				Release (MUTV)
			}
		}

		/* Power resource methods for CAM1 */
		PowerResource (OVFI, 0, 0) {
			Name (STA, 0)
			Method (_ON, 0, Serialized) {
				If (AVP2 == 1) {
					If (STA == 0) {
						/* Enable VSIO regulator +
						daisy chain */
						DOVD(1)

						VAX2 = 1 /* Enable VAUX2 */

						if (AX2V != 52) {
							/* Set VAUX2 as
							1.8006 V */
							AX2V = 52
						}
						Sleep(1)

						\_SB.PCI0.I2C3.PMIC.CLKE()

						VAX1 = 1 /* Enable VAUX1 */
						if (AX1V != 19) {
						/* Set VAUX1 as 1.2132V */
							AX1V = 19
						}
						Sleep(3)

						\_SB.PCI0.I2C3.CAM1.CGP4(1)
						Sleep(3)

						\_SB.PCI0.I2C3.CAM1.CGP5(1)
						Sleep(5)
						STA = 1
					}
				}
			}

			Method (_OFF, 0, Serialized) {
				If (AVP2 == 1) {
					If (STA == 1) {
						Sleep(2)
						\_SB.PCI0.I2C3.PMIC.CLKD()
						Sleep(2)
						\_SB.PCI0.I2C3.CAM1.CGP5(0)
						Sleep(3)
						VAX1 = 0
						Sleep(1)
						\_SB.PCI0.I2C3.CAM1.CGP4(0)
						Sleep(1)
						VAX2 = 0
						Sleep(1)
						DOVD(0)
						Sleep(1)

					}
					STA = 0
				}
			}

			Method (_STA, 0, NotSerialized) {
				Return (STA)
			}
		}
	}

	Device (CAM1)
	{
		Name (_HID, "INT3479") /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "OV 5670 Camera")  /* _DDN: DOS Device Name */
		Name (CAMD, 0x02)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() {\_SB.PCI0.I2C3.PMIC})
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0010, ControllerInitiated, 0x00061A80,
			AddressingMode7Bit, "\\_SB.PCI0.I2C3",
			0x00, ResourceConsumer, ,
			)
		})

		Field (\_SB.PCI0.I2C3.PMIC.GPOP, ByteAcc, NoLock, Preserve)
		{
			Connection
			(
				GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
					IoRestrictionOutputOnly,
					"\\_SB.PCI0.I2C3.PMIC", 0x00,
					ResourceConsumer,,)
				{
					4
				}
			),
			GPO4, 1,
			Connection
			(
				GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
					IoRestrictionOutputOnly,
					"\\_SB.PCI0.I2C3.PMIC", 0x00,
					ResourceConsumer,,)
				{
					5
				}
			),
			GPO5, 1,
		}

		/* Set or clear GPO4 GPIO */
		Method (CGP4, 1, Serialized)
		{
			GPO4 = Arg0
		}

		/* Set or clear GPO5 GPIO */
		Method (CGP5, 1, Serialized)
		{
			GPO5 = Arg0
		}

		Name (_PR0, Package () { ^^I2C3.PMIC.OVFI })
		Name (_PR3, Package () { ^^I2C3.PMIC.OVFI })

		/* Port0 of CAM1 is connected to port1 of CIO2 device */
		Name (_DSD, Package () {
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () { "port0", "PRT0" },
			},
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "clock-frequency", 19200000 },
			}
		})

		Name (PRT0, Package() {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "port", 0 },
			},
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () { "endpoint0", "EP00" },
			}
		})

		Name (EP00, Package() {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "endpoint", 0 },
				Package () { "clock-lanes", 0 },
				Package () { "data-lanes",
					Package () { 1, 2 }
				},
				Package () { "link-frequencies",
					Package() { 844800000 }
				},
				Package () { "remote-endpoint",
					Package() { \_SB.PCI0.CIO2, 1, 0 }
				},
			}
		})

		Method (SSDB, 0, Serialized)
		{
			Return (Buffer (0x5E)
			{
				/* 0000 */   0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0008 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0010 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0018 */   0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00,
				/* 0020 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0028 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0030 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0038 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0040 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0048 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x03,
				/* 0050 */   0x09, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 0x36,
				/* 0058 */   0x6E, 0x01, 0x00, 0x00, 0x00, 0x00
			})
		}
	}
}
