package main

import (
	"cbfs"
	"cbtables"
	"flag"
	"fmt"
	"io/ioutil"
	"kconfig"
	"log"
	"os"
	"os/exec"
)

var ClobberDir = flag.Bool("clobber", false, "Clobber temporary output when finished. Useful for debugging.")

func RunAndSave(output string, name string, arg ...string) {
	cmd := exec.Command(name, arg...)

	f, err := os.Create(output)
	if err != nil {
		log.Fatal(err)
	}

	cmd.Stdout = f
	cmd.Stderr = f

	err = cmd.Start()
	if err != nil {
		log.Fatal(err)
	}
	cmd.Wait()
}

/* Missing features: serial, upload, ssh */

func main() {
	flag.Parse()

	cb, err := cbfs.OpenROM()
	if err != nil {
		log.Fatal(err)
	}
	config, err := cb.GetFile("config")
	if err != nil {
		log.Fatal(err)
	}
	revision, err := cb.GetFile("revision")
	if err != nil {
		log.Fatal(err)
	}

	parsedConfig := kconfig.ParseKConfig(config)
	mainboardDir := kconfig.UnQuote(parsedConfig["CONFIG_MAINBOARD_DIR"])

	tempDir, err := ioutil.TempDir("", "coreboot_board_status")
	if err != nil {
		log.Fatal(err)
	}

	tbl, err := cbtables.Open()
	if err != nil {
		log.Fatal(err)
	}

	taggedVersion, err := tbl.GetVersion()
	if err != nil {
		log.Fatal(err)
	}
	versionTimestamp, err := tbl.GetVersionTimestamp()
	if err != nil {
		log.Fatal(err)
	}
	timestampFormated := versionTimestamp.UTC().Format("2006-01-02T15:04:05Z")
	outputDir := tempDir + "/" + mainboardDir + "/" + taggedVersion + "/" + timestampFormated
	os.MkdirAll(outputDir, 0755)
	fmt.Printf("Temporarily placing output in %s\n", outputDir)
	cf, err := os.Create(outputDir + "/cbfs.txt")
	if err != nil {
		log.Fatal(err)
	}
	defer cf.Close()
	fmt.Fprintf(cf, "%v", cb)

	cf, err = os.Create(outputDir + "/config.txt")
	if err != nil {
		log.Fatal(err)
	}
	defer cf.Close()
	cf.Write(config)

	cf, err = os.Create(outputDir + "/revision.txt")
	if err != nil {
		log.Fatal(err)
	}
	defer cf.Close()
	cf.Write(revision)

	RunAndSave(outputDir+"/kernel_log.txt", "dmesg")

	cons, lost, err := tbl.GetConsole()
	if err != nil {
		log.Fatal(err)
	}

	cf, err = os.Create(outputDir + "/coreboot_console.txt")
	if err != nil {
		log.Fatal(err)
	}
	defer cf.Close()
	cf.Write(cons)
	switch lost {
	case 0:
	case 1:
		fmt.Fprintf(cf, "\none byte lost\n")
	default:
		fmt.Fprintf(cf, "\n%d bytes lost\n", lost)
	}
	timest, err := tbl.GetTimestamps()
	if err != nil {
		log.Fatal(err)
	}

	ts, err := os.Create(outputDir + "/coreboot_timestamps.txt")
	if err != nil {
		log.Fatal(err)
	}
	defer ts.Close()
	fmt.Fprintf(ts, "%v", timest)

	if *ClobberDir {
		os.RemoveAll(tempDir)
	}
}
