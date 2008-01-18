/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

#include <console/console.h>

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"

/*you need to 
	1.add
                                                chip drivers/pci/onboard
                                                        device pci x.0 on end
                                                        register "rom_address" = "0xfff80000"
                                                end
	in your MB mainboard Config.lb
	2. add 
		#       48K for SCSI FW or ATI ROM
		        option ROM_SIZE = 512*1024-48*1024
	in your MB targets Config.lb, afer romimage "normal"
	3. create you vgabios.bin under normal bios and put that in dir that targets Config residues.
		# dd if=/dev/mem of=atix.rom skip=1536 count=96
	4. after build coreboot.rom
		# cat ../atix.rom ./normal/coreboot.rom ./fallback/coreboot.rom > coreboot.rom
	or use nsxv to build you image
		# time ./nsxv s2850

put following in nsxv and put nsxv in your LBROOT

#!/bin/bash
MBVENDOR=tyan
MBMODEL=$1
LBROOT=/home/yhlu/xx/xx

echo $1
date

cd "$LBROOT/freebios2/targets"
rm -rf "$MBVENDOR/$MBMODEL/$MBMODEL"
./buildtarget "$MBVENDOR/$MBMODEL" &> "$LBROOT/x_b.txt"
cd "$MBVENDOR/$MBMODEL/$MBMODEL"
#make clean
eval make &> "$LBROOT/x_m.txt"
        if [ $? -eq 0 ]; then
                echo "ok."
        else
                echo "FAILED! Log excerpt:"
                tail -n 15 "$LBROOT/x_m.txt"
                exit
        fi
cat ../atix.rom ./normal/coreboot.rom ./fallback/coreboot.rom > "$LBROOT/rom/"$MBMODEL"_coreboot.rom"
cp -f "$LBROOT/rom/"$MBMODEL"_coreboot.rom" /home/yhlu/

date


*/
		



static void onboard_enable(device_t dev) 
{
	struct drivers_pci_onboard_config *conf;
        conf = dev->chip_info;
	dev->rom_address = conf->rom_address;
}

struct chip_operations drivers_pci_onboard_ops = {
#if CONFIG_CHIP_NAME == 1
	CHIP_NAME("Onboard PCI")
#endif
	.enable_dev = onboard_enable,
};
