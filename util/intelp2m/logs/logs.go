package logs

import (
	"fmt"
	"log"
	"os"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
)

var (
	linfo    *log.Logger
	lwarning *log.Logger
	lerror   *log.Logger
)

func Init() (*os.File, error) {

	flags := os.O_RDWR | os.O_CREATE | os.O_APPEND
	file, err := os.OpenFile(p2m.Config.LogsPath, flags, 0666)
	if err != nil {
		fmt.Printf("logs: error opening %s file: %v", p2m.Config.LogsPath, err)
		return nil, err
	}

	attributes := log.Lshortfile
	linfo = log.New(file, "INFO:    ", attributes)
	lwarning = log.New(file, "WARNING: ", attributes)
	lerror = log.New(file, "ERROR:   ", attributes)
	return file, nil
}

func Infof(format string, v ...any) {
	if linfo != nil {
		linfo.Output(2, fmt.Sprintf(format, v...))
	}
}

func Warnf(format string, v ...any) {
	if lwarning != nil {
		lwarning.Output(2, fmt.Sprintf(format, v...))
	}
}

func Errorf(format string, v ...any) {
	if lerror != nil {
		lerror.Output(2, fmt.Sprintf(format, v...))
	}
	log.Output(2, fmt.Sprintf(format, v...))
}
