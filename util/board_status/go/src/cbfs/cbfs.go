package cbfs

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"os"
	"sort"
	"strings"
	"text/tabwriter"
)

type CBFSReader interface {
	GetFile(name string) ([]byte, error)
	ListFiles() ([]string, error)
}

type ArchType uint32
type FileType uint32

type CBFSHeader struct {
	Magic         uint32
	Version       uint32
	ROMSize       uint32
	BootBlockSize uint32
	Align         uint32
	Offset        uint32
	Architecture  ArchType
	Pad           [1]uint32
}

func (a ArchType) String() string {
	switch a {
	case 0xFFFFFFFF:
		return "unknown"
	case 0x00000001:
		return "x86"
	case 0x00000010:
		return "arm"
	default:
		return fmt.Sprintf("0x%x", a)
	}
}

func (f FileType) String() string {
	switch f {
	case 0xffffffff:
		return "null"
	case 0x10:
		return "stage"
	case 0x20:
		return "payload"
	case 0x30:
		return "optionrom"
	case 0x40:
		return "bootsplash"
	case 0x50:
		return "raw"
	case 0x51:
		return "vsa"
	case 0x52:
		return "mbi"
	case 0x53:
		return "microcode"
	case 0xaa:
		return "cmos_default"
	case 0x1aa:
		return "cmos_layout"
	default:
		return fmt.Sprintf("0x%x", uint32(f))
	}
}

func (c CBFSHeader) String() (ret string) {
	ret = fmt.Sprintf("bootblocksize: %d\n", c.BootBlockSize)
	ret += fmt.Sprintf("romsize: %d\n", c.ROMSize)
	ret += fmt.Sprintf("offset: 0x%x\n", c.Offset)
	ret += fmt.Sprintf("alignment: %d bytes\n", c.Align)
	ret += fmt.Sprintf("architecture: %v\n", c.Architecture)
	ret += fmt.Sprintf("version: 0x%x\n", c.Version)
	return ret
}

const sizeofFileHeader = 24
const CBFSHeaderMagic = 0x4F524243

type CBFSFileHeader struct {
	Magic    [8]byte
	Len      uint32
	Type     FileType
	CheckSum uint32
	Offset   uint32
}

type cBFSFile struct {
	headerOffset uint64
	header       CBFSFileHeader
	name         string
}

type cBFSDesc struct {
	file      *os.File
	end       uint64
	headerPos uint64
	rOMStart  uint64
	fileNames map[string]cBFSFile
	files     []cBFSFile
	header    CBFSHeader
}

func (c cBFSDesc) align(offset uint32) uint32 {
	a := uint32(c.header.Align)
	return (a + offset - 1) & ^(a - 1)
}

func (c cBFSDesc) ListFiles() (files []string, err error) {
	for name, _ := range c.fileNames {
		files = append(files, name)
	}
	sort.Strings(files)
	return files, nil
}

func (c cBFSDesc) GetFile(name string) ([]byte, error) {
	file, ok := c.fileNames[name]
	if !ok {
		return nil, fmt.Errorf("file not found: %s", name)
	}
	_, err := c.file.Seek(int64(file.headerOffset)+int64(file.header.Offset), 0)
	if err != nil {
		return nil, err
	}
	ret := make([]byte, file.header.Len, file.header.Len)
	r, err := c.file.Read(ret)
	if err != nil {
		return nil, err
	}
	if r != len(ret) {
		return nil, fmt.Errorf("incomplete read")
	}
	return ret, nil
}

func (c cBFSDesc) String() (ret string) {
	ret = c.header.String()
	ret += "\n"
	buf := bytes.NewBuffer([]byte{})
	w := new(tabwriter.Writer)
	w.Init(buf, 15, 0, 1, ' ', 0)
	fmt.Fprintln(w, "Name\tOffset\tType\tSize\t")
	for _, file := range c.files {
		name := file.name
		if file.header.Type == 0xffffffff {
			name = "(empty)"
		}
		fmt.Fprintf(w, "%s\t0x%x\t%v\t%d\t\n",
			name, file.headerOffset-c.rOMStart,
			file.header.Type, file.header.Len)
	}
	w.Flush()
	ret += buf.String()
	return ret
}

func openGeneric(cbfs *cBFSDesc) (CBFSReader, error) {
	_, err := cbfs.file.Seek(int64(cbfs.end-4), 0)
	if err != nil {
		return nil, err
	}
	headerPos := int32(0)
	binary.Read(cbfs.file, binary.LittleEndian, &headerPos)
	if headerPos < 0 {
		cbfs.headerPos = cbfs.end - uint64(-headerPos)
	} else {
		cbfs.headerPos = uint64(headerPos)
	}
	_, err = cbfs.file.Seek(int64(cbfs.headerPos), 0)
	if err != nil {
		return nil, err
	}
	err = binary.Read(cbfs.file, binary.BigEndian, &cbfs.header)
	if err != nil {
		return nil, err
	}
	if cbfs.header.Magic != CBFSHeaderMagic {
		return nil, fmt.Errorf("invalid header magic")
	}

	cbfs.fileNames = map[string]cBFSFile{}

	curptr := cbfs.end - uint64(cbfs.header.ROMSize) + uint64(cbfs.header.Offset)
	cbfs.rOMStart = cbfs.end - uint64(cbfs.header.ROMSize)
	for {
		file := cBFSFile{headerOffset: curptr}
		_, err = cbfs.file.Seek(int64(curptr), 0)
		if err != nil {
			return nil, err
		}
		err = binary.Read(cbfs.file, binary.BigEndian, &file.header)
		if err != nil {
			return nil, err
		}
		if string(file.header.Magic[:]) != "LARCHIVE" {
			return *cbfs, nil
		}
		name := make([]byte, file.header.Offset-sizeofFileHeader, file.header.Offset-sizeofFileHeader)
		_, err = cbfs.file.Read(name)
		if err != nil {
			return nil, err
		}
		nameStr := string(name)
		idx := strings.Index(nameStr, "\000")
		if idx >= 0 {
			nameStr = nameStr[0:idx]
		}
		file.name = nameStr
		cbfs.fileNames[nameStr] = file
		cbfs.files = append(cbfs.files, file)
		curptr += uint64(cbfs.align(file.header.Offset + file.header.Len))
	}
}

func OpenFile(file *os.File) (CBFSReader, error) {
	stat, err := file.Stat()
	if err != nil {
		return nil, err
	}
	cbfs := cBFSDesc{file: file, end: uint64(stat.Size())}
	return openGeneric(&cbfs)
}

func OpenROM() (CBFSReader, error) {
	file, err := os.Open("/dev/mem")
	if err != nil {
		return nil, err
	}
	cbfs := cBFSDesc{file: file, end: 0x100000000}
	return openGeneric(&cbfs)
}
