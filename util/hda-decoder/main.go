// SPDX-License-Identifier: GPL-2.0-only
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"review.coreboot.org/coreboot.git/util/hda-decoder/decoder"
	"strconv"
	"strings"
)

type decodeOperation int

const (
	decodeToHumanReadable decodeOperation = iota
	decodeToVerbs
)

var indentLevel int = 0

func indentedPrintf(format string, args ...interface{}) (n int, err error) {
	s := fmt.Sprintf("%s%s", strings.Repeat("\t", indentLevel), format)
	return fmt.Printf(s, args...)
}

func stringToUint32(s string) uint32 {
	s = strings.Replace(s, "0x", "", -1)
	v, err := strconv.ParseUint(s, 16, 32)
	if err != nil {
		log.Fatal(err)
	}
	return uint32(v)
}

func decodeConfig(config uint32) {
	out := decoder.ToHumanReadable(decoder.Decode(config))

	indentedPrintf("%s,\n", out.PortConnectivity)
	indentedPrintf("%s,\n", out.Location)
	indentedPrintf("%s,\n", out.DefaultDevice)
	indentedPrintf("%s,\n", out.ConnectionType)
	indentedPrintf("%s,\n", out.Color)
	indentedPrintf("%s,\n", out.Misc)
	indentedPrintf("%s, %s\n", out.DefaultAssociation, out.Sequence)
}

func printDisconnectedPort(config uint32) {
	// The value 0x411111f0 is not defined in the specification, but is a
	// common value vendors use to indicate "not connected".
	const nc uint32 = 0x411111f0

	// Setting some values (e.g. 0x40000000) as `AZALIA_PIN_CFG_NC(0)` is
	// probably harmless. However, we will stay on the safe side for now.
	if (config & 0xfffffff0) != nc {
		// Do not decode these values, as they would likely describe a
		// bogus device which could be slighly confusing.
		fmt.Printf("0x%08x), // does not describe a jack or internal device\n", config)
	} else {
		fmt.Printf("AZALIA_PIN_CFG_NC(%d)),\n", (config & 0x0000000f))
	}
}

func decodeFile(path string, codec uint32, operation decodeOperation) {
	file, err := os.Open(path)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	for scanner.Scan() {
		fields := strings.Fields(scanner.Text())
		if len(fields) != 2 {
			fmt.Print("// Something went wrong\n")
			continue
		}

		pin := stringToUint32(fields[0])
		config := stringToUint32(fields[1])

		switch operation {
		case decodeToVerbs:
			fmt.Printf("address: %d, node ID: %#02x, configuration default: %#08x\n",
				codec, pin, config)

			verbs := decoder.ConfigToVerbs(codec, pin, config)
			fmt.Printf("  %#08x\n", verbs[0])
			fmt.Printf("  %#08x\n", verbs[1])
			fmt.Printf("  %#08x\n", verbs[2])
			fmt.Printf("  %#08x\n", verbs[3])
		case decodeToHumanReadable:
			indentedPrintf("AZALIA_PIN_CFG(%d, 0x%02x, ", codec, pin)
			if decoder.PortIsConnected(config) {
				fmt.Printf("AZALIA_PIN_DESC(\n")
				indentLevel += 1
				decodeConfig(config)
				indentLevel -= 1
				indentedPrintf(")),\n")
			} else {
				printDisconnectedPort(config)
			}
		}
	}
}

func getFileContents(path string) string {
	contents, err := os.ReadFile(path)
	if err != nil {
		log.Fatal(err)
	}
	return strings.TrimSpace(string(contents))
}

func getLineCount(path string) int {
	return len(strings.Split(getFileContents(path), "\n"))
}

func decodeDeviceCodec(path string, codec uint32, isLastCodec bool, generate bool) {
	if generate {
		vendorId := getFileContents(path + "/vendor_id")
		vendorName := getFileContents(path + "/vendor_name")
		chipName := getFileContents(path + "/chip_name")
		subsystemId := getFileContents(path + "/subsystem_id")
		lineCount := getLineCount(path + "/init_pin_configs")

		indentedPrintf("%s, // Vendor/Device ID: %s %s\n", vendorId, vendorName, chipName)
		indentedPrintf("%s, // Subsystem ID\n", subsystemId)
		indentedPrintf("%d,\n", lineCount+1)
		indentedPrintf("AZALIA_SUBVENDOR(%d, %s),\n\n", codec, subsystemId)
	}

	decodeFile(path+"/init_pin_configs", codec, decodeToHumanReadable)
	if !isLastCodec {
		fmt.Printf("\n")
	}
}

func decodeDeviceCodecs(generate bool) {
	matches, err := filepath.Glob("/sys/class/sound/hwC0D*")
	if err != nil {
		log.Fatal(err)
	}
	re := regexp.MustCompile(`D([0-9]+)$`)

	for i, match := range matches {
		codec := stringToUint32(re.FindStringSubmatch(match)[1])
		isLastCodec := (i + 1) == len(matches)

		decodeDeviceCodec(match, codec, isLastCodec, generate)
	}
}

func isFlagPassed(name string) bool {
	found := false

	flag.Visit(func(f *flag.Flag) {
		if f.Name == name {
			found = true
		}
	})
	return found
}

func main() {
	codec := flag.Uint64("codec", 0, "Set the codec number when decoding a file\n"+
		"This flag is only meaningful in combination with the 'file' flag")
	config := flag.Uint64("config", 0, "Decode a single configuration")
	file := flag.String("file", "", "Decode configurations in a file\n"+
		"The decoder assumes each line in the file has the format: <pin> <config>")
	generate := flag.Bool("generate", false, "Automatically generate hda_verb.c for the host device")
	toVerbs := flag.Bool("to-verbs", false, "Convert configuration defaults to their corresponding verbs\n"+
		"This flag is only meaningful in combination with the 'file' flag")
	flag.Parse()

	operation := decodeToHumanReadable
	if *toVerbs {
		operation = decodeToVerbs
	}

	if isFlagPassed("config") {
		decodeConfig(uint32(*config))
	} else if isFlagPassed("file") {
		decodeFile(*file, uint32(*codec), operation)
	} else {
		if *generate {
			fmt.Printf("/* SPDX-License-Identifier: GPL-2.0-only */\n\n")
			fmt.Printf("#include <device/azalia_device.h>\n\n")
			fmt.Printf("const u32 cim_verb_data[] = {\n")
			indentLevel += 1
		}
		decodeDeviceCodecs(*generate)
		if *generate {
			indentLevel -= 1
			fmt.Printf("};\n\n")
			fmt.Printf("const u32 pc_beep_verbs[] = {};\n")
			fmt.Printf("AZALIA_ARRAY_SIZES;\n")
		}
	}
}
