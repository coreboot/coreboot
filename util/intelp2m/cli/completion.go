package cli

import (
	"flag"
	"fmt"
	"os"
	"strings"
)

func UseComplete() bool {
	if _, ok := os.LookupEnv("COMP_LINE"); ok {
		argument := os.Args[2]
		argument = strings.TrimLeft(argument, "-")
		flag.VisitAll(func(f *flag.Flag) {
			if argument == "" || strings.HasPrefix(f.Name, argument) {
				fmt.Println("-" + f.Name)
			}
		})
		return true
	}
	return false
}
