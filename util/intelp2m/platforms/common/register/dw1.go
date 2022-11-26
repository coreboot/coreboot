package register

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"

type DW1 struct {
	Register
}

// GetPadTol()
func (dw1 *DW1) GetPadTol() uint32 {
	return dw1.GetFieldVal(bits.DW1, bits.DW1PadTol)
}

// GetIOStandbyState() returns IO Standby State (IOSSTATE)
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
func (dw1 *DW1) GetIOStandbyState() uint32 {
	return dw1.GetFieldVal(bits.DW1, bits.DW1IOStandbyState)
}

// GetIOStandbyTermination() returns IO Standby Termination (IOSTERM)
// 0 = Same as functional mode (no change)
// 1 = Disable Pull-up and Pull-down (no on-die termination)
// 2 = Enable Pull-down
// 3 = Enable Pull-up
func (dw1 *DW1) GetIOStandbyTermination() uint32 {
	return dw1.GetFieldVal(bits.DW1, bits.DW1IOStandbyTermination)
}

// GetTermination() returns the pad termination state defines the different weak
// pull-up and pull-down settings that are supported by the buffer
// 0000 = none; 0010 = 5k PD; 0100 = 20k PD; 1010 = 5k PU; 1100 = 20k PU;
// 1111 = Native controller selected
func (dw1 *DW1) GetTermination() uint32 {
	return dw1.GetFieldVal(bits.DW1, bits.DW1Term)
}

// GetInterruptSelect() returns Interrupt Line number from the GPIO controller
func (dw1 *DW1) GetInterruptSelect() uint32 {
	return dw1.GetFieldVal(bits.DW1, bits.DW1InterruptSelect)
}
