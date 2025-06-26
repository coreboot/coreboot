/* SPDX-License-Identifier: GPL-2.0-only */

Scope(\_SB.PCI0.I2C3)
{
	/*
	 * PA01 Device: Defines how the PAC194x is connected to the I2C Controller.
	 */
	Device(PA01)
	{
		Name(_HID, "MCHP1940")
		Name(_UID, 1)

		/* Lowest power D-State supported by the device is D3 */
		Name (_S0W, 3)

		/* Device Status: present, enabled, and functioning properly */
		Method(_STA, 0x0, NotSerialized)
		{
			Return (0xf)
		}

		/*
		 * Current Resources Settings
		 */
		Method(_CRS, 0x0, NotSerialized)
		{
			Name(RBUF, ResourceTemplate()
			{
				I2CSerialBusV2(0x18,    /* 7-bit Peripheral Address */
				ControllerInitiated,    /* Peripheral or Controller? */
				400000,                 /* Connection Speed in hz */
				AddressingMode7Bit,     /* 7-bit or 10-bit addressing? */
				"\\_SB.PCI0.I2C3",      /* I2C Controller to which PAC is connected */
				0,                      /* Resource Index */
				ResourceConsumer,       /* Consumer or Producer? */
				DEV0,)
			})
			Return(RBUF)
		}

		/*
		 * _DSM - Device Specific Method
		 *
		 * This method returns information that PAC194x/5x driver uses for the
		 * device initial configuration.
		 *
		 * The DSM UUID for the Microchip PAC194x/5x: {721F1534-5D27-4B60-9DF4-41A3C4B7DA3A}.
		 * This must match what the Windows PAC194x/5x driver expects.
		 *
		 * Returns:
		 * Either: A Buffer (for supported Functions, or an error)
		 * A Package containing PAC194x/5x resources (configuration values)
		 *
		 * Input Arguments (per _DSM standard):
		 *
		 *     Arg0: UUID - Function Identifier
		 *     Arg1: Integer - Revision
		 *     Arg2: Integer - Function Index
		 *     Arg3: Package - Parameters (not used in our implementation)
		 */
		Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
		{
			/*
			 *  Is our UUID being invoked?
			 */
			if(LNotEqual(Arg0, ToUUID("721F1534-5D27-4B60-9DF4-41A3C4B7DA3A")))
			{
				return(Buffer() {0x0})  /* incorrect UUID, return NULL for error */
			}

			/*
			 * Switch based on the function number requested...
			 */
			switch(ToInteger(Arg2))
			{
				/*
				 * Function zero returns a bit-mask of supported functions
				 */
				case(0)
				{

					switch(ToInteger(Arg1)) /* revision check */
					{
						/* Revision 0: function 1->7 are supported */
						case(0) {return (Buffer() {0xFF})}
					}
					break;
				}

				/* Function 1 - returns the names of the monitored power rails. */
				/* The names should be compliant with Windows EMI power rail taxonomy. */
				/* If the rail name is NULL ("") -> no EMI created for the channel ("private channel"). */
				case(1)
				{
					Name(BUF1, Package()
					{
							/* Rail Name */
							"SoC_VCCCORE_PH1", /* Channel 1 */
							"SoC_VCCCORE_PH2", /* Channel 2 */
							"SoC_VCCCORE_PH3", /* Channel 3 */
							"SoC_VCCCORE_PH4"  /* Channel 4 */
					})
					return(BUF1)
				}

				/* Function 2 returns the Resistor values expressed in micro-Ohms. */
				case(2)
				{
					/* Return enhanced precision resistor values. */
					Name(BUF2, Package()
					{
							/* Value */
							2000,   /* Channel 1 */
							2000,   /* Channel 2 */
							2000,   /* Channel 3 */
							2000    /* Channel 4 */
					})
					return(BUF2)
				}

				/* Function 3 - returns the EMI enable/disable bit-mask */
				case(3)
				{
					Name(BUF3, Package()
					{
						/* EMI bitmask - CH1:CH2:CH3:CH4 */
						0xF	/* CHn = 1: EMI enable for channel 'n' */
							/* CHn = 0: EMI disable for channel 'n' ("private channel") */
					})
					return(BUF3)
				}

				/* Function 4 - returns Vsense and Vbus polarity and full scale range configs */
				case(4)
				{
					Name(BUF4, Package()
					{
						/* Vsense configs */
						0x0, 0x0, 0x0, 0x0,	/* CFG_VS1, CFG_VS2, CFG_VS3, CFG_VS4 */
									/* CFG_VSn: Vsense configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
									/* Vbus configs */
						0x0, 0x0, 0x0, 0x0	/* CFG_VB1, CFG_VB2, CFG_VB3, CFG_VB4 */
									/* CFG_VBn: Vbus configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
					})
					return(BUF4)
				}

				/* Function 5 - returns the sample frequencies for ACTIVE and IDLE modes */
				case(5)
				{
					Name(BUF5, Package()
					 {
						1024, /* ACTIVE sps - accepted values = {1024, 256, 64, 8} */
						8     /* IDLE sps - accepted values = {1024, 256, 64, 8} */
					 })
					return(BUF5)
				}

				/* Function 6 - returns the REFRESH watchdog timer interval */
				case(6)
				{
					Name(BUF6, Package()
					{
						900 /* seconds (min=60, max=60000) */
					})
					return(BUF6)
				}

				/* Function 7 - returns the Vbus multiplication factors for PAC194x/5x-2 */
				/* PAC194x/5x-2 power meter VBUS+ pins may be connected to voltage dividers */
				/* to measure rail voltage higher than device Vbus FSR. */
				case(7)
				{
					Name(BUF7, Package()
					{ /* K = 1000 * (Vrail / Vbus). Must be integer, positive value. */
						1000, /* Channel 1 K-factor */
						1000, /* Channel 2 K-factor */
						1000, /* Channel 3 K-factor */
						1000  /* Channel 4 K-factor */
					})
					return(BUF7)
				}

			} /* switch(Arg2) */
			/*
			 * Return an error (a buffer with a value of zero)
			 * if we didn't return anything else above
			 */
			return(Buffer() {0x0})
		} /* _DSM */
	} /* PowerMeter Device PA01 Scope End */

	/*
	 * PA02 Device: Defines how the PAC194x is connected to the I2C Controller.
	 */
	Device(PA02)
	{
		Name(_HID, "MCHP1940")
		Name(_UID, 2)

		/* Lowest power D-State supported by the device is D3 */
		Name (_S0W, 3)

		/* Device Status: present, enabled, and functioning properly */
		Method(_STA, 0x0, NotSerialized)
		{
			Return (0xf)
		}

		/*
		 * Current Resources Settings
		 */
		Method(_CRS, 0x0, NotSerialized)
		{
			Name(RBUF, ResourceTemplate()
			{
				I2CSerialBusV2(0x1E,                    /* 7-bit Peripheral Address */
						ControllerInitiated,    /* Peripheral or Controller? */
						400000,                 /* Connection Speed in hz */
						AddressingMode7Bit,     /* 7-bit or 10-bit addressing? */
						"\\_SB.PCI0.I2C3",      /* I2C Controller to which PAC is connected */
						0,                      /* Resource Index */
						ResourceConsumer,       /* Consumer or Producer? */
						DEV0,)
			})
			Return(RBUF)
		}

		/*
		 * _DSM - Device Specific Method
		 *
		 * This method returns information that PAC194x/5x driver uses for the
		 * device initial configuration.
		 *
		 * The DSM UUID for the Microchip PAC194x/5x: {721F1534-5D27-4B60-9DF4-41A3C4B7DA3A}.
		 * This must match what the Windows PAC194x/5x driver expects.
		 *
		 * Returns:
		 * Either: A Buffer (for supported Functions, or an error)
		 * A Package containing PAC194x/5x resources (configuration values)
		 *
		 * Input Arguments (per _DSM standard):
		 *
		 *     Arg0: UUID - Function Identifier
		 *     Arg1: Integer - Revision
		 *     Arg2: Integer - Function Index
		 *     Arg3: Package - Parameters (not used in our implementation)
		 */
		Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
		{
			/*
			 *  Is our UUID being invoked?
			 */
			if(LNotEqual(Arg0, ToUUID("721F1534-5D27-4B60-9DF4-41A3C4B7DA3A")))
			{
				return(Buffer() {0x0})  /* incorrect UUID, return NULL for error */
			}

			/*
			 * Switch based on the function number requested...
			 */
			switch(ToInteger(Arg2))
			{
				/*
				 * Function zero returns a bit-mask of supported functions
				 */
				case(0)
				{
					switch(ToInteger(Arg1)) /* revision check */
					{
						/* Revision 0: function 1->7 are supported */
						case(0) {return (Buffer() {0xFF})}
					}
					break;
				}

				/* Function 1 - returns the names of the monitored power rails. */
				/* The names should be compliant with Windows EMI power rail taxonomy. */
				/* If the rail name is NULL ("") -> no EMI created for the channel ("private channel"). */
				case(1)
				{
					Name(BUF1, Package()
					{
							/* Rail Name */
							"SoC_VCCGT_PH1",  /* Channel 1 */
							"SoC_VCCGT_PH2",  /* Channel 2 */
							"SoC_VCCSA",      /* Channel 3 */
							"SoC_VDDQ"        /* Channel 4 */
					})
					return(BUF1)
				}

				/* Function 2 returns the Resistor values expressed in micro-Ohms. */
				case(2)
				{
					/* Return enhanced precision resistor values. */
					Name(BUF2, Package()
					{
							/* Value */
							2000,   /* Channel 1 */
							2000,   /* Channel 2 */
							2000,   /* Channel 3 */
							2000    /* Channel 4 */
					})
					return(BUF2)
				}

				/* Function 3 - returns the EMI enable/disable bit-mask */
				case(3)
				{
					Name(BUF3, Package()
					{
						/* EMI bitmask - CH1:CH2:CH3:CH4 */
						0xF	/* CHn = 1: EMI enable for channel 'n' */
							/* CHn = 0: EMI disable for channel 'n' ("private channel") */
					})
					return(BUF3)
				}

				/* Function 4 - returns Vsense and Vbus polarity and full scale range configs */
				case(4)
				{
					Name(BUF4, Package()
					{
						/* Vsense configs */
						0x0, 0x0, 0x0, 0x0,	/* CFG_VS1, CFG_VS2, CFG_VS3, CFG_VS4 */
									/* CFG_VSn: Vsense configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
									/* Vbus configs */
						0x0, 0x0, 0x0, 0x0	/* CFG_VB1, CFG_VB2, CFG_VB3, CFG_VB4 */
									/* CFG_VBn: Vbus configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
					})
					return(BUF4)
				}

				/* Function 5 - returns the sample frequencies for ACTIVE and IDLE modes */
				case(5)
				{
					Name(BUF5, Package()
					 {
						1024, /* ACTIVE sps - accepted values = {1024, 256, 64, 8} */
						8     /* IDLE sps - accepted values = {1024, 256, 64, 8} */
					 })
					return(BUF5)
				}

				/* Function 6 - returns the REFRESH watchdog timer interval */
				case(6)
				{
					Name(BUF6, Package()
					{
						900 /* seconds (min=60, max=60000) */
					})
					return(BUF6)
				}

				/* Function 7 - returns the Vbus multiplication factors for PAC194x/5x-2 */
				/* PAC194x/5x-2 power meter VBUS+ pins may be connected to voltage dividers */
				/* to measure rail voltage higher than device Vbus FSR. */
				case(7)
				{
					Name(BUF7, Package()
					{ /* K = 1000 * (Vrail / Vbus). Must be integer, positive value. */
						1000, /* Channel 1 K-factor */
						1000, /* Channel 2 K-factor */
						1000, /* Channel 3 K-factor */
						1000  /* Channel 4 K-factor */
					})
					return(BUF7)
				}

			} /* switch(Arg2) */
			/*
			 * Return an error (a buffer with a value of zero)
			 * if we didn't return anything else above
			 */
			return(Buffer() {0x0})
		} /* _DSM */
	} /* PowerMeter Device PA02 Scope End */

	/*
	 * PA03 Device: Defines how the PAC194x is connected to the I2C Controller.
	 */
	Device(PA03)
	{
		Name(_HID, "MCHP1940")
		Name(_UID, 3)

		/* Lowest power D-State supported by the device is D3 */
		Name (_S0W, 3)

		/* Device Status: present, enabled, and functioning properly */
		Method(_STA, 0x0, NotSerialized)
		{
			Return (0xf)
		}

		/*
		 * Current Resources Settings
		 */
		Method(_CRS, 0x0, NotSerialized)
		{
			Name(RBUF, ResourceTemplate()
			{
				I2CSerialBusV2(0x11,                    /* 7-bit Peripheral Address */
						ControllerInitiated,    /* Peripheral or Controller? */
						400000,                 /* Connection Speed in hz */
						AddressingMode7Bit,     /* 7-bit or 10-bit addressing? */
						"\\_SB.PCI0.I2C3",      /* I2C Controller to which PAC is connected */
						0,                      /* Resource Index */
						ResourceConsumer,       /* Consumer or Producer? */
						DEV0,)
			})
			Return(RBUF)
		}

		/*
		 * _DSM - Device Specific Method
		 *
		 * This method returns information that PAC194x/5x driver uses for the
		 * device initial configuration.
		 *
		 * The DSM UUID for the Microchip PAC194x/5x: {721F1534-5D27-4B60-9DF4-41A3C4B7DA3A}.
		 * This must match what the Windows PAC194x/5x driver expects.
		 *
		 * Returns:
		 * Either: A Buffer (for supported Functions, or an error)
		 * A Package containing PAC194x/5x resources (configuration values)
		 *
		 * Input Arguments (per _DSM standard):
		 *
		 *     Arg0: UUID - Function Identifier
		 *     Arg1: Integer - Revision
		 *     Arg2: Integer - Function Index
		 *     Arg3: Package - Parameters (not used in our implementation)
		 */
		Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
		{
			/*
			 *  Is our UUID being invoked?
			 */
			if(LNotEqual(Arg0, ToUUID("721F1534-5D27-4B60-9DF4-41A3C4B7DA3A")))
			{
				return(Buffer() {0x0})  /* incorrect UUID, return NULL for error */
			}

			/*
			 * Switch based on the function number requested...
			 */
			switch(ToInteger(Arg2))
			{
				/*
				 * Function zero returns a bit-mask of supported functions
				 */
				case(0)
				{
					switch(ToInteger(Arg1)) /* revision check */
					{
						/* Revision 0: function 1->7 are supported */
						case(0) {return (Buffer() {0xFF})}
					}
					break;
				}

				/* Function 1 - returns the names of the monitored power rails. */
				/* The names should be compliant with Windows EMI power rail taxonomy. */
				/* If the rail name is NULL ("") -> no EMI created for the channel ("private channel"). */
				case(1)
				{
					Name(BUF1, Package()
					{
							/* Rail Name */
							"SoC_VCCPRIM_VNNAON", /* Channel 1 */
							"SoC_VCCPRIM_IO", /* Channel 2 */
							"SoC_VCCPRIM3P3", /* Channel 3 */
							"SoC_VCCPRIM1P8"  /* Channel 4 */
					})
					return(BUF1)
				}

				/* Function 2 returns the Resistor values expressed in micro-Ohms. */
				case(2)
				{
					/* Return enhanced precision resistor values. */
					Name(BUF2, Package()
					{
							/* Value */
							1000,    /* Channel 1 */
							4000,    /* Channel 2 */
							22000,   /* Channel 3 */
							10000    /* Channel 4 */
					})
					return(BUF2)
				}

				/* Function 3 - returns the EMI enable/disable bit-mask */
				case(3)
				{
					Name(BUF3, Package()
					{
						/* EMI bitmask - CH1:CH2:CH3:CH4 */
						0xF	/* CHn = 1: EMI enable for channel 'n' */
							/* CHn = 0: EMI disable for channel 'n' ("private channel") */
					})
					return(BUF3)
				}

				/* Function 4 - returns Vsense and Vbus polarity and full scale range configs */
				case(4)
				{
					Name(BUF4, Package()
					{
						/* Vsense configs */
						0x0, 0x0, 0x0, 0x0,	/* CFG_VS1, CFG_VS2, CFG_VS3, CFG_VS4 */
									/* CFG_VSn: Vsense configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
									/* Vbus configs */
						0x0, 0x0, 0x0, 0x0	/* CFG_VB1, CFG_VB2, CFG_VB3, CFG_VB4 */
									/* CFG_VBn: Vbus configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
					})
					return(BUF4)
				}

				/* Function 5 - returns the sample frequencies for ACTIVE and IDLE modes */
				case(5)
				{
					Name(BUF5, Package()
					 {
						1024, /* ACTIVE sps - accepted values = {1024, 256, 64, 8} */
						8     /* IDLE sps - accepted values = {1024, 256, 64, 8} */
					 })
					return(BUF5)
				}

				/* Function 6 - returns the REFRESH watchdog timer interval */
				case(6)
				{
					Name(BUF6, Package()
					{
						900 /* seconds (min=60, max=60000) */
					})
					return(BUF6)
				}

				/* Function 7 - returns the Vbus multiplication factors for PAC194x/5x-2 */
				/* PAC194x/5x-2 power meter VBUS+ pins may be connected to voltage dividers */
				/* to measure rail voltage higher than device Vbus FSR. */
				case(7)
				{
					Name(BUF7, Package()
					{ /* K = 1000 * (Vrail / Vbus). Must be integer, positive value. */
						1000, /* Channel 1 K-factor */
						1000, /* Channel 2 K-factor */
						1000, /* Channel 3 K-factor */
						1000  /* Channel 4 K-factor */
					})
					return(BUF7)
				}

			} /* switch(Arg2) */
			/*
			 * Return an error (a buffer with a value of zero)
			 * if we didn't return anything else above
			 */
			return(Buffer() {0x0})
		} /* _DSM */
	} /* PowerMeter Device PA03 Scope End */

	/*
	 * PA04 Device: Defines how the PAC194x is connected to the I2C Controller.
	 */
	Device(PA04)
	{
		Name(_HID, "MCHP1940")
		Name(_UID, 4)

		/* Lowest power D-State supported by the device is D3 */
		Name (_S0W, 3)

		/* Device Status: present, enabled, and functioning properly */
		Method(_STA, 0x0, NotSerialized)
		{
			Return (0xf)
		}

		/*
		 * Current Resources Settings
		 */
		Method(_CRS, 0x0, NotSerialized)
		{
			Name(RBUF, ResourceTemplate()
			{
				I2CSerialBusV2(0x15,                    /* 7-bit Peripheral Address */
						ControllerInitiated,    /* Peripheral or Controller? */
						400000,                 /* Connection Speed in hz */
						AddressingMode7Bit,     /* 7-bit or 10-bit addressing? */
						"\\_SB.PCI0.I2C3",      /* I2C Controller to which PAC is connected */
						0,                      /* Resource Index */
						ResourceConsumer,       /* Consumer or Producer? */
						DEV0,)
			})
			Return(RBUF)
		}

		/*
		 * _DSM - Device Specific Method
		 *
		 * This method returns information that PAC194x/5x driver uses for the
		 * device initial configuration.
		 *
		 * The DSM UUID for the Microchip PAC194x/5x: {721F1534-5D27-4B60-9DF4-41A3C4B7DA3A}.
		 * This must match what the Windows PAC194x/5x driver expects.
		 *
		 * Returns:
		 * Either: A Buffer (for supported Functions, or an error)
		 * A Package containing PAC194x/5x resources (configuration values)
		 *
		 * Input Arguments (per _DSM standard):
		 *
		 *     Arg0: UUID - Function Identifier
		 *     Arg1: Integer - Revision
		 *     Arg2: Integer - Function Index
		 *     Arg3: Package - Parameters (not used in our implementation)
		 */
		Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
		{
			/*
			 *  Is our UUID being invoked?
			 */
			if(LNotEqual(Arg0, ToUUID("721F1534-5D27-4B60-9DF4-41A3C4B7DA3A")))
			{
				return(Buffer() {0x0})  /* incorrect UUID, return NULL for error */
			}

			/*
			 * Switch based on the function number requested...
			 */
			switch(ToInteger(Arg2))
			{
				/*
				 * Function zero returns a bit-mask of supported functions
				 */
				case(0)
				{
					switch(ToInteger(Arg1)) /* revision check */
					{
						/* Revision 0: function 1->7 are supported */
						case(0) {return (Buffer() {0xFF})}
					}
					break;
				}

				/* Function 1 - returns the names of the monitored power rails. */
				/* The names should be compliant with Windows EMI power rail taxonomy. */
				/* If the rail name is NULL ("") -> no EMI created for the channel ("private channel"). */
				case(1)
				{
					Name(BUF1, Package()
					{
							/* Rail Name */
							"SoC_VCCRTC",   /* Channel 1 */
							"SoC_VCCST",    /* Channel 2 */
							"SoC_VDD2_CPU", /* Channel 3 */
							"SoC_VDDQ_CPU"  /* Channel 4 */
					})
					return(BUF1)
				}

				/* Function 2 returns the Resistor values expressed in micro-Ohms. */
				case(2)
				{
					/* Return enhanced precision resistor values. */
					Name(BUF2, Package()
					{
							/* Value */
							22000,    /* Channel 1 */
							2000,    /* Channel 2 */
							2000,   /* Channel 3 */
							5000    /* Channel 4 */
					})
					return(BUF2)
				}

				/* Function 3 - returns the EMI enable/disable bit-mask */
				case(3)
				{
					Name(BUF3, Package()
					{
						/* EMI bitmask - CH1:CH2:CH3:CH4 */
						0xF	/* CHn = 1: EMI enable for channel 'n' */
							/* CHn = 0: EMI disable for channel 'n' ("private channel") */
					})
					return(BUF3)
				}

				/* Function 4 - returns Vsense and Vbus polarity and full scale range configs */
				case(4)
				{
					Name(BUF4, Package()
					{
						/* Vsense configs */
						0x0, 0x0, 0x0, 0x0,	/* CFG_VS1, CFG_VS2, CFG_VS3, CFG_VS4 */
									/* CFG_VSn: Vsense configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
									/* Vbus configs */
						0x0, 0x0, 0x0, 0x0	/* CFG_VB1, CFG_VB2, CFG_VB3, CFG_VB4 */
									/* CFG_VBn: Vbus configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
					})
					return(BUF4)
				}

				/* Function 5 - returns the sample frequencies for ACTIVE and IDLE modes */
				case(5)
				{
					Name(BUF5, Package()
					 {
						1024, /* ACTIVE sps - accepted values = {1024, 256, 64, 8} */
						8     /* IDLE sps - accepted values = {1024, 256, 64, 8} */
					 })
					return(BUF5)
				}

				/* Function 6 - returns the REFRESH watchdog timer interval */
				case(6)
				{
					Name(BUF6, Package()
					{
						900 /* seconds (min=60, max=60000) */
					})
					return(BUF6)
				}

				/* Function 7 - returns the Vbus multiplication factors for PAC194x/5x-2 */
				/* PAC194x/5x-2 power meter VBUS+ pins may be connected to voltage dividers */
				/* to measure rail voltage higher than device Vbus FSR. */
				case(7)
				{
					Name(BUF7, Package()
					{ /* K = 1000 * (Vrail / Vbus). Must be integer, positive value. */
						1000, /* Channel 1 K-factor */
						1000, /* Channel 2 K-factor */
						1000, /* Channel 3 K-factor */
						1000  /* Channel 4 K-factor */
					})
					return(BUF7)
				}

			} /* switch(Arg2) */
			/*
			 * Return an error (a buffer with a value of zero)
			 * if we didn't return anything else above
			 */
			return(Buffer() {0x0})
		} /* _DSM */
	} /* PowerMeter Device PA04 Scope End */

	/*
	 * PA05 Device: Defines how the PAC194x is connected to the I2C Controller.
	 */
	Device(PA05)
	{
		Name(_HID, "MCHP1940")
		Name(_UID, 5)

		/* Lowest power D-State supported by the device is D3 */
		Name (_S0W, 3)

		/* Device Status: present, enabled, and functioning properly */
		Method(_STA, 0x0, NotSerialized)
		{
			Return (0xf)
		}

		/*
		 * Current Resources Settings
		 */
		Method(_CRS, 0x0, NotSerialized)
		{
			Name(RBUF, ResourceTemplate()
			{
				I2CSerialBusV2(0x19,                    /* 7-bit Peripheral Address */
						ControllerInitiated,    /* Peripheral or Controller? */
						400000,                 /* Connection Speed in hz */
						AddressingMode7Bit,     /* 7-bit or 10-bit addressing? */
						"\\_SB.PCI0.I2C3",      /* I2C Controller to which PAC is connected */
						0,                      /* Resource Index */
						ResourceConsumer,       /* Consumer or Producer? */
						DEV0,)
			})
			Return(RBUF)
		}

		/*
		 * _DSM - Device Specific Method
		 *
		 * This method returns information that PAC194x/5x driver uses for the
		 * device initial configuration.
		 *
		 * The DSM UUID for the Microchip PAC194x/5x: {721F1534-5D27-4B60-9DF4-41A3C4B7DA3A}.
		 * This must match what the Windows PAC194x/5x driver expects.
		 *
		 * Returns:
		 * Either: A Buffer (for supported Functions, or an error)
		 * A Package containing PAC194x/5x resources (configuration values)
		 *
		 * Input Arguments (per _DSM standard):
		 *
		 *     Arg0: UUID - Function Identifier
		 *     Arg1: Integer - Revision
		 *     Arg2: Integer - Function Index
		 *     Arg3: Package - Parameters (not used in our implementation)
		 */
		Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
		{
			/*
			 *  Is our UUID being invoked?
			 */
			if(LNotEqual(Arg0, ToUUID("721F1534-5D27-4B60-9DF4-41A3C4B7DA3A")))
			{
				return(Buffer() {0x0})  /* incorrect UUID, return NULL for error */
			}

			/*
			 * Switch based on the function number requested...
			 */
			switch(ToInteger(Arg2))
			{
				/*
				 * Function zero returns a bit-mask of supported functions
				 */
				case(0)
				{
					switch(ToInteger(Arg1)) /* revision check */
					{
						/* Revision 0: function 1->7 are supported */
						case(0) {return (Buffer() {0xFF})}
					}
					break;
				}

				/* Function 1 - returns the names of the monitored power rails. */
				/* The names should be compliant with Windows EMI power rail taxonomy. */
				/* If the rail name is NULL ("") -> no EMI created for the channel ("private channel"). */
				case(1)
				{
					Name(BUF1, Package()
					{
							/* Rail Name */
							"DRAM_VDD2H",   /* Channel 1 */
							"DRAM_VDD2L",   /* Channel 2 */
							"DRAM_VDDQ",    /* Channel 3 */
							"DRAM_VDD1"     /* Channel 4 */
					})
					return(BUF1)
				}

				/* Function 2 returns the Resistor values expressed in micro-Ohms. */
				case(2)
				{
					/* Return enhanced precision resistor values. */
					Name(BUF2, Package()
					{
							/* Value */
							2000,    /* Channel 1 */
							5000,    /* Channel 2 */
							5000,    /* Channel 3 */
							5000     /* Channel 4 */
					})
					return(BUF2)
				}

				/* Function 3 - returns the EMI enable/disable bit-mask */
				case(3)
				{
					Name(BUF3, Package()
					{
						/* EMI bitmask - CH1:CH2:CH3:CH4 */
						0xF	/* CHn = 1: EMI enable for channel 'n' */
							/* CHn = 0: EMI disable for channel 'n' ("private channel") */
					})
					return(BUF3)
				}

				/* Function 4 - returns Vsense and Vbus polarity and full scale range configs */
				case(4)
				{
					Name(BUF4, Package()
					{
						/* Vsense configs */
						0x0, 0x0, 0x0, 0x0,	/* CFG_VS1, CFG_VS2, CFG_VS3, CFG_VS4 */
									/* CFG_VSn: Vsense configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
									/* Vbus configs */
						0x0, 0x0, 0x0, 0x0	/* CFG_VB1, CFG_VB2, CFG_VB3, CFG_VB4 */
									/* CFG_VBn: Vbus configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
					})
					return(BUF4)
				}

				/* Function 5 - returns the sample frequencies for ACTIVE and IDLE modes */
				case(5)
				{
					Name(BUF5, Package()
					 {
						1024, /* ACTIVE sps - accepted values = {1024, 256, 64, 8} */
						8     /* IDLE sps - accepted values = {1024, 256, 64, 8} */
					 })
					return(BUF5)
				}

				/* Function 6 - returns the REFRESH watchdog timer interval */
				case(6)
				{
					Name(BUF6, Package()
					{
						900 /* seconds (min=60, max=60000) */
					})
					return(BUF6)
				}

				/* Function 7 - returns the Vbus multiplication factors for PAC194x/5x-2 */
				/* PAC194x/5x-2 power meter VBUS+ pins may be connected to voltage dividers */
				/* to measure rail voltage higher than device Vbus FSR. */
				case(7)
				{
					Name(BUF7, Package()
					{ /* K = 1000 * (Vrail / Vbus). Must be integer, positive value. */
						1000, /* Channel 1 K-factor */
						1000, /* Channel 2 K-factor */
						1000, /* Channel 3 K-factor */
						1000  /* Channel 4 K-factor */
					})
					return(BUF7)
				}

			} /* switch(Arg2) */
			/*
			 * Return an error (a buffer with a value of zero)
			 * if we didn't return anything else above
			 */
			return(Buffer() {0x0})
		} /* _DSM */
	} /* PowerMeter Device PA05 Scope End */

	/*
	 * PA06 Device: Defines how the PAC194x is connected to the I2C Controller.
	 */
	Device(PA06)
	{
		Name(_HID, "MCHP1940")
		Name(_UID, 6)

		/* Lowest power D-State supported by the device is D3 */
		Name (_S0W, 3)

		/* Device Status: present, enabled, and functioning properly */
		Method(_STA, 0x0, NotSerialized)
		{
			Return (0xf)
		}

		/*
		 * Current Resources Settings
		 */
		Method(_CRS, 0x0, NotSerialized)
		{
			Name(RBUF, ResourceTemplate()
			{
				I2CSerialBusV2(0x14,                    /* 7-bit Peripheral Address */
						ControllerInitiated,    /* Peripheral or Controller? */
						400000,                 /* Connection Speed in hz */
						AddressingMode7Bit,     /* 7-bit or 10-bit addressing? */
						"\\_SB.PCI0.I2C3",      /* I2C Controller to which PAC is connected */
						0,                      /* Resource Index */
						ResourceConsumer,       /* Consumer or Producer? */
						DEV0,)
			})
			Return(RBUF)
		}

		/*
		 * _DSM - Device Specific Method
		 *
		 * This method returns information that PAC194x/5x driver uses for the
		 * device initial configuration.
		 *
		 * The DSM UUID for the Microchip PAC194x/5x: {721F1534-5D27-4B60-9DF4-41A3C4B7DA3A}.
		 * This must match what the Windows PAC194x/5x driver expects.
		 *
		 * Returns:
		 * Either: A Buffer (for supported Functions, or an error)
		 * A Package containing PAC194x/5x resources (configuration values)
		 *
		 * Input Arguments (per _DSM standard):
		 *
		 *     Arg0: UUID - Function Identifier
		 *     Arg1: Integer - Revision
		 *     Arg2: Integer - Function Index
		 *     Arg3: Package - Parameters (not used in our implementation)
		 */
		Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
		{
			/*
			 *  Is our UUID being invoked?
			 */
			if(LNotEqual(Arg0, ToUUID("721F1534-5D27-4B60-9DF4-41A3C4B7DA3A")))
			{
				return(Buffer() {0x0})  /* incorrect UUID, return NULL for error */
			}

			/*
			 * Switch based on the function number requested...
			 */
			switch(ToInteger(Arg2))
			{
				/*
				 * Function zero returns a bit-mask of supported functions
				 */
				case(0)
				{
					switch(ToInteger(Arg1)) /* revision check */
					{
						/* Revision 0: function 1->7 are supported */
						case(0) {return (Buffer() {0xFF})}
					}
					break;
				}

				/* Function 1 - returns the names of the monitored power rails. */
				/* The names should be compliant with Windows EMI power rail taxonomy. */
				/* If the rail name is NULL ("") -> no EMI created for the channel ("private channel"). */
				case(1)
				{
					Name(BUF1, Package()
					{
							/* Rail Name */
							"DISPLAY_BKLIGHT",    /* Channel 1 */
							"DISPLAY_VDD",        /* Channel 2 */
							"CONNECTIVITY_WLAN",  /* Channel 3 */
							"STORAGE_V3P3A"       /* Channel 4 */
					})
					return(BUF1)
				}

				/* Function 2 returns the Resistor values expressed in micro-Ohms. */
				case(2)
				{
					/* Return enhanced precision resistor values. */
					Name(BUF2, Package()
					{
							/* Value */
							10000,    /* Channel 1 */
							10000,    /* Channel 2 */
							5000,    /* Channel 3 */
							5000     /* Channel 4 */
					})
					return(BUF2)
				}

				/* Function 3 - returns the EMI enable/disable bit-mask */
				case(3)
				{
					Name(BUF3, Package()
					{
						/* EMI bitmask - CH1:CH2:CH3:CH4 */
						0xF	/* CHn = 1: EMI enable for channel 'n' */
							/* CHn = 0: EMI disable for channel 'n' ("private channel") */
					})
					return(BUF3)
				}

				/* Function 4 - returns Vsense and Vbus polarity and full scale range configs */
				case(4)
				{
					Name(BUF4, Package()
					{
						/* Vsense configs */
						0x0, 0x0, 0x0, 0x0,	/* CFG_VS1, CFG_VS2, CFG_VS3, CFG_VS4 */
									/* CFG_VSn: Vsense configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
									/* Vbus configs */
						0x0, 0x0, 0x0, 0x0	/* CFG_VB1, CFG_VB2, CFG_VB3, CFG_VB4 */
									/* CFG_VBn: Vbus configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
					})
					return(BUF4)
				}

				/* Function 5 - returns the sample frequencies for ACTIVE and IDLE modes */
				case(5)
				{
					Name(BUF5, Package()
					 {
						1024, /* ACTIVE sps - accepted values = {1024, 256, 64, 8} */
						8     /* IDLE sps - accepted values = {1024, 256, 64, 8} */
					 })
					return(BUF5)
				}

				/* Function 6 - returns the REFRESH watchdog timer interval */
				case(6)
				{
					Name(BUF6, Package()
					{
						900 /* seconds (min=60, max=60000) */
					})
					return(BUF6)
				}

				/* Function 7 - returns the Vbus multiplication factors for PAC194x/5x-2 */
				/* PAC194x/5x-2 power meter VBUS+ pins may be connected to voltage dividers */
				/* to measure rail voltage higher than device Vbus FSR. */
				case(7)
				{
					Name(BUF7, Package()
					{ /* K = 1000 * (Vrail / Vbus). Must be integer, positive value. */
						1000, /* Channel 1 K-factor */
						1000, /* Channel 2 K-factor */
						1000, /* Channel 3 K-factor */
						1000  /* Channel 4 K-factor */
					})
					return(BUF7)
				}

			} /* switch(Arg2) */
			/*
			 * Return an error (a buffer with a value of zero)
			 * if we didn't return anything else above
			 */
			return(Buffer() {0x0})
		} /* _DSM */
	} /* PowerMeter Device PA06 Scope End */

	/*
	 * PA07 Device: Defines how the PAC194x is connected to the I2C Controller.
	 */
	Device(PA07)
	{
		Name(_HID, "MCHP1940")
		Name(_UID, 7)

		/* Lowest power D-State supported by the device is D3 */
		Name (_S0W, 3)

		/* Device Status: present, enabled, and functioning properly */
		Method(_STA, 0x0, NotSerialized)
		{
			Return (0xf)
		}

		/*
		 * Current Resources Settings
		 */
		Method(_CRS, 0x0, NotSerialized)
		{
			Name(RBUF, ResourceTemplate()
			{
				I2CSerialBusV2(0x17,                    /* 7-bit Peripheral Address */
						ControllerInitiated,    /* Peripheral or Controller? */
						400000,                 /* Connection Speed in hz */
						AddressingMode7Bit,     /* 7-bit or 10-bit addressing? */
						"\\_SB.PCI0.I2C3",      /* I2C Controller to which PAC is connected */
						0,                      /* Resource Index */
						ResourceConsumer,       /* Consumer or Producer? */
						DEV0,)
			})
			Return(RBUF)
		}

		/*
		 * _DSM - Device Specific Method
		 *
		 * This method returns information that PAC194x/5x driver uses for the
		 * device initial configuration.
		 *
		 * The DSM UUID for the Microchip PAC194x/5x: {721F1534-5D27-4B60-9DF4-41A3C4B7DA3A}.
		 * This must match what the Windows PAC194x/5x driver expects.
		 *
		 * Returns:
		 * Either: A Buffer (for supported Functions, or an error)
		 * A Package containing PAC194x/5x resources (configuration values)
		 *
		 * Input Arguments (per _DSM standard):
		 *
		 *     Arg0: UUID - Function Identifier
		 *     Arg1: Integer - Revision
		 *     Arg2: Integer - Function Index
		 *     Arg3: Package - Parameters (not used in our implementation)
		 */
		Function(_DSM, {BuffObj, PkgObj}, {BuffObj, IntObj, IntObj, PkgObj})
		{
			/*
			 *  Is our UUID being invoked?
			 */
			if(LNotEqual(Arg0, ToUUID("721F1534-5D27-4B60-9DF4-41A3C4B7DA3A")))
			{
				return(Buffer() {0x0})  /* incorrect UUID, return NULL for error */
			}

			/*
			 * Switch based on the function number requested...
			 */
			switch(ToInteger(Arg2))
			{
				/*
				 * Function zero returns a bit-mask of supported functions
				 */
				case(0)
				{
					switch(ToInteger(Arg1)) /* revision check */
					{
						/* Revision 0: function 1->7 are supported */
						case(0) {return (Buffer() {0xFF})}
					}
					break;
				}

				/* Function 1 - returns the names of the monitored power rails. */
				/* The names should be compliant with Windows EMI power rail taxonomy. */
				/* If the rail name is NULL ("") -> no EMI created for the channel ("private channel"). */
				case(1)
				{
					Name(BUF1, Package()
					{
							/* Rail Name */
							"SYSTEM_VBATA1",    /* Channel 1 */
							"SYSTEM_VBATA2",    /* Channel 2 */
							"CAMERA_V1P8S",     /* Channel 3 */
							"CAMERA_V3P3S"      /* Channel 4 */
					})
					return(BUF1)
				}

				/* Function 2 returns the Resistor values expressed in micro-Ohms. */
				case(2)
				{
					/* Return enhanced precision resistor values. */
					Name(BUF2, Package()
					{
							/* Value */
							2000,    /* Channel 1 */
							2000,    /* Channel 2 */
							10000,   /* Channel 3 */
							10000    /* Channel 4 */
					})
					return(BUF2)
				}

				/* Function 3 - returns the EMI enable/disable bit-mask */
				case(3)
				{
					Name(BUF3, Package()
					{
						/* EMI bitmask - CH1:CH2:CH3:CH4 */
						0xF	/* CHn = 1: EMI enable for channel 'n' */
							/* CHn = 0: EMI disable for channel 'n' ("private channel") */
					})
					return(BUF3)
				}

				/* Function 4 - returns Vsense and Vbus polarity and full scale range configs */
				case(4)
				{
					Name(BUF4, Package()
					{
						/* Vsense configs */
						0x0, 0x0, 0x0, 0x0,	/* CFG_VS1, CFG_VS2, CFG_VS3, CFG_VS4 */
									/* CFG_VSn: Vsense configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
									/* Vbus configs */
						0x0, 0x0, 0x0, 0x0	/* CFG_VB1, CFG_VB2, CFG_VB3, CFG_VB4 */
									/* CFG_VBn: Vbus configuration for channel 'n' */
									/* {0x0 = unipolar; 0x1 = bipolar; 0x2 = half scale bipolar} */
					})
					return(BUF4)
				}

				/* Function 5 - returns the sample frequencies for ACTIVE and IDLE modes */
				case(5)
				{
					Name(BUF5, Package()
					 {
						1024, /* ACTIVE sps - accepted values = {1024, 256, 64, 8} */
						8     /* IDLE sps - accepted values = {1024, 256, 64, 8} */
					 })
					return(BUF5)
				}

				/* Function 6 - returns the REFRESH watchdog timer interval */
				case(6)
				{
					Name(BUF6, Package()
					{
						900 /* seconds (min=60, max=60000) */
					})
					return(BUF6)
				}

				/* Function 7 - returns the Vbus multiplication factors for PAC194x/5x-2 */
				/* PAC194x/5x-2 power meter VBUS+ pins may be connected to voltage dividers */
				/* to measure rail voltage higher than device Vbus FSR. */
				case(7)
				{
					Name(BUF7, Package()
					{ /* K = 1000 * (Vrail / Vbus). Must be integer, positive value. */
						1000, /* Channel 1 K-factor */
						1000, /* Channel 2 K-factor */
						1000, /* Channel 3 K-factor */
						1000  /* Channel 4 K-factor */
					})
					return(BUF7)
				}

			} /* switch(Arg2) */
			/*
			 * Return an error (a buffer with a value of zero)
			 * if we didn't return anything else above
			 */
			return(Buffer() {0x0})
		} /* _DSM */
	} /* PowerMeter Device PA07 Scope End */
} /* Scope I2C3 */
