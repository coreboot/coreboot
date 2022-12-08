package printer

import (
	"fmt"
	"os"
	"path/filepath"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
)

func Do(rows []string) error {
	path := p2m.Config.OutputPath
	if err := os.MkdirAll(filepath.Dir(path), os.ModePerm); err != nil {
		logs.Errorf("failed to create output directory: %v", err)
		return err
	}

	file, err := os.Create(path)
	if err != nil {
		logs.Errorf("failed to create output file: %v", err)
		return err
	}
	defer file.Close()

	logs.Infof("write lines to file %s", path)
	for i := range rows {
		if _, err := fmt.Fprint(file, rows[i]); err != nil {
			logs.Errorf("failed to write: %v", err)
			return err
		}
	}
	logs.Infof("successfully completed: %d rows", len(rows))
	return nil
}
