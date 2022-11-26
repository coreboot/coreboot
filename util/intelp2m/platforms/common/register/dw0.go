package register

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"

type DW0 struct {
	Register
}

// getResetConfig() returns type reset source for corresponding pad
// PADRSTCFG field in PAD_CFG_DW0 register
func (dw0 *DW0) GetResetConfig() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0PadRstCfg)
}

// getRXPadStateSelect() returns RX Pad State (RXINV)
// 0 = Raw RX pad state directly from RX buffer
// 1 = Internal RX pad state
func (dw0 *DW0) GetRXPadStateSelect() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0RxPadStateSelect)
}

// getRXRawOverrideStatus() returns 1 if the selected pad state is being
// overridden to '1' (RXRAW1 field)
func (dw0 *DW0) GetRXRawOverrideStatus() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0RxRawOverrideTo1)
}

// getRXLevelEdgeConfiguration() returns RX Level/Edge Configuration (RXEVCFG)
// 0h = Level, 1h = Edge, 2h = Drive '0', 3h = Reserved (implement as setting 0h)
func (dw0 *DW0) GetRXLevelEdgeConfiguration() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0RxLevelEdgeConfiguration)
}

// GetRxInvert() returns RX Invert state (RXINV)
// 1 - Inversion, 0 - No inversion
func (dw0 *DW0) GetRxInvert() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0RxInvert)
}

// getRxTxEnableConfig() returns RX/TX Enable Config (RXTXENCFG)
// 0 = Function defined in Pad Mode controls TX and RX Enables
// 1 = Function controls TX Enable and RX Disabled with RX drive 0 internally
// 2 = Function controls TX Enable and RX Disabled with RX drive 1 internally
// 3 = Function controls TX Enabled and RX is always enabled
func (dw0 *DW0) GetRxTxEnableConfig() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0RxTxEnableConfig)
}

// getGPIOInputRouteIOxAPIC() returns 1 if the pad can be routed to cause
// peripheral IRQ when configured in GPIO input mode.
func (dw0 *DW0) GetGPIOInputRouteIOxAPIC() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0InputRouteIOxApic)
}

// getGPIOInputRouteSCI() returns 1 if the pad can be routed to cause SCI when
// configured in GPIO input mode.
func (dw0 *DW0) GetGPIOInputRouteSCI() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0InputRouteSCI)
}

// getGPIOInputRouteSMI() returns 1 if the pad can be routed to cause SMI when
// configured in GPIO input mode
func (dw0 *DW0) GetGPIOInputRouteSMI() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0InputRouteSMI)
}

// getGPIOInputRouteNMI() returns 1 if the pad can be routed to cause NMI when
// configured in GPIO input mode
func (dw0 *DW0) GetGPIOInputRouteNMI() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0InputRouteNMI)
}

// getPadMode() reutrns pad mode or one of the native functions
// 0h = GPIO control the Pad.
// 1h = native function 1, if applicable, controls the Pad
// 2h = native function 2, if applicable, controls the Pad
// 3h = native function 3, if applicable, controls the Pad
// 4h = enable GPIO blink/PWM capability if applicable
func (dw0 *DW0) GetPadMode() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0PadMode)
}

// getGPIORxTxDisableStatus() returns GPIO RX/TX buffer state (GPIORXDIS | GPIOTXDIS)
// 0 - both are enabled, 1 - TX Disable, 2 - RX Disable, 3 - both are disabled
func (dw0 *DW0) GetGPIORxTxDisableStatus() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0RxTxBufDisable)
}

// getGPIORXState() returns GPIO RX State (GPIORXSTATE)
func (dw0 *DW0) GetGPIORXState() uint32 {
	return dw0.GetFieldVal(bits.DW0, bits.DW0RxState)
}

// getGPIOTXState() returns GPIO TX State (GPIOTXSTATE)
func (dw0 *DW0) GetGPIOTXState() uint32 {
	return dw0.GetFieldVal(bits.DW0, 0)
}
