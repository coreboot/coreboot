package generator

import (
	"fmt"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/parser"
)

type collector []string

func (c *collector) Line(str string) {
	*c = append(*c, str)
}

func (c *collector) Linef(format string, args ...interface{}) {
	*c = append(*c, fmt.Sprintf(format, args...))
}

// Run() generates strings with macros
func Run(entries []parser.Entry) ([]string, error) {
	if len(entries) == 0 {
		err := fmt.Errorf("entries array is empty")
		logs.Errorf("%v", err)
		return nil, err
	}
	collection := make(collector, 0)
	logs.Infof("run")
	for _, entry := range entries {
		switch entry.EType {
		case parser.EntryGroup:
			collection.Line("\n")
			collection.Linef("\t/* %s */\n", entry.Function)

		case parser.EntryReserved:
			if p2m.Config.GenLevel >= 2 {
				collection.Line("\n")
			}
			collection.Linef("\t/* %s - %s */\n", entry.ID, entry.Function)

		case parser.EntryPad:
			if p2m.Config.GenLevel >= 2 {
				collection.Line("\n")
				collection.Linef("\t/* %s - %s */\n", entry.ID, entry.Function)
				collection.Linef("\t/* DW0: 0x%0.8x, DW1: 0x%0.8x */\n", entry.DW0, entry.DW1)
			}
			lines := entry.ToMacro()
			if p2m.Config.GenLevel == 1 && len(lines) != 0 {
				collection.Linef("\t%s\t/* %s */\n", lines[0], entry.Function)
				break
			}

			for i := range lines {
				collection.Linef("\t%s\n", lines[i])
			}

		default:
			logs.Errorf("unknown entry type: %d", int(entry.EType))
		}
	}
	logs.Infof("successfully completed: %d rows", len(collection))
	return collection, nil
}
