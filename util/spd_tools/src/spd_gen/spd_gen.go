/* SPDX-License-Identifier: GPL-2.0-or-later */
package main

import (
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"reflect"
	"regexp"
	"sort"
	"strings"
)

/* ------------------------------------------------------------------------------------------ */
/*                                     Program-defined types                                  */
/* ------------------------------------------------------------------------------------------ */
type memParts struct {
	MemParts []memPart `json:"parts"`
}

type memPart struct {
	Name    string
	Attribs interface{}
	SPDId   int
}

type memTech interface {
	/*
	 * Returns the set -> platform mapping for the memory technology. Platforms with the
	 * same SPD requirements should be grouped together into a single set.
	 */
	getSetMap() map[int][]int

	/*
	 * Takes the name and attributes of a part, as read from the memory_parts JSON file.
	 * Validates the attributes, returning an error if any attribute has an invalid value.
	 * Stores the name and attributes internally to be used later.
	 */
	addNewPart(string, interface{}) error

	/*
	 * Takes the name of a part and a set number.
	 * Retrieves the part's attributes which were stored by addNewPart(). Updates them by
	 * setting any optional attributes which weren't specified in the JSON file to their
	 * default values.
	 * Returns these updated attributes.
	 */
	getSPDAttribs(string, int) (interface{}, error)

	/*
	 * Returns the size of an SPD file for this memory technology.
	 */
	getSPDLen() int

	/*
	 * Takes an SPD byte index and the attributes of a part.
	 * Returns the value which that SPD byte should be set to based on the attributes.
	 */
	getSPDByte(int, interface{}) byte
}

/* ------------------------------------------------------------------------------------------ */
/*                                         Constants                                          */
/* ------------------------------------------------------------------------------------------ */

const (
	PlatformTGL = iota
	PlatformADL
	PlatformJSL
	PlatformPCO
	PlatformCZN
	PlatformMDN
	PlatformMTL
	PlatformPHX
	PlatformPTL
	PlatformMax
)

const (
	SPDManifestFileName      = "parts_spd_manifest.generated.txt"
	PlatformManifestFileName = "platforms_manifest.generated.txt"
)

/* ------------------------------------------------------------------------------------------ */
/*                                    Global variables                                        */
/* ------------------------------------------------------------------------------------------ */

var platformNames = map[int]string{
	PlatformTGL: "TGL",
	PlatformADL: "ADL",
	PlatformJSL: "JSL",
	PlatformPCO: "PCO",
	PlatformCZN: "CZN",
	PlatformMDN: "MDN",
	PlatformMTL: "MTL",
	PlatformPHX: "PHX",
	PlatformPTL: "PTL",
}

var memTechMap = map[string]memTech{
	"lp4x": lp4x{},
	"ddr4": ddr4{},
	"lp5":  lp5{},
}

/* ------------------------------------------------------------------------------------------ */
/*                                Conversion Helper Functions                                 */
/* ------------------------------------------------------------------------------------------ */

func convNsToPs(timeNs int) int {
	return timeNs * 1000
}

func convMtbToPs(mtb int) int {
	return mtb * 125
}

func convPsToMtb(timePs int) int {
	return divRoundUp(timePs, 125)
}

func convPsToMtbByte(timePs int) byte {
	return byte(convPsToMtb(timePs) & 0xff)
}

func convPsToFtbByte(timePs int) byte {
	mtb := convPsToMtb(timePs)
	ftb := timePs - convMtbToPs(mtb)

	return byte(ftb)
}

func convNsToMtb(timeNs int) int {
	return convPsToMtb(convNsToPs(timeNs))
}

func convNsToMtbByte(timeNs int) byte {
	return convPsToMtbByte(convNsToPs(timeNs))
}

func convNsToFtbByte(timeNs int) byte {
	return convPsToFtbByte(convNsToPs(timeNs))
}

func divRoundUp(dividend int, divisor int) int {
	return (dividend + divisor - 1) / divisor
}

/* ------------------------------------------------------------------------------------------ */
/*                                        Functions                                           */
/* ------------------------------------------------------------------------------------------ */

func findIndex(dedupedAttribs []interface{}, newSPDAttribs interface{}) int {
	for i := 0; i < len(dedupedAttribs); i++ {
		if reflect.DeepEqual(dedupedAttribs[i], newSPDAttribs) {
			return i
		}
	}

	return -1
}

// readJSONNoComments reads a JSON file and strips supported comment styles:
// - line comments: "// ..." (on their own line, with optional leading spaces)
// - block comments: "/* ... */"
func readJSONNoComments(path string) ([]byte, error) {
	dataBytes, err := ioutil.ReadFile(path)
	if err != nil {
		return nil, err
	}
	re := regexp.MustCompile(`(?m)^\s*//.*`)
	dataBytes = re.ReplaceAll(dataBytes, []byte(""))
	reBlock := regexp.MustCompile(`(?s)/\*.*?\*/`)
	dataBytes = reBlock.ReplaceAll(dataBytes, []byte(""))
	return dataBytes, nil
}

func readMemParts(memPartsFilePath string) (memParts, error) {
	var memParts memParts

	dataBytes, err := readJSONNoComments(memPartsFilePath)
	if err != nil {
		return memParts, err
	}

	if err := json.Unmarshal(dataBytes, &memParts); err != nil {
		return memParts, err
	}

	return memParts, nil
}

func readSingleAttributes(path string) (string, interface{}, error) {
	dataBytes, err := readJSONNoComments(path)
	if err != nil {
		return "", nil, err
	}

	var root interface{}
	if err := json.Unmarshal(dataBytes, &root); err != nil {
		return "", nil, err
	}

	m, ok := root.(map[string]interface{})
	if !ok {
		return "", nil, errors.New("expected JSON object at top-level")
	}

	baseName := ""
	{
		base := filepath.Base(path)
		if i := strings.LastIndex(base, "."); i >= 0 {
			base = base[:i]
		}
		baseName = base
	}

	// Accept either:
	//  1) {"name": "...", "attributes": {...}} (or "attribs")
	//  2) {"parts":[{"name":"...","attribs":{...}}]} (single-part memory_parts.json)
	if partsRaw, ok := m["parts"]; ok {
		parts, ok := partsRaw.([]interface{})
		if !ok || len(parts) == 0 {
			return "", nil, errors.New("expected non-empty 'parts' array")
		}
		if len(parts) != 1 {
			return "", nil, errors.New("expected exactly one part in 'parts' array")
		}
		p, ok := parts[0].(map[string]interface{})
		if !ok {
			return "", nil, errors.New("expected part entry to be a JSON object")
		}
		name, _ := p["name"].(string)
		if name == "" {
			name = baseName
		}
		if a, ok := p["attributes"]; ok {
			return name, a, nil
		}
		if a, ok := p["attribs"]; ok {
			return name, a, nil
		}
		return "", nil, errors.New("missing 'attribs'/'attributes' in part entry")
	}

	name, _ := m["name"].(string)
	if name == "" {
		name = baseName
	}
	if a, ok := m["attributes"]; ok {
		return name, a, nil
	}
	if a, ok := m["attribs"]; ok {
		return name, a, nil
	}

	return "", nil, errors.New("missing 'attribs'/'attributes' in JSON")
}

func createSPD(memAttribs interface{}, t memTech) string {
	var s string

	for i := 0; i < t.getSPDLen(); i++ {
		var b byte = 0
		if memAttribs != nil {
			b = t.getSPDByte(i, memAttribs)
		}

		if (i+1)%16 == 0 {
			s += fmt.Sprintf("%02X\n", b)
		} else {
			s += fmt.Sprintf("%02X ", b)
		}
	}

	return s
}

func writeSPD(memAttribs interface{}, SPDId int, SPDSetDirName string, t memTech) {
	s := createSPD(memAttribs, t)
	SPDFileName := fmt.Sprintf("spd-%d.hex", SPDId)
	ioutil.WriteFile(filepath.Join(SPDSetDirName, SPDFileName), []byte(s), 0644)
}

func writeEmptySPD(SPDSetDirName string, t memTech) {
	s := createSPD(nil, t)
	SPDFileName := "spd-empty.hex"
	ioutil.WriteFile(filepath.Join(SPDSetDirName, SPDFileName), []byte(s), 0644)
}

func getGeneratedString() string {
	return fmt.Sprintf("# Generated by:\n# %s\n\n", strings.Join(os.Args[0:], " "))
}

func writeSPDManifest(memPartArray []memPart, SPDSetDirName string) {
	var s string

	s += getGeneratedString()
	for i := 0; i < len(memPartArray); i++ {
		s += fmt.Sprintf("%s,spd-%d.hex\n", memPartArray[i].Name, memPartArray[i].SPDId)
	}

	ioutil.WriteFile(filepath.Join(SPDSetDirName, SPDManifestFileName), []byte(s), 0644)
}

func writeSetMap(setMap map[int][]int, SPDDirName string) {
	var s string

	s += getGeneratedString()

	var setNumbers []int
	for k, _ := range setMap {
		setNumbers = append(setNumbers, k)
	}
	sort.Ints(setNumbers)

	for _, num := range setNumbers {
		for _, item := range setMap[num] {
			s += fmt.Sprintf("%s,set-%d\n", platformNames[item], num)
		}
	}

	ioutil.WriteFile(filepath.Join(SPDDirName, PlatformManifestFileName), []byte(s), 0644)
}

func usage() {
	fmt.Printf("\nUsage: %s [options] <mem_parts_list_json> <mem_technology> [<output_dir>|<output_hex>]\n\n", os.Args[0])
	fmt.Printf("   where,\n")
	fmt.Printf("   mem_parts_list_json = JSON File containing list of memory parts and attributes\n")
	fmt.Printf("   mem_technology = Memory technology for which to generate SPDs\n")
	fmt.Printf("                    supported technologies: %v\n\n\n",
		reflect.ValueOf(memTechMap).MapKeys())
	fmt.Printf("Options:\n")
	fmt.Printf("   -out <output_dir>           Output directory (default: directory of JSON input)\n")
	fmt.Printf("   -set <n>                    Set number when writing a single output_hex (default: 0)\n\n")
}

func main() {
	fs := flag.NewFlagSet(os.Args[0], flag.ContinueOnError)
	fs.SetOutput(io.Discard)
	fs.Usage = usage

	outOpt := fs.String("out", "", "Output directory (default: directory of JSON input)")
	setOpt := fs.Int("set", 0, "Set number when writing a single output_hex (default: 0)")

	if err := fs.Parse(os.Args[1:]); err != nil {
		usage()
		os.Exit(2)
	}

	if *setOpt < 0 {
		log.Fatal("Invalid -set value: ", *setOpt)
	}

	varPos := fs.Args()
	if len(varPos) != 2 && len(varPos) != 3 {
		usage()
		log.Fatal("Incorrect number of arguments")
	}

	var t memTech
	memPartsFilePath, memTechnology := varPos[0], varPos[1]

	t, ok := memTechMap[strings.ToLower(memTechnology)]
	if !ok {
		log.Fatal("Unsupported memory technology ", memTechnology)
	}

	var SPDDir string
	fileOutput := false
	SPDOutFile := ""
	var err error
	if len(varPos) == 3 && strings.HasSuffix(strings.ToLower(varPos[2]), ".hex") {
		fileOutput = true
		SPDOutFile, err = filepath.Abs(varPos[2])
	} else if *outOpt != "" {
		SPDDir, err = filepath.Abs(*outOpt)
	} else if len(varPos) == 3 {
		SPDDir, err = filepath.Abs(varPos[2])
	} else {
		SPDDir, err = filepath.Abs(filepath.Dir(memPartsFilePath))
	}
	if err != nil {
		log.Fatal(err)
	}

	if fileOutput {
		name, attrs, err := readSingleAttributes(memPartsFilePath)
		if err != nil {
			log.Fatal(err)
		}
		if err := t.addNewPart(name, attrs); err != nil {
			log.Fatal(err)
		}
		spdAttribs, err := t.getSPDAttribs(name, *setOpt)
		if err != nil {
			log.Fatal(err)
		}
		s := createSPD(spdAttribs, t)
		if err := os.MkdirAll(filepath.Dir(SPDOutFile), os.ModePerm); err != nil {
			log.Fatal(err)
		}
		if err := ioutil.WriteFile(SPDOutFile, []byte(s), 0644); err != nil {
			log.Fatal(err)
		}
		return
	}

	memParts, err := readMemParts(memPartsFilePath)
	if err != nil {
		log.Fatal(err)
	}

	memPartExists := make(map[string]bool)
	for i := 0; i < len(memParts.MemParts); i++ {
		if memPartExists[memParts.MemParts[i].Name] {
			log.Fatalf("%s is duplicated in mem_parts_list_json", memParts.MemParts[i].Name)
		}
		memPartExists[memParts.MemParts[i].Name] = true

		if err := t.addNewPart(memParts.MemParts[i].Name, memParts.MemParts[i].Attribs); err != nil {
			log.Fatal(err)
		}
	}

	setMap := t.getSetMap()

	for i := 0; i < len(setMap); i++ {
		var dedupedAttribs []interface{}

		for j := 0; j < len(memParts.MemParts); j++ {
			spdAttribs, _ := t.getSPDAttribs(memParts.MemParts[j].Name, i)
			index := -1

			if index = findIndex(dedupedAttribs, spdAttribs); index == -1 {
				dedupedAttribs = append(dedupedAttribs, spdAttribs)
				index = len(dedupedAttribs) - 1
			}

			memParts.MemParts[j].SPDId = index + 1
		}

		SPDSetDir := fmt.Sprintf("%s/set-%d", SPDDir, i)
		os.MkdirAll(SPDSetDir, os.ModePerm)

		for j := 0; j < len(dedupedAttribs); j++ {
			writeSPD(dedupedAttribs[j], j+1, SPDSetDir, t)
		}

		writeEmptySPD(SPDSetDir, t)

		writeSPDManifest(memParts.MemParts, SPDSetDir)
	}

	writeSetMap(setMap, SPDDir)
}
