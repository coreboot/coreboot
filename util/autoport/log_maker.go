package main

import (
	"io"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"strings"
)

func TryRunAndSave(output string, name string, arg []string) error {
	cmd := exec.Command(name, arg...)

	f, err := os.Create(output)
	if err != nil {
		log.Fatal(err)
	}

	cmd.Stdout = f
	cmd.Stderr = f

	err = cmd.Start()
	if err != nil {
		return err
	}
	cmd.Wait()
	return nil
}

func RunAndSave(output string, name string, arg ...string) {
	err := TryRunAndSave(output, name, arg)
	if err == nil {
		return
	}
	idx := strings.LastIndex(name, "/")
	relname := name
	if idx >= 0 {
		relname = name[idx+1:]
	}
	relname = "./" + relname
	err = TryRunAndSave(output, relname, arg)
	if err != nil {
		log.Fatal(err)
	}
}

func MakeLogs(outDir string) {
	os.MkdirAll(outDir, 0700)
	RunAndSave(outDir+"/lspci.log", "lspci", "-nnvvvxxxx")
	RunAndSave(outDir+"/dmidecode.log", "dmidecode")
	RunAndSave(outDir+"/acpidump.log", "acpidump")
	/* FIXME:XX */
	RunAndSave(outDir+"/inteltool.log", "../inteltool/inteltool", "-a")
	RunAndSave(outDir+"/ectool.log", "../ectool/ectool")

	SysDir := "/sys/class/sound/card0/"
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

	ProcDir := "/proc/asound/card0/"
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
	files, _ = ioutil.ReadDir(ClassInputDir)
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
