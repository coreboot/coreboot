/* SPDX-License-Identifier: GPL-2.0-or-later */

package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"reflect"
	"strconv"
	"strings"
)

/*
 * This program generates de-duplicated SPD files for LPDDR4x memory using the global memory
 * part list provided in CSV format. In addition to that, it also generates SPD manifest in CSV
 * format that contains entries of type (DRAM part name, SPD file name) which provides the SPD
 * file name used by a given DRAM part.
 *
 * It takes as input:
 * Pointer to directory where the generated SPD files will be placed.
 * JSON file containing a list of memory parts with their attributes as per datasheet.
 */
const (
	SPDManifestFileName = "lp4x_spd_manifest.generated.txt"

	PlatformTGLADL = 0
	PlatformJSL = 1
	PlatformCZN    = 2
)

var platformMap = map[string]int {
	"TGL": PlatformTGLADL,
	"JSL": PlatformJSL,
	"ADL": PlatformTGLADL,
	"CZN": PlatformCZN,
}

var currPlatform int

type memAttributes struct {
	/* Primary attributes - must be provided by JSON file for each part */
	DensityPerChannelGb int
	Banks int
	ChannelsPerDie int
	DiesPerPackage int
	BitWidthPerChannel int
	RanksPerChannel int
	SpeedMbps int

	/*
	 * All the following parameters are optional and required only if the part requires
	 * special parameters as per the datasheet.
	 */
	/* Timing parameters */
	TRFCABNs int
	TRFCPBNs int
	TRPABMinNs int
	TRPPBMinNs int
	TCKMinPs int
	TCKMaxPs int
	TAAMinPs int
	TRCDMinNs int

	/* CAS */
	CASLatencies string
	CASFirstByte byte
	CASSecondByte byte
	CASThirdByte byte
}

/* This encodes the density in Gb to SPD values as per JESD 21-C */
var densityGbToSPDEncoding = map[int]byte {
	4: 0x4,
	6: 0xb,
	8: 0x5,
	12: 0x8,
	16: 0x6,
	24: 0x9,
	32: 0x7,
}

/*
 * Table 3 from JESD209-4C.
 * Maps density per physical channel to row-column encoding as per JESD 21-C for a device with
 * x16 physical channel.
 */
var densityGbx16ChannelToRowColumnEncoding = map[int]byte {
	4: 0x19, /* 15 rows, 10 columns */
	6: 0x21, /* 16 rows, 10 columns */
	8: 0x21, /* 16 rows, 10 columns */
	12: 0x29, /* 17 rows, 10 columns */
	16: 0x29, /* 17 rows, 10 columns */
}

/*
 * Table 5 from JESD209-4C.
 * Maps density per physical channel to row-column encoding as per JESD 21-C for a device with
 * x8 physical channel.
 */
var densityGbx8ChannelToRowColumnEncoding = map[int]byte {
	3: 0x21, /* 16 rows, 10 columns */
	4: 0x21, /* 16 rows, 10 columns */
	6: 0x29, /* 17 rows, 10 columns */
	8: 0x29, /* 17 rows, 10 columns */
	12: 0x31, /* 18 rows, 10 columns */
	16: 0x31, /* 18 rows, 10 columns */
}

type refreshTimings struct {
	TRFCABNs int
	TRFCPBNs int
}

/*
 * Table 112 from JESD209-4C
 * Maps density per physical channel to refresh timings. This is the same for x8 and x16
 * devices.
 */
var densityGbPhysicalChannelToRefreshEncoding = map[int]refreshTimings {
	3: {
		TRFCABNs: 180,
		TRFCPBNs: 90,
	},
	4: {
		TRFCABNs: 180,
		TRFCPBNs: 90,
	},
	6: {
		TRFCABNs: 280,
		TRFCPBNs: 140,
	},
	8: {
		TRFCABNs: 280,
		TRFCPBNs: 140,
	},
	12: {
		TRFCABNs: 380,
		TRFCPBNs: 190,
	},
	16: {
		TRFCABNs: 380,
		TRFCPBNs: 190,
	},
}

type speedParams struct {
	TCKMinPs int
	TCKMaxPs int
	CASLatenciesx16Channel string
	CASLatenciesx8Channel string
}

const (
	/* First Byte */
	CAS6 = 1 << 1
	CAS10 = 1 << 4
	CAS14 = 1 << 7
	/* Second Byte */
	CAS16 = 1 << 0
	CAS20 = 1 << 2
	CAS22 = 1 << 3
	CAS24 = 1 << 4
	CAS26 = 1 << 5
	CAS28 = 1 << 6
	/* Third Byte */
	CAS32 = 1 << 0
	CAS36 = 1 << 2
	CAS40 = 1 << 4
)

const (
	/*
	 * JEDEC spec says that TCKmax should be 100ns for all speed grades.
	 * 100ns in MTB units comes out to be 0x320. But since this is a byte field, set it to
	 * 0xFF i.e. 31.875ns.
	 */
	TCKMaxPsDefault = 31875
)

var speedMbpsToSPDEncoding = map[int]speedParams {
	4267: {
		TCKMinPs: 468,           /* 1/4267 * 2 */
		TCKMaxPs: TCKMaxPsDefault,
		CASLatenciesx16Channel: "6 10 14 20 24 28 32 36",
		CASLatenciesx8Channel: "6 10 16 22 26 32 36 40",
	},
	3733: {
		TCKMinPs: 535,           /* 1/3733 * 2 */
		TCKMaxPs: TCKMaxPsDefault,
		CASLatenciesx16Channel: "6 10 14 20 24 28 32",
		CASLatenciesx8Channel: "6 10 16 22 26 32 36",
	},
	3200: {
		TCKMinPs: 625,           /* 1/3200 * 2 */
		TCKMaxPs: TCKMaxPsDefault,
		CASLatenciesx16Channel: "6 10 14 20 24 28",
		CASLatenciesx8Channel: "6 10 16 22 26 32",
	},
}

var bankEncoding = map[int]byte {
	4: 0 << 4,
	8: 1 << 4,
}

const (
	TGLLogicalChannelWidth = 16
)

/* Returns density to encode as per Intel MRC expectations. */
func getMRCDensity(memAttribs *memAttributes) int {
	if currPlatform == PlatformTGLADL {
		/*
		 * Intel MRC on TGL expects density per logical channel to be encoded in
		 * SPDIndexDensityBanks. Logical channel on TGL is an x16 channel.
		 */
		return memAttribs.DensityPerChannelGb * TGLLogicalChannelWidth / memAttribs.BitWidthPerChannel
	} else if currPlatform == PlatformJSL || currPlatform == PlatformCZN {
		/*
		 * Intel MRC on JSL expects density per die to be encoded in
		 * SPDIndexDensityBanks.
		 */
		return memAttribs.DensityPerChannelGb * memAttribs.ChannelsPerDie
	}

	return 0
}

func encodeDensityBanks(memAttribs *memAttributes) byte {
	var b byte

	b = densityGbToSPDEncoding[getMRCDensity(memAttribs)]
	b |= bankEncoding[memAttribs.Banks]

	return b
}

func encodeSdramAddressing(memAttribs *memAttributes) byte {
	densityPerChannelGb := memAttribs.DensityPerChannelGb
	if memAttribs.BitWidthPerChannel == 8 {
		return densityGbx8ChannelToRowColumnEncoding[densityPerChannelGb]
	} else {
		return densityGbx16ChannelToRowColumnEncoding[densityPerChannelGb]
	}
	return 0
}

func encodeChannelsPerDie(channels int) byte {
	var temp byte

	temp = byte(channels >> 1)

	return temp << 2
}

func encodePackage(dies int) byte {
	var temp byte

	if dies > 1 {
		/* If more than one die, then this is a non-monolithic device. */
		temp = 1
	} else {
		/* If only single die, then this is a monolithic device. */
		temp = 0
	}

	return temp << 7
}

/* Per JESD209-4C Dies = ZQ balls on the package */
/* Note that this can be different than the part's die count */
func encodeDiesPerPackage(memAttribs *memAttributes) byte {
	var dies int = 0
	if currPlatform == PlatformTGLADL {
		/* Intel MRC expects logical dies to be encoded for TGL. */
		dies = memAttribs.ChannelsPerDie * memAttribs.RanksPerChannel * memAttribs.BitWidthPerChannel / 16
	} else if currPlatform == PlatformJSL || currPlatform == PlatformCZN {
		/* Intel MRC expects physical dies to be encoded for JSL. */
		/* AMD PSP expects physical dies (ZQ balls) */
		dies = memAttribs.DiesPerPackage
	}

	b := encodePackage(dies) /* Monolithic / Non-monolithic device */
	b |= (byte(dies) - 1) << 4

	return b
}

func encodePackageType(memAttribs *memAttributes) byte {
	var b byte

	b |= encodeChannelsPerDie(memAttribs.ChannelsPerDie)
	b |= encodeDiesPerPackage(memAttribs)

	return b
}

func encodeDataWidth(bitWidthPerChannel int) byte {
	return byte(bitWidthPerChannel / 8)
}

func encodeRanks(ranks int) byte {
	var b byte
	b = byte(ranks - 1)
	return b << 3
}

func encodeModuleOrganization(memAttribs *memAttributes) byte {
	var b byte

	b = encodeDataWidth(memAttribs.BitWidthPerChannel)
	b |= encodeRanks(memAttribs.RanksPerChannel)

	return b
}

const (
	/*
	 * As per advisory 616599:
	 * 7:5 (Number of system channels) = 000 (1 channel always)
	 * 2:0 (Bus width) = 001 (x16 always)
	 * Set to 0x01.
	 */
	SPDValueBusWidthTGL = 0x01
	/*
	 * As per advisory 610202:
	 * 7:5 (Number of system channels) = 001 (2 channel always)
	 * 2:0 (Bus width) = 010 (x32 always)
	 * Set to 0x01.
	 */
	SPDValueBusWidthJSL = 0x22
)

func encodeBusWidth(memAttribs *memAttributes) byte {
	if currPlatform == PlatformTGLADL {
		return SPDValueBusWidthTGL
	} else if currPlatform == PlatformJSL || currPlatform == PlatformCZN {
		return SPDValueBusWidthJSL
	}
	return 0
}

func encodeTCKMin(memAttribs *memAttributes) byte {
	return convPsToMtbByte(memAttribs.TCKMinPs)
}

func encodeTCKMinFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TCKMinPs)
}

func encodeTCKMax(memAttribs *memAttributes) byte {
	return convPsToMtbByte(memAttribs.TCKMaxPs)
}

func encodeTCKMaxFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TCKMaxPs)
}

func encodeCASFirstByte(memAttribs *memAttributes) byte {
	return memAttribs.CASFirstByte
}

func encodeCASSecondByte(memAttribs *memAttributes) byte {
	return memAttribs.CASSecondByte
}

func encodeCASThirdByte(memAttribs *memAttributes) byte {
	return memAttribs.CASThirdByte
}

func divRoundUp(dividend int, divisor int) int {
	return (dividend + divisor - 1) / divisor
}

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

func encodeTAAMin(memAttribs *memAttributes) byte {
	return convPsToMtbByte(memAttribs.TAAMinPs)
}

func encodeTAAMinFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TAAMinPs)
}

func encodeTRCDMin(memAttribs *memAttributes) byte {
	return convNsToMtbByte(memAttribs.TRCDMinNs)
}

func encodeTRCDMinFineOffset(memAttribs *memAttributes) byte {
	return convNsToFtbByte(memAttribs.TRCDMinNs)
}

func encodeTRPABMin(memAttribs *memAttributes) byte {
	return convNsToMtbByte(memAttribs.TRPABMinNs)
}

func encodeTRPABMinFineOffset(memAttribs *memAttributes) byte {
	return convNsToFtbByte(memAttribs.TRPABMinNs)
}

func encodeTRPPBMin(memAttribs *memAttributes) byte {
	return convNsToMtbByte(memAttribs.TRPPBMinNs)
}

func encodeTRPPBMinFineOffset(memAttribs *memAttributes) byte {
	return convNsToFtbByte(memAttribs.TRPPBMinNs)
}

func encodeTRFCABMinMsb(memAttribs *memAttributes) byte {
	return byte((convNsToMtb(memAttribs.TRFCABNs) >> 8) & 0xff)
}

func encodeTRFCABMinLsb(memAttribs *memAttributes) byte {
	return byte(convNsToMtb(memAttribs.TRFCABNs) & 0xff)
}

func encodeTRFCPBMinMsb(memAttribs *memAttributes) byte {
	return byte((convNsToMtb(memAttribs.TRFCPBNs) >> 8) & 0xff)
}

func encodeTRFCPBMinLsb(memAttribs *memAttributes) byte {
	return byte(convNsToMtb(memAttribs.TRFCPBNs) & 0xff)
}

type SPDAttribFunc func (*memAttributes) byte

type SPDAttribTableEntry struct {
	constVal byte
	getVal SPDAttribFunc
}

const (
	/* SPD Byte Index */
	SPDIndexSize = 0
	SPDIndexRevision = 1
	SPDIndexMemoryType = 2
	SPDIndexModuleType = 3
	SPDIndexDensityBanks = 4
	SPDIndexAddressing = 5
	SPDIndexPackageType = 6
	SPDIndexOptionalFeatures = 7
	SPDIndexModuleOrganization = 12
	SPDIndexBusWidth = 13
	SPDIndexTimebases = 17
	SPDIndexTCKMin = 18
	SPDIndexTCKMax = 19
	SPDIndexCASFirstByte = 20
	SPDIndexCASSecondByte = 21
	SPDIndexCASThirdByte = 22
	SPDIndexCASFourthByte = 23
	SPDIndexTAAMin = 24
	SPDIndexReadWriteLatency = 25
	SPDIndexTRCDMin = 26
	SPDIndexTRPABMin = 27
	SPDIndexTRPPBMin = 28
	SPDIndexTRFCABMinLSB = 29
	SPDIndexTRFCABMinMSB = 30
	SPDIndexTRFCPBMinLSB = 31
	SPDIndexTRFCPBMinMSB = 32
	SPDIndexTRPPBMinFineOffset = 120
	SPDIndexTRPABMinFineOffset = 121
	SPDIndexTRCDMinFineOffset = 122
	SPDIndexTAAMinFineOffset = 123
	SPDIndexTCKMaxFineOffset = 124
	SPDIndexTCKMinFineOffset = 125
	SPDIndexManufacturerPartNumberStartByte = 329
	SPDIndexManufacturerPartNumberEndByte = 348

	/* SPD Byte Value */

	/*
	 * From JEDEC spec:
	 * 6:4 (Bytes total) = 2 (512 bytes)
	 * 3:0 (Bytes used) = 3 (384 bytes)
	 * Set to 0x23 for LPDDR4x.
	 */
	SPDValueSize = 0x23

	/*
	 * From JEDEC spec: Revision 1.1
	 * Set to 0x11.
	 */
	SPDValueRevision = 0x11

	/* LPDDR4x memory type = 0x11 */
	SPDValueMemoryType = 0x11

	/*
	 * From JEDEC spec:
	 * 7:7 (Hybrid) = 0 (Not hybrid)
	 * 6:4 (Hybrid media) = 000 (Not hybrid)
	 * 3:0 (Base Module Type) = 1110 (Non-DIMM solution)
	 *
	 * This is dependent on hardware design. LPDDR4x only has memory down solution.
	 * Hence this is not hybrid non-DIMM solution.
	 * Set to 0x0E.
	 */
	SPDValueModuleType = 0x0e

	/*
	 * From JEDEC spec:
	 * 5:4 (Maximum Activate Window) = 00 (8192 * tREFI)
	 * 3:0 (Maximum Activate Count) = 1000 (Unlimited MAC)
	 *
	 * Needs to come from datasheet, but most parts seem to support unlimited MAC.
	 * MR#24 OP3
	 */
	SPDValueOptionalFeatures = 0x08

	/*
	 * From JEDEC spec:
	 * 3:2 (MTB) = 00 (0.125ns)
	 * 1:0 (FTB) = 00 (1ps)
	 * Set to 0x00.
	 */
	SPDValueTimebases = 0x00

	/* CAS fourth byte: All bits are reserved */
	SPDValueCASFourthByte = 0x00

	/* Write Latency Set A and Read Latency DBI-RD disabled. */
	SPDValueReadWriteLatency = 0x00

	/* As per JEDEC spec, unused digits of manufacturer part number are left as blank. */
	SPDValueManufacturerPartNumberBlank = 0x20
)

var SPDAttribTable = map[int]SPDAttribTableEntry {
	SPDIndexSize: { constVal: SPDValueSize },
	SPDIndexRevision: { constVal: SPDValueRevision },
	SPDIndexMemoryType: { constVal: SPDValueMemoryType },
	SPDIndexModuleType: { constVal: SPDValueModuleType },
	SPDIndexDensityBanks: { getVal: encodeDensityBanks },
	SPDIndexAddressing: { getVal: encodeSdramAddressing },
	SPDIndexPackageType: { getVal: encodePackageType },
	SPDIndexOptionalFeatures: { constVal: SPDValueOptionalFeatures },
	SPDIndexModuleOrganization: { getVal: encodeModuleOrganization },
	SPDIndexBusWidth: { getVal: encodeBusWidth },
	SPDIndexTimebases: { constVal: SPDValueTimebases },
	SPDIndexTCKMin: { getVal: encodeTCKMin },
	SPDIndexTCKMax: { getVal: encodeTCKMax },
	SPDIndexTCKMaxFineOffset: { getVal: encodeTCKMaxFineOffset },
	SPDIndexTCKMinFineOffset: { getVal: encodeTCKMinFineOffset },
	SPDIndexCASFirstByte: { getVal: encodeCASFirstByte },
	SPDIndexCASSecondByte: { getVal: encodeCASSecondByte },
	SPDIndexCASThirdByte: { getVal: encodeCASThirdByte },
	SPDIndexCASFourthByte: { constVal: SPDValueCASFourthByte },
	SPDIndexTAAMin: { getVal: encodeTAAMin },
	SPDIndexTAAMinFineOffset: { getVal: encodeTAAMinFineOffset },
	SPDIndexReadWriteLatency: { constVal: SPDValueReadWriteLatency },
	SPDIndexTRCDMin: { getVal: encodeTRCDMin },
	SPDIndexTRCDMinFineOffset: { getVal: encodeTRCDMinFineOffset },
	SPDIndexTRPABMin: { getVal: encodeTRPABMin },
	SPDIndexTRPABMinFineOffset: { getVal: encodeTRPABMinFineOffset },
	SPDIndexTRPPBMin: { getVal: encodeTRPPBMin },
	SPDIndexTRPPBMinFineOffset: { getVal: encodeTRPPBMinFineOffset },
	SPDIndexTRFCABMinLSB: { getVal: encodeTRFCABMinLsb },
	SPDIndexTRFCABMinMSB: { getVal: encodeTRFCABMinMsb },
	SPDIndexTRFCPBMinLSB: { getVal: encodeTRFCPBMinLsb },
	SPDIndexTRFCPBMinMSB: { getVal: encodeTRFCPBMinMsb },
}

type memParts struct {
	MemParts []memPart `json:"parts"`
}

type memPart struct {
	Name string
	Attribs memAttributes
	SPDFileName string
}

func writeSPDManifest(memParts *memParts, SPDDirName string) error {
	var s string

	fmt.Printf("Generating SPD Manifest with following entries:\n")

	for i := 0; i < len(memParts.MemParts); i++ {
		fmt.Printf("%-40s %s\n", memParts.MemParts[i].Name, memParts.MemParts[i].SPDFileName)
		s += fmt.Sprintf("%s,%s\n", memParts.MemParts[i].Name, memParts.MemParts[i].SPDFileName)
	}

	return ioutil.WriteFile(filepath.Join(SPDDirName, SPDManifestFileName), []byte(s), 0644)
}

func isManufacturerPartNumberByte(index int) bool {
	if index >= SPDIndexManufacturerPartNumberStartByte && index <= SPDIndexManufacturerPartNumberEndByte {
		return true
	}
	return false
}

func getSPDByte(index int, memAttribs *memAttributes) byte {
	e, ok := SPDAttribTable[index]
	if ok == false {
		if isManufacturerPartNumberByte(index) {
			return SPDValueManufacturerPartNumberBlank
		}
		return 0x00
	}

	if e.getVal != nil {
		return e.getVal(memAttribs)
	}

	return e.constVal
}

func createSPD(memAttribs *memAttributes) string {
	var s string

	for i := 0; i < 512; i++ {
		b := getSPDByte(i, memAttribs)

		if (i + 1) % 16 == 0 {
			s += fmt.Sprintf("%02X\n", b)
		} else {
			s += fmt.Sprintf("%02X ", b)
		}
	}

	return s
}

func dedupeMemoryPart(dedupedParts []*memPart, memPart *memPart) bool {
	for i := 0; i < len(dedupedParts); i++ {
		if reflect.DeepEqual(dedupedParts[i].Attribs, memPart.Attribs) {
			memPart.SPDFileName = dedupedParts[i].SPDFileName
			return true
		}
	}

	return false
}

func generateSPD(memPart *memPart, SPDId int, SPDDirName string) {
	s := createSPD(&memPart.Attribs)
	memPart.SPDFileName = fmt.Sprintf("lp4x-spd-%d.hex", SPDId)
	ioutil.WriteFile(filepath.Join(SPDDirName, memPart.SPDFileName), []byte(s), 0644)
}

func readMemoryParts(memParts *memParts, memPartsFileName string) error {
	databytes, err := ioutil.ReadFile(memPartsFileName)
	if err != nil {
		return err
	}

	return json.Unmarshal(databytes, memParts)
}

func validateDensityx8Channel(densityPerChannelGb int) error {
	if _, ok := densityGbx8ChannelToRowColumnEncoding[densityPerChannelGb]; ok == false {
		return fmt.Errorf("Incorrect x8 density: ", densityPerChannelGb, "Gb")
	}
	return nil
}

func validateDensityx16Channel(densityPerChannelGb int) error {
	if _, ok := densityGbx16ChannelToRowColumnEncoding[densityPerChannelGb]; ok == false {
		return fmt.Errorf("Incorrect x16 density: ", densityPerChannelGb, "Gb")
	}
	return nil
}

func validateDensity(memAttribs *memAttributes) error {
	if memAttribs.BitWidthPerChannel == 8 {
		return validateDensityx8Channel(memAttribs.DensityPerChannelGb)
	} else if memAttribs.BitWidthPerChannel == 16 {
		return validateDensityx16Channel(memAttribs.DensityPerChannelGb)
	}

	return fmt.Errorf("No density table for this bit width: ", memAttribs.BitWidthPerChannel)
}

func validateBanks(banks int) error {
	if banks != 4 && banks != 8 {
		return fmt.Errorf("Incorrect banks: ", banks)
	}
	return nil
}

func validateChannels(channels int) error {
	if channels != 1 && channels != 2 && channels != 4 {
		return fmt.Errorf("Incorrect channels per die: ", channels)
	}
	return nil
}

func validateDataWidth(width int) error {
	if width != 8 && width != 16 {
		return fmt.Errorf("Incorrect bit width: ", width)
	}
	return nil
}

func validateRanks(ranks int) error {
	if ranks != 1 && ranks != 2 {
		return fmt.Errorf("Incorrect ranks: ", ranks)
	}
	return nil
}

func validateSpeed(speed int) error {
	if _, ok := speedMbpsToSPDEncoding[speed]; ok == false {
		return fmt.Errorf("Incorrect speed: ", speed, " Mbps")
	}
	return nil
}

func validateMemoryParts(memParts *memParts) error {
	for i := 0; i < len(memParts.MemParts); i++ {
		if err := validateBanks(memParts.MemParts[i].Attribs.Banks); err != nil {
			return err
		}
		if err := validateChannels(memParts.MemParts[i].Attribs.ChannelsPerDie); err != nil {
			return err
		}
		if err := validateDataWidth(memParts.MemParts[i].Attribs.BitWidthPerChannel); err != nil {
			return err
		}
		if err := validateDensity(&memParts.MemParts[i].Attribs); err != nil {
			return err
		}
		if err := validateRanks(memParts.MemParts[i].Attribs.RanksPerChannel); err != nil {
			return err
		}
		if err := validateSpeed(memParts.MemParts[i].Attribs.SpeedMbps); err != nil {
			return err
		}
	}
	return nil
}

func encodeLatencies(latency int, memAttribs *memAttributes) error {
	switch latency {
	case 6:
		memAttribs.CASFirstByte |= CAS6
	case 10:
		memAttribs.CASFirstByte |= CAS10
	case 14:
		memAttribs.CASFirstByte |= CAS14
	case 16:
		memAttribs.CASSecondByte |= CAS16
	case 20:
		memAttribs.CASSecondByte |= CAS20
	case 22:
		memAttribs.CASSecondByte |= CAS22
	case 24:
		memAttribs.CASSecondByte |= CAS24
	case 26:
		memAttribs.CASSecondByte |= CAS26
	case 28:
		memAttribs.CASSecondByte |= CAS28
	case 32:
		memAttribs.CASThirdByte |= CAS32
	case 36:
		memAttribs.CASThirdByte |= CAS36
	case 40:
		memAttribs.CASThirdByte |= CAS40
	default:
		fmt.Errorf("Incorrect CAS Latency: ", latency)
	}

	return nil
}

func updateTCK(memAttribs *memAttributes) {
	if memAttribs.TCKMinPs == 0 {
		memAttribs.TCKMinPs = speedMbpsToSPDEncoding[memAttribs.SpeedMbps].TCKMinPs
	}
	if memAttribs.TCKMaxPs == 0 {
		memAttribs.TCKMaxPs = speedMbpsToSPDEncoding[memAttribs.SpeedMbps].TCKMaxPs
	}
}

func getCASLatencies(memAttribs *memAttributes) string {
	if memAttribs.BitWidthPerChannel == 16 {
		return speedMbpsToSPDEncoding[memAttribs.SpeedMbps].CASLatenciesx16Channel
	} else if memAttribs.BitWidthPerChannel == 8 {
		return speedMbpsToSPDEncoding[memAttribs.SpeedMbps].CASLatenciesx8Channel
	}

	return ""
}

func updateCAS(memAttribs *memAttributes) error {
	if len(memAttribs.CASLatencies) == 0 {
		memAttribs.CASLatencies = getCASLatencies(memAttribs)
	}

	latencies := strings.Fields(memAttribs.CASLatencies)
	for i := 0; i < len(latencies); i++ {
		latency,err := strconv.Atoi(latencies[i])
		if err != nil {
			return fmt.Errorf("Unable to convert latency ", latencies[i])
		}
		if err := encodeLatencies(latency, memAttribs); err != nil {
			return err
		}
	}
	return nil
}

func getMinCAS(memAttribs *memAttributes) (int, error) {
	if (memAttribs.CASThirdByte & CAS40) != 0 {
		return 40, nil
	}
	if (memAttribs.CASThirdByte & CAS36) != 0 {
		return 36, nil
	}
	if (memAttribs.CASThirdByte & CAS32) != 0 {
		return 32, nil
	}
	if (memAttribs.CASSecondByte & CAS28) != 0 {
		return 28, nil
	}

	return 0, fmt.Errorf("Unexpected min CAS")
}

func updateTAAMin(memAttribs *memAttributes) error {
	if memAttribs.TAAMinPs == 0 {
		minCAS, err := getMinCAS(memAttribs)
		if err != nil {
			return err
		}
		memAttribs.TAAMinPs = memAttribs.TCKMinPs * minCAS
	}

	return nil
}

func updateTRFCAB(memAttribs *memAttributes) {
	if memAttribs.TRFCABNs == 0 {
		memAttribs.TRFCABNs = densityGbPhysicalChannelToRefreshEncoding[memAttribs.DensityPerChannelGb].TRFCABNs
	}
}

func updateTRFCPB(memAttribs *memAttributes) {
	if memAttribs.TRFCPBNs == 0 {
		memAttribs.TRFCPBNs = densityGbPhysicalChannelToRefreshEncoding[memAttribs.DensityPerChannelGb].TRFCPBNs
	}
}

func updateTRCD(memAttribs *memAttributes) {
	if memAttribs.TRCDMinNs == 0 {
		/* JEDEC spec says max of 18ns */
		memAttribs.TRCDMinNs = 18
	}
}

func updateTRPAB(memAttribs *memAttributes) {
	if memAttribs.TRPABMinNs == 0 {
		/* JEDEC spec says max of 21ns */
		memAttribs.TRPABMinNs = 21
	}
}

func updateTRPPB(memAttribs *memAttributes) {
	if memAttribs.TRPPBMinNs == 0 {
		/* JEDEC spec says max of 18ns */
		memAttribs.TRPPBMinNs = 18
	}
}

func normalizeMemoryAttributes(memAttribs *memAttributes) {
	if currPlatform == PlatformTGLADL {
		/*
		 * TGL does not really use physical organization of dies per package when
		 * generating the SPD. So, set it to 0 here so that deduplication ignores
		 * that field.
		 */
		memAttribs.DiesPerPackage = 0
	}
}

func updateMemoryAttributes(memAttribs *memAttributes) error {
	updateTCK(memAttribs)
	if err := updateCAS(memAttribs); err != nil {
		return err
	}
	if err := updateTAAMin(memAttribs); err != nil {
		return err
	}
	updateTRFCAB(memAttribs)
	updateTRFCPB(memAttribs)
	updateTRCD(memAttribs)
	updateTRPAB(memAttribs)
	updateTRPPB(memAttribs)

	normalizeMemoryAttributes(memAttribs)

	return nil
}

func isPlatformSupported(platform string) error {
	var ok bool

	currPlatform, ok = platformMap[platform]
	if ok == false {
		return fmt.Errorf("Unsupported platform: ", platform)
	}

	return nil
}

func usage() {
	fmt.Printf("\nUsage: %s <spd_dir> <mem_parts_list_json> <platform>\n\n", os.Args[0])
	fmt.Printf("   where,\n")
	fmt.Printf("   spd_dir = Directory path containing SPD files and manifest generated by gen_spd.go\n")
	fmt.Printf("   mem_parts_list_json = JSON File containing list of memory parts and attributes\n")
	fmt.Printf("   platform = SoC Platform for which the SPDs are being generated\n")
	fmt.Printf("              supported platforms: ")
	keys := reflect.ValueOf(platformMap).MapKeys()
	fmt.Println(keys)
	fmt.Printf("\n\n\n")
}

func main() {
	if len(os.Args) != 4 {
		usage()
		log.Fatal("Incorrect number of arguments")
	}

	var memParts memParts
	var dedupedParts []*memPart

	SPDDir, GlobalMemPartsFile, Platform := os.Args[1], os.Args[2], strings.ToUpper(os.Args[3])

	if err := isPlatformSupported(Platform); err != nil {
		log.Fatal(err)
	}

	if err := readMemoryParts(&memParts, GlobalMemPartsFile); err != nil {
		log.Fatal(err)
	}

	if err := validateMemoryParts(&memParts); err != nil {
		log.Fatal(err)
	}

	SPDId := 1

	for i := 0; i < len(memParts.MemParts); i++ {
		if err := updateMemoryAttributes(&memParts.MemParts[i].Attribs); err != nil {
			log.Fatal(err)
		}

		if dedupeMemoryPart(dedupedParts, &memParts.MemParts[i]) == false {
			generateSPD(&memParts.MemParts[i], SPDId, SPDDir)
			SPDId++
			dedupedParts = append(dedupedParts, &memParts.MemParts[i])
		}
	}

	if err := writeSPDManifest(&memParts, SPDDir); err != nil {
		log.Fatal(err)
	}
}
