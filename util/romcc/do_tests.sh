#!/bin/sh
type="simple"
if [ -n "$1" ] ; then
	type=$1
fi
if [ "$type" = "simple" ] ; then
LIST="18 57 21 79 77 40 62 52 65 78 63 81 80 64 12 8 9 7 53 58 37 35 48  11 75 51 49  41 71 23 70 16 13 24 31 55 19 69 67 66 17 28 68 61 32 33 43 60 45 47 36 10 46 15 3 29 50 56 76 34 74 25 26 27 20 1 14 73 5 4"
LIST="82 83 $LIST"
BASE="simple_test"
#REG_SKIP="34 73 5   33 45 47 25 27 26 20 "
#SKIP="$REG_SKIP"
EXPECTED_BAD="34 73 5 4"
fi
if [ "$type" = "linux" ] ; then
LIST="1 2 3 4 5 6 7 8 9 10 11 12"
BASE="linux_test"
#SKIP="5"
EXPECTED_BAD=""
fi
if [ "$type" = "raminit" ] ; then
LIST="1 2 3 4 5 6"
BASE="raminit_test"
#SKIP="6"
EXPECTED_BAD=""
fi
if [ "$type" = "hello" ] ; then
LIST="1 2"
BASE="hello_world"
EXPECTED_BAD=""
fi


SKIPPED=""
GOOD=""
BAD=""
OLD_BAD=""
NEW_BAD=""
NEW_GOOD=""
for i in $LIST ; do
	DO_SKIP=""
	for j in $SKIP ; do
		if [ "$j" = "$i" ] ; then
			DO_SKIP="yes"
			break;
		fi
	done
	if [ ! -z "$DO_SKIP" ] ; then
	 	SKIPPED="$SKIPPED$i "
		continue;
	fi
	PROBLEM=""
	for j in $EXPECTED_BAD ; do
		if [ "$j" = "$i" ] ; then
			PROBLEM=":("
			break;
		fi
	done
	echo -e -n "$i $PROBLEM\t"
	if ./tests.sh $BASE $i > /dev/null 2> /dev/null ; then
		echo OK
		if [ ! -z "$PROBLEM" ] ; then
			NEW_GOOD="$NEW_GOOD$i "
		fi
		GOOD="$GOOD$i "
	else
		echo -n "FAILED: "
		tail -n 1 tests/$BASE$i.debug2 | tr -d '\r\n'
		echo
		if [ -z "$PROBLEM" ] ; then
			NEW_BAD="$NEW_BAD$i "
		else
			OLD_BAD="$OLD_BAD$i "
		fi
		BAD="$BAD$i "
	fi
done
echo -e "SKIPPED\t\t$SKIPPED"
echo -e "FAILED\t\t$BAD"
if [ ! -z "$NEW_BAD" ]; then
	echo -e "NEW FAILED\t$NEW_BAD"
fi
echo -e "OK\t\t$GOOD"
if [ ! -z "$NEW_GOOD" ]; then
	echo -e "NEW OK\t\t$NEW_GOOD"
fi

for i in $NEW_BAD ; do
	printf "%2d: " $i
	tail -n 1 tests/$BASE$i.debug2 | tr -d '\n\r'
	echo
done
echo "-----"
for i in $OLD_BAD ; do
	printf "%2d: " $i
	tail -n 1 tests/$BASE$i.debug2
done
