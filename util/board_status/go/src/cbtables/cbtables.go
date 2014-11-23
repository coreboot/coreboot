package cbtables

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"runtime"
	"strings"
	"time"
)

type Header struct {
	Signature      [4]uint8 /* LBIO */
	HeaderBytes    uint32
	HeaderChecksum uint32
	TableBytes     uint32
	TableChecksum  uint32
	TableEntries   uint32
}

type Record struct {
	Tag  uint32
	Size uint32
}

type rawTable struct {
	record  Record
	payload []byte
}

type parsedTables struct {
	mem     *os.File
	raw     []rawTable
	typeMap map[uint32][]byte
}

var headerSignature [4]byte = [4]byte{'L', 'B', 'I', 'O'}

const HeaderSize = 24
const (
	TagVersion    = 0x0004
	TagForward    = 0x0011
	TagTimestamps = 0x0016
	TagConsole    = 0x0017
	TagVersionTimestamp = 0x0026
)

type CBTablesReader interface {
	GetConsole() (cons []byte, lost uint32, err error)
	GetTimestamps() (*TimeStamps, error)
	GetVersion() (string, error)
	GetVersionTimestamp() (time.Time, error)
}

type CBMemConsole struct {
	Size   uint32
	Cursor uint32
}

type TimeStampEntry struct {
	EntryID    uint32
	EntryStamp uint64
}

type TimeStampHeader struct {
	BaseTime   uint64
	MaxEntries uint32
	NumEntries uint32
}

type TimeStamps struct {
	Head         TimeStampHeader
	Entries      []TimeStampEntry
	FrequencyMHZ uint32
}

var timeStampNames map[uint32]string = map[uint32]string{
	1:    "start of rom stage",
	2:    "before ram initialization",
	3:    "after ram initialization",
	4:    "end of romstage",
	5:    "start of verified boot",
	6:    "end of verified boot",
	8:    "start of copying ram stage",
	9:    "end of copying ram stage",
	10:   "start of ramstage",
	30:   "device enumeration",
	40:   "device configuration",
	50:   "device enable",
	60:   "device initialization",
	70:   "device setup done",
	75:   "cbmem post",
	80:   "write tables",
	90:   "load payload",
	98:   "ACPI wake jump",
	99:   "selfboot jump",
	1000: "depthcharge start",
	1001: "RO parameter init",
	1002: "RO vboot init",
	1003: "RO vboot select firmware",
	1004: "RO vboot select&load kernel",
	1010: "RW vboot select&load kernel",
	1020: "vboot select&load kernel",
	1100: "crossystem data",
	1101: "start kernel",
}

func formatSep(val uint64) string {
	ret := ""
	for val > 1000 {
		ret = fmt.Sprintf(",%03d", val%1000) + ret
		val /= 1000
	}
	ret = fmt.Sprintf("%d", val) + ret
	return ret
}

func formatElapsedTime(ticks uint64, frequency uint32) string {
	if frequency == 0 {
		return formatSep(ticks) + " cycles"
	}
	us := ticks / uint64(frequency)
	return formatSep(us) + " us"
}

func (t TimeStamps) String() string {
	ret := fmt.Sprintf("%d entries total\n\n", len(t.Entries))
	for i, e := range t.Entries {
		name, ok := timeStampNames[e.EntryID]
		if !ok {
			name = "<unknown>"
		}
		ret += fmt.Sprintf("%4d:%-30s %s", e.EntryID, name, formatElapsedTime(e.EntryStamp, t.FrequencyMHZ))
		if i != 0 {
			ret += fmt.Sprintf(" (%s)", formatElapsedTime(e.EntryStamp-t.Entries[i-1].EntryStamp, t.FrequencyMHZ))
		}
		ret += "\n"
	}
	return ret
}

func getFrequency() uint32 {
	/* On non-x86 platforms the timestamp entries are in usecs */
	if runtime.GOARCH != "386" && runtime.GOARCH != "amd64" {
		return 1
	}

	cpuf, err := os.Open("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq")
	if err != nil {
		return 0
	}

	freq := uint64(0)
	fmt.Fscanf(cpuf, "%d", &freq)
	return uint32(freq / 1000)
}

func (p parsedTables) GetVersion() (string, error) {
	str, ok := p.typeMap[TagVersion]
	if !ok {
		return "", fmt.Errorf("no coreboot version")
	}
	s := string(str)
	idx := strings.Index(s, "\000")
	if idx >= 0 {
		s = s[0:idx]
	}
	return s, nil
}

func (p parsedTables) GetVersionTimestamp() (time.Time, error) {
	raw, ok := p.typeMap[TagVersionTimestamp]
	if !ok {
		return time.Time{}, fmt.Errorf("no coreboot version timestamp")
	}
	ts := uint32(0)
	err := binary.Read(bytes.NewReader(raw), binary.LittleEndian, &ts)
	if err != nil {
		return time.Time{}, err
	}
	return time.Unix(int64(ts), 0), nil
}

func (p parsedTables) GetTimestamps() (*TimeStamps, error) {
	addr := uint64(0)
	addrRaw, ok := p.typeMap[TagTimestamps]
	if !ok {
		return nil, fmt.Errorf("no coreboot console")
	}
	err := binary.Read(bytes.NewReader(addrRaw), binary.LittleEndian, &addr)
	if err != nil {
		return nil, err
	}
	mem := p.mem
	_, err = mem.Seek(int64(addr), 0)
	if err != nil {
		return nil, err
	}
	var head TimeStampHeader
	err = binary.Read(mem, binary.LittleEndian, &head)
	if err != nil {
		return nil, err
	}

	entries := make([]TimeStampEntry, head.NumEntries, head.NumEntries)
	err = binary.Read(mem, binary.LittleEndian, &entries)
	if err != nil {
		return nil, err
	}

	return &TimeStamps{Head: head, Entries: entries, FrequencyMHZ: getFrequency()}, nil
}

func (p parsedTables) GetConsole() (console []byte, lost uint32, err error) {
	addr := uint64(0)
	addrRaw, ok := p.typeMap[TagConsole]
	if !ok {
		return nil, 0, fmt.Errorf("no coreboot console")
	}
	err = binary.Read(bytes.NewReader(addrRaw), binary.LittleEndian, &addr)
	if err != nil {
		return nil, 0, err
	}
	mem := p.mem
	_, err = mem.Seek(int64(addr), 0)
	if err != nil {
		return nil, 0, err
	}
	var consDesc CBMemConsole
	err = binary.Read(mem, binary.LittleEndian, &consDesc)
	if err != nil {
		return nil, 0, err
	}

	readSize := consDesc.Cursor
	lost = 0
	if readSize > consDesc.Size {
		lost = readSize - consDesc.Size
		readSize = consDesc.Size
	}

	cons := make([]byte, readSize, readSize)
	mem.Read(cons)
	if err != nil {
		return nil, 0, err
	}

	return cons, lost, nil
}

func IPChecksum(b []byte) uint16 {
	sum := uint32(0)
	/* Oh boy: coreboot really does is little-endian way.  */
	for i := 0; i < len(b); i += 2 {
		sum += uint32(b[i])
	}
	for i := 1; i < len(b); i += 2 {
		sum += uint32(b[i]) << 8
	}

	sum = (sum >> 16) + (sum & 0xffff)
	sum += (sum >> 16)
	return uint16(^sum & 0xffff)
}

func readFromBase(mem *os.File, base uint64) ([]byte, error) {
	_, err := mem.Seek(int64(base), 0)
	if err != nil {
		return nil, err
	}
	var headRaw [HeaderSize]byte
	var head Header
	_, err = mem.Read(headRaw[:])
	if err != nil {
		return nil, err
	}

	err = binary.Read(bytes.NewReader(headRaw[:]), binary.LittleEndian, &head)
	if err != nil {
		return nil, err
	}
	if bytes.Compare(head.Signature[:], headerSignature[:]) != 0 || head.HeaderBytes == 0 {
		return nil, nil
	}
	if IPChecksum(headRaw[:]) != 0 {
		return nil, nil
	}
	table := make([]byte, head.TableBytes, head.TableBytes)
	_, err = mem.Seek(int64(base)+int64(head.HeaderBytes), 0)
	if err != nil {
		return nil, err
	}
	_, err = mem.Read(table)
	if err != nil {
		return nil, err
	}

	if uint32(IPChecksum(table)) != head.TableChecksum {
		return nil, nil
	}
	return table, nil
}

func scanFromBase(mem *os.File, base uint64) ([]byte, error) {
	for i := uint64(0); i < 0x1000; i += 0x10 {
		b, err := readFromBase(mem, base+i)
		if err != nil {
			return nil, err
		}
		if b != nil {
			return b, nil
		}
	}
	return nil, fmt.Errorf("no coreboot table found")
}

func readTables(mem *os.File) ([]byte, error) {
	switch runtime.GOARCH {
	case "arm":
		dt, err := os.Open("/proc/device-tree/firmware/coreboot/coreboot-table")
		defer dt.Close()
		if err != nil {
			return nil, err
		}
		var base uint32
		err = binary.Read(dt, binary.BigEndian, &base)
		if err != nil {
			return nil, err
		}
		return scanFromBase(mem, uint64(base))
	case "386", "amd64":
		tbl, err := scanFromBase(mem, 0)
		if err == nil {
			return tbl, nil
		}
		return scanFromBase(mem, 0xf0000)
	default:
		return nil, fmt.Errorf("unsuppurted arch: %s", runtime.GOARCH)
	}
}

func parseTables(mem *os.File, raw []byte) (p parsedTables, err error) {
	reader := bytes.NewBuffer(raw)
	p.typeMap = map[uint32][]byte{}
	for {
		record := Record{}
		err = binary.Read(reader, binary.LittleEndian, &record)
		if err == io.EOF {
			p.mem = mem
			return p, nil
		}
		if err != nil {
			return p, err
		}
		payload := make([]byte, record.Size-8, record.Size-8)
		reader.Read(payload)
		p.raw = append(p.raw, rawTable{record: record, payload: payload})
		p.typeMap[record.Tag] = payload
		if record.Tag == TagForward {
			base := uint64(0)
			err = binary.Read(bytes.NewBuffer(payload), binary.LittleEndian, &base)
			if err != nil {
				return p, err
			}
			raw, err := readFromBase(mem, base)
			if err != nil {
				return p, err
			}
			if raw == nil {
				return p, fmt.Errorf("no coreboot table found")
			}
			reader = bytes.NewBuffer(raw)
		}
	}
}

func Open() (reader CBTablesReader, err error) {
	mem, err := os.Open("/dev/mem")
	if err != nil {
		return nil, err
	}

	tables, err := readTables(mem)
	if err != nil {
		return nil, err
	}

	return parseTables(mem, tables)
}
