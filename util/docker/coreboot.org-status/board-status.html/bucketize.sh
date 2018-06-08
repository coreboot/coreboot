#!/bin/sh
# usage: $0 [weekly|monthly|quarterly] < filenames
#   sorts files of the form VENDOR/BOARD/COMMIT/DATE/revision.txt
#   into buckets of the given granularity

weekly() {
	date --date="$1" +%GW%V 2>/dev/null
	return $?
}

monthly() {
	date --date="$1" +%Y-%m 2>/dev/null
	return $?
}

quarterly() {
	date --date="$1" "+%Y %m"  2>/dev/null | awk '{ q=int(($2-1)/3+1); print $1 "Q" q}'
	return $?
}

# Restrict $1 to allowed values
if [ "$1" != "weekly" ] && [ "$1" != "monthly" ] && [ "$1" != "quarterly" ]; then
	exit 1
fi

curr=""
sort -r -k4 -t/ | while read file; do
	# Exclude 'oem' directories
	if echo "$file" | grep -q '/oem/'; then continue; fi
	timestamp=$(printf "%s" "$file" | cut -d/ -f4 | tr _ :)

	# If the directory is not a date, skip it.
	new=$($1 "$timestamp");retval=$?
	if [ "$retval" != "0" ]; then continue; fi

	if [ "$new" != "$curr" ]; then
		if [ "$curr" != "" ]; then
			printf "\n"
		fi
		printf "%s:" "$new"
		curr=$new
	fi
	printf "%s " "$file"
done
printf "\n"
