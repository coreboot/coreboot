#!/bin/sh
# Allow core dumps
ulimit -c hard
set -x
N=''
root=simple_test
#root=simple_test
#root=linux_test
#root=raminit_test
if [ -n "$2" -a -n "$1" ] ; then
    root=$1
	N=$2
elif [ -n "$1" ] ; then
	root=$1
else
	echo "too few arguments"
	exit 1
fi
ROMCC=./romcc
stem="$root$N"
base=tests/$stem
op="-Itests/include"
op="$op -feliminate-inefectual-code -fsimplify -fscc-transform "
#op="$op -O2 "
#op="$op -mmmx -msse"
op="$op -finline-policy=defaulton"
#op="$op -finline-policy=nopenalty"
#op="$op -finline-policy=never"
op="$op -fdebug -fdebug-triples -fdebug-interference  -fdebug-verification"
op="$op -fdebug-fdominators"
op="$op -fdebug-inline"
op="$op -fdebug-calls"
#op="$op -mnoop-copy"
#op="$op -fsimplify -fno-simplify-op -fno-simplify-phi -fno-simplify-label -fno-simplify-branch -fno-simplify-copy -fno-simplify-arith -fno-simplify-shift -fno-simplify-bitwise -fno-simplify-logical"
#op="$op -fdebug-rebuild-ssa-form"
op="$op -fmax-allocation-passes=8"
op="$op  -fdebug-live-range-conflicts"
op="$op -fdebug-scc-transform"
op="$op -fdebug-scc-transform2"
#-fdebug-coalescing
#-fdebug-coalesing2
#-fno-simplify-call "
#-fno-always-inline"
#
#op="-O2 -mmmx -msse --debug=4294967295"
#op="-fdebug -fdebug-triples -fdebug-inline -O2 -mmmx -msse -fno-always-inline "
#op="-fdebug -fdebug-inline -O2 -mmmx  "
#op="-fdebug -fdebug-live-range-conflicts -fdebug-live-range-conflicts2 -fno-debug-interference -fdebug-color-graph -fdebug-coalescing -fmax-allocation-passes=10 -O2 -mmmx -msse"
#op="-fdebug -O2 -mmmx -msse"
#op="-fdebug -fdebug-inline -fno-eliminate-inefectual-code -fno-always-inline -mmmx"
#op="-fdebug -fdebug-inline -fno-always-inline -mmmx"
export ALLOC_CHECK_=2
rm -f core $base.S $base.debug $base.debug2 $base.elf $base.out &&
make romcc &&
$ROMCC $op -o $base.S $base.c 2>&1 > $base.debug | tee $base.debug2
if [ '(' -f $base.c ')' -a '(' '!' -f core ')' -a '(' -f $base.S ')' ]; then
	if [ "$stem" = "linux_test$N" ] ; then
		as $base.S -o $base.o &&
		ld -T tests/ldscript.ld $base.o -o $base.elf &&
		./$base.elf > $base.out &&
		diff -u results/$stem.out $base.out
	else
		/bin/true
	fi
else
	/bin/false
fi


