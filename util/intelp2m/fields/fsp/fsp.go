package fsp

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

type FieldCollection struct{}

// field - data structure for creating a new bitfield macro object
// configmap : map to select the current configuration
// value     : the key value in the configmap
// override  : overrides the function to generate the current bitfield macro
type field struct {
	configmap map[uint32]string
	value     uint32
	override  func(configuration map[uint32]string, value uint32)
}

// generate() generates bitfield macro data struct
func generate(macro *common.Macro, fields ...*field) {
	for _, field := range fields {
		if field.override != nil {
			// override if necessary
			field.override(field.configmap, field.value)
			continue
		}

		fieldmacro, valid := field.configmap[field.value]
		if valid {
			macro.Add(fieldmacro).Add(", ")
		} else {
			macro.Add("INVALID, ")
		}
	}
}

// DecodeDW0 - decode value of DW0 register
func (FieldCollection) DecodeDW0(macro *common.Macro) *common.Macro {
	dw0 := macro.Platform.GetRegisterDW0()

	ownershipStatus := func() uint32 {
		if macro.IsOwnershipDriver() {
			return 1
		}
		return 0
	}

	generate(macro,
		&field{
			configmap: map[uint32]string{
				0: "GpioPadModeGpio",
				1: "GpioPadModeNative1",
				2: "GpioPadModeNative2",
				3: "GpioPadModeNative3",
				4: "GpioPadModeNative4",
				5: "GpioPadModeNative5",
			},
			value: dw0.GetPadMode(),
		},

		&field{
			configmap: map[uint32]string{
				0: "GpioHostOwnAcpi",
				1: "GpioHostOwnGpio",
			},
			value: ownershipStatus(),
		},

		&field{
			configmap: map[uint32]string{
				0:            "GpioDirInOut",
				1:            "GpioDirIn",
				2:            "GpioDirOut",
				3:            "GpioDirNone",
				(1 << 4):     "GpioDirInInvOut",
				(1 << 4) | 1: "GpioDirInInv",
			},
			value: dw0.GetRxInvert()<<4 | dw0.GetGPIORxTxDisableStatus(),
		},

		&field{
			configmap: map[uint32]string{
				0: "GpioOutLow",
				1: "GpioOutHigh",
			},
			value: dw0.GetGPIOTXState(),
		},

		&field{
			configmap: map[uint32]string{
				1 << 0: "GpioIntNmi",
				1 << 1: "GpioIntSmi",
				1 << 2: "GpioIntSci",
				1 << 3: "GpioIntApic",
			},
			override: func(configmap map[uint32]string, value uint32) {
				mask := dw0.GetGPIOInputRouteIOxAPIC()<<3 |
					dw0.GetGPIOInputRouteSCI()<<2 |
					dw0.GetGPIOInputRouteSMI()<<1 |
					dw0.GetGPIOInputRouteNMI()
				if mask == 0 {
					macro.Add("GpioIntDis | ")
					return
				}
				for bit, fieldmacro := range configmap {
					if mask&bit != 0 {
						macro.Add(fieldmacro).Add(" | ")
					}
				}
			},
		},

		&field{
			configmap: map[uint32]string{
				0: "GpioIntLevel",
				1: "GpioIntEdge",
				2: "GpioIntLvlEdgDis",
				3: "GpioIntBothEdge",
			},
			value: dw0.GetRXLevelEdgeConfiguration(),
		},

		&field{
			configmap: map[uint32]string{
				0: "GpioResetPwrGood", // TODO: Has multiple values (to GPP and GPD)
				1: "GpioHostDeepReset",
				2: "GpioPlatformReset",
				3: "GpioResumeReset",
			},
			value: dw0.GetResetConfig(),
		},
	)
	return macro
}

// DecodeDW1() decodes DW1 register value and adds it to the macro string
func (FieldCollection) DecodeDW1(macro *common.Macro) *common.Macro {
	dw1 := macro.Platform.GetRegisterDW1()
	generate(macro,
		&field{
			override: func(configmap map[uint32]string, value uint32) {
				if dw1.GetPadTol() != 0 {
					macro.Add("GpioTolerance1v8 | ")
				}
			},
		},
		&field{
			configmap: map[uint32]string{
				0b0000: "GpioTermNone",
				0b0010: "GpioTermWpd5K",
				0b0100: "GpioTermWpd20K",
				0b1001: "GpioTermWpu1K",
				0b1010: "GpioTermWpu5K",
				0b1011: "GpioTermWpu2K",
				0b1100: "GpioTermWpu20K",
				0b1101: "GpioTermWpu1K2K",
				0b1111: "GpioTermNative",
			},
			value: dw1.GetTermination(),
		},
	)
	return macro
}

// GenerateMacro() generates the field macro collection and adds it to the macro string
func (f FieldCollection) GenerateMacro(macro *common.Macro) *common.Macro {
	macro.Add("{ GPIO_SKL_H_").Id().Add(", { ")
	f.DecodeDW0(macro)
	return f.DecodeDW1(macro).Add(" GpioPadConfigLock } },") // TODO: configure GpioPadConfigLock
}
