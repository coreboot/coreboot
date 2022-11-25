package common

// Bit field constants for PAD_CFG_DW0 register
const (
	AllFields uint32 = 0xffffffff

	PadRstCfgShift uint8  = 30
	PadRstCfgMask  uint32 = 0x3 << PadRstCfgShift

	RxPadStateSelectShift uint8  = 29
	RxPadStateSelectMask  uint32 = 0x1 << RxPadStateSelectShift

	RxRawOverrideTo1Shift uint8  = 28
	RxRawOverrideTo1Mask  uint32 = 0x1 << RxRawOverrideTo1Shift

	RxLevelEdgeConfigurationShift uint8  = 25
	RxLevelEdgeConfigurationMask  uint32 = 0x3 << RxLevelEdgeConfigurationShift

	RxInvertShift uint8  = 23
	RxInvertMask  uint32 = 0x1 << RxInvertShift

	RxTxEnableConfigShift uint8  = 21
	RxTxEnableConfigMask  uint32 = 0x3 << RxTxEnableConfigShift

	InputRouteIOxApicShift uint8  = 20
	InputRouteIOxApicMask  uint32 = 0x1 << InputRouteIOxApicShift

	InputRouteSCIShift uint8  = 19
	InputRouteSCIMask  uint32 = 0x1 << InputRouteSCIShift

	InputRouteSMIShift uint8  = 18
	InputRouteSMIMask  uint32 = 0x1 << InputRouteSMIShift

	InputRouteNMIShift uint8  = 17
	InputRouteNMIMask  uint32 = 0x1 << InputRouteNMIShift

	PadModeShift uint8  = 10
	PadModeMask  uint32 = 0x7 << PadModeShift

	RxTxBufDisableShift uint8  = 8
	RxTxBufDisableMask  uint32 = 0x3 << RxTxBufDisableShift

	RxStateShift uint8  = 1
	RxStateMask  uint32 = 0x1 << RxStateShift

	TxStateMask uint32 = 0x1
)

// config DW registers
const (
	PAD_CFG_DW0 = 0
	PAD_CFG_DW1 = 1
	MAX_DW_NUM  = 2
)

// Register - configuration data structure based on DW0/1 dw value
// value    : register value
// mask     : bit fileds mask
// roFileds : read only fields mask
type Register struct {
	value    uint32
	mask     uint32
	roFileds uint32
}

func (reg *Register) ValueSet(value uint32) *Register {
	reg.value = value
	return reg
}

func (reg *Register) ValueGet() uint32 {
	return reg.value
}

func (reg *Register) ReadOnlyFieldsSet(fileldMask uint32) *Register {
	reg.roFileds = fileldMask
	return reg
}

func (reg *Register) ReadOnlyFieldsGet() uint32 {
	return reg.roFileds
}

// Check the mask of the new macro
// Returns true if the macro is generated correctly
func (reg *Register) MaskCheck() bool {
	mask := ^(reg.mask | reg.roFileds)
	return (reg.value & mask) == 0
}

// getResetConfig - get Reset Configuration from PADRSTCFG field in PAD_CFG_DW0_GPx register
func (reg *Register) getFieldVal(mask uint32, shift uint8) uint8 {
	reg.mask |= mask
	return uint8((reg.value & mask) >> shift)
}

// CntrMaskFieldsClear - clear filed in control mask
// fieldMask - mask of the field to be cleared
func (reg *Register) CntrMaskFieldsClear(fieldMask uint32) {
	reg.mask &= ^fieldMask
}

// IgnoredFieldsGet - return mask of unchecked (ignored) fields.
//
//	These bit fields were not read when the macro was
//	generated.
//
// return
//
//	mask of ignored bit field
func (reg *Register) IgnoredFieldsGet() uint32 {
	mask := reg.mask | reg.roFileds
	return reg.value & ^mask
}

// getResetConfig - returns type reset source for corresponding pad
// PADRSTCFG field in PAD_CFG_DW0 register
func (reg *Register) GetResetConfig() uint8 {
	return reg.getFieldVal(PadRstCfgMask, PadRstCfgShift)
}

// getRXPadStateSelect - returns RX Pad State (RXINV)
// 0 = Raw RX pad state directly from RX buffer
// 1 = Internal RX pad state
func (reg *Register) GetRXPadStateSelect() uint8 {
	return reg.getFieldVal(RxPadStateSelectMask, RxPadStateSelectShift)
}

// getRXRawOverrideStatus - returns 1 if the selected pad state is being
// overridden to '1' (RXRAW1 field)
func (reg *Register) GetRXRawOverrideStatus() uint8 {
	return reg.getFieldVal(RxRawOverrideTo1Mask, RxRawOverrideTo1Shift)
}

// getRXLevelEdgeConfiguration - returns RX Level/Edge Configuration (RXEVCFG)
// 0h = Level, 1h = Edge, 2h = Drive '0', 3h = Reserved (implement as setting 0h)
func (reg *Register) GetRXLevelEdgeConfiguration() uint8 {
	return reg.getFieldVal(RxLevelEdgeConfigurationMask, RxLevelEdgeConfigurationShift)
}

// GetRxInvert - returns RX Invert state (RXINV)
// 1 - Inversion, 0 - No inversion
func (reg *Register) GetRxInvert() uint8 {
	return reg.getFieldVal(RxInvertMask, RxInvertShift)
}

// getRxTxEnableConfig - returns RX/TX Enable Config (RXTXENCFG)
// 0 = Function defined in Pad Mode controls TX and RX Enables
// 1 = Function controls TX Enable and RX Disabled with RX drive 0 internally
// 2 = Function controls TX Enable and RX Disabled with RX drive 1 internally
// 3 = Function controls TX Enabled and RX is always enabled
func (reg *Register) GetRxTxEnableConfig() uint8 {
	return reg.getFieldVal(RxTxEnableConfigMask, RxTxEnableConfigShift)
}

// getGPIOInputRouteIOxAPIC - returns 1 if the pad can be routed to cause
// peripheral IRQ when configured in GPIO input mode.
func (reg *Register) GetGPIOInputRouteIOxAPIC() uint8 {
	return reg.getFieldVal(InputRouteIOxApicMask, InputRouteIOxApicShift)
}

// getGPIOInputRouteSCI - returns 1 if the pad can be routed to cause SCI when
// configured in GPIO input mode.
func (reg *Register) GetGPIOInputRouteSCI() uint8 {
	return reg.getFieldVal(InputRouteSCIMask, InputRouteSCIShift)
}

// getGPIOInputRouteSMI - returns 1 if the pad can be routed to cause SMI when
// configured in GPIO input mode
func (reg *Register) GetGPIOInputRouteSMI() uint8 {
	return reg.getFieldVal(InputRouteSMIMask, InputRouteSMIShift)
}

// getGPIOInputRouteNMI - returns 1 if the pad can be routed to cause NMI when
// configured in GPIO input mode
func (reg *Register) GetGPIOInputRouteNMI() uint8 {
	return reg.getFieldVal(InputRouteNMIMask, InputRouteNMIShift)
}

// getPadMode - reutrns pad mode or one of the native functions
// 0h = GPIO control the Pad.
// 1h = native function 1, if applicable, controls the Pad
// 2h = native function 2, if applicable, controls the Pad
// 3h = native function 3, if applicable, controls the Pad
// 4h = enable GPIO blink/PWM capability if applicable
func (reg *Register) GetPadMode() uint8 {
	return reg.getFieldVal(PadModeMask, PadModeShift)
}

// getGPIORxTxDisableStatus - returns GPIO RX/TX buffer state (GPIORXDIS | GPIOTXDIS)
// 0 - both are enabled, 1 - TX Disable, 2 - RX Disable, 3 - both are disabled
func (reg *Register) GetGPIORxTxDisableStatus() uint8 {
	return reg.getFieldVal(RxTxBufDisableMask, RxTxBufDisableShift)
}

// getGPIORXState - returns GPIO RX State (GPIORXSTATE)
func (reg *Register) GetGPIORXState() uint8 {
	return reg.getFieldVal(RxStateMask, RxStateShift)
}

// getGPIOTXState - returns GPIO TX State (GPIOTXSTATE)
func (reg *Register) GetGPIOTXState() uint8 {
	return reg.getFieldVal(TxStateMask, 0)
}

// Bit field constants for PAD_CFG_DW1 register
const (
	PadTolShift uint8  = 25
	PadTolMask  uint32 = 0x1 << PadTolShift

	IOStandbyStateShift uint8  = 14
	IOStandbyStateMask  uint32 = 0xF << IOStandbyStateShift

	TermShift uint8  = 10
	TermMask  uint32 = 0xF << TermShift

	IOStandbyTerminationShift uint8  = 8
	IOStandbyTerminationMask  uint32 = 0x3 << IOStandbyTerminationShift

	InterruptSelectMask uint32 = 0xFF
)

// GetPadTol
func (reg *Register) GetPadTol() uint8 {
	return reg.getFieldVal(PadTolMask, PadTolShift)
}

// getIOStandbyState - return IO Standby State (IOSSTATE)
// 0 = Tx enabled driving last value driven, Rx enabled
// 1 = Tx enabled driving 0, Rx disabled and Rx driving 0 back to its controller internally
// 2 = Tx enabled driving 0, Rx disabled and Rx driving 1 back to its controller internally
// 3 = Tx enabled driving 1, Rx disabled and Rx driving 0 back to its controller internally
// 4 = Tx enabled driving 1, Rx disabled and Rx driving 1 back to its controller internally
// 5 = Tx enabled driving 0, Rx enabled
// 6 = Tx enabled driving 1, Rx enabled
// 7 = Hi-Z, Rx driving 0 back to its controller internally
// 8 = Hi-Z, Rx driving 1 back to its controller internally
// 9 = Tx disabled, Rx enabled
// 15 = IO-Standby is ignored for this pin (same as functional mode)
// Others reserved
func (reg *Register) GetIOStandbyState() uint8 {
	return reg.getFieldVal(IOStandbyStateMask, IOStandbyStateShift)
}

// getIOStandbyTermination - return IO Standby Termination (IOSTERM)
// 0 = Same as functional mode (no change)
// 1 = Disable Pull-up and Pull-down (no on-die termination)
// 2 = Enable Pull-down
// 3 = Enable Pull-up
func (reg *Register) GetIOStandbyTermination() uint8 {
	return reg.getFieldVal(IOStandbyTerminationMask, IOStandbyTerminationShift)
}

// getTermination - returns the pad termination state defines the different weak
// pull-up and pull-down settings that are supported by the buffer
// 0000 = none; 0010 = 5k PD; 0100 = 20k PD; 1010 = 5k PU; 1100 = 20k PU;
// 1111 = Native controller selected
func (reg *Register) GetTermination() uint8 {
	return reg.getFieldVal(TermMask, TermShift)
}

// getInterruptSelect - returns Interrupt Line number from the GPIO controller
func (reg *Register) GetInterruptSelect() uint8 {
	return reg.getFieldVal(InterruptSelectMask, 0)
}
