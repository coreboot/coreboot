#!/bin/sh
# usage: $0 [weekly|monthly|quarterly] < filenames
#   sorts files of the form VENDOR/BOARD/COMMIT/DATE/revision.txt
#   into buckets of the given granularity

weekly() {
	date --date="$1" +%GW%V
}

monthly() {
	date --date="$1" +%Y-%m
}

quarterly() {
	date --date="$1" "+%Y %m"  | awk '{ q=int(($2-1)/3+1); print $1 "Q" q}'
}

# TODO: restrict $1 to allowed values

curr=""
sort -r -k4 -t/ | while read file; do
	timestamp=`printf $file | cut -d/ -f4`
	new=`$1 $timestamp`
	if [ "$new" != "$curr" ]; then
		if [ "$curr" != "" ]; then
			printf "\n"
		fi
		printf "$new:"
		curr=$new
	fi
	printf "$file "
done
printf "\n"

