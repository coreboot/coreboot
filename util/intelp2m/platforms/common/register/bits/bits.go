package bits

type Offset uint8

const All32 uint32 = 0b11111111111111111111111111111111

const (
	DW0PadRstCfg                Offset = 30
	DW0RxPadStateSelect         Offset = 29
	DW0RxRawOverrideTo1         Offset = 28
	DW0RxLevelEdgeConfiguration Offset = 25
	DW0RxInvert                 Offset = 23
	DW0RxTxEnableConfig         Offset = 21
	DW0InputRouteIOxApic        Offset = 20
	DW0InputRouteSCI            Offset = 19
	DW0InputRouteSMI            Offset = 18
	DW0InputRouteNMI            Offset = 17
	DW0PadMode                  Offset = 10
	DW0RxTxBufDisable           Offset = 8
	DW0RxState                  Offset = 1
	DW0TxState                  Offset = 0
)

const (
	RstCfgPWROK  = 0b00
	RstCfgDEEP   = 0b01
	RstCfgPLTRST = 0b10
	RstCfgRSMRST = 0b11
)

const (
	TrigLEVEL       = 0b00
	TrigEDGE_SINGLE = 0b01
	TrigOFF         = 0b10
	TrigEDGE_BOTH   = 0b11
)

type Fields map[Offset]uint32

var DW0 = Fields{
	DW0PadRstCfg:                0b11 << DW0PadRstCfg,
	DW0RxPadStateSelect:         0b1 << DW0RxPadStateSelect,
	DW0RxRawOverrideTo1:         0b1 << DW0RxRawOverrideTo1,
	DW0RxLevelEdgeConfiguration: 0b11 << DW0RxLevelEdgeConfiguration,
	DW0RxInvert:                 0b1 << DW0RxInvert,
	DW0RxTxEnableConfig:         0b11 << DW0RxTxEnableConfig,
	DW0InputRouteIOxApic:        0b1 << DW0InputRouteIOxApic,
	DW0InputRouteSCI:            0b1 << DW0InputRouteSCI,
	DW0InputRouteSMI:            0b1 << DW0InputRouteSMI,
	DW0InputRouteNMI:            0b1 << DW0InputRouteNMI,
	DW0PadMode:                  0b111 << DW0PadMode,
	DW0RxTxBufDisable:           0b11 << DW0RxTxBufDisable,
	DW0RxState:                  0b1 << DW0RxState,
	DW0TxState:                  0b1 << DW0TxState,
}

const (
	DW1PadTol               Offset = 25
	DW1IOStandbyState       Offset = 14
	DW1Term                 Offset = 10
	DW1IOStandbyTermination Offset = 8
	DW1InterruptSelect      Offset = 0
)

const (
	IOStateTxLASTRxE     = 0b0000
	IOStateTx0RxDCRx0    = 0b0001
	IOStateTx0RxDCRx1    = 0b0010
	IOStateTx1RxDCRx0    = 0b0011
	IOStateTx1RxDCRx1    = 0b0100
	IOStateTx0RxE        = 0b0101
	IOStateTx1RxE        = 0b0110
	IOStateHIZCRx0       = 0b0111
	IOStateHIZCRx1       = 0b1000
	IOStateTxDRxE        = 0b1001
	IOStateStandbyIgnore = 0b1111
)

const (
	IOTermSAME    = 0b00
	IOTermDISPUPD = 0b01
	IOTermENPD    = 0b10
	IOTermENPU    = 0b11
)

var DW1 = Fields{
	DW1PadTol:               0b1 << DW1PadTol,
	DW1IOStandbyState:       0b1111 << DW1IOStandbyState,
	DW1Term:                 0b1111 << DW1Term,
	DW1IOStandbyTermination: 0b11 << DW1IOStandbyTermination,
	DW1InterruptSelect:      0b11111111 << DW1InterruptSelect,
}
