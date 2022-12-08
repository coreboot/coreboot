package common

import (
	"fmt"
	"strconv"
	"sync"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
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
	DW0       register.DW0
	DW1       register.DW1
	padID     string
	str       string
	ownership uint8
	Fields
}

var instanceMacro *Macro
var once sync.Once

// GetInstance returns singleton
func GetInstanceMacro(p PlatformSpecific, f Fields) *Macro {
	once.Do(func() {
		instanceMacro = &Macro{Platform: p, Fields: f}
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

func (macro *Macro) GetRegisterDW0() *register.DW0 {
	return &macro.DW0
}

func (macro *Macro) GetRegisterDW1() *register.DW1 {
	return &macro.DW1
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
	dw0 := macro.GetRegisterDW0()
	resetsrc := map[uint32]string{
		0b00: "PWROK",
		0b01: "DEEP",
		0b10: "PLTRST",
		0b11: "RSMRST",
	}
	source, exist := resetsrc[dw0.GetResetConfig()]
	if !exist {
		source = "ERROR"
	}
	return macro.Separator().Add(source)
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
	dw0 := macro.GetRegisterDW0()
	return macro.Separator().Add(strconv.Itoa(int(dw0.GetGPIOTXState())))
}

// Adds Pad GPO value to macro string as a new argument
// return: Macro
func (macro *Macro) Trig() *Macro {
	dw0 := macro.GetRegisterDW0()
	trig := map[uint32]string{
		0b00: "LEVEL",
		0b01: "EDGE_SINGLE",
		0b10: "OFF",
		0b11: "EDGE_BOTH",
	}
	level, exist := trig[dw0.GetRXLevelEdgeConfiguration()]
	if !exist {
		level = "ERROR"
	}
	return macro.Separator().Add(level)
}

// Adds Pad Polarity Inversion Stage (RXINV) to macro string as a new argument
// return: Macro
func (macro *Macro) Invert() *Macro {
	if macro.GetRegisterDW0().GetRxInvert() != 0 {
		return macro.Separator().Add("INVERT")
	}
	return macro.Separator().Add("NONE")
}

// Adds input/output buffer state
// return: Macro
func (macro *Macro) Bufdis() *Macro {
	dw0 := macro.GetRegisterDW0()
	states := map[uint32]string{
		0b00: "NO_DISABLE",    // both buffers are enabled
		0b01: "TX_DISABLE",    // output buffer is disabled
		0b10: "RX_DISABLE",    // input buffer is disabled
		0b11: "TX_RX_DISABLE", // both buffers are disabled
	}
	state, exist := states[dw0.GetGPIORxTxDisableStatus()]
	if !exist {
		state = "ERROR"
	}
	return macro.Separator().Add(state)
}

// Adds macro to set the host software ownership
// return: Macro
func (macro *Macro) Own() *Macro {
	if macro.IsOwnershipDriver() {
		return macro.Separator().Add("DRIVER")
	}
	return macro.Separator().Add("ACPI")
}

// Adds pad native function (PMODE) as a new argument
// return: Macro
func (macro *Macro) Padfn() *Macro {
	dw0 := macro.GetRegisterDW0()
	if number := dw0.GetPadMode(); number != 0 {
		return macro.Separator().Add(fmt.Sprintf("NF%d", number))
	}
	// GPIO used only for PAD_FUNC(x) macro
	return macro.Add("GPIO")
}

// Add a line to the macro that defines IO Standby State
// return: macro
func (macro *Macro) IOSstate() *Macro {
	states := map[uint32]string{
		bits.IOStateTxLASTRxE:     "TxLASTRxE",
		bits.IOStateTx0RxDCRx0:    "Tx0RxDCRx0",
		bits.IOStateTx0RxDCRx1:    "Tx0RxDCRx1",
		bits.IOStateTx1RxDCRx0:    "Tx1RxDCRx0",
		bits.IOStateTx1RxDCRx1:    "Tx1RxDCRx1",
		bits.IOStateTx0RxE:        "Tx0RxE",
		bits.IOStateTx1RxE:        "Tx1RxE",
		bits.IOStateHIZCRx0:       "HIZCRx0",
		bits.IOStateHIZCRx1:       "HIZCRx1",
		bits.IOStateTxDRxE:        "TxDRxE",
		bits.IOStateStandbyIgnore: "IGNORE",
	}
	dw1 := macro.GetRegisterDW1()
	state, exist := states[dw1.GetIOStandbyState()]
	if !exist {
		// ignore setting for incorrect value
		state = "ERROR"
	}
	return macro.Separator().Add(state)
}

// Add a line to the macro that defines IO Standby Termination
// return: macro
func (macro *Macro) IOTerm() *Macro {
	dw1 := macro.GetRegisterDW1()
	terminations := map[uint32]string{
		bits.IOTermSAME:    "SAME",
		bits.IOTermDISPUPD: "DISPUPD",
		bits.IOTermENPD:    "ENPD",
		bits.IOTermENPU:    "ENPU",
	}
	termination, exist := terminations[dw1.GetIOStandbyTermination()]
	if !exist {
		termination = "ERROR"
	}
	return macro.Separator().Add(termination)
}

// Check created macro
func (macro *Macro) check() *Macro {
	dw0 := macro.GetRegisterDW0()
	if !dw0.MaskCheck() {
		return macro.GenerateFields()
	}
	return macro
}

// or - Set " | " if its needed
func (macro *Macro) Or() *Macro {
	if str := macro.Get(); str[len(str)-1] == ')' {
		macro.Add(" | ")
	}
	return macro
}

func (macro *Macro) DecodeIgnoredFieldsDW0() *Macro {
	if p2m.Config.Field == p2m.FspFlds {
		return macro
	}
	dw0 := macro.GetRegisterDW0()
	if ignored := dw0.IgnoredFieldsGet(); ignored != 0 {
		saved := dw0.Value
		dw0.Value = ignored
		macro.Add("/* DW0: ")
		macro.Fields.DecodeDW0()
		macro.Add(" - IGNORED */\n")
		dw0.Value = saved
	}
	return macro
}

func (macro *Macro) DecodeIgnoredFieldsDW1() *Macro {
	if p2m.Config.Field == p2m.FspFlds {
		return macro
	}
	dw1 := macro.GetRegisterDW1()
	if ignored := dw1.IgnoredFieldsGet(); ignored != 0 {
		saved := dw1.Value
		dw1.Value = ignored
		macro.Add("/* DW0: ")
		macro.DecodeDW1()
		macro.Add(" - IGNORED */\n")
		dw1.Value = saved
	}
	return macro
}

// GenerateFields - generate bitfield macros
func (macro *Macro) GenerateFields() *Macro {
	dw0 := macro.GetRegisterDW0()
	dw1 := macro.GetRegisterDW1()

	// Get mask of ignored bit fields.
	dw0Ignored := dw0.IgnoredFieldsGet()
	dw1Ignored := dw1.IgnoredFieldsGet()

	if p2m.Config.GenLevel != 4 {
		macro.Clear()
	}
	if p2m.Config.GenLevel >= 3 {
		// Add string of reference macro as a comment
		reference := macro.Get()
		macro.Clear()
		/* DW0 : PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | 1 - IGNORED */
		macro.DecodeIgnoredFieldsDW0()
		macro.DecodeIgnoredFieldsDW1()
		if p2m.Config.GenLevel >= 4 {
			/* PAD_CFG_NF(GPP_B23, 20K_PD, PLTRST, NF2), */
			macro.Add("/* ").Add(reference).Add(" */\n")
		}
	}
	if p2m.Config.IgnoredFields {
		// Consider bit fields that should be ignored when regenerating
		// advansed macros
		tempVal := dw0.Value & ^dw0Ignored
		dw0.Value = tempVal

		tempVal = dw1.Value & ^dw1Ignored
		dw1.Value = tempVal
	}

	macro.Fields.GenerateString()
	return macro
}

// Generate macro for bi-directional GPIO port
func (macro *Macro) Bidirection() {
	dw1 := macro.GetRegisterDW1()
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

// Gets base string of current macro
// return: string of macro
func (macro *Macro) Generate() string {
	const rxDisable uint32 = 0x2
	const txDisable uint32 = 0x1

	macro.Platform.RemmapRstSrc()
	macro.Set("PAD_CFG")
	if dw0 := macro.GetRegisterDW0(); dw0.GetPadMode() == 0 {
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

	if p2m.Config.Field != p2m.NoFlds {
		// Clear control mask to generate advanced macro only
		return macro.GenerateFields().Get()
	}

	if !p2m.Config.AutoCheck {
		body := macro.Get()
		if p2m.Config.GenLevel >= 3 {
			macro.Clear()
			macro.DecodeIgnoredFieldsDW0()
			macro.DecodeIgnoredFieldsDW1()
			comment := macro.Get()
			if p2m.Config.GenLevel >= 4 {
				macro.Clear().Add("/* ")
				macro.Fields.GenerateString()
				macro.Add(" */\n")
				comment += macro.Get()
			}
			return comment + body
		}
		return body
	}

	return macro.check().Get()
}
