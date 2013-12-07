#!/bin/sh
export GIT_DIR=../coreboot/.git
CODE_GITWEB="http://review.coreboot.org/gitweb?p=coreboot.git;a=commitdiff;h="
STATUS_GITWEB="http://review.coreboot.org/gitweb?p=board-status.git;a=blob;hb=HEAD;f="
if [ -f `dirname $0`/foreword.wiki ]; then
	cat `dirname $0`/foreword.wiki
fi
while read line; do
	timeframe=`echo $line | cut -d: -f1`
	rest=`echo $line | cut -d: -f2-`
	echo "= $timeframe ="
	for i in $rest; do
		vendor_board=`echo $i | cut -d/ -f1-2`
		commit=`echo $i | cut -d/ -f3`
		datetime=`echo $i | cut -d/ -f4`
		datetime_human=`LC_ALL=C TZ=UTC date --date="$datetime"`
		upstream=`grep "^Upstream revision:" $vendor_board/$commit/$datetime/revision.txt |cut -d: -f2-`
		upstream=`git log -1 --format=%H $upstream`
		echo "[[Board:$vendor_board|$vendor_board]] at $datetime_human"
		echo "[$CODE_GITWEB$upstream upstream tree] ("
		ls $vendor_board/$commit/$datetime/* |grep -v '/revision.txt$' | while read file; do
			echo "[$STATUS_GITWEB$file `basename $file`] "
		done
		echo ")"
		echo
	done
done

