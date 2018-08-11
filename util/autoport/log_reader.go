package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"regexp"
	"strconv"
	"strings"
)

type LogDevReader struct {
	InputDirectory string
	ACPITables     map[string][]byte
	EC             []byte
}

func isXDigit(x uint8) bool {
	if x >= '0' && x <= '9' {
		return true
	}
	if x >= 'a' && x <= 'f' {
		return true
	}
	if x >= 'A' && x <= 'F' {
		return true
	}
	return false
}

type HexLine struct {
	length uint
	values [16]byte
	start  uint
}

func (l *LogDevReader) ReadHexLine(line string) (hex HexLine) {
	hex.start = 0
	line = strings.Trim(line, " ")
	fmt.Sscanf(line, "%x:", &hex.start)
	ll, _ := fmt.Sscanf(line, "%x: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x", &hex.start,
		&hex.values[0], &hex.values[1], &hex.values[2],
		&hex.values[3], &hex.values[4], &hex.values[5],
		&hex.values[6], &hex.values[7], &hex.values[8],
		&hex.values[9], &hex.values[10], &hex.values[11],
		&hex.values[12], &hex.values[13], &hex.values[14],
		&hex.values[15])
	hex.length = uint(ll - 1)
	return
}

func (l *LogDevReader) AssignHexLine(inp string, target []byte) []byte {
	hex := l.ReadHexLine(inp)
	if hex.start+hex.length > uint(len(target)) {
		target = target[0 : hex.start+hex.length]
	}
	copy(target[hex.start:hex.start+hex.length], hex.values[0:hex.length])
	return target
}

func (l *LogDevReader) GetEC() []byte {
	if l.EC != nil {
		return l.EC
	}
	l.EC = make([]byte, 0x100, 0x100)

	file, err := os.Open(l.InputDirectory + "/ectool.log")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	for scanner.Scan() {
		line := scanner.Text()
		if len(line) > 7 && isXDigit(line[0]) && isXDigit(line[1]) && line[2] == ':' {
			l.EC = l.AssignHexLine(line, l.EC)
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}

	return l.EC
}

func (l *LogDevReader) GetACPI() (Tables map[string][]byte) {
	if l.ACPITables != nil {
		return l.ACPITables
	}
	l.ACPITables = Tables

	file, err := os.Open(l.InputDirectory + "/acpidump.log")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	Tables = map[string][]byte{}

	curTable := ""
	for scanner.Scan() {
		line := scanner.Text()
		/* Only supports ACPI tables up to 0x10000 in size, FIXME if needed */
		is_hexline, _ := regexp.MatchString(" *[0-9A-Fa-f]{4}: ", line)
		switch {
		case len(line) >= 6 && line[5] == '@':
			curTable = line[0:4]
			Tables[curTable] = make([]byte, 0, 100000)
		case is_hexline:
			Tables[curTable] = l.AssignHexLine(line, Tables[curTable])
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}

	return
}

func (l *LogDevReader) GetPCIList() (PCIList []PCIDevData) {
	file, err := os.Open(l.InputDirectory + "/lspci.log")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	PCIList = []PCIDevData{}

	for scanner.Scan() {
		line := scanner.Text()
		switch {
		case !(len(line) < 7 || !isXDigit(line[0]) || !isXDigit(line[1]) || line[2] != ':' || !isXDigit(line[3]) || !isXDigit(line[4]) || line[5] != '.' || !isXDigit(line[6])):
			cur := PCIDevData{}
			fmt.Sscanf(line, "%x:%x.%x", &cur.Bus, &cur.Dev, &cur.Func)
			lc := strings.LastIndex(line, ":")
			li := strings.LastIndex(line[0:lc], "[")
			if li < 0 {
				continue
			}
			ven := 0
			dev := 0
			fmt.Sscanf(line[li+1:], "%x:%x", &ven, &dev)
			cur.PCIDevID = uint16(dev)
			cur.PCIVenID = uint16(ven)
			cur.ConfigDump = make([]byte, 0x100, 0x1000)
			PCIList = append(PCIList, cur)
		case len(line) > 7 && isXDigit(line[0]) && line[1] == '0' && line[2] == ':':
			start := 0
			fmt.Sscanf(line, "%x:", &start)
			cur := &PCIList[len(PCIList)-1]
			cur.ConfigDump = l.AssignHexLine(line, cur.ConfigDump)
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}

	return
}

func (l *LogDevReader) GetInteltool() (ret InteltoolData) {
	file, err := os.Open(l.InputDirectory + "/inteltool.log")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	paragraph := ""
	ret.GPIO = map[uint16]uint32{}
	ret.RCBA = map[uint16]uint32{}
	ret.IGD = map[uint32]uint32{}
	for scanner.Scan() {
		line := scanner.Text()
		switch {
		case len(line) > 7 && line[0] == '0' && line[1] == 'x' && line[6] == ':' && paragraph == "RCBA":
			addr, value := 0, 0
			fmt.Sscanf(line, "0x%x: 0x%x", &addr, &value)
			ret.RCBA[uint16(addr)] = uint32(value)
		case len(line) > 9 && line[0] == '0' && line[1] == 'x' && line[8] == ':' && paragraph == "IGD":
			addr, value := 0, 0
			fmt.Sscanf(line, "0x%x: 0x%x", &addr, &value)
			ret.IGD[uint32(addr)] = uint32(value)
		case strings.Contains(line, "DEFAULT"):
			continue
		case strings.Contains(line, "DIFF"):
			continue
		case strings.HasPrefix(line, "gpiobase"):
			addr, value := 0, 0
			fmt.Sscanf(line, "gpiobase+0x%x: 0x%x", &addr, &value)
			ret.GPIO[uint16(addr)] = uint32(value)
		case strings.HasPrefix(line, "============="):
			paragraph = strings.Trim(line, "= ")
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
	return
}

func (l *LogDevReader) GetDMI() (ret DMIData) {
	file, err := os.Open(l.InputDirectory + "/dmidecode.log")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	paragraph := ""
	for scanner.Scan() {
		line := scanner.Text()
		if !strings.HasPrefix(line, "\t") {
			paragraph = strings.TrimSpace(line)
			continue
		}
		idx := strings.Index(line, ":")
		if idx < 0 {
			continue
		}
		name := strings.TrimSpace(line[0:idx])
		value := strings.TrimSpace(line[idx+1:])
		switch paragraph + ":" + name {
		case "System Information:Manufacturer":
			ret.Vendor = value
		case "System Information:Product Name":
			ret.Model = value
		case "System Information:Version":
			ret.Version = value
		case "Chassis Information:Type":
			ret.IsLaptop = (value == "Notebook" || value == "Laptop")
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
	return
}

func (l *LogDevReader) GetAzaliaCodecs() (ret []AzaliaCodec) {
	for codecno := 0; codecno < 10; codecno++ {
		cur := AzaliaCodec{CodecNo: codecno, PinConfig: map[int]uint32{}}
		codec, err := os.Open(l.InputDirectory + "/codec#" + strconv.Itoa(codecno))
		if err != nil {
			continue
		}
		defer codec.Close()
		pin, err := os.Open(l.InputDirectory + "/pin_hwC0D" + strconv.Itoa(codecno))
		if err != nil {
			continue
		}
		defer pin.Close()

		scanner := bufio.NewScanner(codec)
		for scanner.Scan() {
			line := scanner.Text()
			if strings.HasPrefix(line, "Codec:") {
				fmt.Sscanf(line, "Codec: %s", &cur.Name)
				continue
			}
			if strings.HasPrefix(line, "Vendor Id:") {
				fmt.Sscanf(line, "Vendor Id: 0x%x", &cur.VendorID)
				continue
			}
			if strings.HasPrefix(line, "Subsystem Id:") {
				fmt.Sscanf(line, "Subsystem Id: 0x%x", &cur.SubsystemID)
				continue
			}
		}

		scanner = bufio.NewScanner(pin)
		for scanner.Scan() {
			line := scanner.Text()
			addr := 0
			val := uint32(0)
			fmt.Sscanf(line, "0x%x 0x%x", &addr, &val)
			cur.PinConfig[addr] = val
		}
		ret = append(ret, cur)
	}
	return
}

func (l *LogDevReader) GetIOPorts() []IOPorts {
	file, err := os.Open(l.InputDirectory + "/ioports.log")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()
	scanner := bufio.NewScanner(file)
	ret := make([]IOPorts, 0, 100)
	for scanner.Scan() {
		line := scanner.Text()
		el := IOPorts{}
		fmt.Sscanf(line, " %x-%x : %s", &el.Start, &el.End, &el.Usage)
		ret = append(ret, el)
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
	return ret

}

func (l *LogDevReader) GetCPUModel() (ret []uint32) {
	file, err := os.Open(l.InputDirectory + "/cpuinfo.log")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	ret = make([]uint32, 0, 100)
	proc := 0
	for scanner.Scan() {
		line := scanner.Text()
		sep := strings.Index(line, ":")
		if sep < 0 {
			continue
		}
		key := strings.TrimSpace(line[0:sep])
		val := strings.TrimSpace(line[sep+1:])

		if key == "processor" {
			proc, _ := strconv.Atoi(val)
			if len(ret) <= proc {
				ret = ret[0 : proc+1]
			}
			continue
		}
		if key == "cpu family" {
			family, _ := strconv.Atoi(val)
			ret[proc] |= uint32(((family & 0xf) << 8) | ((family & 0xff0) << 16))
		}
		if key == "model" {
			model, _ := strconv.Atoi(val)
			ret[proc] |= uint32(((model & 0xf) << 4) | ((model & 0xf0) << 12))
		}
		if key == "stepping" {
			stepping, _ := strconv.Atoi(val)
			ret[proc] |= uint32(stepping & 0xf)
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
	return
}

func (l *LogDevReader) HasPS2() bool {
	file, err := os.Open(l.InputDirectory + "/input_bustypes.log")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		if strings.Index(line, "0011") >= 0 {
			return true
		}
	}
	return false
}

var FlagLogInput = flag.String("input_log", ".", "Input log directory")
var FlagLogMkLogs = flag.Bool("make_logs", false, "Dump logs")

func MakeLogReader() *LogDevReader {
	if *FlagLogMkLogs {
		MakeLogs(*FlagLogInput)
	}
	return &LogDevReader{InputDirectory: *FlagLogInput}
}
