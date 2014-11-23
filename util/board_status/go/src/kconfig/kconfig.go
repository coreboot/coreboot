package kconfig

import (
	"bufio"
	"bytes"
	"strings"
)

func ParseKConfig(raw []byte) map[string]string {
	buffer := bytes.NewBuffer(raw)

	scanner := bufio.NewScanner(buffer)
	ret := map[string]string{}
	for scanner.Scan() {
		line := scanner.Text()
		if line[0] == '#' {
			continue
		}
		idx := strings.Index(line, "=")
		if idx < 0 {
			continue
		}
		ret[line[0:idx]] = line[idx+1:]
	}
	return ret
}

func UnQuote(in string) string {
	return in[1 : len(in)-1]
}
