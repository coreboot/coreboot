#!/bin/sh
#
# This file is part of the coreboot project.
#
# Copyright 2016 Jonathan Neuschäfer <j.neuschaefer@gmx.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.


# These tests are currently known to be broken and should not result in a
# build failure:
XFAIL_TESTS="
	simple_test4.c
	simple_test6.c
	simple_test25.c
	simple_test26.c
	simple_test46.c
	simple_test47.c
	simple_test54.c
	simple_test72.c
	simple_test73.c
	linux_test2.c
	linux_test5.c
	linux_test10.c
	linux_test11.c
	linux_test12.c
"

# ------------------------------------------------------------------- #

BASEDIR="$(dirname "$0")"
BUILDDIR="$BASEDIR/build"
LOGDIR="$BUILDDIR/logs"
mkdir -p "$BUILDDIR"
mkdir -p "$LOGDIR"

usage () {
	echo "Usage: test.sh [--nocolor] CLASS"
	echo ""
	echo "CLASS selects a group of tests to run. It must be one of the following:"
	echo "  all     - all tests"
	echo "  simple  - simple tests"
	echo "  linux   - linux programs whose output is checked against a reference"
	echo ""
	echo "--nocolor disables colors."
	exit 1
}

COLORS=1
if [ "$1" = "--nocolor" ]; then
	shift
	COLORS=0
fi


if [ -t 1 -a "$COLORS" -eq 1 ]; then
	red()   { printf "\033[1;31m%s\033[0m" "$*"; }
	green() { printf "\033[1;32m%s\033[0m" "$*"; }
	blue()  { printf "\033[1;34m%s\033[0m" "$*"; }
else
	red()   { printf "%s" "$*"; }
	green() { printf "%s" "$*"; }
	blue()  { printf "%s" "$*"; }
fi

init_stats() {
	NUM_TOTAL=0	# Number of tests that were run
	NUM_FAIL=0	# Number of tests that failed unexpectedly
	NUM_BROKEN=0	# Number of tests that failed expectedly
	NUM_PASS=0	# Number of tests that passed expectedly
	NUM_FIXED=0	# Number of tests that passed unexpectedly
}

get_romcc() {
	ROMCC="$BUILDDIR/romcc"
	if [ ! -f "$ROMCC" ]; then
		echo "romcc not found! Please run \"make\"."
		exit 1
	fi
}

init_testing() {
	init_stats
	get_romcc
}

show_stats() {
	printf "passed: %s\t(%s newly fixed)\n" $NUM_PASS $NUM_FIXED
	printf "failed: %s\t(%s known broken)\n" $NUM_FAIL $NUM_BROKEN
	printf "total:  %s\n" $NUM_TOTAL
}

is_xfail() {
	local t
	for t in $XFAIL_TESTS; do
		if [ "$t" = "$1" ]; then
			return 0
		fi
	done
	return 1
}

pass() {
	NUM_TOTAL=$((NUM_TOTAL + 1))
	NUM_PASS=$((NUM_PASS + 1))

	green "passed"
	if is_xfail "$(basename "$1")"; then
		blue " (fixed)"
		NUM_FIXED=$((NUM_FIXED + 1))
	fi
	echo
}

fail() {
	NUM_TOTAL=$((NUM_TOTAL + 1))
	NUM_FAIL=$((NUM_FAIL + 1))

	red "failed"
	if is_xfail "$(basename "$1")"; then
		blue " (known broken)"
		NUM_BROKEN=$((NUM_BROKEN + 1))
	fi
	echo
}

run_simple_test() {
	# TODO: "timeout" is not POSIX compliant. Use something that is.
	timeout 60 "$ROMCC" $1 "$2" -o "$BUILDDIR/dummy.S"
}

run_simple_tests() {
	echo "Running simple tests..."

	local t
	for t in $(find "$BASEDIR/tests" -name 'simple_test*.c'); do
		printf "%s" "$(basename "$t")"

		local result=pass
		local logfile="$LOGDIR/$(basename "$t").log"
		rm "$logfile" >/dev/null 2>&1
		for opt in "" "-O" "-O2" "-mmmx" "-msse" "-mmmx -msse" \
		           "-O -mmmx" "-O -msse" "-O  -mmmx -msse" \
		           "-O2 -mmmx" "-O2 -msse" "-O2 -mmmx -msse"; do
			if run_simple_test "$opt" "$t" \
					>> "$logfile" 2>&1; then
				printf .
			else
				result=fail
				break
			fi
		done
		printf " "
		$result "$t"
	done

	echo
}

run_linux_test() {
	local base="$(basename "$1")"

	# TODO: "timeout" is not POSIX compliant. Use something that is.

	timeout 60 "$ROMCC" "$1" -o "$BUILDDIR/$base.S" || return 1
	as --32 "$BUILDDIR/$base.S" -o "$BUILDDIR/$base.o" || return 1
	ld -m elf_i386 -T "$BASEDIR/tests/ldscript.ld" \
		"$BUILDDIR/$base.o" -o "$BUILDDIR/$base.elf" || return 1
	timeout 60 "$BUILDDIR/$base.elf" > "$BUILDDIR/$base.out" || return 1

	diff -u "$BASEDIR/results/${base%.c}.out" "$BUILDDIR/$base.out"
}

run_linux_tests() {
	echo "Running linux tests..."

	local t
	for t in $(find "$BASEDIR/tests" -name 'linux_test*.c'); do
		printf "%s... " "$(basename "$t")"

		local logfile="$LOGDIR/$(basename "$t").log"
		if run_linux_test "$t" > "$logfile" 2>&1; then
			pass "$t"
		else
			fail "$t"
		fi
	done

	echo
}


if [ $# -ne 1 ]; then
	usage
fi

CLASS="$1"

case "$CLASS" in
	all)
		init_testing
		run_simple_tests
		run_linux_tests
		show_stats
		;;
	simple)
		init_testing
		run_simple_tests
		show_stats
		;;
	linux)
		init_testing
		run_linux_tests
		show_stats
		;;
	*)
		echo "Invalid test class $CLASS"
		echo
		usage
		;;
esac

if [ $NUM_FAIL -ne $NUM_BROKEN ]; then
	exit 1
fi
