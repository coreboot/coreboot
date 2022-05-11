package fsp

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

type FieldMacros struct {}

// field - data structure for creating a new bitfield macro object
// configmap : map to select the current configuration
// value     : the key value in the configmap
// override  : overrides the function to generate the current bitfield macro
type field struct {
	configmap map[uint8]string
	value     uint8
	override  func(configuration map[uint8]string, value uint8)
}

// generate - wrapper for generating bitfield macros string
// fields : field structure
func generate(fields ...*field) {
	macro := common.GetMacro()
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
func (FieldMacros) DecodeDW0() {
	macro := common.GetMacro()
	dw0 := macro.Register(common.PAD_CFG_DW0)

	ownershipStatus := func() uint8 {
		if macro.IsOwnershipDriver() { return 1 }
		return 0
	}

	generate(
		&field {
			configmap : map[uint8]string{
				0: "GpioPadModeGpio",
				1: "GpioPadModeNative1",
				2: "GpioPadModeNative2",
				3: "GpioPadModeNative3",
				4: "GpioPadModeNative4",
				5: "GpioPadModeNative5",
			},
			value : dw0.GetPadMode(),
		},

		&field {
			configmap : map[uint8]string {
				0: "GpioHostOwnAcpi",
				1: "GpioHostOwnGpio",
			},
			value : ownershipStatus(),
		},

		&field {
			configmap : map[uint8]string {
				0:          "GpioDirInOut",
				1:          "GpioDirIn",
				2:          "GpioDirOut",
				3:          "GpioDirNone",
				1 << 4 | 0: "GpioDirInInvOut",
				1 << 4 | 1: "GpioDirInInv",
			},
			value : dw0.GetRxInvert() << 4 | dw0.GetGPIORxTxDisableStatus(),
		},

		&field {
			configmap : map[uint8]string {
				0: "GpioOutLow",
				1: "GpioOutHigh",
			},
			value : dw0.GetGPIOTXState(),
		},

		&field {
			configmap : map[uint8]string {
				1 << 0: "GpioIntNmi",
				1 << 1: "GpioIntSmi",
				1 << 2: "GpioIntSci",
				1 << 3: "GpioIntApic",
			},
			override : func(configmap map[uint8]string, value uint8) {
				mask := dw0.GetGPIOInputRouteIOxAPIC() << 3 |
							dw0.GetGPIOInputRouteSCI() << 2 |
							dw0.GetGPIOInputRouteSMI() << 1 |
							dw0.GetGPIOInputRouteNMI()
				if mask == 0 {
					macro.Add("GpioIntDis | ")
					return
				}
				for bit, fieldmacro := range configmap {
					if mask & bit != 0 {
						macro.Add(fieldmacro).Add(" | ")
					}
				}
			},
		},

		&field {
			configmap : map[uint8]string {
				0: "GpioIntLevel",
				1: "GpioIntEdge",
				2: "GpioIntLvlEdgDis",
				3: "GpioIntBothEdge",
			},
			value : dw0.GetRXLevelEdgeConfiguration(),
		},

		&field {
			configmap : map[uint8]string {
				0: "GpioResetPwrGood",	// TODO: Has multiple values (to GPP and GPD)
				1: "GpioHostDeepReset",
				2: "GpioPlatformReset",
				3: "GpioResumeReset",
			},
			value : dw0.GetResetConfig(),
		},
	)
}

// DecodeDW1 - decode value of DW1 register
func (FieldMacros) DecodeDW1() {
	macro := common.GetMacro()
	dw1 := macro.Register(common.PAD_CFG_DW1)
	generate(
		&field {
			override : func(configmap map[uint8]string, value uint8) {
				if dw1.GetPadTol() != 0 {
					macro.Add("GpioTolerance1v8 | ")
				}
			},
		},

		&field {
			configmap : map[uint8]string {
				0x0: "GpioTermNone",
				0x2: "GpioTermWpd5K",
				0x4: "GpioTermWpd20K",
				0x9: "GpioTermWpu1K",
				0xa: "GpioTermWpu5K",
				0xb: "GpioTermWpu2K",
				0xc: "GpioTermWpu20K",
				0xd: "GpioTermWpu1K2K",
				0xf: "GpioTermNative",
			},
			value : dw1.GetTermination(),
		},
	)
}

// GenerateString - generates the entire string of bitfield macros.
func (bitfields FieldMacros) GenerateString() {
	macro := common.GetMacro()
	macro.Add("{ GPIO_SKL_H_").Id().Add(", { ")
	bitfields.DecodeDW0()
	bitfields.DecodeDW1()
	macro.Add(" GpioPadConfigLock } },") // TODO: configure GpioPadConfigLock
}
