/* SPDX-License-Identifier: GPL-2.0-only */

package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
	"regexp"
	"strings"
)

type subsystem struct {
	name       string
	maintainer []string
	paths      []string
	globs      []*regexp.Regexp
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

func path_to_regexstr(path string) string {
	regexstr := glob_to_regex(path)

	/* Handle path with trailing '/' as prefix */
	if regexstr[len(regexstr)-2:] == "/$" {
		regexstr = regexstr[:len(regexstr)-1] + ".*$"
	}

	return regexstr;
}

func path_to_regex(path string) *regexp.Regexp {
	regexstr := path_to_regexstr(path)
	return regexp.MustCompile(regexstr)
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
			case 'R', 'M':
				/* Add subsystem maintainer */
				current.maintainer = append(current.maintainer, line[3:len(line)])
			case 'F':
				// add files
				current.paths = append(current.paths, line[3:len(line)])
				current.globs = append(current.globs, path_to_regex(line[3:len(line)]))
				break
			case 'L', 'S', 'T', 'W': // ignore
			default:
				fmt.Println("No such specifier: ", line)
			}
		}
	}
}

func print_maintainers() {
	for _, subsystem := range subsystems {
		fmt.Println(subsystem.name)
		fmt.Println("  ", subsystem.maintainer)
		fmt.Println("  ", subsystem.paths)
	}
}

func match_file(fname string, component subsystem) bool {
	for _, glob := range component.globs {
		if glob.Match([]byte(fname)) {
			return true
		}
	}
	return false
}

func find_maintainer(fname string) {
	var success bool

	for _, subsystem := range subsystems {
		matched := match_file(fname, subsystem)
		if matched {
			success = true
			fmt.Println(fname, "is in subsystem",
				subsystem.name)
			fmt.Println("Maintainers: ", strings.Join(subsystem.maintainer, ", "))
		}
	}
	if !success {
		fmt.Println(fname, "has no subsystem defined in MAINTAINERS")
	}
}

func find_unmaintained(fname string) {
	var success bool

	for _, subsystem := range subsystems {
		matched := match_file(fname, subsystem)
		if matched {
			success = true
			fmt.Println(fname, "is in subsystem",
				subsystem.name)
		}
	}
	if !success {
		fmt.Println(fname, "has no subsystem defined in MAINTAINERS")
	}
}

// taken from https://github.com/zyedidia/glob/blob/master/glob.go which is
// Copyright (c) 2016: Zachary Yedidia.
// and was published under the MIT "Expat" license.
//
// only change: return the string, instead of a compiled golang regex
func glob_to_regex(glob string) string {
	regex := ""
	inGroup := 0
	inClass := 0
	firstIndexInClass := -1
	arr := []byte(glob)

	for i := 0; i < len(arr); i++ {
		ch := arr[i]

		switch ch {
		case '\\':
			i++
			if i >= len(arr) {
				regex += "\\"
			} else {
				next := arr[i]
				switch next {
				case ',':
					// Nothing
				case 'Q', 'E':
					regex += "\\\\"
				default:
					regex += "\\"
				}
				regex += string(next)
			}
		case '*':
			if inClass == 0 {
				regex += "[^/]*"
			} else {
				regex += "*"
			}
		case '?':
			if inClass == 0 {
				regex += "."
			} else {
				regex += "?"
			}
		case '[':
			inClass++
			firstIndexInClass = i + 1
			regex += "["
		case ']':
			inClass--
			regex += "]"
		case '.', '(', ')', '+', '|', '^', '$', '@', '%':
			if inClass == 0 || (firstIndexInClass == i && ch == '^') {
				regex += "\\"
			}
			regex += string(ch)
		case '!':
			if firstIndexInClass == i {
				regex += "^"
			} else {
				regex += "!"
			}
		case '{':
			inGroup++
			regex += "("
		case '}':
			inGroup--
			regex += ")"
		case ',':
			if inGroup > 0 {
				regex += "|"
			} else {
				regex += ","
			}
		default:
			regex += string(ch)
		}
	}
	return "^" + regex + "$"
}

var is_email *regexp.Regexp

func extract_maintainer(maintainer string) string {
	if is_email == nil {
		is_email = regexp.MustCompile("<[^>]*>")
	}

	if match := is_email.FindStringSubmatch(maintainer); match != nil {
		return match[0][1 : len(match[0])-1]
	}
	return maintainer
}

func do_print_gerrit_rules() {
	for _, subsystem := range subsystems {
		if len(subsystem.paths) == 0 || len(subsystem.maintainer) == 0 {
			continue
		}
		fmt.Println("#", subsystem.name)
		for _, path := range subsystem.paths {
			fmt.Println("[filter \"file:\\\"" + path_to_regexstr(path) + "\\\"\"]")
			for _, maint := range subsystem.maintainer {
				fmt.Println("  reviewer =", extract_maintainer(maint))
			}
		}
		fmt.Println()
	}
}

func main() {
	var (
		files              []string
		err                error
		print_gerrit_rules = flag.Bool("print-gerrit-rules", false, "emit the MAINTAINERS rules in a format suitable for Gerrit's reviewers plugin")
		debug              = flag.Bool("debug", false, "emit additional debug output")
	)
	flag.Parse()

	/* get and build subsystem database */
	maintainers, err := get_maintainers()
	if err != nil {
		log.Fatalf("Oops.")
		return
	}
	build_maintainers(maintainers)

	if *debug {
		print_maintainers()
	}

	if *print_gerrit_rules {
		do_print_gerrit_rules()
		return
	}

	args := flag.Args()
	if len(args) == 0 {
		/* get the filenames */
		files, err = get_git_files()
		if err != nil {
			log.Fatalf("Oops.")
			return
		}
		for _, file := range files {
			find_unmaintained(file)
		}
	} else {
		files = args

		/* Find maintainers for each file */
		for _, file := range files {
			find_maintainer(file)
		}
	}
}
