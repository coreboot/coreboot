package main

import (
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"strings"
	"bytes"
)

type LogMakingProgram struct {
	name string
	prefixes []string
	args []string
}

func (prog LogMakingProgram) TryRunAndSave(output string, prefix string) error {
	f, err := os.Create(output)
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()

	cmd := exec.Command(prefix+prog.name, prog.args...)
	cmd.Stdout = f
	cmd.Stderr = f

	err = cmd.Start()
	if err != nil {
		return err
	}
	return cmd.Wait()
}

func (prog LogMakingProgram) RunAndSave(outDir string) {
	output := fmt.Sprintf("%s/%s.log", outDir, prog.name)
	cmdline := strings.Join(append([]string{prog.name}, prog.args...), " ")

	fmt.Println("Running: "+cmdline)

	var sb strings.Builder
	for _, prefix := range prog.prefixes {
		err := prog.TryRunAndSave(output, prefix)
		if err == nil {
			return
		}
		sb.WriteString("\nError running '"+prefix+cmdline+"': "+err.Error()+"\n")
		data, ferr := os.ReadFile(output)
		if ferr != nil {
			sb.WriteString("<failed to open log>\n")
		} else {
			if len(data) > 0 {
				sb.WriteString("Program output:\n\n")
				sb.WriteString(string(data))
			}
		}
	}

	fmt.Println("\nCould not run program: '"+cmdline+"'")
	log.Fatal(sb.String())
}

const MAXPROMPTRETRY = 3

func PromptUser(prompt string, opts []string) (match string, err error) {
	for i := 1; i < MAXPROMPTRETRY; i++ {
		fmt.Printf("%s. (%s) Default:%s\n", prompt,
		           strings.Join(opts, "/"), opts[0])
		var usrInput string
		fmt.Scanln(&usrInput)

		// Check for default entry
		if usrInput == "" {
			match = opts[0]
			return
		}

		for _, opt := range opts {
			if opt == usrInput {
				match = opt
				return
			}
		}
	}
	err = errors.New("max retries exceeded")
	fmt.Fprintln(os.Stderr, "ERROR: max retries exceeded")
	return
}

func AppendYesNo(yesFirst bool, yeah []string, nope []string) []string {
	if yesFirst {
		return append(yeah, nope...)
	} else {
		return append(nope, yeah...)
	}
}

func PromptUserBool(prompt string, fallback bool) bool {
	yeah := []string{"y", "yes"}
	nope := []string{"n", "no"}

	opt, err := PromptUser(prompt, AppendYesNo(fallback, yeah, nope))
	if err != nil {
		// Continue even if there is an error
		return fallback
	}
	for _, val := range yeah {
		if opt == val {
			return true
		}
	}
	return false
}

func MakeHDALogs(outDir string, cardName string) {
	SysDir := "/sys/class/sound/" + cardName + "/"
	files, _ := ioutil.ReadDir(SysDir)
	for _, f := range files {
		if (strings.HasPrefix(f.Name(), "hw") || strings.HasPrefix(f.Name(), "hdaudio")) && f.IsDir() {
			in, err := os.Open(SysDir + f.Name() + "/init_pin_configs")
			defer in.Close()
			if err != nil {
				log.Fatal(err)
			}
			out, err := os.Create(outDir + "/pin_" + strings.Replace(f.Name(), "hdaudio", "hw", -1))
			if err != nil {
				log.Fatal(err)
			}
			defer out.Close()
			io.Copy(out, in)
		}
	}

	ProcDir := "/proc/asound/" + cardName + "/"
	files, _ = ioutil.ReadDir(ProcDir)
	for _, f := range files {
		if strings.HasPrefix(f.Name(), "codec#") && !f.IsDir() {
			in, err := os.Open(ProcDir + f.Name())
			defer in.Close()
			if err != nil {
				log.Fatal(err)
			}
			out, err := os.Create(outDir + "/" + f.Name())
			if err != nil {
				log.Fatal(err)
			}
			defer out.Close()
			io.Copy(out, in)
		}
	}
}

func MakeLogs(outDir string) {
	os.MkdirAll(outDir, 0700)

	probeGFX := PromptUserBool("WARNING: Running inteltool MAY cause your system to hang when it attempts "+
		"to probe for graphics registers.  Having the graphics registers will help create a better port. "+
		"Should autoport probe these registers?",
		true)

	inteltoolArgs := "-a"
	if probeGFX {
		inteltoolArgs += "f"
	}

	var programs = []LogMakingProgram {
		LogMakingProgram {
			name: "lspci",
			prefixes: []string{""},
			args: []string{"-nnvvvxxxx"},
		},
		LogMakingProgram {
			name: "dmidecode",
			prefixes: []string{""},
			args: []string{},
		},
		LogMakingProgram {
			name: "acpidump",
			prefixes: []string{""},
			args: []string{},
		},
		LogMakingProgram {
			name: "inteltool",
			prefixes: []string{"../inteltool/", ""},
			args: []string{inteltoolArgs},
		},
		LogMakingProgram {
			name: "ectool",
			prefixes: []string{"../ectool/", ""},
			args: []string{"-pd"},
		},
		LogMakingProgram {
			name: "superiotool",
			prefixes: []string{"../superiotool/", ""},
			args: []string{"-ade"},
		},
	}

	fmt.Println("Making logs...")
	for _, prog := range programs {
		prog.RunAndSave(outDir)
	}

	SysSound := "/sys/class/sound/"
	card := ""
	cards, _ := ioutil.ReadDir(SysSound)
	for _, f := range cards {
		if strings.HasPrefix(f.Name(), "card") {
			cid, err := ioutil.ReadFile(SysSound + f.Name() + "/id")
			if err == nil && bytes.Equal(cid, []byte("PCH\n")) {
				fmt.Fprintln(os.Stderr, "PCH sound card is", f.Name())
				card = f.Name()
			}
		}
	}

	if card != "" {
		MakeHDALogs(outDir, card)
	} else {
		fmt.Fprintln(os.Stderr, "HDAudio not found on PCH.")
	}

	for _, fname := range []string{"cpuinfo", "ioports"} {
		in, err := os.Open("/proc/" + fname)
		defer in.Close()
		if err != nil {
			log.Fatal(err)
		}
		out, err := os.Create(outDir + "/" + fname + ".log")
		if err != nil {
			log.Fatal(err)
		}
		defer out.Close()
		io.Copy(out, in)
	}

	out, err := os.Create(outDir + "/input_bustypes.log")
	if err != nil {
		log.Fatal(err)
	}
	defer out.Close()

	ClassInputDir := "/sys/class/input/"
	files, _ := ioutil.ReadDir(ClassInputDir)
	for _, f := range files {
		if strings.HasPrefix(f.Name(), "input") && !f.Mode().IsRegular() { /* Allow both dirs and symlinks.  */
			in, err := os.Open(ClassInputDir + f.Name() + "/id/bustype")
			defer in.Close()
			if err != nil {
				log.Fatal(err)
			}
			io.Copy(out, in)
		}
	}
}
