package main

import (
	"fmt"
	"io/fs"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
	"strings"
	"time"
)

// Color returns a HTML color code between green and yellow based on the
// number of days that passed since ds.
func (ds DateString) Color() string {
	date, _ := time.Parse("2006-01-02T15:04:05Z", string(ds))
	days := int(time.Since(date).Hours() / 24)
	if days > 255 {
		days = 255
	}
	return fmt.Sprintf("#%02xff00", days)
}

func fetchLogs(dirs chan<- NamedFS) {
	err := fs.WalkDir(bsdirFS, ".", func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		if path[0] == '.' {
			return nil
		}
		if d.IsDir() && len(strings.Split(path, string(filepath.Separator))) == 4 {
			dirs <- NamedFS{
				FS:   bsdirFS,
				Name: path,
			}
		}
		return nil
	})
	if err != nil {
		fmt.Fprintf(os.Stderr, "Reading logs failed: %v\n", err)
	}
	close(dirs)
}

func collectLogs(dirs <-chan NamedFS) {
	data.Logs = make(map[Timeframe][]Log)
	data.VendorBoardDate = make(map[string]DateString)
	data.VendorBoardReferenced = make(map[string]bool)
	timeframes := make(map[Timeframe]bool)
	gitcache := make(map[string]string)
	for dir := range dirs {
		upstream := ""
		revB, err := fs.ReadFile(dir.FS, filepath.Join(dir.Name, "revision.txt"))
		if err != nil {
			continue
		}
		rev := string(revB)
		skipDir := false
		for _, line := range strings.Split(rev, "\n") {
			item := strings.SplitN(line, ":", 2)
			if len(item) != 2 {
				// This is an error, but let's try to extract
				// as much value out of revision.txt files as
				// possible, even if some lines are erroneous.
				continue
			}
			if item[0] == "Upstream revision" {
				upstream = strings.TrimSpace(item[1])
				// tried using go-git, but its resolver
				// couldn't expand short hashes despite the
				// docs claiming that it can.
				if val, ok := gitcache[upstream]; ok {
					upstream = val
				} else {
					res, err := exec.Command("/usr/bin/git", "-C", cbdir, "log", "-n1", "--format=%H", upstream).Output()
					if err != nil {
						fmt.Fprintf(os.Stderr, "revision %s not found \n", upstream)
						skipDir = true
						break
					}
					gitcache[upstream] = strings.TrimSpace(string(res))
					upstream = gitcache[upstream]
				}
			}
		}
		if skipDir {
			continue
		}

		rawFiles, err := fs.Glob(dir.FS, filepath.Join(dir.Name, "*"))
		if err != nil {
			fmt.Fprintf(os.Stderr, "Could not fetch log data, skipping: %v\n", err)
			continue
		}

		pieces := strings.Split(dir.Name, string(filepath.Separator))
		if len(pieces) < 4 {
			fmt.Fprintf(os.Stderr, "log directory %s is malformed, skipping\n", dir.Name)
			continue
		}
		vendorBoard := strings.Join(pieces[:2], "/")
		// TODO: these need to become "second to last" and "last" item
		// but only after compatibility to the current system has been ensured.
		dateTimePath := pieces[3]
		dateTime, err := time.Parse(time.RFC3339, strings.ReplaceAll(dateTimePath, "_", ":"))
		if err != nil {
			fmt.Fprintf(os.Stderr, "Could not parse timestamp from %s: %v\n", dir.Name, err)
			continue
		}
		dateTimeNormal := dateTime.UTC().Format("2006-01-02T15:04:05Z")
		dateTimeHuman := dateTime.UTC().Format(time.UnixDate)
		tfYear, tfWeek := dateTime.ISOWeek()
		timeframe := Timeframe(fmt.Sprintf("%dW%02d", tfYear, tfWeek))

		if !timeframes[timeframe] {
			timeframes[timeframe] = true
			data.Timeframes = append(data.Timeframes, timeframe)
			data.Logs[timeframe] = []Log{}
		}

		files := []Path{}
		l := len(dir.Name) + 1
		for _, file := range rawFiles {
			if file[l:] == "revision.txt" {
				continue
			}
			files = append(files, Path{
				Path:     dir.Name + "/",
				Basename: file[l:],
			})
		}

		data.Logs[timeframe] = append(data.Logs[timeframe], Log{
			VendorBoard:  vendorBoard,
			Time:         dateTimeNormal,
			TimeReadable: dateTimeHuman,
			Upstream:     upstream,
			Files:        files,
		})
	}
	sort.Slice(data.Timeframes, func(i, j int) bool {
		// reverse sort
		return data.Timeframes[i] > data.Timeframes[j]
	})
	for bi := range data.Logs {
		bucket := data.Logs[bi]
		sort.Slice(data.Logs[bi], func(i, j int) bool {
			if bucket[i].Time == bucket[j].Time {
				return (bucket[i].VendorBoard > bucket[j].VendorBoard)
			}
			return (bucket[i].Time > bucket[j].Time)
		})
	}
	for _, ts := range data.Timeframes {
		for li, l := range data.Logs[ts] {
			if _, match := data.VendorBoardDate[l.VendorBoard]; match {
				continue
			}
			data.VendorBoardDate[l.VendorBoard] = DateString(l.Time)
			data.Logs[ts][li].Reference = true
		}
	}
}
