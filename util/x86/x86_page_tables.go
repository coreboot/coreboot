/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

package main

import "bufio"
import "encoding/binary"
import "encoding/csv"
import "flag"
import "fmt"
import "io"
import "log"
import "os"
import "sort"
import "strconv"
import "strings"

// This program generates 32-bit PAE page tables based on a CSV input file.
// By default each PDPTE entry is allocated a PD page such that it's easy
// fault in new entries that are 2MiB aligned and size.

var iomapFilePtr = flag.String("iomap_file", "", "CSV file detailing page table mapping")
var ptCFilePtr = flag.String("pt_output_c_file", "", "File to write page tables to in C code")
var ptBinFilePtr = flag.String("pt_output_bin_file", "", "File to write page tables to in binary")
var pdptCFilePtr = flag.String("pdpt_output_c_file", "", "File to write PDPT to in C code")
var pdptBinFilePtr = flag.String("pdpt_output_bin_file", "", "File to write PDPT to in binary")
var pagesBaseAddress = flag.Uint64("metadata_base_address", BASE_ADDR, "Physical base address where metadata pages allocated from")

var generatedCodeLicense string =
`/*
 * Copyright 2018 Generated Code
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ` + "``" + `AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
`

const (
	PAT_UC      = 0
	PAT_WC      = 1
	PAT_WT      = 4
	PAT_WP      = 5
	PAT_WB      = 6
	PAT_UCMINUS = 7

	COMMENT_CHAR = '#'

	NUM_PDPTE = 4
	NUM_PDE   = 512
	NUM_PTE   = 512

	SIZE_4KiB = uint64(1 << 12)
	MASK_4KiB = SIZE_4KiB - 1
	SIZE_2MiB = uint64(1 << 21)
	MASK_2MiB = SIZE_2MiB - 1

	// This is a fake physical address for doing fixups when loading
	// the page tables. There's room for 4096 4KiB physical PD or PTE
	// tables. Anything with the present bit set will be pointing to an
	// offset based on this address. At runtime the entries will be fixed up
	BASE_ADDR = uint64(0xaa000000)

	// Size of PD and PT structures
	METADATA_TABLE_SIZE = 4096

	PDPTE_PRES = uint64(1 << 0)
	PDPTE_PWT  = uint64(1 << 3)
	PDPTE_PCD  = uint64(1 << 4)

	PDE_PRES = uint64(1 << 0)
	PDE_RW   = uint64(1 << 1)
	PDE_US   = uint64(1 << 2)
	PDE_PWT  = uint64(1 << 3)
	PDE_PCD  = uint64(1 << 4)
	PDE_A    = uint64(1 << 5)
	PDE_D    = uint64(1 << 6) // only valid with PS=1
	PDE_PS   = uint64(1 << 7)
	PDE_G    = uint64(1 << 8)  // only valid with PS=1
	PDE_PAT  = uint64(1 << 12) // only valid with PS=1
	PDE_XD   = uint64(1 << 63)

	PTE_PRES = uint64(1 << 0)
	PTE_RW   = uint64(1 << 1)
	PTE_US   = uint64(1 << 2)
	PTE_PWT  = uint64(1 << 3)
	PTE_PCD  = uint64(1 << 4)
	PTE_A    = uint64(1 << 5)
	PTE_D    = uint64(1 << 6)
	PTE_PAT  = uint64(1 << 7)
	PTE_G    = uint64(1 << 8)
	PTE_XD   = uint64(1 << 63)

	PDPTE_IDX_SHIFT = 30
	PDPTE_IDX_MASK  = 0x3

	PDE_IDX_SHIFT = 21
	PDE_IDX_MASK  = 0x1ff

	PTE_IDX_SHIFT = 12
	PTE_IDX_MASK  = 0x1ff
)

// Different 'writers' implement this interface.
type pageTableEntryWriter interface {
	WritePageEntry(data interface{}) error
}

// The full page objects, page directories and page tables, implement this
// interface to write their entire contents out
type pageTableWriter interface {
	WritePage(wr pageTableEntryWriter) error
}

type binaryWriter struct {
	wr io.Writer
}

func (bw *binaryWriter) WritePageEntry(data interface{}) error {
	return binary.Write(bw.wr, binary.LittleEndian, data)
}

type cWriter struct {
	name         string
	wr           io.Writer
	totalEntries uint
	currentIndex uint
}

func newCWriter(wr io.Writer, name string, nr_entries uint) *cWriter {
	cw := &cWriter{wr: wr, name: name, totalEntries: nr_entries}
	return cw
}

func (cw *cWriter) WritePageEntry(data interface{}) error {
	var entry uint64
	doPrint := false

	entry, ok := data.(uint64)
	if !ok {
		return fmt.Errorf("entry not uint64 %T", data)
	}

	if cw.currentIndex == 0 {
		if _, err := fmt.Fprint(cw.wr, generatedCodeLicense); err != nil {
		return err
	}
		includes := []string{
			"stdint.h",
		}
		for _, l := range includes {
			if _, err := fmt.Fprintf(cw.wr, "#include <%s>\n", l); err != nil {
				return err
			}
		}

		if _, err := fmt.Fprintf(cw.wr, "uint64_t %s[] = {\n", cw.name); err != nil {
			return err
		}
	}

	if cw.currentIndex%NUM_PTE == 0 {
		doPrint = true
		page_num := cw.currentIndex / NUM_PTE
		if _, err := fmt.Fprintf(cw.wr, "\t/* Page %d */\n", page_num); err != nil {
			return err
		}
	}

	// filter out 0 entries
	if entry != 0 || doPrint {
		_, err := fmt.Fprintf(cw.wr, "\t[%d] = %#016xULL,\n", cw.currentIndex, entry)
		if err != nil {
			return err
		}
	}

	cw.currentIndex += 1

	if cw.currentIndex == cw.totalEntries {
		if _, err := fmt.Fprintln(cw.wr, "};"); err != nil {
			return err
		}
	}

	return nil
}

// This map represents what the IA32_PAT MSR should be at runtime. The indicies
// are what the linux kernel uses. Reserved entries are not used.
//  0    WB : _PAGE_CACHE_MODE_WB
//  1    WC : _PAGE_CACHE_MODE_WC
//  2    UC-: _PAGE_CACHE_MODE_UC_MINUS
//  3    UC : _PAGE_CACHE_MODE_UC
//  4    WB : Reserved
//  5    WP : _PAGE_CACHE_MODE_WP
//  6    UC-: Reserved
//  7    WT : _PAGE_CACHE_MODE_WT
// In order to use WP and WC then the IA32_PAT MSR needs to be updated
// as these are not the power on reset values.
var patMsrIndexByType = map[uint]uint{
	PAT_WB:      0,
	PAT_WC:      1,
	PAT_UCMINUS: 2,
	PAT_UC:      3,
	PAT_WP:      5,
	PAT_WT:      7,
}

type addressRange struct {
	begin uint64
	end   uint64
	pat   uint
	nx    bool
}

type addrRangeMerge func(a, b *addressRange) bool

func (ar *addressRange) Size() uint64 {
	return ar.end - ar.begin
}

func (ar *addressRange) Base() uint64 {
	return ar.begin
}

func (ar *addressRange) Pat() uint {
	return ar.pat
}

func (ar *addressRange) Nx() bool {
	return ar.nx
}

func (ar *addressRange) String() string {
	var nx string
	if ar.nx {
		nx = "NX"
	} else {
		nx = "  "
	}
	return fmt.Sprintf("%016x -- %016x %s %s", ar.begin, ar.end, patTypeToString(ar.pat), nx)
}

type pageTableEntry struct {
	physAddr uint64
	flags    uint64
}

func (pte *pageTableEntry) Encode() uint64 {
	return pte.physAddr | pte.flags
}

func ptePatFlags(base uint64, pat uint) uint64 {
	idx, ok := patMsrIndexByType[pat]
	patStr, _ := patTypesToString[pat]

	if !ok {
		log.Fatalf("Invalid pat entry for page %x: %s\n", base, patStr)
	}

	switch idx {
	case 0:
		return 0
	case 1:
		return PTE_PWT
	case 2:
		return PTE_PCD
	case 3:
		return PTE_PCD | PTE_PWT
	case 4:
		return PTE_PAT
	case 5:
		return PTE_PAT | PTE_PWT
	case 6:
		return PTE_PAT | PTE_PCD
	case 7:
		return PTE_PAT | PTE_PCD | PTE_PWT
	}

	log.Fatalf("Invalid PAT index %d for PTE %x %s\n", idx, base, patStr)
	return 0
}

func (pte *pageTableEntry) SetMapping(base uint64, pat uint, nx bool) {
	// Present and accessed
	pte.flags |= PTE_PRES | PTE_A

	// Non write protected entries mark as writable and dirty
	if pat != PAT_WP {
		pte.flags |= PTE_RW
		pte.flags |= PTE_D
	}

	if nx {
		pte.flags |= PTE_XD
	}

	pte.flags |= ptePatFlags(base, pat)
	pte.physAddr = base
}

type pageTable struct {
	ptes [NUM_PTE]pageTableEntry
}

func (pt *pageTable) WritePage(wr pageTableEntryWriter) error {
	for i := range pt.ptes {
		pte := &pt.ptes[i]
		err := wr.WritePageEntry(pte.Encode())
		if err != nil {
			return err
		}
	}
	return nil
}

type pageDirectoryEntry struct {
	physAddr uint64
	flags    uint64
	pt       *pageTable
}

func (pde *pageDirectoryEntry) Encode() uint64 {
	return pde.physAddr | pde.flags
}

func pdeTablePatFlags(pat uint) uint64 {
	idx, ok := patMsrIndexByType[pat]
	patStr, _ := patTypesToString[pat]

	if !ok || idx >= 4 {
		log.Fatalf("Invalid pat entry for PDE page table %s\n", patStr)
	}

	switch idx {
	case 0:
		return 0
	case 1:
		return PDE_PWT
	case 2:
		return PDE_PCD
	case 3:
		return PDE_PCD | PDE_PWT
	}

	log.Fatalf("Invalid PAT index %d for PDE page table %s\n", idx, patStr)
	return 0
}

func pdeLargePatFlags(base uint64, pat uint) uint64 {
	idx, ok := patMsrIndexByType[pat]
	patStr, _ := patTypesToString[pat]

	if !ok {
		log.Fatalf("Invalid pat entry for large page %x: %s\n", base, patStr)
	}

	switch idx {
	case 0:
		return 0
	case 1:
		return PDE_PWT
	case 2:
		return PDE_PCD
	case 3:
		return PDE_PCD | PDE_PWT
	case 4:
		return PDE_PAT
	case 5:
		return PDE_PAT | PDE_PWT
	case 6:
		return PDE_PAT | PDE_PCD
	case 7:
		return PDE_PAT | PDE_PCD | PDE_PWT
	}

	log.Fatalf("Invalid PAT index %d for PDE %x %s\n", idx, base, patStr)
	return 0
}

func (pde *pageDirectoryEntry) SetPageTable(pt_addr uint64, pat uint) {
	// Set writable for whole region covered by page table. Individual
	// ptes will have the correct writability flags
	pde.flags |= PDE_PRES | PDE_A | PDE_RW

	pde.flags |= pdeTablePatFlags(pat)

	pde.physAddr = pt_addr
}

func (pde *pageDirectoryEntry) SetMapping(base uint64, pat uint, nx bool) {
	// Present, accessed, and large
	pde.flags |= PDE_PRES | PDE_A | PDE_PS

	// Non write protected entries mark as writable and dirty
	if pat != PAT_WP {
		pde.flags |= PDE_RW
		pde.flags |= PDE_D
	}

	if nx {
		pde.flags |= PDE_XD
	}

	pde.flags |= pdeLargePatFlags(base, pat)
	pde.physAddr = base
}

type pageDirectory struct {
	pdes [NUM_PDE]pageDirectoryEntry
}

func (pd *pageDirectory) WritePage(wr pageTableEntryWriter) error {
	for i := range pd.pdes {
		pde := &pd.pdes[i]
		err := wr.WritePageEntry(pde.Encode())
		if err != nil {
			return nil
		}
	}
	return nil
}

type pageDirectoryPointerEntry struct {
	physAddr uint64
	flags    uint64
	pd       *pageDirectory
}

func (pdpte *pageDirectoryPointerEntry) Encode() uint64 {
	return pdpte.physAddr | pdpte.flags
}

func (pdpte *pageDirectoryPointerEntry) Init(addr uint64, pat uint) {
	idx, ok := patMsrIndexByType[pat]

	// Only 2 bits worth of PAT indexing in PDPTE
	if !ok || idx >= 4 {
		patStr, _ := patTypesToString[pat]
		log.Fatalf("Can't use type '%s' as PDPTE type.\n", patStr)
	}

	pdpte.physAddr = addr
	pdpte.flags = PDPTE_PRES

	switch idx {
	case 0:
		pdpte.flags |= 0
	case 1:
		pdpte.flags |= PDPTE_PWT
	case 2:
		pdpte.flags |= PDPTE_PCD
	case 3:
		pdpte.flags |= PDPTE_PCD | PDPTE_PWT
	default:
		log.Fatalf("Invalid PAT index %d for PDPTE\n", idx)
	}
}

type addressSpace struct {
	ranges            []*addressRange
	mergeFunc         addrRangeMerge
	metatdataBaseAddr uint64
	pdptes            [NUM_PDPTE]pageDirectoryPointerEntry
	numMetaPages      uint
	page_writers      []pageTableWriter
}

func (as *addressSpace) newPage(pw pageTableWriter) uint64 {
	v := as.metatdataBaseAddr + METADATA_TABLE_SIZE*uint64(as.numMetaPages)
	as.numMetaPages += 1
	as.page_writers = append(as.page_writers, pw)
	return v
}

func newAddrSpace(mergeFunc addrRangeMerge, metatdataBaseAddr uint64) *addressSpace {
	as := &addressSpace{mergeFunc: mergeFunc, metatdataBaseAddr: metatdataBaseAddr}
	// Fill in all PDPTEs
	for i := range as.pdptes {
		pdpte := &as.pdptes[i]
		pdpte.pd = &pageDirectory{}
		// fetch paging structures as WB
		pdpte.Init(as.newPage(pdpte.pd), PAT_WB)
	}
	return as
}

func (as *addressSpace) deleteEntries(indicies []int) {
	// deletions need to be processed in reverse order so as not
	// delete the wrong entries
	sort.Sort(sort.Reverse(sort.IntSlice(indicies)))
	for _, i := range indicies {
		as.ranges = append(as.ranges[:i], as.ranges[i+1:]...)
	}
}

func (as *addressSpace) mergeRanges() {
	var toRemove []int
	var prev *addressRange

	for i, cur := range as.ranges {
		if prev == nil {
			prev = cur
			continue
		}

		// merge previous with current
		if as.mergeFunc(prev, cur) {
			prev.end = cur.end
			toRemove = append(toRemove, i)
			cur = prev
		}
		prev = cur
	}

	as.deleteEntries(toRemove)
}

type addressRangeSlice []*addressRange

func (p addressRangeSlice) Len() int {
	return len(p)
}

func (p addressRangeSlice) Less(i, j int) bool {
	return !p[i].After(p[j])
}

func (p addressRangeSlice) Swap(i, j int) {
	p[i], p[j] = p[j], p[i]
}

func (as *addressSpace) insertRange(r *addressRange) {
	as.ranges = append(as.ranges, r)
	sort.Sort(addressRangeSlice(as.ranges))
}

// Remove complete entries or trim existing ones
func (as *addressSpace) trimRanges(r *addressRange) {
	var toRemove []int

	// First remove all entries that are completely overlapped
	for i, cur := range as.ranges {
		if r.FullyOverlaps(cur) {
			toRemove = append(toRemove, i)
			continue
		}
	}
	as.deleteEntries(toRemove)

	var ar *addressRange

	// Process partial overlaps
	for _, cur := range as.ranges {
		// Overlapping may be at beginning, middle, end. Only the
		// middle overlap needs to create a new range since the
		// beginning and end overlap can just adjust the current
		// range.
		if r.Overlaps(cur) {

			// beginning overlap
			if r.begin <= cur.begin {
				cur.begin = r.end
				continue
			}

			// end overlap
			if r.end >= cur.end {
				cur.end = r.begin
				continue
			}

			// middle overlap. create new entry from the hole
			// punched in the current entry. There's nothing
			// further to do after this
			begin := r.end
			end := cur.end
			pat := cur.pat
			nx := cur.nx

			// current needs new ending
			cur.end = r.begin

			ar = newAddrRange(begin, end, pat, nx)

			break
		}
	}

	if ar != nil {
		as.insertRange(ar)
	}
}

func (as *addressSpace) PrintEntries() {
	for _, cur := range as.ranges {
		log.Println(cur)
	}
}

func (as *addressSpace) AddRange(r *addressRange) {
	as.trimRanges(r)
	as.insertRange(r)
	as.mergeRanges()
}

func (as *addressSpace) insertMapping(base uint64, size uint64, pat uint, nx bool) {
	pdpteIndex := (base >> PDPTE_IDX_SHIFT) & PDPTE_IDX_MASK
	pdeIndex := (base >> PDE_IDX_SHIFT) & PDE_IDX_MASK
	pteIndex := (base >> PTE_IDX_SHIFT) & PTE_IDX_MASK

	pd := as.pdptes[pdpteIndex].pd
	pde := &pd.pdes[pdeIndex]

	if size == SIZE_2MiB {
		pde.SetMapping(base, pat, nx)
		return
	}

	if pde.pt == nil {
		pde.pt = &pageTable{}
		// Fetch paging structures as WB
		pde.SetPageTable(as.newPage(pde.pt), PAT_WB)
	}

	pte := &pde.pt.ptes[pteIndex]
	pte.SetMapping(base, pat, nx)
}

func (as *addressSpace) CreatePageTables() {
	var size uint64
	var base uint64

	for _, r := range as.ranges {
		size = r.Size()
		base = r.Base()
		pat := r.Pat()
		nx := r.Nx()

		numSmallEntries := 0
		numBigEntries := 0

		for size != 0 {
			mappingSize := SIZE_4KiB

			if (base&MASK_2MiB) == 0 && size >= SIZE_2MiB {
				mappingSize = SIZE_2MiB
				numBigEntries += 1
			} else {
				numSmallEntries += 1
			}

			as.insertMapping(base, mappingSize, pat, nx)

			base += mappingSize
			size -= mappingSize

		}

		log.Printf("%s : %d big %d small\n", r, numBigEntries, numSmallEntries)
	}
}

func (as *addressSpace) PageTableSize() uint {
	return as.numMetaPages * METADATA_TABLE_SIZE
}

func (as *addressSpace) NumPages() uint {
	return as.numMetaPages
}

func (as *addressSpace) WritePageTable(ptew pageTableEntryWriter) error {
	for _, pw := range as.page_writers {
		err := pw.WritePage(ptew)
		if err != nil {
			return err
		}
	}

	return nil
}

func (as *addressSpace) WritePageDirectoryPointerTable(ptew pageTableEntryWriter) error {
	for i := range as.pdptes {
		err := ptew.WritePageEntry(as.pdptes[i].Encode())
		if err != nil {
			return err
		}
	}

	return nil
}

var pat_types_from_str = map[string]uint{
	"UC":  PAT_UC,
	"WC":  PAT_WC,
	"WT":  PAT_WT,
	"WP":  PAT_WP,
	"WB":  PAT_WB,
	"UC-": PAT_UCMINUS,
}

var patTypesToString = map[uint]string{
	PAT_UC:      "UC",
	PAT_WC:      "WC",
	PAT_WT:      "WT",
	PAT_WP:      "WP",
	PAT_WB:      "WB",
	PAT_UCMINUS: "UC-",
}

func openCsvFile(file string) (*csv.Reader, error) {
	f, err := os.Open(file)

	if err != nil {
		return nil, err
	}

	csvr := csv.NewReader(f)
	csvr.Comment = COMMENT_CHAR
	csvr.TrimLeadingSpace = true
	return csvr, nil
}

// After returns true if ar beings at or after other.end.
func (ar addressRange) After(other *addressRange) bool {
	return ar.begin >= other.end
}

func (ar addressRange) FullyOverlaps(other *addressRange) bool {
	return ar.begin <= other.begin && ar.end >= other.end
}

func (ar addressRange) Overlaps(other *addressRange) bool {
	if other.end <= ar.begin || other.begin >= ar.end {
		return false
	}
	return true
}

func MergeByPat(a, b *addressRange) bool {
	// 'b' is assumed to be following 'a'
	if a.end != b.begin {
		return false
	}

	if a.pat != b.pat {
		return false
	}

	return true
}

func MergeByNx(a, b *addressRange) bool {
	// 'b' is assumed to be following 'a'
	if a.end != b.begin {
		return false
	}

	if a.nx != b.nx {
		return false
	}

	return true
}

func MergeByPatNx(a, b *addressRange) bool {
	return MergeByPat(a, b) && MergeByNx(a, b)
}

func hexNumber(s string) (uint64, error) {
	return strconv.ParseUint(strings.TrimSpace(s), 0, 0)
}

func patTypeToString(pat uint) string {
	return patTypesToString[pat]
}

func patTypeFromString(s string) (uint, error) {
	s1 := strings.TrimSpace(s)
	v, ok := pat_types_from_str[s1]

	if !ok {
		return 0, fmt.Errorf("No PAT type '%s'", s1)
	}

	return v, nil
}

func removeComment(field, comment string) string {
	str_slice := strings.Split(field, comment)
	return strings.TrimSpace(str_slice[0])
}

func newAddrRange(begin, end uint64, pat uint, nx bool) *addressRange {
	return &addressRange{begin: begin, end: end, pat: pat, nx: nx}
}

func readRecords(csvr *csv.Reader, as *addressSpace) {
	i := 0
	for true {
		fields, err := csvr.Read()
		i++

		if err == io.EOF {
			break
		}

		if err != nil {
			log.Fatal(err)
		}

		if len(fields) < 3 {
			log.Fatal("Need at least 3 fields: begin, end, PAT\n")
		}

		begin, err := hexNumber(fields[0])

		if err != nil {
			log.Fatal(err)
		}

		end, err := hexNumber(fields[1])

		if err != nil {
			log.Fatal(err)
		}

		if begin&MASK_4KiB != 0 {
			log.Fatalf("begin %x must be at least 4KiB aligned\n", begin)
		}

		if end&MASK_4KiB != 0 {
			log.Fatalf("end %x must be at least 4KiB aligned\n", end)
		}
		if begin >= end {
			log.Fatalf("%x must be < %x at record %d\n", begin, end, i)
		}

		pat, err := patTypeFromString(fields[2])

		if err != nil {
			log.Fatal(err)
		}

		var nx bool = false

		if len(fields) > 3 && len(removeComment(fields[3], string(COMMENT_CHAR))) > 0 {
			nx = true
		}

		as.AddRange(newAddrRange(begin, end, pat, nx))
	}
}

func main() {
	log.SetFlags(0)
	flag.Parse()
	var ptWriters []pageTableEntryWriter
	var pdptWriters []pageTableEntryWriter

	if *iomapFilePtr == "" {
		log.Fatal("No iomap_file provided.\n")
	}

	csvr, err := openCsvFile(*iomapFilePtr)
	if err != nil {
		log.Fatal(err)
	}

	as := newAddrSpace(MergeByPatNx, *pagesBaseAddress)
	readRecords(csvr, as)

	log.Println("Merged address space:")
	as.CreatePageTables()
	log.Println()
	log.Printf("Total Pages of page tables: %d\n", as.NumPages())
	log.Println()
	log.Printf("Pages linked using base address of %#x.\n", *pagesBaseAddress)

	if *ptCFilePtr != "" {
		f, err := os.Create(*ptCFilePtr)
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		bwr := bufio.NewWriter(f)
		defer bwr.Flush()
		cw := newCWriter(bwr, "page_tables", as.NumPages()*NUM_PTE)
		ptWriters = append(ptWriters, cw)
	}

	if *ptBinFilePtr != "" {
		f, err := os.Create(*ptBinFilePtr)
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		bwr := bufio.NewWriter(f)
		defer bwr.Flush()
		bw := &binaryWriter{wr: bwr}
		ptWriters = append(ptWriters, bw)
	}

	if *pdptCFilePtr != "" {
		f, err := os.Create(*pdptCFilePtr)
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		bwr := bufio.NewWriter(f)
		defer bwr.Flush()
		cw := newCWriter(bwr, "pdptes", NUM_PDPTE)
		pdptWriters = append(pdptWriters, cw)
	}

	if *pdptBinFilePtr != "" {
		f, err := os.Create(*pdptBinFilePtr)
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		bwr := bufio.NewWriter(f)
		defer bwr.Flush()
		bw := &binaryWriter{wr: bwr}
		pdptWriters = append(pdptWriters, bw)
	}

	// Write out page tables
	for _, w := range ptWriters {
		err = as.WritePageTable(w)
		if err != nil {
			log.Fatal(err)
		}
	}

	// Write out pdptes
	for _, w := range pdptWriters {
		err = as.WritePageDirectoryPointerTable(w)
		if err != nil {
			log.Fatal(err)
		}
	}
}
