package common

import (
	"strconv"
	"sync"

	"review.coreboot.org/coreboot.git/util/intelp2m/config"
)

type Fields interface {
	DecodeDW0()
	DecodeDW1()
	GenerateString()
}

const (
	PAD_OWN_ACPI   = 0
	PAD_OWN_DRIVER = 1
)

const (
	TxLASTRxE     = 0x0
	Tx0RxDCRx0    = 0x1
	Tx0RxDCRx1    = 0x2
	Tx1RxDCRx0    = 0x3
	Tx1RxDCRx1    = 0x4
	Tx0RxE        = 0x5
	Tx1RxE        = 0x6
	HIZCRx0       = 0x7
	HIZCRx1       = 0x8
	TxDRxE        = 0x9
	StandbyIgnore = 0xf
)

const (
	IOSTERM_SAME	= 0x0
	IOSTERM_DISPUPD	= 0x1
	IOSTERM_ENPD	= 0x2
	IOSTERM_ENPU    = 0x3
)

const (
	TRIG_LEVEL       = 0
	TRIG_EDGE_SINGLE = 1
	TRIG_OFF         = 2
	TRIG_EDGE_BOTH   = 3
)

const (
	RST_PWROK  = 0
	RST_DEEP   = 1
	RST_PLTRST = 2
	RST_RSMRST = 3
)

// PlatformSpecific - platform-specific interface
type PlatformSpecific interface {
	RemmapRstSrc()
	Pull()
	GpiMacroAdd()
	GpoMacroAdd()
	NativeFunctionMacroAdd()
	NoConnMacroAdd()
}

// Macro - contains macro information and methods
// Platform : platform-specific interface
// padID    : pad ID string
// str      : macro string entirely
// Reg      : structure of configuration register values and their masks
type Macro struct {
	Platform  PlatformSpecific
	Reg       [MAX_DW_NUM]Register
	padID     string
	str       string
	ownership uint8
	Fields
}

var	instanceMacro *Macro
var	once           sync.Once

// GetInstance returns singleton
func GetInstanceMacro(p PlatformSpecific, f Fields) *Macro {
	once.Do(func() {
		instanceMacro = &Macro{ Platform : p, Fields : f }
	})
	return instanceMacro
}

func GetMacro() *Macro {
	return GetInstanceMacro(nil, nil)
}

func (macro *Macro) PadIdGet() string {
	return macro.padID
}

func (macro *Macro) PadIdSet(padid string) *Macro {
	macro.padID = padid
	return macro
}

func (macro *Macro) SetPadOwnership(own uint8) *Macro {
	macro.ownership = own
	return macro
}

func (macro *Macro) IsOwnershipDriver() bool {
	return macro.ownership == PAD_OWN_DRIVER
}

// returns <Register> data configuration structure
// number : register number
func (macro *Macro) Register(number uint8) *Register {
	return &macro.Reg[number]
}

// add a string to macro
func (macro *Macro) Add(str string) *Macro {
	macro.str += str
	return macro
}

// set a string in a macro instead of its previous contents
func (macro *Macro) Set(str string) *Macro {
	macro.str = str
	return macro
}

// get macro string
func (macro *Macro) Get() string {
	return macro.str
}

// set a string in a macro instead of its previous contents
func (macro *Macro) Clear() *Macro {
	macro.Set("")
	return macro
}

// Adds PAD Id to the macro as a new argument
// return: Macro
func (macro *Macro) Id() *Macro {
	return macro.Add(macro.padID)
}

// Add Separator to macro if needed
func (macro *Macro) Separator() *Macro {
	str := macro.Get()
	c := str[len(str)-1]
	if c != '(' && c != '_' {
		macro.Add(", ")
	}
	return macro
}

// Adds the PADRSTCFG parameter from DW0 to the macro as a new argument
// return: Macro
func (macro *Macro) Rstsrc() *Macro {
	dw0 := macro.Register(PAD_CFG_DW0)
	var resetsrc = map[uint8]string {
		0: "PWROK",
		1: "DEEP",
		2: "PLTRST",
		3: "RSMRST",
	}
	return macro.Separator().Add(resetsrc[dw0.GetResetConfig()])
}

// Adds The Pad Termination (TERM) parameter from DW1 to the macro as a new argument
// return: Macro
func (macro *Macro) Pull() *Macro {
	macro.Platform.Pull()
	return macro
}

// Adds Pad GPO value to macro string as a new argument
// return: Macro
func (macro *Macro) Val() *Macro {
	dw0 := macro.Register(PAD_CFG_DW0)
	return macro.Separator().Add(strconv.Itoa(int(dw0.GetGPIOTXState())))
}

// Adds Pad GPO value to macro string as a new argument
// return: Macro
func (macro *Macro) Trig() *Macro {
	dw0 := macro.Register(PAD_CFG_DW0)
	var trig = map[uint8]string{
		0x0: "LEVEL",
		0x1: "EDGE_SINGLE",
		0x2: "OFF",
		0x3: "EDGE_BOTH",
	}
	return macro.Separator().Add(trig[dw0.GetRXLevelEdgeConfiguration()])
}

// Adds Pad Polarity Inversion Stage (RXINV) to macro string as a new argument
// return: Macro
func (macro *Macro) Invert() *Macro {
	macro.Separator()
	if macro.Register(PAD_CFG_DW0).GetRxInvert() !=0 {
		return macro.Add("INVERT")
	}
	return macro.Add("NONE")
}

// Adds input/output buffer state
// return: Macro
func (macro *Macro) Bufdis() *Macro {
	var buffDisStat = map[uint8]string{
		0x0: "NO_DISABLE",    // both buffers are enabled
		0x1: "TX_DISABLE",    // output buffer is disabled
		0x2: "RX_DISABLE",    // input buffer is disabled
		0x3: "TX_RX_DISABLE", // both buffers are disabled
	}
	state := macro.Register(PAD_CFG_DW0).GetGPIORxTxDisableStatus()
	return macro.Separator().Add(buffDisStat[state])
}

// Adds macro to set the host software ownership
// return: Macro
func (macro *Macro) Own() *Macro {
	if macro.IsOwnershipDriver() {
		return macro.Separator().Add("DRIVER")
	}
	return macro.Separator().Add("ACPI")
}

//Adds pad native function (PMODE) as a new argument
//return: Macro
func (macro *Macro) Padfn() *Macro {
	dw0 := macro.Register(PAD_CFG_DW0)
	nfnum := int(dw0.GetPadMode())
	if nfnum != 0 {
		return macro.Separator().Add("NF" + strconv.Itoa(nfnum))
	}
	// GPIO used only for PAD_FUNC(x) macro
	return macro.Add("GPIO")
}

// Add a line to the macro that defines IO Standby State
// return: macro
func (macro *Macro) IOSstate() *Macro {
	var stateMacro = map[uint8]string{
		TxLASTRxE:     "TxLASTRxE",
		Tx0RxDCRx0:    "Tx0RxDCRx0",
		Tx0RxDCRx1:    "Tx0RxDCRx1",
		Tx1RxDCRx0:    "Tx1RxDCRx0",
		Tx1RxDCRx1:    "Tx1RxDCRx1",
		Tx0RxE:        "Tx0RxE",
		Tx1RxE:        "Tx1RxE",
		HIZCRx0:       "HIZCRx0",
		HIZCRx1:       "HIZCRx1",
		TxDRxE:        "TxDRxE",
		StandbyIgnore: "IGNORE",
	}
	dw1 := macro.Register(PAD_CFG_DW1)
	str, valid := stateMacro[dw1.GetIOStandbyState()]
	if !valid {
		// ignore setting for incorrect value
		str = "IGNORE"
	}
	return macro.Separator().Add(str)
}

// Add a line to the macro that defines IO Standby Termination
// return: macro
func (macro *Macro) IOTerm() *Macro {
	var ioTermMacro = map[uint8]string{
		IOSTERM_SAME:    "SAME",
		IOSTERM_DISPUPD: "DISPUPD",
		IOSTERM_ENPD:    "ENPD",
		IOSTERM_ENPU:    "ENPU",
	}
	dw1 := macro.Register(PAD_CFG_DW1)
	return macro.Separator().Add(ioTermMacro[dw1.GetIOStandbyTermination()])
}

// Check created macro
func (macro *Macro) check() *Macro {
	if !macro.Register(PAD_CFG_DW0).MaskCheck() {
		return macro.GenerateFields()
	}
	return macro
}

// or - Set " | " if its needed
func (macro *Macro) Or() *Macro {

		if str := macro.Get(); str[len(str) - 1] == ')' {
			macro.Add(" | ")
		}
		return macro
}

// DecodeIgnored - Add info about ignored field mask
// reg : PAD_CFG_DW0 or PAD_CFG_DW1 register
func (macro *Macro) DecodeIgnored(reg uint8) *Macro {
	var decode = map[uint8]func() {
		PAD_CFG_DW0: macro.Fields.DecodeDW0,
		PAD_CFG_DW1: macro.Fields.DecodeDW1,
	}
	decodefn, valid := decode[reg]
	if !valid || config.IsFspStyleMacro() {
		return macro
	}
	dw := macro.Register(reg)
	ignored := dw.IgnoredFieldsGet()
	if ignored != 0 {
		temp := dw.ValueGet()
		dw.ValueSet(ignored)
		regnum := strconv.Itoa(int(reg))
		macro.Add("/* DW" + regnum + ": ")
		decodefn()
		macro.Add(" - IGNORED */\n\t")
		dw.ValueSet(temp)
	}
	return macro
}

// GenerateFields - generate bitfield macros
func (macro *Macro) GenerateFields() *Macro {
	dw0 := macro.Register(PAD_CFG_DW0)
	dw1 := macro.Register(PAD_CFG_DW1)

	// Get mask of ignored bit fields.
	dw0Ignored := dw0.IgnoredFieldsGet()
	dw1Ignored := dw1.IgnoredFieldsGet()

	if config.InfoLevelGet() != 4 {
		macro.Clear()
	}
	if config.InfoLevelGet() >= 3 {
		// Add string of reference macro as a comment
		reference := macro.Get()
		macro.Clear()
		/* DW0 : PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | 1 - IGNORED */
		macro.DecodeIgnored(PAD_CFG_DW0).DecodeIgnored(PAD_CFG_DW1)
		if config.InfoLevelGet() >= 4 {
			/* PAD_CFG_NF(GPP_B23, 20K_PD, PLTRST, NF2), */
			macro.Add("/* ").Add(reference).Add(" */\n\t")
		}
	}
	if config.AreFieldsIgnored() {
		// Consider bit fields that should be ignored when regenerating
		// advansed macros
		var tempVal uint32 = dw0.ValueGet() & ^dw0Ignored
		dw0.ValueSet(tempVal)

		tempVal = dw1.ValueGet() & ^dw1Ignored
		dw1.ValueSet(tempVal)
	}

	macro.Fields.GenerateString()
	return macro
}

// Generate macro for bi-directional GPIO port
func (macro *Macro) Bidirection() {
	dw1 := macro.Register(PAD_CFG_DW1)
	ios := dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0
	macro.Set("PAD_CFG_GPIO_BIDIRECT")
	if ios {
		macro.Add("_IOS")
	}
	// PAD_CFG_GPIO_BIDIRECT(pad, val, pull, rst, trig, own)
	macro.Add("(").Id().Val().Pull().Rstsrc().Trig()
	if ios {
		// PAD_CFG_GPIO_BIDIRECT_IOS(pad, val, pull, rst, trig, iosstate, iosterm, own)
		macro.IOSstate().IOTerm()
	}
	macro.Own().Add("),")
}

const (
	rxDisable uint8 = 0x2
	txDisable uint8 = 0x1
)

// Gets base string of current macro
// return: string of macro
func (macro *Macro) Generate() string {
	dw0 := macro.Register(PAD_CFG_DW0)

	macro.Platform.RemmapRstSrc()
	macro.Set("PAD_CFG")
	if dw0.GetPadMode() == 0 {
		// GPIO
		switch dw0.GetGPIORxTxDisableStatus() {
		case txDisable:
			macro.Platform.GpiMacroAdd() // GPI

		case rxDisable:
			macro.Platform.GpoMacroAdd() // GPO

		case rxDisable | txDisable:
			macro.Platform.NoConnMacroAdd() // NC

		default:
			macro.Bidirection()
		}
	} else {
		macro.Platform.NativeFunctionMacroAdd()
	}

	if config.IsFieldsMacroUsed() {
		// Clear control mask to generate advanced macro only
		return macro.GenerateFields().Get()
	}

	if config.IsNonCheckingFlagUsed() {
		body := macro.Get()
		if config.InfoLevelGet() >= 3 {
			macro.Clear().DecodeIgnored(PAD_CFG_DW0).DecodeIgnored(PAD_CFG_DW1)
			comment := macro.Get()
			if config.InfoLevelGet() >= 4 {
				macro.Clear().Add("/* ")
				macro.Fields.GenerateString()
				macro.Add(" */\n\t")
				comment += macro.Get()
			}
			return comment + body
		}
		return body
	}

	return macro.check().Get()
}
