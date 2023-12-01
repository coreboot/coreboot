package cli

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
)

var name = filepath.Base(os.Args[0])

const usagePlatform = `usage: -platform <type>
    type: adl | apl | cnl | ebg | ehl | jsl | lbg | mtl | snr | tgl | ?
        adl - Alder Lake PCH
        apl - Apollo Lake SoC
        cnl - CannonLake-LP or Whiskeylake/Coffeelake/Cometlake-U SoC
        ebg - Emmitsburg PCH with Xeon SP
        ehl - Elkhart Lake SoC
        jsl - Jasper Lake SoC
        lbg - Lewisburg PCH with Xeon SP
        mtl - MeteorLake SoC
        snr - Sunrise PCH or Skylake/Kaby Lake SoC
        tgl - TigerLake-H SoC
        ?   - show details
`
const usageFields = `usage: -fields <type>
    type: cb | fsp | raw | ?
        cb  - coreboot style
        fsp - Intel FSP style
        raw - raw register value (don't convert, print as is)
        ?   - show details
`
const usageExclude = `usage: -e | -exclude
    Exclude fields that should be ignored to generate "short" macro.
    Details:
        The utility should ignore "PAD_TRIG(OFF)" field in the "_PAD_CFG_STRUCT(GPP_I12,
        PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | 1, 0)"
        "long" macro to generate the "PAD_CFG_GPO(GPP_I12, 1, PLTRST)" macro.

        The utility with this option generates the "long" macro with only those fields that are
        required to create the "short" one (in thia case without the "PAD_TRIG(OFF)" field):
        "_PAD_CFG_STRUCT(GPP_I12, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_BUF(RX_DISABLE) | 1, 0)"
`

const usageUnchecked = `usage: -u | -unchecked
    Disable automatic bitfield checking before generating.
    Details:
        The utility automatically checks the bit fields of the DW registers before generating
        the macro. If a bit is set in the register and it is not used in the "short" macro, the
        utility will generate the "long" one instead of the first one.

        This option is used to disable automatic verification. In this case, all macros are
        generated in the "short" format:
            PAD_NC(GPP_F18, NONE),
            PAD_CFG_NF(GPP_F19, NONE, PLTRST, NF1),
            ...
`

const usageInfo = `usage: -i | -ii | -iii | -iiii
    Generate additional information about macros.
    Level 1 information (-i):
        _PAD_CFG_STRUCT(GPP_A4, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | (1 << 1), 0), /* LAD3 */

    Level 2 information (-ii):
        /* GPP_A4 - LAD3 */
        /* DW0: 0x80000402, DW1: 0x00000000 */
        _PAD_CFG_STRUCT(GPP_A4, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | (1 << 1), 0),

    Level 3 information (-iii):
        /* GPP_A3 - LAD2 */
        /* DW0: 0x80000402, DW1: 0x00000000 */
        /* DW0: (1 << 1) - IGNORED */
        _PAD_CFG_STRUCT(GPP_A3, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | (1 << 1), 0),

    Level 4 information (-iiii):
        /* GPP_A4 - LAD3 */
        /* DW0: 0x80000402, DW1: 0x00000000 */
        /* DW0: (1 << 1) - IGNORED */
        /* PAD_CFG_NF(GPP_A4, NONE, PLTRST, NF1), */
        _PAD_CFG_STRUCT(GPP_A4, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | (1 << 1), 0),
`

const usage = `
basic functions:
    -h | -help              Print help
    -v | -version           Print version
         -file <path>       Set path to the inteltool file. <inteltool.log> by default
         -out <path>        Set path to the generated file. <generate/gpio.h> by default
         -logs <path>       Override the log file path. <logs.txt> by default
    -p | -platform <type>   Set the PCH platform type. <sunrise> by default
                            (enter ? to show datails)

generation of long composite macros with bit field definitions:
    -f | -fields <type>     Set the bitfield type for the generated long macro
                            (enter ? to show datails)

control flags:
    -e | -exclude           Exclude fields that should be ignored to generate short macro
    -u | -unchecked         Disable automatic bitfield checking before generating
                            (enter ? to show datails)

generate additional information:
    -i                      Add function to the comments
    -ii                     Add DW0/DW1 register value to the comments
    -iii                    Add ignored bitfields to the comments
    -iiii                   Add target PAD_CFG() macro to the comments
                            (enter ? to show datails)
`

func Usage() {
	fmt.Printf(`usage: %s <switches> %s`, name, usage)
}

func cbOptionsPlatform(value string) error {
	if value == "?" {
		fmt.Printf("%s", usagePlatform)
		os.Exit(0)
	}

	if err := p2m.SetPlatformType(value); err != nil {
		fmt.Printf("error: %v\n%s", err, usagePlatform)
		os.Exit(0)
	}

	return nil
}

func cbOptionsFields(value string) error {
	if value == "?" {
		fmt.Printf("%s", usageFields)
		os.Exit(0)
	}

	if err := p2m.SetFieldType(value); err != nil {
		fmt.Printf("error: %v\n%s", err, usageFields)
		os.Exit(0)
	}

	return nil
}

func cbOptionsExclude(_ string) error {
	if numArgs := flag.NArg(); numArgs == 1 {
		if flag.Arg(0) == "?" {
			fmt.Printf("%s", usageExclude)
		}
		Usage()
		os.Exit(0)
	}
	p2m.Config.IgnoredFields = true
	return nil
}

func cbOptionsUnchecked(_ string) error {
	if numArgs := flag.NArg(); numArgs == 1 {
		if flag.Arg(0) == "?" {
			fmt.Printf("%s", usageUnchecked)
		}
		Usage()
		os.Exit(0)
	}
	p2m.Config.AutoCheck = false
	return nil
}

func cbOptionsInfo(_ string) error {
	if numArgs := flag.NArg(); numArgs == 1 {
		if flag.Arg(0) == "?" {
			fmt.Printf("%s", usageInfo)
			os.Exit(0)
		}
		Usage()
		os.Exit(0)
	}

	table := map[string]int{
		"-i":    1,
		"-ii":   2,
		"-iii":  3,
		"-iiii": 4,
	}
	for _, arg := range os.Args {
		if level, exist := table[arg]; exist {
			p2m.Config.GenLevel = level
			return nil
		}
	}
	Usage()
	return nil
}

func ParseOptions() {
	flag.Usage = Usage

	flag.StringVar(&p2m.Config.InputPath, "file", "inteltool.log", "")
	flag.StringVar(&p2m.Config.OutputPath, "out", "output/gpio.h", "")
	flag.StringVar(&p2m.Config.LogsPath, "logs", "logs.txt", "")

	help := flag.Bool("help", false, "")

	vers, v := flag.Bool("version", false, ""), flag.Bool("v", false, "")

	flag.Func("platform", usagePlatform, cbOptionsPlatform)
	flag.Func("p", usagePlatform, cbOptionsPlatform)

	flag.Func("fields", usageFields, cbOptionsFields)
	flag.Func("f", usageFields, cbOptionsFields)

	flag.BoolFunc("exclude", usageExclude, cbOptionsExclude)
	flag.BoolFunc("e", usageExclude, cbOptionsExclude)

	flag.BoolFunc("unchecked", usageUnchecked, cbOptionsUnchecked)
	flag.BoolFunc("u", usageUnchecked, cbOptionsUnchecked)

	flag.BoolFunc("i", usageInfo, cbOptionsInfo)
	flag.BoolFunc("ii", usageInfo, cbOptionsInfo)
	flag.BoolFunc("iii", usageInfo, cbOptionsInfo)
	flag.BoolFunc("iiii", usageInfo, cbOptionsInfo)

	if UseComplete() {
		os.Exit(0)
	}

	flag.Parse()

	if *help {
		Usage()
		os.Exit(0)
	}

	if *vers || *v {
		fmt.Println(p2m.Config.Version)
		os.Exit(0)
	}
}
