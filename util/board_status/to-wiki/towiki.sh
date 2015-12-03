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
		for file in "$vendor_board/$commit/$datetime/"*; do
			if [ "$file" = "$vendor_board/$commit/$datetime/revision.txt" ]; then
				continue
			fi
			detailed="$detailed[$STATUS_GITWEB$file `basename $file`] $nl"
		done
		detailed="$detailed)$nl$nl"
	done
done

cat <<EOF
== Motherboards supported in coreboot ==

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
! align="left" | <span title="ROM chip package">ROM&nbsp;<sup>1</sup></span>
! align="left" | <span title="ROM chip protocol">P&nbsp;<sup>2</sup></span>
! align="left" | <span title="ROM chip socketed?">S&nbsp;<sup>3</sup></span>
! align="left" | <span title="Board supported by flashrom?">F&nbsp;<sup>4</sup></span>
! align="left" | <span title="Vendor Cooperation Score">VCS<sup>5</sup></span>
EOF

for category in laptop server desktop half mini settop "eval" sbc emulation misc unclass; do
	last_vendor=
	color=eeeeee
	case "$category" in
		desktop)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Desktops / Workstations</h4>

EOF
			;;
		server)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Servers</h4>

EOF
			;;
		laptop)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Laptops</h4>

EOF
			;;
		half)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Embedded / PC/104 / Half-size boards</h4>

EOF
			;;
		mini)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Mini-ITX / Micro-ITX / Nano-ITX</h4>

EOF
			;;
		settop)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Set-top-boxes / Thin clients</h4>

EOF
			;;
		"eval")
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Devel/Eval Boards</h4>

EOF
			;;
		sbc)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Single-Board computer</h4>

EOF
			;;
		emulation)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Emulation</h4>

EOF
			;;
		misc)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Miscellaneous</h4>

EOF
			;;
		unclass)
			cat <<EOF
|- bgcolor="#6699ff"
| colspan="13" | <h4>Unclassified</h4>

EOF
			;;
	esac

	for vendor_board_dir in "$COREBOOT_DIR"/src/mainboard/*/* ; do
		board="$(basename "$vendor_board_dir")"
		vendor="$(basename "$(dirname "$vendor_board_dir")")"
		if [ "$board" = Kconfig ] || [ "$board" = Kconfig.name ]; then
			continue
		fi

		if [ -f "$vendor_board_dir/board_info.txt" ]; then
			cur_category="$(sed -n "/^[[:space:]]*Category:/ s,^[[:space:]]*Category:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
		else
			cur_category=
		fi

		case "$cur_category" in
			desktop|server|laptop|half|mini|settop|"eval"|emulation|sbc|misc)
				;;
			*)
				cur_category=unclass
				;;
		esac
		if [ "$cur_category" != "$category" ]; then
			continue
		fi

		if [ -f "$vendor_board_dir/board_info.txt" ]; then
			vendor_2nd="$(sed -n "/^[[:space:]]*Vendor name:/ s,^[[:space:]]*Vendor name:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
			board_nice="$(sed -n "/^[[:space:]]*Board name:/ s,^[[:space:]]*Board name:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
			rom_package="$(sed -n "/^[[:space:]]*ROM package:/ s,^[[:space:]]*ROM package:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
			rom_protocol="$(sed -n "/^[[:space:]]*ROM protocol:/ s,^[[:space:]]*ROM protocol:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
			rom_socketed="$(sed -n "/^[[:space:]]*ROM socketed:/ s,^[[:space:]]*ROM socketed:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
			flashrom_support="$(sed -n "/^[[:space:]]*Flashrom support:/ s,^[[:space:]]*Flashrom support:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
			vendor_cooperation_score="$(sed -n "/^[[:space:]]*Vendor cooperation score:/ s,^[[:space:]]*Vendor cooperation score:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
			vendor_cooperation_page="$(sed -n "/^[[:space:]]*Vendor cooperation page:/ s,^[[:space:]]*Vendor cooperation page:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
			board_url="$(sed -n "/^[[:space:]]*Board URL:/ s,^[[:space:]]*Board URL:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
			clone_of="$(sed -n "/^[[:space:]]*Clone of:/ s,^[[:space:]]*Clone of:[[:space:]]*,,p" "$vendor_board_dir/board_info.txt")"
		else
			vendor_2nd=
			board_nice=
			rom_package=
			rom_protocol=
			rom_socketed=
			flashrom_support=
			vendor_cooperation_score=
			vendor_cooperation_page=
			board_url=
			clone_of=
		fi
		if [ "$last_vendor" != "$vendor" ]; then
			last_vendor="$vendor"
			if [ "$color" = dddddd ]; then
				color=eeeeee
			else
				color=dddddd
			fi
		fi

		vendor_nice="$(grep -A1 -i "config VENDOR_$vendor" "$COREBOOT_DIR"/src/mainboard/$vendor/Kconfig.name|tail -n1|sed -n 's,^[[:space:]]*bool[[:space:]]*"\(.*\)"[[:space:]]*$,\1,p')"

		if [ -z "$vendor_nice" ]; then
			vendor_nice="$(echo "$vendor" |sed -e "s/\(.\)/\u\1/g")";
		fi
		if [ -z "$board_nice" ]; then
			board_nice="$(grep -A2 -i "config MAINBOARD_PART_NUMBER" "$COREBOOT_DIR"/src/mainboard/$vendor/$board/Kconfig|tail -n1|sed -n 's,^[[:space:]]*default[[:space:]]*"\(.*\)"[[:space:]]*$,\1,p')"
		fi
		if [ -z "$board_nice" ]; then
			board_nice="$(echo "$board" |sed -e "s,_, ,g;s/\(.\)/\u\1/g")";
		fi

		venboard="$vendor/$board"
		if [ -n "$clone_of" ]; then
		    venboard="$clone_of"
		fi

		lastgood="$(echo "$have"| sed -n "/^$(echo "$venboard"|sed 's,/,\\/,g'):/ s,^[^:]*:,,gp")"

		vendor_board_dir="$COREBOOT_DIR"/src/mainboard/"$venboard";

		northbridge="$(sed -n "/^[[:space:]]*select NORTHBRIDGE_/ s,^[[:space:]]*select NORTHBRIDGE_,,p" "$vendor_board_dir/Kconfig")"
		northbridge_nice="$(echo "$northbridge"|sed 's,AMD_AGESA_FAMILY\([0-9a-fA-F]*\)\(.*\),AMD Family \1h\2 (AGESA),g;s,AMD_PI_\(.*\),AMD \1 (PI),g;s,INTEL_FSP_\(.*\),Intel® \1 (FSP),g;s,AMD_FAMILY\([0-9a-fA-F]*\),AMD Family \1h,g;s,AMD_AMDFAM\([0-9a-fA-F]*\),AMD Family \1h,g;s,_, ,g;s,INTEL,Intel®,g;')"

		southbridge="$(sed -n "/[[:space:]]*select SOUTHBRIDGE_/ s,[[:space:]]*select SOUTHBRIDGE_\([^ ]*\).*$,\1,p" "$vendor_board_dir/Kconfig"|grep -v SKIP_|grep -v DISABLE_)"
		southbridge_nice="$(echo "$southbridge"|sed 's,_, ,g;s,INTEL,Intel®,g')"
		superio="$(sed -n "/[[:space:]]*select SUPERIO_/ s,[[:space:]]*select SUPERIO_,,p" "$vendor_board_dir/Kconfig"|grep -v OVERRIDE_FANCTL)"
		superio_nice="$(echo "$superio"|sed 's,_, ,g;s,WINBOND,Winbond™,g;s,ITE,ITE™,g;s,SMSC,SMSC®,g;s,NUVOTON,Nuvoton ,g')"
		cpu="$(sed -n "/	select CPU_/ s,	select CPU_,,p" "$vendor_board_dir/Kconfig"|grep -v "AMD_AGESA_FAMILY"|grep -v CPU_MICROCODE_CBFS_NONE)"
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
			AMD_SOCKET_G34|AMD_SOCKET_G34_NON_AGESA)
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
			INTEL_HASWELL)
			        cpu_nice="Intel® 4th Gen (Haswell) Core i3/i5/i7"
			        socket_nice="?"
				;;
			INTEL_FSP_RANGELEY)
			        cpu_nice="Intel® Atom Rangeley (FSP)"
			        socket_nice="?"
				;;
			INTEL_SOCKET_RPGA989|INTEL_SOCKET_LGA1155|INTEL_SOCKET_RPGA988B)
			        socket_nice="`echo $socket | sed 's,INTEL_SOCKET_,Socket ,g'`"
				case $northbridge in
					INTEL_HASWELL)
						cpu_nice="Intel® 4th Gen (Haswell) Core i3/i5/i7";;
					INTEL_IVYBRIDGE|INTEL_IVYBRIDGE_MRC|INTEL_FSP_IVYBRIDGE)
						cpu_nice="Intel® 3rd Gen (Ivybridge) Core i3/i5/i7";;
					INTEL_SANDYBRIDGE|INTEL_SANDYBRIDGE_MRC)
						cpu_nice="Intel® 2nd Gen (Sandybridge) Core i3/i5/i7";;
					*)
						cpu_nice="$northbridge";;
				esac
				;;
			INTEL_SOCKET_441)
				cpu_nice="Intel® Atom™ 230";
				socket_nice="Socket 441";;
			INTEL_SOCKET_BGA956)
				case $northbridge in
				    INTEL_GM45)
					cpu_nice="Intel® Core 2 Duo (Penryn)"
					socket_nice="Socket P";;
				    *)
					cpu_nice="Intel® Pentium® M";
					socket_nice="BGA956";;
				esac
				;;
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
						cpu_nice="Intel® 1st Gen (Nehalem) Core i3/i5/i7"
						socket_nice="?";;
					RDC_R8610)
						cpu_nice="RDC 8610"
						socket_nice="—";;
					AMD_AGESA_*|AMD_PI_*)
						cpu_nice="$northbridge_nice"
						socket_nice="?";;
					*)
						cpu_nice="$northbridge"
						socket_nice="$northbridge";;
				esac
				;;
			*)
				cpu_nice="$cpu"
				socket_nice="$cpu";;
		esac

		echo "|- bgcolor=\"#$color\""

		if [ -z "$board_url" ]; then
			echo "| $vendor_nice"
		else
			echo "| [$board_url $vendor_nice]"
		fi
		if ! [ -z "$vendor_2nd" ]; then
			echo "( $vendor_2nd )"
		fi

		echo "| [[Board:$vendor/$board|$board_nice]]"

		if [ -z "$lastgood" ]; then
			echo "| style=\"background:red\" | Unknown"
		else
			echo "| style=\"background:lime\" | [[#$vendor/$board|$lastgood]]"
		fi

		echo "| $northbridge_nice"
		echo "| $southbridge_nice"
		echo "| $superio_nice"
		echo "| $cpu_nice"
		echo "| $socket_nice"
		if [ "$rom_package" = "" ]; then
			echo "| ?"
		else
			echo "| $rom_package"
		fi
		if [ "$rom_protocol" = "" ]; then
			echo "| ?"
		else
			echo "| $rom_protocol"
		fi
		if [ "$rom_socketed" = "y" ]; then
			echo "| style=\"background:lime\" | Y"
		elif [ "$rom_socketed" = "n" ]; then
			echo "| style=\"background:red\" | N"
		elif [ "$flashrom_support" = "variable" ]; then
			echo "| ...<sup>7</sup>"
		elif [ "$rom_socketed" = "" ]; then
			echo "| ?"
		else
			echo "| $rom_socketed"
		fi
		if [ "$flashrom_support" = "y" ]; then
			echo "| style=\"background:lime\" | Y"
		elif [ "$flashrom_support" = "n" ]; then
			echo "| style=\"background:red\" | N"
		elif [ "$flashrom_support" = "coreboot-only" ]; then
			echo "| style=\"background:yellow\" | ...<sup>6</sup>"
		elif [ "$flashrom_support" = "" ]; then
			echo "| ?"
		else
			echo "| $flashrom_support"
		fi
		if [ "$vendor_cooperation_score" = "4" ]; then
			echo -n "| style=\"background:lime\""
		elif [ "$vendor_cooperatio_scoren" = "3" ]; then
			echo -n "| style=\"background:yellow\""
		fi
		if [ "$vendor_cooperation_page" != "" ]; then
			echo "| [[$vendor_cooperation_page|$vendor_cooperation_score]]"
		elif [ "$vendor_cooperation_score" = "" ]; then
			echo "| —"
		else
			echo "| $vendor_cooperation_score"
		fi
		echo
	done
done
echo "|}"

cat <<EOF
<small>
<sup>1</sup> ROM chip package (PLCC, DIP32, DIP8, SOIC8).<br />
<sup>2</sup> ROM chip protocol/type (parallel flash, LPC, FWH, SPI).<br />
<sup>3</sup> ROM chip socketed (Y/N)?<br />
<sup>4</sup> Board supported by [http://www.flashrom.org flashrom] (Y/N)?<br />
<sup>5</sup> Vendor Cooperation Score.<br />
<sup>6</sup> [http://www.flashrom.org flashrom] does not work when the vendor BIOS is booted, but it does work when the machine is booted with coreboot.<br />
<sup>7</sup> Some boards have ROM sockets, others are soldered.<br />
</small>
EOF


echo "$detailed"
