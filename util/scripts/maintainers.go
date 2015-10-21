/*
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path/filepath"
)

type subsystem struct {
	name       string
	maintainer []string
	file       []string
}

var subsystems []subsystem

func get_git_files() ([]string, error) {
	var files []string

	/* Read in list of all files in the git repository */
	cmd := exec.Command("git", "ls-files")
	out, err := cmd.StdoutPipe()
	if err != nil {
		log.Fatalf("git ls-files failed: %v", err)
		return files, err
	}
	if err := cmd.Start(); err != nil {
		log.Fatalf("Could not start %v: %v", cmd, err)
		return files, err
	}

	r := bufio.NewScanner(out)
	for r.Scan() {
		/* Cut out leading tab */
		files = append(files, r.Text())
	}

	cmd.Wait()

	return files, nil
}

func get_maintainers() ([]string, error) {
	var maintainers []string

	/* Read in all maintainers */
	file, err := os.Open("MAINTAINERS")
	if err != nil {
		log.Fatalf("Can't open MAINTAINERS file: %v", err)
		log.Fatalf("Are you running from the top-level directory?")
		return maintainers, err
	}
	defer file.Close()

	keep := false
	s := bufio.NewScanner(file)
	for s.Scan() {
		/* Are we in the "data" section and have a non-empty line? */
		if keep && s.Text() != "" {
			maintainers = append(maintainers, s.Text())
		}
		/* Skip everything before the delimiter */
		if s.Text() == "\t\t-----------------------------------" {
			keep = true
		}
	}

	return maintainers, nil
}

func build_maintainers(maintainers []string) {
	var current *subsystem
	for _, line := range maintainers {
		if line[1] != ':' {
			/* Create new subsystem entry */
			var tmp subsystem
			subsystems = append(subsystems, tmp)
			current = &subsystems[len(subsystems)-1]
			current.name = line
		} else {
			switch line[0] {
			case 'R':
			case 'M':
				{
					/* Add subsystem maintainer */
					current.maintainer =
						append(current.maintainer,
							line[3:len(line)])
					break
				}
			case 'S':
				{
					break
				}
			case 'L':
				{
					break
				}
			case 'T':
				{
					break
				}
			case 'F':
				{
					// add files
					current.file =
						append(current.file,
							line[3:len(line)])
					break
				}
			default:
				{
					fmt.Println("No such specifier: ", line)
					break
				}
			}
		}
	}
}

func print_maintainers() {
	for _, subsystem := range subsystems {
		fmt.Println(subsystem.name)
		fmt.Println("  ", subsystem.maintainer)
		fmt.Println("  ", subsystem.file)
	}
}

func match_file(fname string, files []string) (bool, error) {
	var matched bool
	var err error

	for _, file := range files {
		/* Direct match */
		matched, err = filepath.Match(file, fname)
		if err != nil {
			return false, err
		}
		if matched {
			return true, nil
		}

		/* There are three cases that match_file can handle:
		 *
		 *  dirname/filename
		 *  dirname/*
		 *  dirname/
		 *
		 * The first case is an exact match, the second case is a
		 * direct match of everything in that directory, and the third
		 * is a direct match of everything in that directory and its
		 * subdirectories.
		 *
		 * The first two cases are handled above, the code below is
		 * only for that latter case, so if file doesn't end in /,
		 * skip to the next file.
		 */
		if file[len(file)-1] != '/' {
			continue
		}

		/* Remove / because we add it again below */
		file = file[:len(file)-1]

		/* Maximum tree depth, as calculated by
		 * $(( `git ls-files | tr -d "[a-z][A-Z][0-9]\-\_\." | \
		 *     sort -u | tail -1 | wc -c` - 1 ))
		 * 11
		 */
		max_depth := 11

		for i := 0; i < max_depth; i++ {
			/* Subdirectory match */
			file += "/*"

			if matched, err = filepath.Match(file, fname); err != nil {
				return false, err
			}
			if matched {
				return true, nil
			}

		}
	}
	return false, nil
}

func find_maintainer(fname string) {
	for _, subsystem := range subsystems {
		matched, err := match_file(fname, subsystem.file)
		if err != nil {
			log.Fatalf("match_file failed: %v", err)
			return
		}
		if matched && subsystem.name != "THE REST" {
			fmt.Println(fname, "is in subsystem",
				subsystem.name)
			fmt.Println("Maintainers: ", subsystem.maintainer)
			return
		}
	}
	fmt.Println(fname, "has no subsystem defined in MAINTAINERS")
}

func find_unmaintained(fname string) {
	for _, subsystem := range subsystems {
		matched, err := match_file(fname, subsystem.file)
		if err != nil {
			log.Fatalf("match_file failed: %v", err)
			return
		}
		if matched && subsystem.name != "THE REST" {
			fmt.Println(fname, "is in subsystem",
				subsystem.name)
			return
		}
	}
	fmt.Println(fname, "has no subsystem defined in MAINTAINERS")
}

func main() {
	var files []string
	var maint bool
	var debug bool
	var err error

	args := os.Args[1:]
	if len(args) == 0 {
		/* get the filenames */
		files, err = get_git_files()
		if err != nil {
			log.Fatalf("Oops.")
			return
		}
		maint = false
	} else {
		files = args
		maint = true
	}

	maintainers, err := get_maintainers()
	if err != nil {
		log.Fatalf("Oops.")
		return
	}

	/* build subsystem database */
	build_maintainers(maintainers)

	if debug {
		print_maintainers()
	}

	if maint {
		/* Find maintainers for each file */
		for _, file := range files {
			find_maintainer(file)
		}
	} else {
		for _, file := range files {
			find_unmaintained(file)
		}
	}
}
