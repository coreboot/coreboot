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
	"regexp"
	"strconv"
	"strings"
)

/*
 * This program generates de-duplicated SPD files for DDR4 memory using the global memory
 * part list provided in CSV format. In addition to that, it also generates SPD manifest in CSV
 * format that contains entries of type (DRAM part name, SPD file name) which provides the SPD
 * file name used by a given DRAM part.
 *
 * It takes as input:
 * Pointer to directory where the generated SPD files will be placed.
 * JSON file containing a list of memory parts with their attributes as per datasheet.
 */
const (
	SPDManifestFileName = "ddr4_spd_manifest.generated.txt"

	PlatformTGL = 0
	PlatformPCO = 1
	PlatformPLK = 2
)

var platformMap = map[string]int{
	"TGL": PlatformTGL,
	"PCO": PlatformPCO,
	"PLK": PlatformPLK,
}

var currPlatform int

type memAttributes struct {
	/* Primary attributes - must be provided by JSON file for each part */
	SpeedMTps        int
	CL_nRCD_nRP      int
	CapacityPerDieGb int
	DiesPerPackage   int
	PackageBusWidth  int
	RanksPerPackage  int

	/*
	 * All the following parameters are optional and required only if the part requires
	 * special parameters as per the datasheet.
	 */
	/* Timing parameters */
	TAAMinPs   int
	TRCDMinPs  int
	TRPMinPs   int
	TRASMinPs  int
	TRCMinPs   int
	TCKMinPs   int
	TCKMaxPs   int
	TRFC1MinPs int
	TRFC2MinPs int
	TRFC4MinPs int
	TFAWMinPs  int
	TRRDLMinPs int
	TRRDSMinPs int
	TCCDLMinPs int
	TWRMinPs   int
	TWTRLMinPs int
	TWTRSMinPs int

	/* CAS */
	CASLatencies  string
	CASFirstByte  byte
	CASSecondByte byte
	CASThirdByte  byte
	CASFourthByte byte

	/* The following is for internal-use only and is not overridable */
	dieBusWidth int
}

/* This encodes the density in Gb to SPD low nibble value as per JESD 4.1.2.L-5 R29 */
var densityGbToSPDEncoding = map[int]byte{
	2:  0x3,
	4:  0x4,
	8:  0x5,
	16: 0x6,
}

/*
 * Tables 4 thru Table 7 from JESD79-4C.
 * Maps density per die to row-column encoding for a device with x8/x16
 * physical channel.
 */
var densityGbx8x16DieCapacityToRowColumnEncoding = map[int]byte{
	2:  0x11, /* 14 rows, 10 columns */
	4:  0x19, /* 15 rows, 10 columns */
	8:  0x21, /* 16 rows, 10 columns */
	16: 0x29, /* 17 rows, 10 columns */
}

/*
 * Tables 169 & 170 in the JESD79-4C spec
 * Maps die density to refresh timings. This is the same for x8 and x16
 * devices.
 */

/* maps die density to rcf1 timing in pico seconds */
var tRFC1Encoding = map[int]int{
	2:  160000,
	4:  260000,
	8:  350000,
	16: 550000,
}

/* maps die density to rcf2 timing in pico seconds */
var tRFC2Encoding = map[int]int{
	2:  110000,
	4:  160000,
	8:  260000,
	16: 350000,
}

/* maps die density to rcf4 timing in pico seconds */
var tRFC4Encoding = map[int]int{
	2:  90000,
	4:  110000,
	8:  160000,
	16: 260000,
}

func getTRCMinPs(memAttribs *memAttributes) int {
	return memAttribs.TAAMinPs + memAttribs.TRASMinPs
}

func getDefaultTCKMinPs(memAttribs *memAttributes) int {
	/* value 2000000 = 2 * 1000000, where 1000000 is to convert mS to pS */
	return 2000000 / memAttribs.SpeedMTps
}

type speedBinAttributes struct {
	TRASMinPs int
	TCKMaxPs  int
}

var speedBinToSPDEncoding = map[int]speedBinAttributes{
	1600: {
		TRASMinPs: 35000,
		TCKMaxPs:  1500,
	},
	1866: {
		TRASMinPs: 34000,
		TCKMaxPs:  1250,
	},
	2133: {
		TRASMinPs: 33000,
		TCKMaxPs:  1071,
	},
	2400: {
		TRASMinPs: 32000,
		TCKMaxPs:  937,
	},
	2666: {
		TRASMinPs: 32000,
		TCKMaxPs:  833,
	},
	2933: {
		TRASMinPs: 32000,
		TCKMaxPs:  750,
	},
	3200: {
		TRASMinPs: 32000,
		TCKMaxPs:  682,
	},
}

func getBankGroups(memAttribs *memAttributes) byte {
	var bg byte

	switch memAttribs.PackageBusWidth {
	case 8:
		bg = 4
	case 16:
		if memAttribs.DiesPerPackage == 1 {
			bg = 2 /* x16 SDP has 2 bank groups */
		} else {
			bg = 4 /* x16 DDP has 4 bank groups */
		}
	}

	return bg
}

func encodeBankGroups(bg byte) byte {
	var val byte

	switch bg {
	case 2:
		val = 1
	case 4:
		val = 2
	}

	return val << 6
}

func encodeDensityBanks(memAttribs *memAttributes) byte {
	var b byte

	b = densityGbToSPDEncoding[memAttribs.CapacityPerDieGb]
	b |= encodeBankGroups(getBankGroups(memAttribs))
	/* No need to encode banksPerGroup.it's always 4 ([4:5] = 0) */

	return b
}

func encodeSdramAddressing(memAttribs *memAttributes) byte {
	var b byte

	b = densityGbx8x16DieCapacityToRowColumnEncoding[memAttribs.CapacityPerDieGb]

	return b
}

func encodePackageDeviceType(dies int) byte {
	var b byte

	if dies > 1 {
		/* If more than one die, then this is a non-monolithic device. */
		b = 1
	} else {
		/* If only single die, then this is a monolithic device. */
		b = 0
	}

	return b << 7
}

func encodeSignalLoadingFromDieCount(dies int) byte {
	var loading byte

	/*
	 * If die count = 1, signal loading = "not specified" = 0
	 * If die count > 1, signal loading = "multi" = 2
	 */
	if dies == 1 {
		loading = 0
	} else {
		loading = 1
	}

	return loading
}

func encodeDiesPerPackage(dies int) byte {
	var b byte

	b = encodePackageDeviceType(dies) /* Monolithic / Non-monolithic device */
	b |= (byte(dies) - 1) << 4

	return b
}

func encodePackageType(memAttribs *memAttributes) byte {
	var b byte

	b = encodeDiesPerPackage(memAttribs.DiesPerPackage)
	b |= encodeSignalLoadingFromDieCount(memAttribs.DiesPerPackage)

	return b
}

func encodeDataWidth(bitWidthPerDevice int) byte {
	var width byte

	switch bitWidthPerDevice {
	case 8:
		width = 1
	case 16:
		width = 2
	}

	return width
}

func encodeRanks(ranks int) byte {
	var b byte

	b = byte(ranks - 1)

	return b << 3
}

func encodeModuleOrganization(memAttribs *memAttributes) byte {
	var b byte

	b = encodeDataWidth(memAttribs.dieBusWidth)
	b |= encodeRanks(memAttribs.RanksPerPackage)

	return b
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

func encodeTAAMin(memAttribs *memAttributes) byte {
	return convPsToMtbByte(memAttribs.TAAMinPs)
}

func encodeTAAMinFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TAAMinPs)
}

func encodeTRCDMin(memAttribs *memAttributes) byte {
	return convPsToMtbByte(memAttribs.TRCDMinPs)
}

func encodeTRCDMinFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TRCDMinPs)
}

func encodeTRPMin(memAttribs *memAttributes) byte {
	return convPsToMtbByte(memAttribs.TRPMinPs)
}

func encodeTRCMinFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TRCMinPs)
}

func encodeTRPMinFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TRPMinPs)
}

func encodeTRASRCMinMSNs(memAttribs *memAttributes) byte {
	var b byte

	b = byte((convPsToMtb(memAttribs.TRASMinPs) >> 4) & 0xf0)
	b |= byte((convPsToMtb(memAttribs.TRCMinPs) >> 8) & 0x0f)

	return b
}

func encodeTRASMinLsb(memAttribs *memAttributes) byte {
	return byte(convPsToMtb(memAttribs.TRASMinPs) & 0xff)
}

func encodeTRCMinLsb(memAttribs *memAttributes) byte {
	return byte(convPsToMtb(memAttribs.TRCMinPs) & 0xff)
}

/* This takes memAttribs.PackageBusWidth as an index */
var pageSizefromBusWidthEncoding = map[int]int{
	8:  1,
	16: 2,
}

/*
 * Per Table 169 & Table 170 of Jedec JESD79-4C
 * tFAW timing is based on :
 *  Speed bin and page size
 */
func getTFAWMinPs(memAttribs *memAttributes) int {
	var tFAWFixed int

	if pageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] == 1 {
		switch memAttribs.SpeedMTps {
		case 1600:
			tFAWFixed = 25000
		case 1866:
			tFAWFixed = 23000
		default:
			tFAWFixed = 21000
		}
	} else if pageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] == 2 {
		switch memAttribs.SpeedMTps {
		case 1600:
			tFAWFixed = 35000
		default:
			tFAWFixed = 30000
		}
	}

	return tFAWFixed
}

/* Update settings based on data sheet (json) supplied memory attributes */

func updateTFAWMin(memAttribs *memAttributes) {
	var tFAWFromTck int

	if memAttribs.TFAWMinPs == 0 {
		memAttribs.TFAWMinPs = getTFAWMinPs(memAttribs)
	}

	switch pageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] {
	case 1:
		tFAWFromTck = 20 * memAttribs.TCKMinPs
	case 2:
		tFAWFromTck = 28 * memAttribs.TCKMinPs
	}

	if memAttribs.TFAWMinPs < tFAWFromTck {
		memAttribs.TFAWMinPs = tFAWFromTck
	}
}

func updateTRFC1Min(memAttribs *memAttributes) {
	if memAttribs.TRFC1MinPs == 0 {
		memAttribs.TRFC1MinPs = tRFC1Encoding[memAttribs.CapacityPerDieGb]
	}
}

func updateTRFC2Min(memAttribs *memAttributes) {
	if memAttribs.TRFC2MinPs == 0 {
		memAttribs.TRFC2MinPs = tRFC2Encoding[memAttribs.CapacityPerDieGb]
	}
}

func updateTRFC4Min(memAttribs *memAttributes) {
	if memAttribs.TRFC4MinPs == 0 {
		memAttribs.TRFC4MinPs = tRFC4Encoding[memAttribs.CapacityPerDieGb]
	}
}

func getTRRDLMinPs(memAttribs *memAttributes) int {
	var tRRDLFixed int

	/*
	 * Per JESD79-4C Tables 169 & 170, tRRD_L is based on :
	 *  Speed bin and page size
	 */
	switch pageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] {
	case 1:
		switch memAttribs.SpeedMTps {
		case 1600:
			tRRDLFixed = 6000
		default:
			tRRDLFixed = 5300
		}
	case 2:
		switch memAttribs.SpeedMTps {
		case 1600:
			tRRDLFixed = 7500
		default:
			tRRDLFixed = 6400
		}
	}

	return tRRDLFixed
}

func updateTRRDLMin(memAttribs *memAttributes) {
	var tRRDLFromTck int

	if memAttribs.TRRDLMinPs == 0 {
		memAttribs.TRRDLMinPs = getTRRDLMinPs(memAttribs)
	}

	tRRDLFromTck = 4 * memAttribs.TCKMinPs

	if memAttribs.TRRDLMinPs < tRRDLFromTck {
		memAttribs.TRRDLMinPs = tRRDLFromTck
	}
}

var speedToTRRDSMinPsOneKPageSize = map[int]int{
	1600: 5000,
	1866: 4200,
	2133: 3700,
	2400: 3300,
	2666: 3000,
	2933: 2700,
	3200: 2500,
}

var speedToTRRDSMinPsTwoKPageSize = map[int]int{
	1600: 6000,
	1866: 5300,
	2133: 5300,
	2400: 5300,
	2666: 5300,
	2933: 5300,
	3200: 5300,
}

func getTRRDSMinPs(memAttribs *memAttributes) int {
	var tRRDFixed int

	switch pageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] {
	case 1:
		tRRDFixed = speedToTRRDSMinPsOneKPageSize[memAttribs.SpeedMTps]
	case 2:
		tRRDFixed = speedToTRRDSMinPsTwoKPageSize[memAttribs.SpeedMTps]
	}

	return tRRDFixed
}

func updateTRRDSMin(memAttribs *memAttributes) {
	var tRRDFromTck int

	if memAttribs.TRRDSMinPs == 0 {
		memAttribs.TRRDSMinPs = getTRRDSMinPs(memAttribs)
	}

	tRRDFromTck = 4 * memAttribs.TCKMinPs

	if memAttribs.TRRDSMinPs < tRRDFromTck {
		memAttribs.TRRDSMinPs = tRRDFromTck
	}
}

/*
 * Per JESD79-4C Tables 169 and 170,
 * tCCD_L is based on :
 *  Speed Bin
 */
func getTCCDLMinPs(memAttribs *memAttributes) int {
	var tCCDLFixed int

	switch memAttribs.SpeedMTps {
	case 1600:
		tCCDLFixed = 6250
	case 1866:
		tCCDLFixed = 5355
	case 2133:
		tCCDLFixed = 5355
	default:
		tCCDLFixed = 5000
	}

	return tCCDLFixed
}

func updateTCCDLMin(memAttribs *memAttributes) {
	var tCCDLFromTck int

	if memAttribs.TCCDLMinPs == 0 {
		memAttribs.TCCDLMinPs = getTCCDLMinPs(memAttribs)
	}

	tCCDLFromTck = 5 * memAttribs.TCKMinPs

	if memAttribs.TCCDLMinPs < tCCDLFromTck {
		memAttribs.TCCDLMinPs = tCCDLFromTck
	}
}

func encodeTRFC1MinLsb(memAttribs *memAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC1MinPs)

	return byte(mtb & 0xff)
}

func encodeTRFC1MinMsb(memAttribs *memAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC1MinPs)

	return byte((mtb >> 8) & 0xff)
}

func encodeTRFC2MinLsb(memAttribs *memAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC2MinPs)

	return byte(mtb & 0xff)
}

func encodeTRFC2MinMsb(memAttribs *memAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC2MinPs)

	return byte((mtb >> 8) & 0xff)
}

func encodeTRFC4MinLsb(memAttribs *memAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC4MinPs)

	return byte(mtb & 0xff)
}

func encodeTRFC4MinMsb(memAttribs *memAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC4MinPs)

	return byte((mtb >> 8) & 0xff)
}

func encodeTFAWMinMSN(memAttribs *memAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TFAWMinPs)

	return byte((mtb >> 8) & 0x0f)
}

func encodeTFAWMinLsb(memAttribs *memAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TFAWMinPs)

	return byte(mtb & 0xff)
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

func encodeCASFourthByte(memAttribs *memAttributes) byte {
	return memAttribs.CASFourthByte
}

func encodeTRRDSMin(memAttribs *memAttributes) byte {
	return convPsToMtbByte(memAttribs.TRRDSMinPs)
}

func encodeTRRDSMinFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TRRDSMinPs)
}

func encodeTRRDLMin(memAttribs *memAttributes) byte {
	return convPsToMtbByte(memAttribs.TRRDLMinPs)
}

func encodeTRRDLMinFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TRRDLMinPs)
}

func encodeTCCDLMin(memAttribs *memAttributes) byte {
	return convPsToMtbByte(memAttribs.TCCDLMinPs)
}

func encodeTCCDLMinFineOffset(memAttribs *memAttributes) byte {
	return convPsToFtbByte(memAttribs.TCCDLMinPs)
}

func encodeTWRMinMSN(memAttribs *memAttributes) byte {
	return byte((convPsToMtb(TimingValueTWRMinPs) >> 8) & 0x0f)
}

func encodeTWRMinLsb(memAttribs *memAttributes) byte {
	return byte(convPsToMtb(TimingValueTWRMinPs) & 0xff)
}

func encodeTWTRMinMSNs(memAttribs *memAttributes) byte {
	var b byte

	b = byte((convPsToMtb(memAttribs.TWTRLMinPs) >> 4) & 0xf0)
	b |= byte((convPsToMtb(memAttribs.TWTRSMinPs) >> 8) & 0x0f)

	return b
}

func encodeTWTRSMinLsb(memAttribs *memAttributes) byte {
	return byte(convPsToMtb(memAttribs.TWTRSMinPs) & 0xff)
}

func encodeTWTRLMinLsb(memAttribs *memAttributes) byte {
	return byte(convPsToMtb(memAttribs.TWTRLMinPs) & 0xff)
}

type SPDMemAttribFunc func(*memAttributes) byte
type SPDConvConstFunc func() byte

type SPDAttribTableEntry struct {
	constVal byte
	getVal   SPDMemAttribFunc
}

const (
	/* SPD Byte Index */
	SPDIndexSize                            = 0
	SPDIndexRevision                        = 1
	SPDIndexMemoryType                      = 2
	SPDIndexModuleType                      = 3
	SPDIndexDensityBanks                    = 4
	SPDIndexAddressing                      = 5
	SPDIndexPackageType                     = 6
	SPDIndexOptionalFeatures                = 7
	SPDIndexModuleOrganization              = 12
	SPDIndexBusWidth                        = 13
	SPDIndexTimebases                       = 17
	SPDIndexTCKMin                          = 18
	SPDIndexTCKMax                          = 19
	SPDIndexCASFirstByte                    = 20
	SPDIndexCASSecondByte                   = 21
	SPDIndexCASThirdByte                    = 22
	SPDIndexCASFourthByte                   = 23
	SPDIndexTAAMin                          = 24
	SPDIndexTRCDMin                         = 25
	SPDIndexTRPMin                          = 26
	SPDIndexTRASRCMinMSNs                   = 27
	SPDIndexTRASMinLsb                      = 28
	SPDIndexTRCMinLsb                       = 29
	SPDIndexTRFC1MinLsb                     = 30
	SPDIndexTRFC1MinMsb                     = 31
	SPDIndexTRFC2MinLsb                     = 32
	SPDIndexTRFC2MinMsb                     = 33
	SPDIndexTRFC4MinLsb                     = 34
	SPDIndexTRFC4MinMsb                     = 35
	SPDIndexTFAWMinMSN                      = 36
	SPDIndexTFAWMinLsb                      = 37
	SPDIndexTRRDSMin                        = 38
	SPDIndexTRRDLMin                        = 39
	SPDIndexTCCDLMin                        = 40
	SPDIndexTWRMinMSN                       = 41
	SPDIndexTWRMinLsb                       = 42
	SPDIndexTWTRMinMSNs                     = 43
	SPDIndexWTRSMinLsb                      = 44
	SPDIndexWTRLMinLsb                      = 45
	SPDIndexTCCDLMinFineOffset              = 117
	SPDIndexTRRDLMinFineOffset              = 118
	SPDIndexTRRDSMinFineOffset              = 119
	SPDIndexTRCMinFineOffset                = 120
	SPDIndexTRPMinFineOffset                = 121
	SPDIndexTRCDMinFineOffset               = 122
	SPDIndexTAAMinFineOffset                = 123
	SPDIndexTCKMaxFineOffset                = 124
	SPDIndexTCKMinFineOffset                = 125
	SPDIndexManufacturerPartNumberStartByte = 329
	SPDIndexManufacturerPartNumberEndByte   = 348

	/* SPD Byte Value */

	/*
	 * From JEDEC spec:
	 * 6:4 (Bytes total) = 2 (512 bytes)
	 * 3:0 (Bytes used) = 3 (384 bytes)
	 * Set to 0x23 for DDR4.
	 */
	SPDValueSize = 0x23

	/*
	 * From JEDEC spec: Revision 1.1
	 * Set to 0x11.
	 */
	SPDValueRevision = 0x11

	/* DDR4 memory type = 0x0C */
	SPDValueMemoryType = 0x0C

	/*
		 * From JEDEC spec:
	 	 * Module Type [0:3] :
		 *  0 = Undefined
		 *  1 = RDIMM (width = 133.35 mm nom)
		 *  2 = UDIMM (width = 133.35 mm nom)
		 *  3 = SO-DIMM (width = 68.60 mm nom)
		 *  4 = LRDIMM (width = 133.35 mm nom)
		 *
		 * DDR4 on TGL uses SO-DIMM type for for both memory down and DIMM config.
		 * Set to 0x03.
	*/
	SPDValueModuleType = 0x03

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
	 * 2:0 Primary Bus Width in Bits = 011 (x64 always)
	 * Set to 0x03.
	 */
	SPDValueModuleBusWidth = 0x03

	/*
	 * From JEDEC spec:
	 * 3:2 (MTB) = 00 (0.125ns)
	 * 1:0 (FTB) = 00 (1ps)
	 * Set to 0x00.
	 */
	SPDValueTimebases = 0x00

	/* CAS fourth byte: All bits are reserved */
	SPDValueCASFourthByte = 0x00

	/* As per JEDEC spec, unused digits of manufacturer part number are left as blank. */
	SPDValueManufacturerPartNumberBlank = 0x20
)

const (
	/*
	 * As per Table 75 of Jedec spec 4.1.20-L-5 R29 v103:
	 * tWRMin = 15nS for all DDR4 Speed Bins
	 * Set to 15000 pS
	 */
	TimingValueTWRMinPs = 15000

	/*
	 * As per Table 78 of Jedec spec 4.1.20-L-5 R29 v103:
	 * tWTR_SMin = 2.5nS for all DDR4 Speed Bins
	 * Set to 2500 pS
	 */
	TimingValueTWTRSMinPs = 2500

	/*
	 * As per Table 80 of Jedec spec 4.1.20-L-5 R29 v103:
	 * tWTR_LMin = 7.5 nS for all DDR4 Speed Bins
	 * Set to 7500 pS
	 */
	TimingValueTWTRLMinPs = 7500
)

var SPDAttribTable = map[int]SPDAttribTableEntry{
	SPDIndexSize:               {constVal: SPDValueSize},
	SPDIndexRevision:           {constVal: SPDValueRevision},
	SPDIndexMemoryType:         {constVal: SPDValueMemoryType},
	SPDIndexModuleType:         {constVal: SPDValueModuleType},
	SPDIndexDensityBanks:       {getVal: encodeDensityBanks},
	SPDIndexAddressing:         {getVal: encodeSdramAddressing},
	SPDIndexPackageType:        {getVal: encodePackageType},
	SPDIndexOptionalFeatures:   {constVal: SPDValueOptionalFeatures},
	SPDIndexModuleOrganization: {getVal: encodeModuleOrganization},
	SPDIndexBusWidth:           {constVal: SPDValueModuleBusWidth},
	SPDIndexTimebases:          {constVal: SPDValueTimebases},
	SPDIndexTCKMin:             {getVal: encodeTCKMin},
	SPDIndexTCKMinFineOffset:   {getVal: encodeTCKMinFineOffset},
	SPDIndexTCKMax:             {getVal: encodeTCKMax},
	SPDIndexTCKMaxFineOffset:   {getVal: encodeTCKMaxFineOffset},
	SPDIndexCASFirstByte:       {getVal: encodeCASFirstByte},
	SPDIndexCASSecondByte:      {getVal: encodeCASSecondByte},
	SPDIndexCASThirdByte:       {getVal: encodeCASThirdByte},
	SPDIndexCASFourthByte:      {getVal: encodeCASFourthByte},
	SPDIndexTAAMin:             {getVal: encodeTAAMin},
	SPDIndexTAAMinFineOffset:   {getVal: encodeTAAMinFineOffset},
	SPDIndexTRCDMin:            {getVal: encodeTRCDMin},
	SPDIndexTRCDMinFineOffset:  {getVal: encodeTRCDMinFineOffset},
	SPDIndexTRPMin:             {getVal: encodeTRPMin},
	SPDIndexTRPMinFineOffset:   {getVal: encodeTRPMinFineOffset},
	SPDIndexTRASRCMinMSNs:      {getVal: encodeTRASRCMinMSNs},
	SPDIndexTRASMinLsb:         {getVal: encodeTRASMinLsb},
	SPDIndexTRCMinLsb:          {getVal: encodeTRCMinLsb},
	SPDIndexTRCMinFineOffset:   {getVal: encodeTRCMinFineOffset},
	SPDIndexTRFC1MinLsb:        {getVal: encodeTRFC1MinLsb},
	SPDIndexTRFC1MinMsb:        {getVal: encodeTRFC1MinMsb},
	SPDIndexTRFC2MinLsb:        {getVal: encodeTRFC2MinLsb},
	SPDIndexTRFC2MinMsb:        {getVal: encodeTRFC2MinMsb},
	SPDIndexTRFC4MinLsb:        {getVal: encodeTRFC4MinLsb},
	SPDIndexTRFC4MinMsb:        {getVal: encodeTRFC4MinMsb},
	SPDIndexTFAWMinMSN:         {getVal: encodeTFAWMinMSN},
	SPDIndexTFAWMinLsb:         {getVal: encodeTFAWMinLsb},
	SPDIndexTRRDSMin:           {getVal: encodeTRRDSMin},
	SPDIndexTRRDSMinFineOffset: {getVal: encodeTRRDSMinFineOffset},
	SPDIndexTRRDLMin:           {getVal: encodeTRRDLMin},
	SPDIndexTRRDLMinFineOffset: {getVal: encodeTRRDLMinFineOffset},
	SPDIndexTCCDLMin:           {getVal: encodeTCCDLMin},
	SPDIndexTCCDLMinFineOffset: {getVal: encodeTCCDLMinFineOffset},
	SPDIndexTWRMinMSN:          {getVal: encodeTWRMinMSN},
	SPDIndexTWRMinLsb:          {getVal: encodeTWRMinLsb},
	SPDIndexTWTRMinMSNs:        {getVal: encodeTWTRMinMSNs},
	SPDIndexWTRSMinLsb:         {getVal: encodeTWTRSMinLsb},
	SPDIndexWTRLMinLsb:         {getVal: encodeTWTRLMinLsb},
}

type memParts struct {
	MemParts []memPart `json:"parts"`
}

type memPart struct {
	Name        string
	Attribs     memAttributes
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
		var b byte = 0
		if memAttribs != nil {
			b = getSPDByte(i, memAttribs)
		}

		if (i+1)%16 == 0 {
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
	memPart.SPDFileName = fmt.Sprintf("ddr4-spd-%d.hex", SPDId)
	ioutil.WriteFile(filepath.Join(SPDDirName, memPart.SPDFileName), []byte(s), 0644)
}

func generateEmptySPD(SPDDirName string) {

	s := createSPD(nil)
	SPDFileName := "ddr4-spd-empty.hex"
	ioutil.WriteFile(filepath.Join(SPDDirName, SPDFileName), []byte(s), 0644)
}

func readMemoryParts(memParts *memParts, memPartsFileName string) error {
	databytes, err := ioutil.ReadFile(memPartsFileName)
	if err != nil {
		return err
	}

	// Strip comments from json file
	re := regexp.MustCompile(`(?m)^\s*//.*`)
	databytes = re.ReplaceAll(databytes, []byte(""))

	return json.Unmarshal(databytes, memParts)
}

func validateSpeedMTps(speedBin int) error {
	if _, ok := speedBinToSPDEncoding[speedBin]; ok == false {
		return fmt.Errorf("Incorrect speed bin: DDR4-", speedBin)
	}
	return nil
}

func validateCapacityPerDie(capacityPerDieGb int) error {
	if _, ok := densityGbToSPDEncoding[capacityPerDieGb]; ok == false {
		return fmt.Errorf("Incorrect capacity per die: ", capacityPerDieGb)
	}
	return nil
}

func validateDiesPerPackage(dieCount int) error {
	if dieCount >= 1 && dieCount <= 2 {
		return nil
	}
	return fmt.Errorf("Incorrect dies per package count: ", dieCount)
}

func validatePackageBusWidth(width int) error {
	if width != 8 && width != 16 {
		return fmt.Errorf("Incorrect device bus width: ", width)
	}
	return nil
}

func validateRanksPerPackage(ranks int) error {
	if ranks >= 1 && ranks <= 2 {
		return nil
	}
	return fmt.Errorf("Incorrect package ranks: ", ranks)
}

func validateCASLatency(CL int) error {
	if CL >= 10 && CL <= 24 && CL != 23 {
		return nil
	}
	return fmt.Errorf("Incorrect CAS latency: ", CL)
}

/*
1) validate memory parts
2) remove any fields that Intel does not care about
*/

/* verify the supplied CAS Latencies supported does not match default */
func verifySupportedCASLatencies(part *memPart) error {
	if part.Attribs.CASLatencies == getDefaultCASLatencies(&part.Attribs) {
		return fmt.Errorf("CASLatencies for %s already matches default,\nPlease remove CASLatencies override line from the %s part attributes in the global part list and regenerate SPD Manifest", part.Name, part.Name)
	}

	return nil
}

func validateMemoryParts(memParts *memParts) error {
	memPartExists := make(map[string]bool)

	for i := 0; i < len(memParts.MemParts); i++ {
		if memPartExists[memParts.MemParts[i].Name] {
			return fmt.Errorf(memParts.MemParts[i].Name + " is duplicated in mem_parts_list_json")
		}
		memPartExists[memParts.MemParts[i].Name] = true

		if err := validateSpeedMTps(memParts.MemParts[i].Attribs.SpeedMTps); err != nil {
			return err
		}
		if err := validateCapacityPerDie(memParts.MemParts[i].Attribs.CapacityPerDieGb); err != nil {
			return err
		}
		if err := validateDiesPerPackage(memParts.MemParts[i].Attribs.DiesPerPackage); err != nil {
			return err
		}
		if err := validatePackageBusWidth(memParts.MemParts[i].Attribs.PackageBusWidth); err != nil {
			return err
		}
		if err := validateRanksPerPackage(memParts.MemParts[i].Attribs.RanksPerPackage); err != nil {
			return err
		}
		if err := validateCASLatency(memParts.MemParts[i].Attribs.CL_nRCD_nRP); err != nil {
			return err
		}
		/* If CAS Latency was supplied, make sure it doesn't match default value */
		if len(memParts.MemParts[i].Attribs.CASLatencies) != 0 {
			if err := verifySupportedCASLatencies(&memParts.MemParts[i]); err != nil {
				return err
			}
		}
	}

	return nil
}

const (
	/* First Byte */
	CAS9  = 1 << 2
	CAS10 = 1 << 3
	CAS11 = 1 << 4
	CAS12 = 1 << 5
	CAS13 = 1 << 6
	CAS14 = 1 << 7
	/* Second Byte */
	CAS15 = 1 << 0
	CAS16 = 1 << 1
	CAS17 = 1 << 2
	CAS18 = 1 << 3
	CAS19 = 1 << 4
	CAS20 = 1 << 5
	CAS21 = 1 << 6
	CAS22 = 1 << 7
	/* Third Byte */
	CAS24 = 1 << 1
)

func encodeLatencies(latency int, memAttribs *memAttributes) error {
	switch latency {
	case 9:
		memAttribs.CASFirstByte |= CAS9
	case 10:
		memAttribs.CASFirstByte |= CAS10
	case 11:
		memAttribs.CASFirstByte |= CAS11
	case 12:
		memAttribs.CASFirstByte |= CAS12
	case 13:
		memAttribs.CASFirstByte |= CAS13
	case 14:
		memAttribs.CASFirstByte |= CAS14
	case 15:
		memAttribs.CASSecondByte |= CAS15
	case 16:
		memAttribs.CASSecondByte |= CAS16
	case 17:
		memAttribs.CASSecondByte |= CAS17
	case 18:
		memAttribs.CASSecondByte |= CAS18
	case 19:
		memAttribs.CASSecondByte |= CAS19
	case 20:
		memAttribs.CASSecondByte |= CAS20
	case 21:
		memAttribs.CASSecondByte |= CAS21
	case 22:
		memAttribs.CASSecondByte |= CAS22
	case 24:
		memAttribs.CASThirdByte |= CAS24
	default:
		fmt.Errorf("Incorrect CAS Latency: ", latency)
	}

	return nil
}

/* Default CAS Latencies from Speed Bin tables in JEDS79-4C */
func getDefaultCASLatencies(memAttribs *memAttributes) string {
	var str string

	switch memAttribs.SpeedMTps {
	case 1600:
		switch memAttribs.CL_nRCD_nRP {
		case 10:
			str = "9 10 11 12"
		case 11:
			str = "9 11 12"
		case 12:
			str = "10 12"
		}
	case 1866:
		switch memAttribs.CL_nRCD_nRP {
		case 12:
			str = "9 10 12 13 14"
		case 13:
			str = "9 11 12 13 14"
		case 14:
			str = "10 12 14"
		}
	case 2133:
		switch memAttribs.CL_nRCD_nRP {
		case 14:
			str = "9 10 12 14 15 16"
		case 15:
			str = "9 11 12 13 14 15 16"
		case 16:
			str = "10 12 14 16"
		}
	case 2400:
		switch memAttribs.CL_nRCD_nRP {
		case 15:
			str = "9 10 12 14 15 16 17 18"
		case 16:
			str = "9 11 12 13 14 15 16 17 18"
		case 17:
			str = "10 11 12 13 14 15 16 17 18"
		case 18:
			str = "10 12 14 16 18"
		}
	case 2666:
		switch memAttribs.CL_nRCD_nRP {
		case 17:
			str = "9 10 11 12 13 14 15 16 17 18 19 20"
		case 18:
			str = "9 10 11 12 13 14 15 16 17 18 19 20"
		case 19:
			str = "10 11 12 13 14 15 16 17 18 19 20"
		case 20:
			str = "10 12 14 16 18 20"
		}
	case 2933:
		switch memAttribs.CL_nRCD_nRP {
		case 19:
			str = "9 10 11 12 13 14 15 16 17 18 19 20 21 22"
		case 20:
			str = "10 11 12 13 14 15 16 17 18 19 20 21 22"
		case 21:
			str = "10 11 12 13 14 15 16 17 18 19 20 21 22"
		case 22:
			str = "10 12 14 16 18 20 22"
		}
	case 3200:
		switch memAttribs.CL_nRCD_nRP {
		case 20:
			str = "9 10 11 12 13 14 15 16 17 18 19 20 21 22 24"
		case 22:
			str = "10 11 12 13 14 15 16 17 18 19 20 21 22 24"
		case 24:
			str = "10 12 14 16 18 20 22 24"
		}
	}

	return str
}

func updateDieBusWidth(memAttribs *memAttributes) {
	if memAttribs.PackageBusWidth == 16 && memAttribs.RanksPerPackage == 1 &&
		memAttribs.DiesPerPackage == 2 {
		/*
		 * If a x16 part has 2 die with single rank, PackageBusWidth
		 * needs to be converted to match die bus width.
		 */
		memAttribs.dieBusWidth = 8
	} else {
		memAttribs.dieBusWidth = memAttribs.PackageBusWidth
	}
}

func updateCAS(memAttribs *memAttributes) error {
	if len(memAttribs.CASLatencies) == 0 {
		memAttribs.CASLatencies = getDefaultCASLatencies(memAttribs)
	}

	latencies := strings.Fields(memAttribs.CASLatencies)
	for i := 0; i < len(latencies); i++ {
		latency, err := strconv.Atoi(latencies[i])
		if err != nil {
			return fmt.Errorf("Unable to convert latency ", latencies[i])
		}
		if err := encodeLatencies(latency, memAttribs); err != nil {
			return err
		}
	}

	return nil
}

func getTAAMinPs(memAttribs *memAttributes) int {
	return (memAttribs.CL_nRCD_nRP * 2000000) / memAttribs.SpeedMTps
}

func updateTAAMin(memAttribs *memAttributes) {
	if memAttribs.TAAMinPs == 0 {
		memAttribs.TAAMinPs = getTAAMinPs(memAttribs)
	}
}

func updateTRCDMin(memAttribs *memAttributes) {
	/* tRCDmin is same as tAAmin for all cases */
	if memAttribs.TRCDMinPs == 0 {
		memAttribs.TRCDMinPs = getTAAMinPs(memAttribs)
	}
}

func updateTRPMin(memAttribs *memAttributes) {
	/* tRPmin is same as tAAmin for all cases */
	if memAttribs.TRPMinPs == 0 {
		memAttribs.TRPMinPs = getTAAMinPs(memAttribs)
	}
}

func updateTRASMin(memAttribs *memAttributes) {
	if memAttribs.TRASMinPs == 0 {
		memAttribs.TRASMinPs = speedBinToSPDEncoding[memAttribs.SpeedMTps].TRASMinPs
	}
}

func updateTRCMin(memAttribs *memAttributes) {
	if memAttribs.TRCMinPs == 0 {
		memAttribs.TRCMinPs = getTRCMinPs(memAttribs)
	}
}

func updateTCK(memAttribs *memAttributes) {
	if memAttribs.TCKMinPs == 0 {
		memAttribs.TCKMinPs = getDefaultTCKMinPs(memAttribs)
	}
	if memAttribs.TCKMaxPs == 0 {
		memAttribs.TCKMaxPs = speedBinToSPDEncoding[memAttribs.SpeedMTps].TCKMaxPs
	}
}

func updateTWRMin(memAttribs *memAttributes) {
	if memAttribs.TWRMinPs == 0 {
		memAttribs.TWRMinPs = TimingValueTWRMinPs
	}
}

func updateTWTRMin(memAttribs *memAttributes) {
	if memAttribs.TWTRLMinPs == 0 {
		memAttribs.TWTRLMinPs = TimingValueTWTRLMinPs
	}
	if memAttribs.TWTRSMinPs == 0 {
		memAttribs.TWTRSMinPs = TimingValueTWTRSMinPs
	}
}

func updateMemoryAttributes(memAttribs *memAttributes) {
	updateDieBusWidth(memAttribs)
	updateTCK(memAttribs)
	updateTAAMin(memAttribs)
	updateTRCDMin(memAttribs)
	updateTRPMin(memAttribs)
	updateTRASMin(memAttribs)
	updateTRCMin(memAttribs)
	updateTWRMin(memAttribs)
	updateTWTRMin(memAttribs)
	updateCAS(memAttribs)
	updateTRFC1Min(memAttribs)
	updateTRFC2Min(memAttribs)
	updateTRFC4Min(memAttribs)
	updateTCCDLMin(memAttribs)
	updateTRRDSMin(memAttribs)
	updateTRRDLMin(memAttribs)
	updateTFAWMin(memAttribs)
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
	fmt.Printf("   platform = SoC Platform for which the SPDs are being generated\n\n\n")
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
		updateMemoryAttributes(&memParts.MemParts[i].Attribs)
		if dedupeMemoryPart(dedupedParts, &memParts.MemParts[i]) == false {
			generateSPD(&memParts.MemParts[i], SPDId, SPDDir)
			SPDId++
			dedupedParts = append(dedupedParts, &memParts.MemParts[i])
		}
	}

	generateEmptySPD(SPDDir)

	if err := writeSPDManifest(&memParts, SPDDir); err != nil {
		log.Fatal(err)
	}
}
