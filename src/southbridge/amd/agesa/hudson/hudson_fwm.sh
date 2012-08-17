#!/bin/sh

filesize()
{
    fwm="$1"
    if [ -z $fwm ]; then
	size=0
    else
	[ -r $fwm ] || { echo "$$fwm not found"; exit 1; };
	size=`ls -l $fwm | awk '{print $5}'`;
    fi
}

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# In our coreboot implementation, we concatenate the ROM images
# next to the ROMSIG. EC ROM should be 64K aligned.
# Final hudson.bin:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |                   USB3 ROM                              |
# +---------------------------------------------------------+
# |                   GEC ROM                               |
# +---------------------------------------------------------+
# |                   EC ROM                                |
# +---------------------------------------------------------+
#
print_romsig()
{
    echo $1 $2 $3 $4 |
    LC_ALL=C awk '
	function print_raw_dword(number)
	{
		#printf ("%x\n", number)
		printf ("%c%c%c%c", number % 0x100, number/0x100 % 0x100, number/0x10000 % 0x100, number/0x1000000);
	}
	{       # $1=ROMSIG address, $2=EC_ROM_size, $3=GEC_ROM_size, $4=USB3_ROM_size
		print_raw_dword(0x55AA55AA);
		imc_offset = $1 + 16 + $3 + 65535;
		imc_offset = imc_offset - (imc_offset) % 65536; #align 64K
		if ($2) print_raw_dword(imc_offset);    else print_raw_dword(0);
		if ($3) print_raw_dword($1 + 16 + $3);  else print_raw_dword(0);
		if ($4) print_raw_dword($1 + 16);       else print_raw_dword(0);
	} '
}

#main
#param:
# $1=$(CONFIG_HUDSON_FWM_POSITION)
# $2=$obj
# $3=$(CONFIG_HUDSON_XHCI_FWM_FILE)
# $4=$(CONFIG_HUDSON_IMC_FWM_FILE)
# $5=$(CONFIG_HUDSON_GEC_FWM_FILE)
base=`printf %d $1`

filesize $3
xhci_size=$size
filesize $4
imc_size=$size
filesize $5
gec_size=$size

print_romsig $base $imc_size $gec_size $xhci_size > $2.tmp
[ -z $3 ] || cat $3 >> $2.tmp
[ -z $5 ] || cat $5 >> $2.tmp
if [ ! -z $4 ]; then
    ls -l $2.tmp | awk '{ print $5 }' | LC_ALL=C awk '{for (i=$1; i<($1+65535) - ($1+65535)%65536; i++) {printf "%c", 255}}' >> $2.tmp # align 64K
    cat $4 >> $2.tmp
fi

mv $2.tmp $2
