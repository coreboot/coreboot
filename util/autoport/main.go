/* This is just an experiment. Full automatic porting
   is probably not possible but a lot can be automated. */
package main

import (
	"bytes"
	"flag"
	"fmt"
	"log"
	"os"
	"sort"
	"strings"
)

type PCIAddr struct {
	Bus  int
	Dev  int
	Func int
}

type PCIDevData struct {
	PCIAddr
	PCIVenID   uint16
	PCIDevID   uint16
	ConfigDump []uint8
}

type PCIDevice interface {
	Scan(ctx Context, addr PCIDevData)
}

type InteltoolData struct {
	GPIO map[uint16]uint32
	RCBA map[uint16]uint32
	IGD  map[uint32]uint32
}

type DMIData struct {
	Vendor   string
	Model    string
	Version  string
	IsLaptop bool
}

type AzaliaCodec struct {
	Name        string
	VendorID    uint32
	SubsystemID uint32
	CodecNo     int
	PinConfig   map[int]uint32
}

type DevReader interface {
	GetPCIList() []PCIDevData
	GetDMI() DMIData
	GetInteltool() InteltoolData
	GetAzaliaCodecs() []AzaliaCodec
	GetACPI() map[string][]byte
	GetCPUModel() []uint32
	GetEC() []byte
	GetIOPorts() []IOPorts
	HasPS2() bool
}

type IOPorts struct {
	Start uint16
	End   uint16
	Usage string
}

type SouthBridger interface {
	GetGPIOHeader() string
	EncodeGPE(int) int
	DecodeGPE(int) int
	EnableGPE(int)
	NeedRouteGPIOManually()
}

var SouthBridge SouthBridger
var BootBlockFiles map[string]string = map[string]string{}
var ROMStageFiles map[string]string = map[string]string{}
var RAMStageFiles map[string]string = map[string]string{}
var SMMFiles map[string]string = map[string]string{}
var MainboardInit string
var MainboardEnable string
var MainboardIncludes []string

type Context struct {
	MoboID        string
	KconfigName   string
	Vendor        string
	Model         string
	BaseDirectory string
	InfoSource    DevReader
	SaneVendor    string
}

type IOAPICIRQ struct {
	APICID int
	IRQNO  [4]int
}

var IOAPICIRQs map[PCIAddr]IOAPICIRQ = map[PCIAddr]IOAPICIRQ{}
var KconfigBool map[string]bool = map[string]bool{}
var KconfigComment map[string]string = map[string]string{}
var KconfigString map[string]string = map[string]string{}
var KconfigHex map[string]uint32 = map[string]uint32{}
var KconfigInt map[string]int = map[string]int{}
var ROMSizeKB = 0
var ROMProtocol = ""
var FlashROMSupport = ""

func GetLE16(inp []byte) uint16 {
	return uint16(inp[0]) | (uint16(inp[1]) << 8)
}

func FormatHexLE16(inp []byte) string {
	return fmt.Sprintf("0x%04x", GetLE16(inp))
}

func FormatHex32(u uint32) string {
	return fmt.Sprintf("0x%08x", u)
}

func FormatHex8(u uint8) string {
	return fmt.Sprintf("0x%02x", u)
}

func FormatInt32(u uint32) string {
	return fmt.Sprintf("%d", u)
}

func FormatHexLE32(d []uint8) string {
	u := uint32(d[0]) | (uint32(d[1]) << 8) | (uint32(d[2]) << 16) | (uint32(d[3]) << 24)
	return FormatHex32(u)
}

func FormatBool(inp bool) string {
	if inp {
		return "1"
	} else {
		return "0"
	}
}

func sanitize(inp string) string {
	result := strings.ToLower(inp)
	result = strings.Replace(result, " ", "_", -1)
	result = strings.Replace(result, ",", "_", -1)
	result = strings.Replace(result, "-", "_", -1)
	for strings.HasSuffix(result, ".") {
		result = result[0 : len(result)-1]
	}
	return result
}

func AddBootBlockFile(Name string, Condition string) {
	BootBlockFiles[Name] = Condition
}

func AddROMStageFile(Name string, Condition string) {
	ROMStageFiles[Name] = Condition
}

func AddRAMStageFile(Name string, Condition string) {
	RAMStageFiles[Name] = Condition
}

func AddSMMFile(Name string, Condition string) {
	SMMFiles[Name] = Condition
}

func IsIOPortUsedBy(ctx Context, port uint16, name string) bool {
	for _, io := range ctx.InfoSource.GetIOPorts() {
		if io.Start <= port && port <= io.End && io.Usage == name {
			return true
		}
	}
	return false
}

var FlagOutDir = flag.String("coreboot_dir", ".", "Resulting coreboot directory")

func writeMF(mf *os.File, files map[string]string, category string) {
	keys := []string{}
	for file, _ := range files {
		keys = append(keys, file)
	}

	sort.Strings(keys)

	for _, file := range keys {
		condition := files[file]
		if condition == "" {
			fmt.Fprintf(mf, "%s-y += %s\n", category, file)
		} else {
			fmt.Fprintf(mf, "%s-$(%s) += %s\n", category, condition, file)
		}
	}
}

func Create(ctx Context, name string) *os.File {
	li := strings.LastIndex(name, "/")
	if li > 0 {
		os.MkdirAll(ctx.BaseDirectory+"/"+name[0:li], 0700)
	}
	mf, err := os.Create(ctx.BaseDirectory + "/" + name)
	if err != nil {
		log.Fatal(err)
	}
	return mf
}

func Add_gpl(f *os.File) {
	fmt.Fprintln(f, "/* SPDX-License-Identifier: GPL-2.0-only */")
	fmt.Fprintln(f)
}

func RestorePCI16Simple(f *os.File, pcidev PCIDevData, addr uint16) {
	fmt.Fprintf(f, "	pci_write_config16(PCI_DEV(%d, 0x%02x, %d), 0x%02x, 0x%02x%02x);\n",
		pcidev.Bus, pcidev.Dev, pcidev.Func, addr,
		pcidev.ConfigDump[addr+1],
		pcidev.ConfigDump[addr])
}

func RestorePCI32Simple(f *os.File, pcidev PCIDevData, addr uint16) {
	fmt.Fprintf(f, "	pci_write_config32(PCI_DEV(%d, 0x%02x, %d), 0x%02x, 0x%02x%02x%02x%02x);\n",
		pcidev.Bus, pcidev.Dev, pcidev.Func, addr,
		pcidev.ConfigDump[addr+3],
		pcidev.ConfigDump[addr+2],
		pcidev.ConfigDump[addr+1],
		pcidev.ConfigDump[addr])
}

func RestoreRCBA32(f *os.File, inteltool InteltoolData, addr uint16) {
	fmt.Fprintf(f, "\tRCBA32(0x%04x) = 0x%08x;\n", addr, inteltool.RCBA[addr])
}

type PCISlot struct {
	PCIAddr
	alias             string
	additionalComment string
	writeEmpty        bool
}

type DevTreeNode struct {
	Bus           int
	Dev           int
	Func          int
	Disabled      bool
	Registers     map[string]string
	IOs           map[uint16]uint16
	Children      []DevTreeNode
	PCISlots      []PCISlot
	PCIController bool
	ChildPCIBus   int
	MissingParent string
	SubVendor     uint16
	SubSystem     uint16
	Chip          string
	Comment       string
}

var DevTree DevTreeNode
var MissingChildren map[string][]DevTreeNode = map[string][]DevTreeNode{}
var unmatchedPCIChips map[PCIAddr]DevTreeNode = map[PCIAddr]DevTreeNode{}
var unmatchedPCIDevices map[PCIAddr]DevTreeNode = map[PCIAddr]DevTreeNode{}

func Offset(dt *os.File, offset int) {
	for i := 0; i < offset; i++ {
		fmt.Fprintf(dt, "\t")
	}
}

func MatchDev(dev *DevTreeNode) {
	for idx := range dev.Children {
		MatchDev(&dev.Children[idx])
	}

	for _, slot := range dev.PCISlots {
		slotChip, ok := unmatchedPCIChips[slot.PCIAddr]

		if !ok {
			continue
		}

		if slot.additionalComment != "" && slotChip.Comment != "" {
			slotChip.Comment = slot.additionalComment + " " + slotChip.Comment
		} else {
			slotChip.Comment = slot.additionalComment + slotChip.Comment
		}

		delete(unmatchedPCIChips, slot.PCIAddr)
		MatchDev(&slotChip)
		dev.Children = append(dev.Children, slotChip)
	}

	if dev.PCIController {
		for slot, slotDev := range unmatchedPCIChips {
			if slot.Bus == dev.ChildPCIBus {
				delete(unmatchedPCIChips, slot)
				MatchDev(&slotDev)
				dev.Children = append(dev.Children, slotDev)
			}
		}
	}

	for _, slot := range dev.PCISlots {
		slotDev, ok := unmatchedPCIDevices[slot.PCIAddr]
		if !ok {
			if slot.writeEmpty {
				dev.Children = append(dev.Children,
					DevTreeNode{
						Registers: map[string]string{},
						Chip:      "pci",
						Bus:       slot.Bus,
						Dev:       slot.Dev,
						Func:      slot.Func,
						Comment:   slot.additionalComment,
						Disabled:  true,
					},
				)
			}
			continue
		}

		if slot.additionalComment != "" && slotDev.Comment != "" {
			slotDev.Comment = slot.additionalComment + " " + slotDev.Comment
		} else {
			slotDev.Comment = slot.additionalComment + slotDev.Comment
		}

		MatchDev(&slotDev)
		dev.Children = append(dev.Children, slotDev)
		delete(unmatchedPCIDevices, slot.PCIAddr)
	}

	if dev.MissingParent != "" {
		for _, child := range MissingChildren[dev.MissingParent] {
			MatchDev(&child)
			dev.Children = append(dev.Children, child)
		}
		delete(MissingChildren, dev.MissingParent)
	}

	if dev.PCIController {
		for slot, slotDev := range unmatchedPCIDevices {
			if slot.Bus == dev.ChildPCIBus {
				MatchDev(&slotDev)
				dev.Children = append(dev.Children, slotDev)
				delete(unmatchedPCIDevices, slot)
			}
		}
	}
}

func writeOn(dt *os.File, dev DevTreeNode) {
	if dev.Disabled {
		fmt.Fprintf(dt, "off")
	} else {
		fmt.Fprintf(dt, "on")
	}
}

func WriteDev(dt *os.File, offset int, alias string, dev DevTreeNode) {
	Offset(dt, offset)
	switch dev.Chip {
	case "cpu_cluster", "lapic", "domain", "ioapic":
		fmt.Fprintf(dt, "device %s 0x%x ", dev.Chip, dev.Dev)
		writeOn(dt, dev)
	case "pci", "pnp":
		if alias != "" {
			fmt.Fprintf(dt, "device ref %s ", alias)
		} else {
			fmt.Fprintf(dt, "device %s %02x.%x ", dev.Chip, dev.Dev, dev.Func)
		}
		writeOn(dt, dev)
	case "i2c":
		fmt.Fprintf(dt, "device %s %02x ", dev.Chip, dev.Dev)
		writeOn(dt, dev)
	default:
		fmt.Fprintf(dt, "chip %s", dev.Chip)
	}
	if dev.Comment != "" {
		fmt.Fprintf(dt, " # %s", dev.Comment)
	}
	fmt.Fprintf(dt, "\n")
	if dev.Chip == "pci" && dev.SubSystem != 0 && dev.SubVendor != 0 {
		Offset(dt, offset+1)
		fmt.Fprintf(dt, "subsystemid 0x%04x 0x%04x\n", dev.SubVendor, dev.SubSystem)
	}

	ioapic, ok := IOAPICIRQs[PCIAddr{Bus: dev.Bus, Dev: dev.Dev, Func: dev.Func}]
	if dev.Chip == "pci" && ok {
		for pin, irq := range ioapic.IRQNO {
			if irq != 0 {
				Offset(dt, offset+1)
				fmt.Fprintf(dt, "ioapic_irq %d INT%c 0x%x\n", ioapic.APICID, 'A'+pin, irq)
			}
		}
	}

	keys := []string{}
	for reg, _ := range dev.Registers {
		keys = append(keys, reg)
	}

	sort.Strings(keys)

	for _, reg := range keys {
		val := dev.Registers[reg]
		Offset(dt, offset+1)
		fmt.Fprintf(dt, "register \"%s\" = \"%s\"\n", reg, val)
	}

	ios := []int{}
	for reg, _ := range dev.IOs {
		ios = append(ios, int(reg))
	}

	sort.Ints(ios)

	for _, reg := range ios {
		val := dev.IOs[uint16(reg)]
		Offset(dt, offset+1)
		fmt.Fprintf(dt, "io 0x%x = 0x%x\n", reg, val)
	}

	for _, child := range dev.Children {
		alias = ""
		for _, slot := range dev.PCISlots {
			if slot.PCIAddr.Bus == child.Bus &&
				slot.PCIAddr.Dev == child.Dev && slot.PCIAddr.Func == child.Func {
				alias = slot.alias
			}
		}
		WriteDev(dt, offset+1, alias, child)
	}

	Offset(dt, offset)
	fmt.Fprintf(dt, "end\n")
}

func PutChip(domain string, cur DevTreeNode) {
	MissingChildren[domain] = append(MissingChildren[domain], cur)
}

func PutPCIChip(addr PCIDevData, cur DevTreeNode) {
	unmatchedPCIChips[addr.PCIAddr] = cur
}

func PutPCIDevParent(addr PCIDevData, comment string, parent string) {
	cur := DevTreeNode{
		Registers:     map[string]string{},
		Chip:          "pci",
		Bus:           addr.Bus,
		Dev:           addr.Dev,
		Func:          addr.Func,
		MissingParent: parent,
		Comment:       comment,
	}
	if addr.ConfigDump[0xa] == 0x04 && addr.ConfigDump[0xb] == 0x06 {
		cur.PCIController = true
		cur.ChildPCIBus = int(addr.ConfigDump[0x19])

		loopCtr := 0
		for capPtr := addr.ConfigDump[0x34]; capPtr != 0; capPtr = addr.ConfigDump[capPtr+1] {
			/* Avoid hangs. There are only 0x100 different possible values for capPtr.
			   If we iterate longer than that, we're in endless loop. */
			loopCtr++
			if loopCtr > 0x100 {
				break
			}
			if addr.ConfigDump[capPtr] == 0x0d {
				cur.SubVendor = GetLE16(addr.ConfigDump[capPtr+4 : capPtr+6])
				cur.SubSystem = GetLE16(addr.ConfigDump[capPtr+6 : capPtr+8])
			}
		}
	} else {
		cur.SubVendor = GetLE16(addr.ConfigDump[0x2c:0x2e])
		cur.SubSystem = GetLE16(addr.ConfigDump[0x2e:0x30])
	}
	unmatchedPCIDevices[addr.PCIAddr] = cur
}

func PutPCIDev(addr PCIDevData, comment string) {
	PutPCIDevParent(addr, comment, "")
}

type GenericPCI struct {
	Comment       string
	Bus0Subdiv    string
	MissingParent string
}

type GenericVGA struct {
	GenericPCI
}

type DSDTInclude struct {
	Comment string
	File    string
}

type DSDTDefine struct {
	Key     string
	Comment string
	Value   string
}

var DSDTIncludes []DSDTInclude
var DSDTPCI0Includes []DSDTInclude
var DSDTDefines []DSDTDefine

func (g GenericPCI) Scan(ctx Context, addr PCIDevData) {
	PutPCIDevParent(addr, g.Comment, g.MissingParent)
}

var IGDEnabled bool = false

func (g GenericVGA) Scan(ctx Context, addr PCIDevData) {
	KconfigString["VGA_BIOS_ID"] = fmt.Sprintf("%04x,%04x",
		addr.PCIVenID,
		addr.PCIDevID)
	PutPCIDevParent(addr, g.Comment, g.MissingParent)
	IGDEnabled = true
}

func makeKconfigName(ctx Context) {
	kn := Create(ctx, "Kconfig.name")
	defer kn.Close()

	fmt.Fprintf(kn, "config %s\n\tbool \"%s\"\n", ctx.KconfigName, ctx.Model)
}

func makeComment(name string) string {
	cmt, ok := KconfigComment[name]
	if !ok {
		return ""
	}
	return " # " + cmt
}

func makeKconfig(ctx Context) {
	kc := Create(ctx, "Kconfig")
	defer kc.Close()

	fmt.Fprintf(kc, "if %s\n\n", ctx.KconfigName)

	fmt.Fprintf(kc, "config BOARD_SPECIFIC_OPTIONS\n\tdef_bool y\n")
	keys := []string{}
	for name, val := range KconfigBool {
		if val {
			keys = append(keys, name)
		}
	}

	sort.Strings(keys)

	for _, name := range keys {
		fmt.Fprintf(kc, "\tselect %s%s\n", name, makeComment(name))
	}

	keys = nil
	for name, val := range KconfigBool {
		if !val {
			keys = append(keys, name)
		}
	}

	sort.Strings(keys)

	for _, name := range keys {
		fmt.Fprintf(kc, `
config %s%s
	bool
	default n
`, name, makeComment(name))
	}

	keys = nil
	for name, _ := range KconfigString {
		keys = append(keys, name)
	}

	sort.Strings(keys)

	for _, name := range keys {
		fmt.Fprintf(kc, `
config %s%s
	string
	default "%s"
`, name, makeComment(name), KconfigString[name])
	}

	keys = nil
	for name, _ := range KconfigHex {
		keys = append(keys, name)
	}

	sort.Strings(keys)

	for _, name := range keys {
		fmt.Fprintf(kc, `
config %s%s
	hex
	default 0x%x
`, name, makeComment(name), KconfigHex[name])
	}

	keys = nil
	for name, _ := range KconfigInt {
		keys = append(keys, name)
	}

	sort.Strings(keys)

	for _, name := range keys {
		fmt.Fprintf(kc, `
config %s%s
	int
	default %d
`, name, makeComment(name), KconfigInt[name])
	}

	fmt.Fprintf(kc, "endif\n")
}

const MoboDir = "/src/mainboard/"

func makeVendor(ctx Context) {
	vendor := ctx.Vendor
	vendorSane := ctx.SaneVendor
	vendorDir := *FlagOutDir + MoboDir + vendorSane
	vendorUpper := strings.ToUpper(vendorSane)
	kconfig := vendorDir + "/Kconfig"
	if _, err := os.Stat(kconfig); os.IsNotExist(err) {
		f, err := os.Create(kconfig)
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		f.WriteString(`if VENDOR_` + vendorUpper + `

choice
	prompt "Mainboard model"

source "src/mainboard/` + vendorSane + `/*/Kconfig.name"

endchoice

source "src/mainboard/` + vendorSane + `/*/Kconfig"

config MAINBOARD_VENDOR
	string
	default "` + vendor + `"

endif # VENDOR_` + vendorUpper + "\n")
	}
	kconfigName := vendorDir + "/Kconfig.name"
	if _, err := os.Stat(kconfigName); os.IsNotExist(err) {
		f, err := os.Create(kconfigName)
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		f.WriteString(`config VENDOR_` + vendorUpper + `
	bool "` + vendor + `"
`)
	}

}

func GuessECGPE(ctx Context) int {
	/* FIXME:XX Use iasl -d and/or better parsing  */
	dsdt := ctx.InfoSource.GetACPI()["DSDT"]
	idx := bytes.Index(dsdt, []byte{0x08, '_', 'G', 'P', 'E', 0x0a}) /* Name (_GPE, byte).  */
	if idx > 0 {
		return int(dsdt[idx+6])
	}
	return -1
}

func GuessSPDMap(ctx Context) []uint8 {
	dmi := ctx.InfoSource.GetDMI()

	if dmi.Vendor == "LENOVO" {
		return []uint8{0x50, 0x52, 0x51, 0x53}
	}
	return []uint8{0x50, 0x51, 0x52, 0x53}
}

func main() {
	flag.Parse()

	ctx := Context{}

	ctx.InfoSource = MakeLogReader()

	dmi := ctx.InfoSource.GetDMI()

	ctx.Vendor = dmi.Vendor

	if dmi.Vendor == "LENOVO" {
		ctx.Model = dmi.Version
	} else {
		ctx.Model = dmi.Model
	}

	if dmi.IsLaptop {
		KconfigBool["SYSTEM_TYPE_LAPTOP"] = true
	}
	ctx.SaneVendor = sanitize(ctx.Vendor)
	for {
		last := ctx.SaneVendor
		for _, suf := range []string{"_inc", "_co", "_corp"} {
			ctx.SaneVendor = strings.TrimSuffix(ctx.SaneVendor, suf)
		}
		if last == ctx.SaneVendor {
			break
		}
	}
	ctx.MoboID = ctx.SaneVendor + "/" + sanitize(ctx.Model)
	ctx.KconfigName = "BOARD_" + strings.ToUpper(ctx.SaneVendor+"_"+sanitize(ctx.Model))
	ctx.BaseDirectory = *FlagOutDir + MoboDir + ctx.MoboID
	KconfigString["MAINBOARD_DIR"] = ctx.MoboID
	KconfigString["MAINBOARD_PART_NUMBER"] = ctx.Model

	os.MkdirAll(ctx.BaseDirectory, 0700)

	makeVendor(ctx)

	ScanRoot(ctx)

	if IGDEnabled {
		KconfigBool["MAINBOARD_HAS_LIBGFXINIT"] = true
		KconfigComment["MAINBOARD_HAS_LIBGFXINIT"] = "FIXME: check this"
		AddRAMStageFile("gma-mainboard.ads", "CONFIG_MAINBOARD_USE_LIBGFXINIT")
	}

	if len(BootBlockFiles) > 0 || len(ROMStageFiles) > 0 || len(RAMStageFiles) > 0 || len(SMMFiles) > 0 {
		mf := Create(ctx, "Makefile.inc")
		defer mf.Close()
		writeMF(mf, BootBlockFiles, "bootblock")
		writeMF(mf, ROMStageFiles, "romstage")
		writeMF(mf, RAMStageFiles, "ramstage")
		writeMF(mf, SMMFiles, "smm")
	}

	devtree := Create(ctx, "devicetree.cb")
	defer devtree.Close()

	MatchDev(&DevTree)
	WriteDev(devtree, 0, "", DevTree)

	if MainboardInit != "" || MainboardEnable != "" || MainboardIncludes != nil {
		mainboard := Create(ctx, "mainboard.c")
		defer mainboard.Close()
		Add_gpl(mainboard)
		mainboard.WriteString("#include <device/device.h>\n")
		for _, include := range MainboardIncludes {
			mainboard.WriteString("#include <" + include + ">\n")
		}
		mainboard.WriteString("\n")
		if MainboardInit != "" {
			mainboard.WriteString(`static void mainboard_init(struct device *dev)
{
` + MainboardInit + "}\n\n")
		}
		if MainboardInit != "" || MainboardEnable != "" {
			mainboard.WriteString("static void mainboard_enable(struct device *dev)\n{\n")
			if MainboardInit != "" {
				mainboard.WriteString("\tdev->ops->init = mainboard_init;\n\n")
			}
			mainboard.WriteString(MainboardEnable)
			mainboard.WriteString("}\n\n")
			mainboard.WriteString(`struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
`)
		}
	}

	bi := Create(ctx, "board_info.txt")
	defer bi.Close()

	fixme := ""

	if dmi.IsLaptop {
		bi.WriteString("Category: laptop\n")
	} else {
		bi.WriteString("Category: desktop\n")
		fixme += "check category, "
	}

	missing := "ROM package, ROM socketed"

	if ROMProtocol != "" {
		fmt.Fprintf(bi, "ROM protocol: %s\n", ROMProtocol)
	} else {
		missing += ", ROM protocol"
	}

	if FlashROMSupport != "" {
		fmt.Fprintf(bi, "Flashrom support: %s\n", FlashROMSupport)
	} else {
		missing += ", Flashrom support"
	}

	missing += ", Release year"

	if fixme != "" {
		fmt.Fprintf(bi, "FIXME: %s, put %s\n", fixme, missing)
	} else {
		fmt.Fprintf(bi, "FIXME: put %s\n", missing)
	}

	if ROMSizeKB == 0 {
		KconfigBool["BOARD_ROMSIZE_KB_2048"] = true
		KconfigComment["BOARD_ROMSIZE_KB_2048"] = "FIXME: correct this"
	} else {
		KconfigBool[fmt.Sprintf("BOARD_ROMSIZE_KB_%d", ROMSizeKB)] = true
	}

	makeKconfig(ctx)
	makeKconfigName(ctx)

	dsdt := Create(ctx, "dsdt.asl")
	defer dsdt.Close()

	for _, define := range DSDTDefines {
		if define.Comment != "" {
			fmt.Fprintf(dsdt, "\t/* %s. */\n", define.Comment)
		}
		dsdt.WriteString("#define " + define.Key + " " + define.Value + "\n")
	}

	Add_gpl(dsdt)
	dsdt.WriteString(
		`
#include <acpi/acpi.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20141018	/* OEM revision */
)
{
	#include <acpi/dsdt_top.asl>
	#include "acpi/platform.asl"
`)

	for _, x := range DSDTIncludes {
		if x.Comment != "" {
			fmt.Fprintf(dsdt, "\t/* %s. */\n", x.Comment)
		}
		fmt.Fprintf(dsdt, "\t#include <%s>\n", x.File)
	}

	dsdt.WriteString(`
	Device (\_SB.PCI0)
	{
`)
	for _, x := range DSDTPCI0Includes {
		if x.Comment != "" {
			fmt.Fprintf(dsdt, "\t/* %s. */\n", x.Comment)
		}
		fmt.Fprintf(dsdt, "\t\t#include <%s>\n", x.File)
	}
	dsdt.WriteString(
		`	}
}
`)

	if IGDEnabled {
		gma := Create(ctx, "gma-mainboard.ads")
		defer gma.Close()

		gma.WriteString(`-- SPDX-License-Identifier: GPL-2.0-or-later

with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

private package GMA.Mainboard is

   -- FIXME: check this
   ports : constant Port_List :=
     (DP1,
      DP2,
      DP3,
      HDMI1,
      HDMI2,
      HDMI3,
      Analog,
      LVDS,
      eDP);

end GMA.Mainboard;
`)
	}
}
