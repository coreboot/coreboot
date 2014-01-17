#!/bin/sh
export COREBOOT_DIR="../coreboot"
export GIT_DIR="$COREBOOT_DIR/.git"
CODE_GITWEB="http://review.coreboot.org/gitweb?p=coreboot.git;a=commitdiff;h="
STATUS_GITWEB="http://review.coreboot.org/gitweb?p=board-status.git;a=blob;hb=HEAD;f="
if [ -f `dirname $0`/foreword.wiki ]; then
	cat `dirname $0`/foreword.wiki
fi
detailed=
nl="
"
have=
while read line; do
	timeframe=`echo $line | cut -d: -f1`
	rest=`echo $line | cut -d: -f2-`
	detailed="$detailed= $timeframe =$nl"
	for i in $rest; do
		vendor_board=`echo $i | cut -d/ -f1-2`
		commit=`echo $i | cut -d/ -f3`
		datetime=`echo $i | cut -d/ -f4`
		datetime_human=`LC_ALL=C TZ=UTC date --date="$datetime"`
		upstream=`grep "^Upstream revision:" $vendor_board/$commit/$datetime/revision.txt |cut -d: -f2-`
		upstream=`git log -1 --format=%H $upstream`
		if ! echo "$have"| grep  "^$vendor_board:" > /dev/null; then
		    detailed="$detailed<span id=\"$vendor_board\"></span>$nl"
		    have="$have$vendor_board:$datetime$nl"
		fi

		detailed="$detailed[[Board:$vendor_board|$vendor_board]] at $datetime_human$nl"
		detailed="$detailed[$CODE_GITWEB$upstream upstream tree] ($nl"
		ls $vendor_board/$commit/$datetime/* |grep -v '/revision.txt$' | while read file; do
			detailed="$detailed[$STATUS_GITWEB$file `basename $file`] $nl"
		done
		detailed="$detailed)$nl$nl"
	done
done

cat <<EOF
== Motherboards supported in coreboot v4 ==

{| border="0" style="font-size: smaller"
|- bgcolor="#6699ff"
! align="left" | Vendor
! align="left" | Mainboard
! align="left" | Latest known good
! align="left" | Northbridge
! align="left" | Southbridge
! align="left" | Super&nbsp;I/O
! align="left" | CPU
! align="left" | Socket
EOF

for vendor_board_dir in "$COREBOOT_DIR"/src/mainboard/*/* ; do
    board="$(basename "$vendor_board_dir")"
    vendor="$(basename "$(dirname "$vendor_board_dir")")"
    if [ "$board" = Kconfig ]; then
	continue
    fi
    vendor_nice="$(echo "$vendor" |sed -e "s/\(.\)/\u\1/g")";
    board_nice="$(echo "$board" |sed -e "s,_, ,g;s/\(.\)/\u\1/g")";

    lastgood="$(echo "$have"| sed -n "/^$vendor\/$board:/ s,^[^:]*:,,gp")"

    northbridge="$(sed -n "/^[[:space:]]*select NORTHBRIDGE_/ s,^[[:space:]]*select NORTHBRIDGE_,,p" "$vendor_board_dir/Kconfig")"
    northbridge_nice="$(echo "$northbridge"|sed 's,_, ,g;s,INTEL,Intel®,g;s,AMD_AGESA_FAMILY\(.*\),AMD Family \1h (AGESA),g;s,AMD_FAMILY\(.*\),AMD Family \1h,g;')"

    southbridge="$(sed -n "/	select SOUTHBRIDGE_/ s,	select SOUTHBRIDGE_,,p" "$vendor_board_dir/Kconfig")"
    southbridge_nice="$(echo "$southbridge"|sed 's,_, ,g;s,INTEL,Intel®,g')"
    superio_nice="$(echo "$superio"|sed 's,_, ,g;s,WINBOND,Winbond™,g;s,ITE,ITE™,g;s,SMSC,SMSC®,g')"

    superio="$(sed -n "/	select SUPERIO_/ s,	select SUPERIO_,,p" "$vendor_board_dir/Kconfig")"
    cpu="$(sed -n "/	select CPU_/ s,	select CPU_,,p" "$vendor_board_dir/Kconfig"|grep -v "AMD_AGESA_FAMILY")"
    case "$cpu" in
	ALLWINNER_A10)
	    cpu_nice="Allwinner A10"
	    socket_nice="?";;
	AMD_GEODE_*)
	    cpu_nice="AMD Geode™ ${cpu#AMD_GEODE_}";
	    socket_nice="—";;
	AMD_SOCKET_754)
	    cpu_nice="AMD Sempron™ / Athlon™ 64 / Turion™ 64";
	    socket_nice="Socket 754";;
	AMD_SOCKET_ASB2)
	    cpu_nice="AMD Turion™ II Neo/Athlon™ II Neo";
	    socket_nice="ASB2 (BGA812)";;
	AMD_SOCKET_S1G1)
	    cpu_nice="AMD Turion™ / X2  Sempron™";
	    socket_nice="Socket S1G1";;
	AMD_SOCKET_G34)
	    cpu_nice="AMD Opteron™ Magny-Cours/Interlagos";
	    socket_nice="Socket G34";;
	AMD_SOCKET_C32|AMD_SOCKET_C32_NON_AGESA)
	    cpu_nice="AMD Opteron™ Magny-Cours/Interlagos";
	    socket_nice="Socket C32";;
	AMD_SOCKET_AM2)
	    cpu_nice="?";
	    socket_nice="Socket AM2"
	    ;;
	AMD_SOCKET_AM3)
	    cpu_nice="AMD Athlon™ 64 / FX / X2";
	    socket_nice="Socket AM3"
	    ;;
	AMD_SOCKET_AM2R2)
	    cpu_nice="AMD Athlon™ 64 / X2 / FX, Sempron™";
	    socket_nice="Socket AM2+"
	    ;;
	AMD_SOCKET_F)
	    cpu_nice="AMD Opteron™";
	    socket_nice="Socket F"
	    ;;
	AMD_SOCKET_F_1207)
	    cpu_nice="AMD Opteron™";
	    socket_nice="Socket F 1207"
	    ;;
	AMD_SOCKET_940)
	    cpu_nice="AMD Opteron™";
	    socket_nice="Socket 940"
	    ;;
	AMD_SOCKET_939)
	    cpu_nice="AMD Athlon™ 64 / FX / X2";
	    socket_nice="Socket 939"
	    ;;
	AMD_SC520)
	    cpu_nice="AMD Élan™SC520";
	    socket_nice="—";;
	ARMLTD_CORTEX_A9)
	    cpu_nice="ARM Cortex A9";
	    socket_nice="?";;
	DMP_VORTEX86EX)
	    cpu_nice="DMP VORTEX86EX";
	    socket_nice="?";;
	SAMSUNG_EXYNOS5420)
	    cpu_nice="Samsung Exynos 5420";
	    socket_nice="?";;
	SAMSUNG_EXYNOS5250)
	    cpu_nice="Samsung Exynos 5250";
	    socket_nice="?";;
	TI_AM335X)
	    cpu_nice="TI AM335X";
	    socket_nice="?";;
	INTEL_SLOT_1)
	    cpu_nice="Intel® Pentium® II/III, Celeron®";
	    socket_nice="Slot 1";;
	INTEL_SOCKET_MPGA604)
	    cpu_nice="Intel® Xeon®";
	    socket_nice="Socket 604";;
	INTEL_SOCKET_MFCPGA478)
	    cpu_nice="Intel® Core™ 2 Duo Mobile, Core™ Duo/Solo, Celeron® M";
	    socket_nice="Socket mPGA478";;
	INTEL_SOCKET_LGA771)
	    cpu_nice="Intel Xeon™ 5000 series";
	    socket_nice="Socket LGA771";;
	INTEL_SOCKET_PGA370)
	    cpu_nice="Intel® Pentium® III-800, Celeron®"
	    socket_nice="Socket 370";;
	INTEL_SOCKET_MPGA479M)
	    cpu_nice="Intel® Mobile Celeron"
	    socket_nice="Socket 479"
	    ;;
	INTEL_SOCKET_RPGA989)
	    case $northbridge in
		INTEL_HASWELL)
		cpu_nice="Intel® 4th Gen (Haswell) Core i3/i5/i7"
		socket_nice="Socket RPGA989";;
		INTEL_IVYBRIDGE|INTEL_FSP_IVYBRIDGE)
		cpu_nice="Intel® 3rd Gen (Ivybridge) Core i3/i5/i7"
		socket_nice="Socket RPGA989";;
		INTEL_SANDYBRIDGE)
		cpu_nice="Intel® 2nd Gen (Sandybridge) Core i3/i5/i7"
		socket_nice="Socket RPGA989";;
		*)
		echo "unknown northbridge $northbridge" >&2;
		exit 1;;
	    esac
	    ;;
	INTEL_SOCKET_441)
	    cpu_nice="Intel® Atom™ 230";
	    socket_nice="Socket 441";;
	INTEL_SOCKET_BGA956)
	    cpu_nice="Intel® Pentium® M";
	    socket_nice="BGA956";;
	INTEL_SOCKET_FC_PGA370)
	    cpu_nice="Intel® Pentium® III / Celeron®";
	    socket_nice="Socket 370"
	    ;;
	INTEL_EP80579)
	    cpu_nice="Intel® EP80579";
	    socket_nice="Intel® EP80579"
	    ;;
	INTEL_SOCKET_MFCBGA479)
	    cpu_nice="Intel® Mobile Celeron"
	    socket_nice="Socket 479";;
	VIA_C3)
	    cpu_nice="VIA C3™";
	    socket_nice="?"
	    ;;
	VIA_C7)
	    cpu_nice="VIA C7™";
	    socket_nice="?"
	    ;;
	VIA_NANO)
	    cpu_nice="VIA Nano™";
	    socket_nice="?"
	    ;;
	QEMU_X86)
	    cpu_nice="QEMU x86";
	    socket_nice="—"
	    ;;
	"")
	    case $northbridge in
		INTEL_NEHALEM)
		    cpu_nice="Intel® 1st Gen (Haswell) Core i3/i5/i7"
		    socket_nice="?";;
		RDC_R8610)
		    cpu_nice="RDC 8610"
		    socket_nice="—";;
		AMD_AGESA_FAMILY14|AMD_AGESA_FAMILY15_TN|AMD_AGESA_FAMILY15_KB|AMD_AGESA_FAMILY16_KB|AMD_AGESA_FAMILY12)
		    cpu_nice="?"
		    socket_nice="?";;
		*)
		    echo "unknown northbridge $northbridge ($vendor/$board)" >&2;
		    exit 1;;
	    esac
	    ;;
	*)
	    echo "unknown CPU $cpu" >&2;
	    exit 1;;
    esac
    echo
    echo "|- bgcolor=\"#dddddd\""
    echo "| $vendor_nice"
    echo "| $board_nice"

    if [ -z "$lastgood" ]; then
	echo "| style=\"background:red\" | Unknown"
    else
	echo "| style=\"background:lime\" | [[#$vendor/$board|$lastgood]]"
    fi

    echo "| $northbridge_nice"
    echo "| $southbridge_nice"
    echo "| $superio"
    echo "| $cpu_nice"
    echo "| $socket_nice"
    echo
done
echo "|}"

echo "$detailed"
