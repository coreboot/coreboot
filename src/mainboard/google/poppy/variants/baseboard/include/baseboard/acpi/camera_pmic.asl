/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C2)
{
	Device (PMIC)
	{
		Name (_HID, "INT3472") /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "TPS68470 PMIC")  /* _DDN: DOS Device Name */
		Name (CAMD, 0x64)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Method (PMON, 0, Serialized) {
			/*
			 * Turn on 3V3_VDD. It takes around 1 ms for voltage to
			 * settle to 3.3 Volt. Provide additional 2 ms delay.
			 */
			STXS(EN_PP3300_DX_CAM)
			Sleep (3)
		}

		Method (PMOF, 0, Serialized) {
			/* Make Sure all PMIC outputs are off. */
			If (VSIC == 0) {
				CTXS(EN_PP3300_DX_CAM)
			}
		}

		Name (_PR0, Package (0x01) { CPMC })
		Name (_PR3, Package (0x01) { CPMC })

		/* Power resource methods for PMIC */
		PowerResource (CPMC, 0, 0) {
			Name (RSTO, 1)
			Method (_ON, 0, Serialized) {
				PMON()
				/* Do not reset PMIC across S3 and S0ix cycle */
				if (RSTO == 1) {
					CTXS(EN_CAM_PMIC_RST_L)
					Sleep(1)
					STXS(EN_CAM_PMIC_RST_L)
					Sleep (3)
					RSTO = 0
				}
			}
			Method (_OFF, 0, Serialized) {
				PMOF()
			}
			Method (_STA, 0, Serialized) {
				Return (GTXS(EN_PP3300_DX_CAM))
			}
		}

		/* Marks the availability of all the operation regions */
		Name (AVBL, 0)
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
			If (Arg0 == 0xb0)
			{
				/* Marks the availability of
				 * TI_PMIC_POWER_OPREGION_ID */
				AVB0 = Arg1
			}
			If (Arg0 == 0xb1)
			{
				/* Marks the availability of
				 * TI_PMIC_VR_VAL_OPREGION_ID */
				AVB1 = Arg1
			}
			If (Arg0 == 0xb2)
			{
				/* Marks the availability of
				 * TI_PMIC_CLK_OPREGION_ID */
				AVB2 = Arg1
			}
			If (Arg0 == 0xb3)
			{
				/* Marks the availability of
				 * TI_PMIC_CLK_FREQ_OPREGION_ID */
				AVB3 = Arg1
			}
			If (AVGP && AVB0 && AVB1 && AVB2 && AVB3)
			{
				/* Marks the availability of all opregions */
				AVBL = 1
			}
			Else
			{
				AVBL = 0
			}
		}

		OperationRegion (GPOP, GeneralPurposeIo, 0, 0x2)
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x004D, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,
			)
			/* GPIO.1 is sensor SDA in daisy chain mode */
			GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
				IoRestrictionInputOnly, "\\_SB.PCI0.I2C2.PMIC",
				0x00, ResourceConsumer,,)
			{
				1
			}
			/* GPIO.2 is sensor SCL in daisy chain mode */
			GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
				IoRestrictionInputOnly, "\\_SB.PCI0.I2C2.PMIC",
				0x00, ResourceConsumer,,)
			{
				2
			}
			/* GPIO.4 is AVDD pin for user facing camera */
			GpioIo (Exclusive, PullDown, 0x0000, 0x0000,
				IoRestrictionOutputOnly, "\\_SB.PCI0.I2C2.PMIC",
				0x00, ResourceConsumer,,)
			{
				4
			}
			/* GPIO.5 is XSHUTDOWN pin for user facing camera */
			GpioIo (Exclusive, PullDown, 0x0000, 0x0000,
				IoRestrictionOutputOnly, "\\_SB.PCI0.I2C2.PMIC",
				0x00, ResourceConsumer,,)
			{
				5
			}
			/* GPIO.9 is XSHUTDOWN pin for world facing camera */
			GpioIo (Exclusive, PullDown, 0x0000, 0x0000,
				IoRestrictionOutputOnly, "\\_SB.PCI0.I2C2.PMIC",
				0x00, ResourceConsumer,,)
			{
				9
			}
		})

		Field (\_SB.PCI0.I2C2.PMIC.GPOP, ByteAcc, NoLock, Preserve)
		{
			Connection
			(
				GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
					IoRestrictionInputOnly, "\\_SB.PCI0.I2C2.PMIC",
					0x00, ResourceConsumer,,)
				{
					1
				}
			),
			GPO1, 1,
			Connection
			(
				GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
					IoRestrictionInputOnly, "\\_SB.PCI0.I2C2.PMIC",
					0x00, ResourceConsumer,,)
				{
					2
				}
			),
			GPO2, 1,
			Connection
			(
				GpioIo (Exclusive, PullDown, 0x0000, 0x0000,
					IoRestrictionOutputOnly,
					"\\_SB.PCI0.I2C2.PMIC", 0x00,
					ResourceConsumer,,)
				{
					9
				}
			),
			GRST, 1,
			Connection
			(
				GpioIo (Exclusive, PullDown, 0x0000, 0x0000,
					IoRestrictionOutputOnly,
					"\\_SB.PCI0.I2C2.PMIC", 0x00,
					ResourceConsumer,,)
				{
					4
				}
			),
			GPO4, 1,
			Connection
			(
				GpioIo (Exclusive, PullDown, 0x0000, 0x0000,
					IoRestrictionOutputOnly,
					"\\_SB.PCI0.I2C2.PMIC", 0x00,
					ResourceConsumer,,)
				{
					5
				}
			),
			GPO5, 1,
		}

		/* Set or clear GRST GPIO */
		Method (CRST, 1, Serialized)
		{
			GRST = Arg0
		}

		/* Read GPO1 GPIO, to configure as input */
		Method (CGP1, 0, Serialized)
		{
			Return (GPO1)
		}

		/* Read GPO2 GPIO, to configure as input */
		Method (CGP2, 0, Serialized)
		{
			Return (GPO2)
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

		Method (CLK, 1, Serialized) {
			If (Arg0 == 0) {
				BODI = 0
				BUDI = 0
				PSWR = 0
				XTDV = 0
				PLDV = 0
				PODV = 0
				PDV2 = 0
				/* Disable clocks for both the
				sensors */
				CFG2 = 0
				CFG1 = 0
				PCTL = 0
				Sleep(1)
			} ElseIf (Arg0 == 1) {
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
				/* Set PLL output divider for HCLK_B */
				PDV2 = 1
				/* Enable clocks for both the sensors
				 * CFG1: output selection for HCLK_A and
				 * HCLK_B.
				 * CFG2: set drive strength for HCLK_A
				 * and HCLK_B.
				 */
				CFG2 = 5
				CFG1 = 10
				/* Enable PLL output, crystal oscillator
				 * input capacitance control and set
				 * Xtal oscillator as clock source.
				 */
				PCTL = 209
				Sleep(1)
			}
		}

		/* Reference count for VSIO */
		Name (VSIC, 0)
		Method (DOVD, 1, Serialized) {
			/* Turn off VSIO */
			If (Arg0 == 0) {
				/* Decrement only if VSIC > 0 */
				if (VSIC > 0) {
					VSIC--
					If (VSIC == 0) {
						VSIO = 0
						Sleep(1)
						PMOF()
					}
				}
			} ElseIf (Arg0 == 1) {
				/* Increment only if VSIC < 4 */
				If (VSIC < 4) {
					/* Turn on VSIO */
					If (VSIC == 0) {
						PMON()
						VSIO = 3

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
					}
					VSIC++
				}
			}
		}

		/* Power resource methods for both CAMs */
		PowerResource (OVCM, 0, 0) {
			Name (STA, 0)
			Method (_ON, 0, Serialized) {
				If (AVBL == 1) {
					If (STA == 0) {
						/* Enable VSIO regulator +
						daisy chain */
						DOVD(1)

						CLK(1)
						STA = 1
					}
				}
			}
			Method (_OFF, 0, Serialized) {
				If (AVBL == 1) {
					If (STA == 1) {
						CLK(0)
						Sleep(2)
						DOVD(0)
					}
				}
				STA = 0
			}
			Method (_STA, 0, NotSerialized) {
				Return (STA)
			}
		}

		/* Power resource methods for CAM0 */
		PowerResource (OVTH, 0, 1) {
			Name (STA, 0)
			Method (_ON, 0, Serialized) {
				/* TODO: Read Voltage and Sleep values from Sensor Obj */
				If (AVBL == 1) {
					If (STA == 0) {
						\_SB.PCI0.I2C2.PMIC.CGP1()
						\_SB.PCI0.I2C2.PMIC.CGP2()

						/* Set ANA at 2.8152V */
						ACVA = 109
						VACT = 1

						/* Set CORE at 1.2V */
						DCVA = 12
						VDCT = 1

						/*
						 * Wait for all regulator
						 * outputs to settle.
						 */
						Sleep(1)

						\_SB.PCI0.I2C2.PMIC.CRST(1)

						/*
						 * 5 ms needed before
						 * streaming on.
						 */
						Sleep(5)

						STA = 1
					}
				}
			}

			Method (_OFF, 0, Serialized) {
				If (AVBL == 1) {
					If (STA == 1) {
						Sleep(2)
						\_SB.PCI0.I2C2.PMIC.CRST(0)
						Sleep(3)
						VDCT = 0
						Sleep(3)
						VACT = 0
						Sleep(1)
					}
				}
				STA = 0
			}
			Method (_STA, 0, NotSerialized) {
				Return (STA)
			}
		}

		/* Power resource methods for CAM1 */
		PowerResource (OVFI, 0, 1) {
			Name (STA, 0)
			Method (_ON, 0, Serialized) {
				/* TODO: Read Voltage and Sleep values from Sensor Obj */
				If (AVBL == 1) {
					If (STA == 0) {
						/* Set VAUX2 as 1.8006 V */
						AX2V = 52
						VAX2 = 1 /* Enable VAUX2 */

						\_SB.PCI0.I2C2.PMIC.CGP4(1)

						/*
						 * Wait for DOVDD and AVDD
						 * to settle.
						 */
						Sleep(1)

						/* Set VAUX1 as 1.2132V */
						AX1V = 19
						VAX1 = 1 /* Enable VAUX1 */

						/* Wait for VDD to settle. */
						Sleep(1)

						\_SB.PCI0.I2C2.PMIC.CGP5(1)
						/*
						 * Ensure 10 ms between
						 * power-up and streamon.
						 */
						Sleep(10)
						STA = 1
					}
				}
			}

			Method (_OFF, 0, Serialized) {
				If (AVBL == 1) {
					If (STA == 1) {
						Sleep(2)
						\_SB.PCI0.I2C2.PMIC.CGP5(0)
						Sleep(3)
						VAX1 = 0
						Sleep(1)
						\_SB.PCI0.I2C2.PMIC.CGP4(0)
						Sleep(1)
						VAX2 = 0
						Sleep(1)
					}
					STA = 0
				}
			}

			Method (_STA, 0, NotSerialized) {
				Return (STA)
			}
		}

		/* Power resource methods for VCM */
		PowerResource (VCMP, 0, 0) {
			Name (STA, 0)
			Method (_ON, 0, Serialized) {
				If (AVBL == 1) {
					If (STA == 0) {
						/* Enable VSIO regulator +
						daisy chain */
						DOVD(1)

						/* Enable VCM regulator */
						VCMC = 1
						/* Set VCM value at 2.8152 V */
						VCMV = 109
						Sleep(3)

						STA = 1
					}
				}
			}

			Method (_OFF, 0, Serialized) {
				If (AVBL == 1) {
					If (STA == 1) {
						VCMC = 0 /* Disable regulator */
						Sleep(1)
						DOVD(0) /* Disable regulator */
						STA = 0
					}
				}
			}

			Method (_STA, 0, NotSerialized) {
				Return (STA)
			}
		}

		/* Power resource methods for NVM */
		PowerResource (NVMP, 0, 0) {
			Name (STA, 0)
			Method (_ON, 0, Serialized) {
				If (AVBL == 1) {
					If (STA == 0) {
						/* Enable VSIO regulator +
						daisy chain */
						DOVD(1)
						STA = 1
					}
				}
			}

			Method (_OFF, 0, Serialized) {
				If (AVBL == 1) {
					If (STA == 1) {
						DOVD(0) /* Disable regulator */
						STA = 0
					}
				}
			}

			Method (_STA, 0, NotSerialized) {
				Return (STA)
			}
		}
	}

}
