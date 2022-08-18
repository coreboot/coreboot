/* SPDX-License-Identifier: GPL-2.0-only */

#undef ENABLE_TOUCH_WAKE

Scope (\_SB.PCI0.I2C0)
{
	Device (ATPA)
	{
		Name (_HID, "ATML0000")
		Name (_DDN, "Atmel Touchpad")
		Name (_UID, 2)
		Name (_S0W, 4)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TRACKPAD_I2C_ADDR,	// SlaveAddress
				ControllerInitiated,		// SlaveMode
				400000,				// ConnectionSpeed
				AddressingMode7Bit,		// AddressingMode
				"\\_SB.PCI0.I2C0",		// ResourceSource
			)

			// GPIO13 is PIRQL
			Interrupt (ResourceConsumer, Level, ActiveLow) { 27 }
		})

		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Local0 = BOARD_TRACKPAD_WAKE_GPIO
			If (Arg0 == 1) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}

		Method (_STA)
		{
			If (\S1EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}

	Device (CODC)
	{
		/*
		 * TODO(kane): Need official HID.
		 *
		 */
		Name (_HID, "RT5677CE")
		Name (_DDN, "RT5667 Codec")
		Name (_UID, 1)

		Name (MB1, 1)   /* MICBIAS1 = 2.970V */
		Name (DACR, 1)  /* Use codec internal 1.8V as DACREF source */
		Name (DCLK, 0)  /* RT5677_DMIC_CLK1 */
		Name (PCLK, 1)  /* RT5677_PDM_CLK_DIV2 (~3MHz) */
		Name (IN1, 1)   /* IN1 differential */
		Name (IN2, 0)   /* IN2 not differential */
		Name (OUT1, 1)  /* LOUT1 differential */
		Name (OUT2, 1)  /* LOUT2 differential */
		Name (OUT3, 0)  /* LOUT3 differential */
		Name (ASRC, 1)  /* Enable I2S1 ASRC */
		Name (JD1, 0)   /* JackDetect1 is not used */
		Name (JD2, 2)   /* Use GPIO5 as JackDetect2 */
		Name (JD3, 3)   /* Use GPIO6 as JackDetect3 */

		/* Add DT style bindings with _DSD */
		Name (_DSD, Package () {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "realtek,micbias1", 1 },
				Package () { "realtek,internal-dacref-en", 1 },
				Package () { "realtek,in1-differential", 1 },
				Package () { "realtek,in2-differential", 0 },
				Package () { "realtek,lout1-differential", 1 },
				Package () { "realtek,lout2-differential", 1 },
				Package () { "realtek,lout3-differential", 0 },
				Package () { "realtek,pdm_clk_div", 1 },
				Package () { "realtek,dmic2_clk_pin", 0 },
				Package () { "realtek,asrc-en", 1 },
				Package () { "realtek,jd1-gpio", 0 },
				Package () { "realtek,jd2-gpio", 2 },
				Package () { "realtek,jd3-gpio", 3 },
				Package () { "realtek,gpio-config",
					Package () { 0, 0, 0, 0, 0, 2 } },
			}
		})

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_CODEC_I2C_ADDR,		// SlaveAddress
				ControllerInitiated,		// SlaveMode
				400000,				// ConnectionSpeed
				AddressingMode7Bit,		// AddressingMode
				"\\_SB.PCI0.I2C0",		// ResourceSource
			)

			/* GPIO46 is PIRQO (use HOTWORD_DET as codec IRQ) */
			Interrupt (ResourceConsumer, Edge, ActiveHigh) { 30 }

			/*
			 * Codec GPIOs are 1-based in the schematic
			 *
			 * [0] = Jack Detect (INPUT)
			 * [1] = Mic Present (INPUT)
			 * [2] = Interrupt to the host (OUTPUT)
			 * [3] = Interrupt to the host (OUTPUT)
			 * [4] = Headphone amp control (OUTPUT)
			 */

			/* Index 0:  Jack Detect - PLUG_DET is GPIO5 */
			GpioIo (Exclusive, PullUp, , , IoRestrictionInputOnly,
				"\\_SB.PCI0.I2C0.CODC") { 4 }

			/* Index 1:  Mic Present - MIC_PRESENT_L is GPIO6 */
			GpioIo (Exclusive, PullUp, , , IoRestrictionInputOnly,
				"\\_SB.PCI0.I2C0.CODC") { 5 }

			/* Index 2:  Codec IRQ - HOTWORD_DET_L is GPIO1 */
			GpioIo (Exclusive, PullUp, , , IoRestrictionOutputOnly,
				"\\_SB.PCI0.I2C0.CODC") { 0 }

			/* Index 3:  Codec Wake - DSP_INT is GPIO4 */
			GpioIo (Exclusive, PullUp, , , IoRestrictionOutputOnly,
				"\\_SB.PCI0.I2C0.CODC") { 3 }

			/* Index 4:  Headphone amp - HP_AMP_SHDN_L is GPIO2 */
			GpioIo (Exclusive, PullDown, , , IoRestrictionOutputOnly,
				"\\_SB.PCI0.I2C0.CODC") { 1 }
		})

		Name (_PRW, Package() { BOARD_CODEC_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Local0 = BOARD_CODEC_WAKE_GPIO
			If (Arg0 == 1) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}

		Method (_STA)
		{
			If (\S1EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
}

Scope (\_SB.PCI0.I2C1)
{
	Device (ATSA)
	{
		Name (_HID, "ATML0001")
		Name (_DDN, "Atmel Touchscreen")
		Name (_UID, 5)
		Name (_S0W, 4)
		Name (ISTP, 0) /* TouchScreen */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TOUCHSCREEN_I2C_ADDR,	// SlaveAddress
				ControllerInitiated,		// SlaveMode
				400000,				// ConnectionSpeed
				AddressingMode7Bit,		// AddressingMode
				"\\_SB.PCI0.I2C1",		// ResourceSource
			)

			// GPIO14 is PIRQM
			Interrupt (ResourceConsumer, Level, ActiveLow) { 28 }
		})

#ifdef ENABLE_TOUCH_WAKE
		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Local0 = BOARD_CODEC_WAKE_GPIO
			If (Arg0 == 1) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}
#endif

		Method (_STA)
		{
			If (\S2EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
}

Scope (\_SB.PCI0.SPI0)
{
	Device (CODC)
	{
		// TODO: Need official HID.
		Name (_HID, "RT5677AA")
		Name (_UID, 1)
		Name (_CRS, ResourceTemplate ()
		{
			SpiSerialBus (
				0,                   // DeviceSelection (CS0?)
				PolarityLow,         // DeviceSelectionPolarity
				FourWireMode,        // WireMode
				8,                   // DataBitLength
				ControllerInitiated, // SlaveMode
				1000000,             // ConnectionSpeed (1MHz)
				ClockPolarityLow,    // ClockPolarity
				ClockPhaseFirst,     // ClockPhase
				"\\_SB.PCI0.SPI0",   // ResourceSource
				0,                   // ResourceSourceIndex
				ResourceConsumer,    // ResourceUsage
			)
		})

		Method (_STA)
		{
			Return (0xB)	//hidden from Windows
		}
	}
}
