package main

import (
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"strings"
)

func readBoardInfo(dir NamedFS) map[string]string {
	result := make(map[string]string)
	c, err := fs.ReadFile(dir.FS, filepath.Join(dir.Name, "board_info.txt"))
	if err != nil {
		return result
	}
	ls := strings.Split(string(c), "\n")
	for _, l := range ls {
		spl := strings.SplitN(l, ":", 2)
		if len(spl) != 2 {
			// This shouldn't ever happen, but let's try to
			// extract as much information from erroneous
			// board_info files (if they exist) as possible.
			continue
		}
		result[strings.TrimSpace(spl[0])] = strings.TrimSpace(spl[1])
	}
	return result
}

func fetchBoards(dirs chan<- NamedFS) {
	defer close(dirs)
	ds, err := fs.Glob(cbdirFS, filepath.Join("src", "mainboard", "*", "*"))
	if err != nil {
		fmt.Fprintf(os.Stderr, "Could not find mainboard directories: %v\n", err)
		return
	}
	for _, d := range ds {
		if _, err := fs.ReadDir(cbdirFS, d); err != nil {
			continue
		}
		dirs <- NamedFS{
			FS:   cbdirFS,
			Name: d,
		}
	}
}

var niceVendors = make(map[string]string)

func getNiceVendor(dir string, vendor string) (string, error) {
	if _, exists := niceVendors[vendor]; !exists {
		c, err := fs.ReadFile(cbdirFS, filepath.Join(dir, "..", "Kconfig.name"))
		if err != nil {
			return "", err
		}
		re, err := regexp.Compile("(?i)config VENDOR_" + vendor)
		if err != nil {
			return "", err
		}
		ls := strings.Split(string(c), "\n")
		next := false
		for _, l := range ls {
			if next {
				niceVendors[vendor] = strings.Split(l, "\"")[1]
				break
			}
			if re.Match([]byte(l)) {
				next = true
			}
		}
	}
	return niceVendors[vendor], nil
}

func readKconfig(dir NamedFS) (string, string, string, string, string, error) {
	var north, south, superio, cpu, partnum string
	c, err := fs.ReadFile(dir.FS, filepath.Join(dir.Name, "Kconfig"))
	if err != nil {
		return north, south, superio, cpu, partnum, err
	}
	ls := strings.Split(string(c), "\n")
	partoffset := 0
	for _, l := range ls {
		l = strings.TrimSpace(l)
		if len(l) < 7 {
			continue
		}
		// TODO: handling of MAINBOARD_PART_NUMBER is rather broken
		// and fragile. Doesn't help that many boards use different
		// part numbers for different models and this code can't
		// figure it out.
		if strings.Contains(strings.ToLower(l), "config mainboard_part_number") {
			partoffset = 2
			continue
		}
		if partoffset > 0 {
			partoffset--
			if strings.Contains(l, "default") {
				partnum = strings.Split(l, "\"")[1]
				continue
			}
		}
		if l[0:7] != "select " {
			continue
		}
		l = l[7:]
		if len(l) > 12 && l[0:12] == "NORTHBRIDGE_" {
			north = l[12:]
			continue
		}
		if len(l) > 12 && l[0:12] == "SOUTHBRIDGE_" {
			if strings.Contains(l, "SKIP_") ||
				strings.Contains(l, "DISABLE_") {
				continue
			}
			south = l[12:]
			continue
		}
		if len(l) > 8 && l[0:8] == "SUPERIO_" {
			superio = l[8:]
			continue
		}
		if len(l) > 4 && (l[0:4] == "CPU_" || l[0:4] == "SOC_") {
			if strings.Contains(l, "AMD_AGESA_FAMILY") ||
				strings.Contains(l, "AMD_COMMON_") ||
				strings.Contains(l, "INTEL_COMMON_") ||
				strings.Contains(l, "INTEL_DISABLE_") ||
				strings.Contains(l, "INTEL_CSE_") ||
				strings.Contains(l, "CPU_MICROCODE_CBFS_NONE") {
				continue
			}
			cpu = l[4:]
		}
	}
	return north, south, superio, cpu, partnum, nil
}

type reReplace struct {
	pattern *regexp.Regexp
	replace string
}

func prettify(input string, rules *[]reReplace) string {
	for _, rule := range *rules {
		input = rule.pattern.ReplaceAllString(input, rule.replace)
	}
	return input
}

var northbridgeRules = []reReplace{
	{
		pattern: regexp.MustCompile("AMD_AGESA_FAMILY([0-9a-fA-F]*)(.*)"),
		replace: "AMD Family ${1}h${2} (AGESA)",
	},
	{
		pattern: regexp.MustCompile("AMD_PI_(.*)"),
		replace: "AMD ${1} (PI)",
	},
	{
		pattern: regexp.MustCompile("INTEL_FSP_(.*)"),
		replace: "Intel® ${1} (FSP)",
	},
	{
		pattern: regexp.MustCompile("AMD_FAMILY([0-9a-fA-F]*)"),
		replace: "AMD Family ${1}h,",
	},
	{
		pattern: regexp.MustCompile("AMD_AMDFAM([0-9a-fA-F]*)"),
		replace: "AMD Family ${1}h",
	},
	{
		pattern: regexp.MustCompile("_"),
		replace: " ",
	},
	{
		pattern: regexp.MustCompile("INTEL"),
		replace: "Intel®",
	},
}

func prettifyNorthbridge(northbridge string) string {
	return prettify(northbridge, &northbridgeRules)
}

var southbridgeRules = []reReplace{
	{
		pattern: regexp.MustCompile("_"),
		replace: " ",
	},
	{
		pattern: regexp.MustCompile("INTEL"),
		replace: "Intel®",
	},
}

func prettifySouthbridge(southbridge string) string {
	return prettify(southbridge, &southbridgeRules)
}

var superIORules = []reReplace{
	{
		pattern: regexp.MustCompile("_"),
		replace: " ",
	},
	{
		pattern: regexp.MustCompile("WINBOND"),
		replace: "Winbond™,",
	},
	{
		pattern: regexp.MustCompile("ITE"),
		replace: "ITE™",
	},
	{
		pattern: regexp.MustCompile("SMSC"),
		replace: "SMSC®",
	},
	{
		pattern: regexp.MustCompile("NUVOTON"),
		replace: "Nuvoton ",
	},
}

func prettifySuperIO(superio string) string {
	return prettify(superio, &superIORules)
}

type cpuMapping struct {
	cpu    string
	socket string
}

var cpuMappings = map[string]cpuMapping{
	"ALLWINNER_A10": {
		cpu:    "Allwinner A10",
		socket: "?",
	},
	"AMD_GEODE_LX": {
		cpu:    "AMD Geode™ LX",
		socket: "—",
	},
	"AMD_SOCKET_754": {
		cpu:    "AMD Sempron™ / Athlon™ 64 / Turion™ 64",
		socket: "Socket 754",
	},
	"AMD_SOCKET_ASB2": {
		cpu:    "AMD Turion™ II Neo/Athlon™ II Neo",
		socket: "ASB2 (BGA812)",
	},
	"AMD_SOCKET_S1G1": {
		cpu:    "AMD Turion™ / X2  Sempron™",
		socket: "Socket S1G1",
	},
	"AMD_SOCKET_G34": {
		cpu:    "AMD Opteron™ Magny-Cours/Interlagos",
		socket: "Socket G34",
	},
	"AMD_SOCKET_G34_NON_AGESA": {
		cpu:    "AMD Opteron™ Magny-Cours/Interlagos",
		socket: "Socket G34",
	},
	"AMD_SOCKET_C32": {
		cpu:    "AMD Opteron™ Magny-Cours/Interlagos",
		socket: "Socket C32",
	},
	"AMD_SOCKET_C32_NON_AGESA": {
		cpu:    "AMD Opteron™ Magny-Cours/Interlagos",
		socket: "Socket C32",
	},
	"AMD_SOCKET_AM2": {
		cpu:    "?",
		socket: "Socket AM2",
	},
	"AMD_SOCKET_AM3": {
		cpu:    "AMD Athlon™ 64 / FX / X2",
		socket: "Socket AM3",
	},
	"AMD_SOCKET_AM2R2": {
		cpu:    "AMD Athlon™ 64 / X2 / FX, Sempron™",
		socket: "Socket AM2+",
	},
	"AMD_SOCKET_F": {
		cpu:    "AMD Opteron™",
		socket: "Socket F",
	},
	"AMD_SOCKET_F_1207": {
		cpu:    "AMD Opteron™",
		socket: "Socket F 1207",
	},
	"AMD_SOCKET_940": {
		cpu:    "AMD Opteron™",
		socket: "Socket 940",
	},
	"AMD_SOCKET_939": {
		cpu:    "AMD Athlon™ 64 / FX / X2",
		socket: "Socket 939",
	},
	"AMD_SC520": {
		cpu:    "AMD Élan™SC520",
		socket: "—",
	},
	"AMD_STONEYRIDGE_FP4": {
		cpu:    "AMD Stoney Ridge",
		socket: "FP4 BGA",
	},
	"ARMLTD_CORTEX_A9": {
		cpu:    "ARM Cortex A9",
		socket: "?",
	},
	"DMP_VORTEX86EX": {
		cpu:    "DMP VORTEX86EX",
		socket: "?",
	},
	"MEDIATEK_MT8173": {
		cpu:    "MediaTek MT8173",
		socket: "—",
	},
	"NVIDIA_TEGRA124": {
		cpu:    "NVIDIA Tegra 124",
		socket: "—",
	},
	"NVIDIA_TEGRA210": {
		cpu:    "NVIDIA Tegra 210",
		socket: "—",
	},
	"SAMSUNG_EXYNOS5420": {
		cpu:    "Samsung Exynos 5420",
		socket: "?",
	},
	"SAMSUNG_EXYNOS5250": {
		cpu:    "Samsung Exynos 5250",
		socket: "?",
	},
	"TI_AM335X": {
		cpu:    "TI AM335X",
		socket: "?",
	},
	"INTEL_APOLLOLAKE": {
		cpu:    "Intel® Apollo Lake",
		socket: "—",
	},
	"INTEL_BAYTRAIL": {
		cpu:    "Intel® Bay Trail",
		socket: "—",
	},
	"INTEL_BRASWELL": {
		cpu:    "Intel® Braswell",
		socket: "—",
	},
	"INTEL_BROADWELL": {
		cpu:    "Intel® Broadwell",
		socket: "—",
	},
	"INTEL_DENVERTON_NS": {
		cpu:    "Intel® Denverton-NS",
		socket: "—",
	},
	"INTEL_FSP_BROADWELL_DE": {
		cpu:    "Intel® Broadwell-DE",
		socket: "—",
	},
	"INTEL_GLK": {
		cpu:    "Intel® Gemini Lake",
		socket: "—",
	},
	"INTEL_GEMINILAKE": {
		cpu:    "Intel® Gemini Lake",
		socket: "—",
	},
	"INTEL_ICELAKE": {
		cpu:    "Intel® Ice Lake",
		socket: "—",
	},
	"INTEL_KABYLAKE": {
		cpu:    "Intel® Kaby Lake",
		socket: "—",
	},
	"INTEL_SANDYBRIDGE": {
		cpu:    "Intel® Sandy Bridge",
		socket: "—",
	},
	"INTEL_SKYLAKE": {
		cpu:    "Intel® Skylake",
		socket: "—",
	},
	"INTEL_SLOT_1": {
		cpu:    "Intel® Pentium® II/III, Celeron®",
		socket: "Slot 1",
	},
	"INTEL_SOCKET_MPGA604": {
		cpu:    "Intel® Xeon®",
		socket: "Socket 604",
	},
	"INTEL_SOCKET_M": {
		cpu:    "Intel® Core™ 2 Duo Mobile, Core™ Duo/Solo, Celeron® M",
		socket: "Socket M (mPGA478MT)",
	},
	"INTEL_SOCKET_LGA771": {
		cpu:    "Intel Xeon™ 5000 series",
		socket: "Socket LGA771",
	},
	"INTEL_SOCKET_LGA775": {
		cpu:    "Intel® Core 2, Pentium 4/D",
		socket: "Socket LGA775",
	},
	"INTEL_SOCKET_PGA370": {
		cpu:    "Intel® Pentium® III-800, Celeron®",
		socket: "Socket PGA370",
	},
	"INTEL_SOCKET_MPGA479M": {
		cpu:    "Intel® Mobile Celeron",
		socket: "Socket 479",
	},
	"INTEL_HASWELL": {
		cpu:    "Intel® 4th Gen (Haswell) Core i3/i5/i7",
		socket: "?",
	},
	"INTEL_FSP_RANGELEY": {
		cpu:    "Intel® Atom Rangeley (FSP)",
		socket: "?",
	},
	"INTEL_SOCKET_441": {
		cpu:    "Intel® Atom™ 230",
		socket: "Socket 441",
	},
	"INTEL_SOCKET_FC_PGA370": {
		cpu:    "Intel® Pentium® III, Celeron®",
		socket: "Socket PGA370",
	},
	"INTEL_EP80579": {
		cpu:    "Intel® EP80579",
		socket: "Intel® EP80579",
	},
	"INTEL_SOCKET_MFCBGA479": {
		cpu:    "Intel® Mobile Celeron",
		socket: "Socket 479",
	},
	"INTEL_WHISKEYLAKE": {
		cpu:    "Intel® Whiskey Lake",
		socket: "—",
	},
	"QC_IPQ806X": {
		cpu:    "Qualcomm IPQ806x",
		socket: "—",
	},
	"QUALCOMM_QCS405": {
		cpu:    "Qualcomm QCS405",
		socket: "—",
	},
	"ROCKCHIP_RK3288": {
		cpu:    "Rockchip RK3288",
		socket: "—",
	},
	"ROCKCHIP_RK3399": {
		cpu:    "Rockchip RK3399",
		socket: "—",
	},
	"VIA_C3": {
		cpu:    "VIA C3™",
		socket: "?",
	},
	"VIA_C7": {
		cpu:    "VIA C7™",
		socket: "?",
	},
	"VIA_NANO": {
		cpu:    "VIA NANO™",
		socket: "?",
	},
	"QEMU_X86": {
		cpu:    "QEMU x86",
		socket: "—",
	},
}

func prettifyCPU(cpu, north string, northNice string) (string, string) {
	if match, ok := cpuMappings[cpu]; ok {
		return match.cpu, match.socket
	}
	if cpu == "" {
		if match, ok := cpuMappings[north]; ok {
			return match.cpu, match.socket
		}
		if north == "INTEL_IRONLAKE" {
			return "Intel® 1st Gen (Westmere) Core i3/i5/i7", "?"
		}
		if north == "RDC_R8610" {
			return "RDC R8610", "—"
		}
		if (len(north) > 10 && north[0:10] == "AMD_AGESA_") || (len(north) > 7 && north[0:7] == "AMD_PI_") {
			return northNice, "?"
		}
		return north, north
	}
	if cpu == "INTEL_SOCKET_BGA956" {
		if north == "INTEL_GM45" {
			return "Intel® Core 2 Duo (Penryn)", "Socket P"
		}
		return "Intel® Pentium® M", "BGA956"
	}
	if cpu == "INTEL_SOCKET_RPGA989" || cpu == "INTEL_SOCKET_LGA1155" || cpu == "INTEL_SOCKET_RPGA988B" {
		socket := "Socket " + cpu[13:]
		if north == "INTEL_HASWELL" {
			return "Intel® 4th Gen (Haswell) Core i3/i5/i7", socket
		}
		if north == "INTEL_IVYBRIDGE" || north == "INTEL_FSP_IVYBRIDGE" {
			return "Intel® 3rd Gen (Ivybridge) Core i3/i5/i7", socket
		}
		if north == "INTEL_SANDYBRIDGE" {
			return "Intel® 2nd Gen (Sandybridge) Core i3/i5/i7", socket
		}
		return north, socket
	}
	return cpu, cpu
}

func collectBoards(dirs <-chan NamedFS) {
	for dir := range dirs {
		path := strings.Split(dir.Name, string(filepath.Separator))
		vendor, board := path[2], path[3]
		vendorNice, err := getNiceVendor(dir.Name, vendor)

		if err != nil {
			fmt.Fprintf(os.Stderr, "Could not find nice vendor name for %s: %v\n", dir.Name, err)
			continue
		}

		bi := readBoardInfo(dir)
		cat := Category(bi["Category"])
		if _, ok := data.CategoryNiceNames[cat]; !ok {
			cat = "unclass"
		}
		if bi["Vendor cooperation score"] == "" {
			bi["Vendor cooperation score"] = "—"
		}

		venboard := vendor + string(filepath.Separator) + board
		if bi["Clone of"] != "" {
			venboard = bi["Clone of"]
			venboard = strings.ReplaceAll(venboard, "/", string(filepath.Separator))
			newpath := filepath.Join(dir.Name, "..", "..", venboard)
			dir.Name = newpath
		}

		north, south, superio, cpu, partnum, err := readKconfig(dir)
		if err != nil {
			fmt.Fprintf(os.Stderr, "'%s' is not a mainboard directory: %v\n", dir.Name, err)
			// Continue with the path because that's what the
			// shell script did. We might want to change semantics
			// later.
		}
		northbridgeNice := prettifyNorthbridge(north)
		southbridgeNice := prettifySouthbridge(south)
		superIONice := prettifySuperIO(superio)
		cpuNice, socketNice := prettifyCPU(cpu, north, northbridgeNice)

		boardNice := bi["Board name"]
		if boardNice == "" {
			boardNice = partnum
		}
		if boardNice == "" {
			boardNice = strings.ReplaceAll(boardNice, "_", " ")
			boardNice = strings.ToUpper(boardNice)
		}

		b := Board{
			Vendor:                 vendor,
			Vendor2nd:              bi["Vendor name"],
			VendorNice:             vendorNice,
			VendorBoard:            vendor + "/" + board,
			Board:                  board,
			BoardNice:              boardNice,
			BoardURL:               bi["Board URL"],
			NorthbridgeNice:        northbridgeNice,
			SouthbridgeNice:        southbridgeNice,
			SuperIONice:            superIONice,
			CPUNice:                cpuNice,
			SocketNice:             socketNice,
			ROMPackage:             bi["ROM package"],
			ROMProtocol:            bi["ROM protocol"],
			ROMSocketed:            bi["ROM socketed"],
			FlashromSupport:        bi["Flashrom support"],
			VendorCooperationScore: bi["Vendor cooperation score"],
			VendorCooperationPage:  bi["Vendor cooperation page"],
		}
		if b.ROMPackage == "" {
			b.ROMPackage = "?"
		}
		if b.ROMProtocol == "" {
			b.ROMProtocol = "?"
		}

		if data.BoardsByCategory[cat] == nil {
			data.BoardsByCategory[cat] = []Board{}
		}
		data.BoardsByCategory[cat] = append(data.BoardsByCategory[cat], b)
	}
	for ci := range data.BoardsByCategory {
		cat := data.BoardsByCategory[ci]
		sort.Slice(data.BoardsByCategory[ci], func(i, j int) bool {
			if cat[i].Vendor == cat[j].Vendor {
				return cat[i].Board < cat[j].Board
			}
			return cat[i].Vendor < cat[j].Vendor
		})
	}
}
