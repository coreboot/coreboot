package common

import (
	"fmt"
	"strconv"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
)

const (
	Driver bool = true
	Acpi   bool = false
)

type FieldsIf interface {
	DecodeDW0(*Macro) *Macro
	DecodeDW1(*Macro) *Macro
	GenerateMacro(*Macro) *Macro
}

type PlatformIf interface {
	RemapResetSource(*Macro)
	Pull(*Macro)
	AddGpiMacro(*Macro)
	AddGpoMacro(*Macro)
	AddNativeFunctionMacro(*Macro)
	AddNoConnMacro(*Macro)
	GetRegisterDW0() *register.DW0
	GetRegisterDW1() *register.DW1
}

type Macro struct {
	line      string
	id        string
	ownership bool
	Platform  PlatformIf
	Fields    FieldsIf
}

func CreateFrom(id string, ownership bool, pi PlatformIf, fi FieldsIf) Macro {
	return Macro{
		id:        id,
		ownership: ownership,
		Platform:  pi,
		Fields:    fi,
	}
}

func (m *Macro) String() string {
	return m.line
}

func (m *Macro) GetPadId() string {
	return m.id
}

func (m Macro) IsOwnershipDriver() bool {
	return m.ownership
}

func (m *Macro) SetOwnershipAcpi() {
	m.ownership = Acpi
}

func (m *Macro) SetOwnershipDriver() {
	m.ownership = Driver
}

func (m *Macro) Add(str string) *Macro {
	m.line += str
	return m
}

func (m *Macro) Set(str string) *Macro {
	m.line = str
	return m
}

func (m *Macro) Clear() *Macro {
	m.line = ""
	return m
}

// Id() adds Pad Id to the macro string
func (m *Macro) Id() *Macro {
	return m.Add(m.id)
}

// Separator() adds separator ", " to macro if needed
func (m *Macro) Separator() *Macro {
	line := m.line
	c := line[len(line)-1]
	if c != '(' && c != '_' {
		m.Add(", ")
	}
	return m
}

// or - Set " | " if its needed
func (m *Macro) Or() *Macro {
	if str := m.line; str[len(str)-1] == ')' {
		m.Add(" | ")
	}
	return m
}

// Rstsrc() adds PADRSTCFG parameter
func (m *Macro) Rstsrc() *Macro {
	dw0 := m.Platform.GetRegisterDW0()
	resetsrc := map[uint32]string{
		0b00: "PWROK",
		0b01: "DEEP",
		0b10: "PLTRST",
		0b11: "RSMRST",
	}
	source, exist := resetsrc[dw0.GetResetConfig()]
	if !exist {
		logs.Errorf("%s: ResetConfig error: map does not contain %d",
			m.id, dw0.GetResetConfig())
		source = "ERROR"
	}
	return m.Separator().Add(source)
}

// Pull() adds Pad Termination TERM parameter
func (m *Macro) Pull() *Macro {
	m.Platform.Pull(m)
	return m
}

// Val() adds pad GPO value to macro string
func (m *Macro) Val() *Macro {
	dw0 := m.Platform.GetRegisterDW0()
	return m.Separator().Add(strconv.Itoa(int(dw0.GetGPIOTXState())))
}

// Trig() adds Pad GPO value to macro string
func (m *Macro) Trig() *Macro {
	dw0 := m.Platform.GetRegisterDW0()
	trig := map[uint32]string{
		0b00: "LEVEL",
		0b01: "EDGE_SINGLE",
		0b10: "OFF",
		0b11: "EDGE_BOTH",
	}
	level, exist := trig[dw0.GetRXLevelEdgeConfiguration()]
	if !exist {
		logs.Errorf("%s: RXLevelEdgeConfig error: map does not contain %d",
			m.id, dw0.GetRXLevelEdgeConfiguration())
		level = "ERROR"
	}
	return m.Separator().Add(level)
}

// Invert() adds Pad Polarity Inversion Stage (RXINV) to macro string
func (m *Macro) Invert() *Macro {
	if dw0 := m.Platform.GetRegisterDW0(); dw0.GetRxInvert() != 0 {
		return m.Separator().Add("INVERT")
	}
	return m.Separator().Add("NONE")
}

// Bufdis() adds input/output buffer state
func (m *Macro) Bufdis() *Macro {
	dw0 := m.Platform.GetRegisterDW0()
	states := map[uint32]string{
		0b00: "NO_DISABLE",    // both buffers are enabled
		0b01: "TX_DISABLE",    // output buffer is disabled
		0b10: "RX_DISABLE",    // input buffer is disabled
		0b11: "TX_RX_DISABLE", // both buffers are disabled
	}
	state, exist := states[dw0.GetGPIORxTxDisableStatus()]
	if !exist {
		logs.Errorf("%s: GPIORxTxDisableStatus error: map does not contain %d",
			m.id, dw0.GetGPIORxTxDisableStatus())
		state = "ERROR"
	}
	return m.Separator().Add(state)
}

// Own() adds macro to set the host software ownership
func (m *Macro) Own() *Macro {
	if m.IsOwnershipDriver() {
		return m.Separator().Add("DRIVER")
	}
	return m.Separator().Add("ACPI")
}

// Padfn() adds pad native function (PMODE)
func (m *Macro) Padfn() *Macro {
	dw0 := m.Platform.GetRegisterDW0()
	if number := dw0.GetPadMode(); number != 0 {
		return m.Separator().Add(fmt.Sprintf("NF%d", number))
	}
	// GPIO used only for PAD_FUNC(x) macro
	return m.Add("GPIO")
}

// IOSstate() adds a line to the macro that defines IO Standby State
func (m *Macro) IOSstate() *Macro {
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
	dw1 := m.Platform.GetRegisterDW1()
	state, exist := states[dw1.GetIOStandbyState()]
	if !exist {
		logs.Errorf("%s: IOStandbyState error: map does not contain %d",
			m.id, dw1.GetIOStandbyState())
		state = "ERROR"
	}
	return m.Separator().Add(state)
}

// IOTerm() add a line to the macro that defines IO Standby Termination
func (m *Macro) IOTerm() *Macro {
	dw1 := m.Platform.GetRegisterDW1()
	terminations := map[uint32]string{
		bits.IOTermSAME:    "SAME",
		bits.IOTermDISPUPD: "DISPUPD",
		bits.IOTermENPD:    "ENPD",
		bits.IOTermENPU:    "ENPU",
	}
	termination, exist := terminations[dw1.GetIOStandbyTermination()]
	if !exist {
		logs.Errorf("%s: IOStandbyTermination error: map does not contain %d",
			m.id, dw1.GetIOStandbyTermination())
		termination = "ERROR"
	}
	return m.Separator().Add(termination)
}

// Check created macro
func (m *Macro) Check() *Macro {
	dw0 := m.Platform.GetRegisterDW0()
	if !dw0.MaskCheck() {
		return m.GenerateFields()
	}
	return m
}

func (m *Macro) DecodeIgnoredFieldsDW0() *Macro {
	if p2m.Config.Field == p2m.FspFlds {
		logs.Infof("%s: decoding of extracted fields is not applied for fsp", m.id)
		return m
	} else if m.Fields == nil {
		logs.Errorf("%s: field collection is not set in the macro structure", m.id)
		return m
	}

	dw0 := m.Platform.GetRegisterDW0()
	if ignored := dw0.IgnoredFieldsGet(); ignored != 0 {
		saved := dw0.Value
		dw0.Value = ignored
		m.Add("/* DW0: ").Fields.DecodeDW0(m).Add(" - IGNORED */\n")
		dw0.Value = saved
	}

	return m
}

func (m *Macro) DecodeIgnoredFieldsDW1() *Macro {
	if p2m.Config.Field == p2m.FspFlds {
		logs.Infof("%s: decoding of extracted fields is not applied for fsp", m.id)
		return m
	} else if m.Fields == nil {
		logs.Errorf("%s: field collection is not set in the macro structure", m.id)
		return m
	}

	dw1 := m.Platform.GetRegisterDW1()
	if ignored := dw1.IgnoredFieldsGet(); ignored != 0 {
		saved := dw1.Value
		dw1.Value = ignored
		m.Add("/* DW1: ").Fields.DecodeDW1(m).Add(" - IGNORED */\n")
		dw1.Value = saved
	}

	return m
}

// GenerateFields() generates bitfield macros
func (m *Macro) GenerateFields() *Macro {
	if m.Fields == nil {
		logs.Errorf("%s: field collection is not set in the macro structure", m.id)
		return m
	}

	dw0 := m.Platform.GetRegisterDW0()
	dw1 := m.Platform.GetRegisterDW1()

	// Get mask of ignored bit fields.
	dw0Ignored := dw0.IgnoredFieldsGet()
	dw1Ignored := dw1.IgnoredFieldsGet()

	if p2m.Config.GenLevel != 4 {
		m.Clear()
	}
	if p2m.Config.GenLevel >= 3 {
		// Add string of reference m as a comment
		reference := m.line
		m.Clear()
		/* DW0 : PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | 1 - IGNORED */
		m.DecodeIgnoredFieldsDW0()
		m.DecodeIgnoredFieldsDW1()
		if p2m.Config.GenLevel >= 4 {
			/* PAD_CFG_NF(GPP_B23, 20K_PD, PLTRST, NF2), */
			m.Add("/* ").Add(reference).Add(" */\n")
		}
	}
	if p2m.Config.IgnoredFields {
		// Consider bit fields that should be ignored when regenerating
		// advansed ms
		tempVal := dw0.Value & ^dw0Ignored
		dw0.Value = tempVal

		tempVal = dw1.Value & ^dw1Ignored
		dw1.Value = tempVal
	}

	return m.Fields.GenerateMacro(m)
}

// Bidirection() generates macro for bi-directional GPIO port
func (m *Macro) Bidirection() {
	dw1 := m.Platform.GetRegisterDW1()
	ios := dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0
	m.Set("PAD_CFG_GPIO_BIDIRECT")
	if ios {
		m.Add("_IOS")
	}
	// PAD_CFG_GPIO_BIDIRECT(pad, val, pull, rst, trig, own)
	m.Add("(").Id().Val().Pull().Rstsrc().Trig()
	if ios {
		// PAD_CFG_GPIO_BIDIRECT_IOS(pad, val, pull, rst, trig, iosstate, iosterm, own)
		m.IOSstate().IOTerm()
	}
	m.Own().Add("),")
}

// Generate() generates string of macro
func (m *Macro) Generate() string {
	m.Platform.RemapResetSource(m)
	if dw0 := m.Platform.GetRegisterDW0(); dw0.GetPadMode() == 0 {
		const txDisable uint32 = 0b01
		const rxDisable uint32 = 0b10
		switch m.Set("PAD_CFG"); dw0.GetGPIORxTxDisableStatus() {
		case txDisable:
			m.Platform.AddGpiMacro(m) // GPI

		case rxDisable:
			m.Platform.AddGpoMacro(m) // GPO

		case rxDisable | txDisable:
			m.Platform.AddNoConnMacro(m) // NC

		default:
			m.Bidirection()
		}
	} else {
		m.Platform.AddNativeFunctionMacro(m)
	}

	if p2m.Config.Field != p2m.NoFlds {
		// clear control mask to generate field collection macro
		return m.GenerateFields().line
	}

	if !p2m.Config.AutoCheck {
		body := m.line
		if p2m.Config.GenLevel >= 3 {
			if m.Fields == nil {
				logs.Errorf("%s: field collection is not set in the macro structure", m.id)
				return body
			}
			m.Clear()
			m.DecodeIgnoredFieldsDW0()
			m.DecodeIgnoredFieldsDW1()
			comment := m.line
			if p2m.Config.GenLevel >= 4 {
				comment += m.Clear().Add("/* ").Fields.GenerateMacro(m).Add(" */\n").line
			}
			return comment + body
		}
		return body
	}

	return m.Check().line
}
